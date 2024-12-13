#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SD.h>
#include <SPI.h>

const char* ssid = "ria";
const char* password = "irbx4738";

// Define the token for authentication
const String validToken = "your_token_here";

String websites[10];
String passwords[10];

// Initialize SD card pins
int sck = 42;
int miso = 1;
int mosi = 2;
int cs = 41;

// Create an AsyncWebServer object on port 80
AsyncWebServer server(80);

// Password strength calculation function
String calculatePasswordStrength(const String& password) {
  int n = password.length();
  int strength = 0;
  String strengthDisplay = "";
  
  // Define character categories
  bool hasLower = false, hasUpper = false;
  bool hasNum = false, specialChar = false;
  String normalChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890 ";

  // Check character types in the password
  for (int i = 0; i < n; i++) {
    if (islower(password[i])) hasLower = true;
    if (isupper(password[i])) hasUpper = true;
    if (isdigit(password[i])) hasNum = true;

    // Check for special characters
    if (normalChars.indexOf(password[i]) == -1) { // if character is not in the normalChars string
      specialChar = true;
    }
  }

  // Score password strength based on criteria
  if (n >= 12) strength += 1;
  if (n >= 16) strength += 1;
  if (hasLower) strength += 1;
  if (hasUpper) strength += 1;
  if (specialChar) strength += 1;
  if (hasNum) strength += 1;

  // Determine password strength rating (stars)
  for (int i = 0; i < strength; ++i)
    strengthDisplay += "*";

  return strengthDisplay;
}

// This function loads a CSV file from the SD card, extracts the website names, passwords, and strength, and formats it as HTML
String loadCSV() {
  File file = SD.open("/data.csv");  // Open the file from SD card
  String csvContent = "<html><body><h2>Website Passwords</h2><table border='1' style='width:80%; margin:auto; border-collapse: collapse; text-align:left;'>"
                      "<tr><th>Website</th><th>Password</th><th>Strength</th></tr>";

  if (file) {
    bool firstLine = true;
    while (file.available()) {
      String line = file.readStringUntil('\n');
      
      // Skip the first line (header)
      if (firstLine) {
        firstLine = false;
        continue;
      }

      // Parse the line by splitting it by commas
      int commaIndex1 = line.indexOf(',');
      int commaIndex2 = line.indexOf(',', commaIndex1 + 1);
      int commaIndex3 = line.indexOf(',', commaIndex2 + 1);
      
      if (commaIndex1 > 0 && commaIndex2 > 0 && commaIndex3 > 0) {
        String website = line.substring(0, commaIndex1);    // The 'name' column
        String password = line.substring(commaIndex3 + 1);  // The 'password' column
        String strength = calculatePasswordStrength(password); // Calculate the password strength
        
        // Add each website, password, and strength to the HTML table
        csvContent += "<tr><td>" + website + "</td><td>" + password + "</td><td>" + strength + "</td></tr>";
      }
    }
    file.close();
  }
  
  csvContent += "</table></body></html>";
  return csvContent;
}

// Update password in CSV file on SD card
void updateCSVFile() {
  File file = SD.open("/data.csv", FILE_WRITE);  
  if (file) {
    file.println("website,url,username,password");  // Add header again
    for (int i = 0; i < 10; i++) {
      if (websites[i] != "") {
        file.print(websites[i] + ",www." + websites[i] + ",user," + passwords[i]);
        file.println();
      }
    }
    file.close();
  }
}

// Function to handle authentication
bool isValidToken(AsyncWebServerRequest *request) {
  // Check if token is present as a query parameter
  if (request->hasParam("token")) {
    String token = request->getParam("token")->value();
    return token == validToken;
  }
  return false;
}

void setup() {
  // Start Serial Monitor
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  SPI.begin(sck, miso, mosi, cs);

  // Initialize SD card
  if (!SD.begin(cs)) {
    Serial.println("SD card initialization failed!");
    return;
  }
  Serial.println("SD card initialized.");

  // Serve the CSV content at the root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    if (isValidToken(request)) {
      // If token is valid, send the CSV content
      String htmlContent = loadCSV();  // Load CSV data and convert it to HTML
      request->send(200, "text/html", htmlContent);
    } else {
      // If token is invalid, return an error message
      request->send(403, "text/plain", "Forbidden: Invalid Token");
    }
  });

  // Handle password update request
  server.on("/update", HTTP_POST, [](AsyncWebServerRequest *request){
    if (isValidToken(request)) {
      String index = request->getParam("index")->value();
      String new_password = request->getParam("new_password")->value();
      
      int idx = index.toInt();
      
      if (idx >= 0 && idx < 10) {
        passwords[idx] = new_password;  // Update the password in the array
        
        // Update the CSV file with the new password
        updateCSVFile();
        
        request->send(200, "text/html", "<html><body>Password updated successfully! <a href='/'>Back</a></body></html>");
      } else {
        request->send(400, "text/plain", "Invalid index");
      }
    } else {
      request->send(403, "text/plain", "Forbidden: Invalid Token");
    }
  });
  
  // Start the server
  server.begin();
}

void loop() {
  // Nothing to do here for this web server
}
