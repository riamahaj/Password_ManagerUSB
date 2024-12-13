#include "functions.h"
#include <CSV_Parser.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Bounce2.h>
#include "SimpleArduinoEncryption.h"
#include <SPI.h>
#include "SD.h"

#ifndef ARDUINO_USB_MODE
#error This ESP32 SoC has no Native USB interface
#elif ARDUINO_USB_MODE == 1
#warning This sketch should be used when USB is in OTG mode
void setup() {}
void loop() {}
#else

#include "USB.h"
#include "USBHIDKeyboard.h"
USBHIDKeyboard Keyboard;

File csvFile;

//struct to hold name and password
struct name_password{
  char *Name;
  char *password;
};

typedef struct name_password Struct;


// Encryption keys
const byte ENCRYPTION_KEYS[] = {0xAA, 0xBB, 0xCC, 0xCB};
const size_t NUM_KEYS = sizeof(ENCRYPTION_KEYS) / sizeof(ENCRYPTION_KEYS[0]);

// Initialize encryption object with keys
SimpleArduinoEncryption encryption(ENCRYPTION_KEYS, NUM_KEYS);

CSV_Parser en(/*format*/ "s", /*has_header*/ false);
CSV_Parser de(/*format*/ "s--s-", /*has_header*/ false);

// INSTANTIATE A Button OBJECT FROM THE Bounce2 NAMESPACE
Bounce2::Button button1 = Bounce2::Button();
Bounce2::Button button2 = Bounce2::Button();
Bounce2::Button button3 = Bounce2::Button();
Bounce2::Button button4 = Bounce2::Button();

char **en_hex;
char **Name;
char **password;

extern int buttonPin1;
extern int buttonPin2;
extern int buttonPin3;
extern int buttonPin4;

extern Adafruit_SSD1306 display;


Struct result;



Struct decrypt_display(char *encryptedHex, int row)
{ 
  Struct s;

  // Calculate buffer length and allocate memory
  size_t length = strlen(encryptedHex) / 2;
  byte message[length + 1];

  // Convert hexadecimal string to bytes
  SimpleArduinoEncryption::hexToBytes(encryptedHex, message, length);

  // Print encrypted data in hex format
  Serial.print("Encrypted String (Hex): ");
  SimpleArduinoEncryption::printHex(message, length);

  // Decrypt the message
  encryption.decrypt((char*)message);
  String decryptedString = String((char*)message);

  Serial.print("Decrypted String: ");
  Serial.println(decryptedString);

  de << decryptedString;
  de.parseLeftover();

  //set 0th column as name, 3th column as password
  Name = (char**)de[0];
  password = (char**)de[3];

  s.Name = Name[row];
  s.password = password[row];
  return s;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  setup_sd();
  
  button1.attach(buttonPin1 ,INPUT_PULLUP ); 
  button2.attach(buttonPin2 ,INPUT_PULLUP ); 
  button3.attach(buttonPin3, INPUT_PULLUP);
  button4.attach(buttonPin4, INPUT_PULLUP);
  
  // DEBOUNCE INTERVAL IN MILLISECONDS
  button1.interval(10);
  button2.interval(5); 
  button3.interval(10);
  button4.interval(10);
  
  // INDICATE THAT THE LOW STATE CORRESPONDS TO PHYSICALLY PRESSING THE BUTTON
  button1.setPressedState(LOW); 
  button2.setPressedState(LOW);
  button3.setPressedState(LOW);
  button4.setPressedState(LOW);
  
  Keyboard.begin();
  USB.begin();

  setup_screen(); 

    // Open the CSV file for reading
  csvFile = SD.open("/encrypt_pass.csv", FILE_READ);

    // Temporary storage for processed lines
  String newContent = "";
  
  while (csvFile.available()) {
    // Read a line from the CSV file
    String line = csvFile.readStringUntil('\n');

    // Check if the line contains any character values
    if (containsCharacter(line)) {
      // Encrypt the line
      String encryptedLine = encryptLine(line);
      newContent += encryptedLine + "\n"; // Append the encrypted line
    } else {
      // If no character, keep the line as is
      newContent += line + "\n";
    }
  }

  csvFile.close();

  // Reopen the file for writing
  csvFile = SD.open("/encrypt_pass.csv", FILE_WRITE);

  // Write the updated content back to the file
  csvFile.print(newContent);
  csvFile.close();

  Serial.println("File processing complete.");


  //csv parser will read the encrypted file and store it internally in one "block"
  if (en.readSDfile("/encrypt_pass.csv")) {
    
    en_hex=(char**)en[0];

    
    // output parsed values (allows to check that the file was parsed correctly)
    en.print(); // assumes that "Serial.begin()" was called before (otherwise it won't work)
    
  } else {
  Serial.println("ERROR: File called '/file.csv' does not exist...");
  }
  
  display.fillScreen(SSD1306_WHITE);
  display.display();
}
// button variables
int buttonState1;         // variable for reading the pushbutton status
int buttonState2;
int lastState1 = LOW;

int counter = 0;
int i = 0;

void loop() {

  // read the state of the pushbutton value:
  button1.update();
  button2.update();
  button3.update();
  button4.update();

  // check if the pushbutton is pressed. If it is, the buttonState is LOW:
  if (button1.pressed()) {
    //button pressed
    //increment name/password field in CSV file
    Serial.println("button press");
    result = decrypt_display(en_hex[counter],counter);

    if (counter < en.getRowsCount()-1 && counter >= 0)
    {
      ++counter;
    }
    else {
      counter = 0;
    }
  }
  
  else
  {
    //display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(20,10);
    display.setTextSize(2);
    display.println(result.Name);
    show();
    Serial.println(result.Name);
  }
  
    
  if (button2.pressed()){
    //button2 pressed
    //output the password field to PC
    Keyboard.print(result.password);
    delay(25);
    Keyboard.write(KEY_RETURN);
  }
  
  //increment row by 5
  if (button3.pressed())
  {
    Serial.println("button press");

    if (counter < en.getRowsCount()-1 && counter >= 0)
    {
      counter = counter + 2;
    }
    else {
      counter = 0;
    }
    result = decrypt_display(en_hex[counter],counter);
  }
  //decrement row
  if (button4.pressed())
  {
    Serial.println("button press");

    if (counter < en.getRowsCount()-1 && counter > 0)
    {
      counter = counter - 1;
    }
    else {
      counter = 0;
    }
    result = decrypt_display(en_hex[counter],counter);
  }
  
}
//checks if there are characters in the line
bool containsCharacter(const String &line) {
  for (size_t i = 0; i < line.length(); i++) {
    char c = line[i];
    // Check if the character is alphabetic but not a valid hexadecimal character
    if (isAlpha(c) && !isHexadecimalDigit(c)) {
      return true;
    }
  }
  return false;
}

// Helper function to check if a character is a valid hexadecimal digit
bool isHexadecimalDigit(char c) {
  return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

// Placeholder encryption function
String encryptLine(String messageString) {
  // String to encrypt
  Serial.print("Original String: ");
  Serial.println(messageString);

  // Convert String to char array
  size_t length = messageString.length();
  char message[length + 1];
  messageString.toCharArray(message, sizeof(message));

  // Encrypt message
  encryption.encrypt(message);

  // Convert encrypted byte array to hexadecimal string
  char hexStr[length * 2 + 1];
  SimpleArduinoEncryption::bytesToHex((const byte*)message, length, hexStr);

  Serial.print("Encrypted String (Hex): ");
  Serial.println(hexStr);

  return hexStr;
}
#endif
