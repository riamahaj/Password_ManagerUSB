#include "functions.h"
#include <CSV_Parser.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Bounce2.h>
#include "SimpleArduinoEncryption.h"
#include <SPI.h>
#include "SD.h"
/**/#include <string>

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

//struct to hold name and password
struct name_password{
  char *Name;
  char *password;
/**/char *strength;
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

char **en_hex;
char **Name;
char **password;
/**/char **strength

extern int buttonPin1;
extern int buttonPin2;

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

  ////////DanielBreakingStuffStart/////////
    int n = password.length();
    int Strength = 0;
    string strength = "";
// Initialize varibles and define special characters
    bool hasLower = false, hasUpper = false;
    bool hasNum = false, specialChar = false;
    string normalChars = "abcdefghijklmnopqrstu"
        "vwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890 ";

// Check what character types are present
    for (int i = 0; i < n; i++) {
        if (islower(password[i]))
            hasLower = true;
        if (isupper(password[i]))
            hasUpper = true;
        if (isdigit(password[i]))
            hasNum = true;
        size_t special = password.find_first_not_of(normalChars);
        if (special != string::npos)
            specialChar = true;
    }

// Algorithm to score password strength
    if((n >= 8) == true) 
	Strength += 1;
    if(hasLower == true)
	Strength += 1;
    if(hasUpper == true)
	Strength += 1;
    if(specialChar == true)
	Strength += 1;
    if(hasNum == true)
	Strength += 1;
// Print stars #1
    for (int i = 0; i < Strength; ++i)
        Serial.print("*");
// Print stars #2
    if (Strength == 1) {
        strength = "*";
    } else if (Strength == 2) {
        strength = "**";
    } else if (Strength == 3) {
        strength = "***";
    } else if (Strength == 4) {
        strength = "****";
    } else if (Strength == 5) {
        strength = "*****";
}    
  ////////DanielBreakingStuffEnd/////////
  s.Name = Name[row];
  s.password = password[row];
  s.strength = strength;
  return s;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  setup_sd();
  //setup_button();
  
  button1.attach(buttonPin1 ,INPUT_PULLUP ); 
  button2.attach(buttonPin2 ,INPUT_PULLUP ); 
  
  // DEBOUNCE INTERVAL IN MILLISECONDS
  button1.interval(10);
  button2.interval(5); 
  
  // INDICATE THAT THE LOW STATE CORRESPONDS TO PHYSICALLY PRESSING THE BUTTON
  button1.setPressedState(LOW); 
  button2.setPressedState(LOW);

  Keyboard.begin();
  USB.begin();

  setup_screen(); 

  // The line below (readSDfile) wouldn't work if SD.begin wasn't called before.
  // readSDfile can be used as conditional, it returns 'false' if the file does not exist.
  if (en.readSDfile("/encrypt_pass.csv")) {
    
    en_hex=(char**)en[0];

    
    // output parsed values (allows to check that the file was parsed correctly)
    en.print(); // assumes that "Serial.begin()" was called before (otherwise it won't work)
    
  } else {
  Serial.println("ERROR: File called '/file.csv' does not exist...");
  }
  //result = decrypt_display(en_hex[0],0);

}
// button variables
int buttonState1;         // variable for reading the pushbutton status
int buttonState2;
int lastState1 = LOW;

extern Adafruit_SSD1306 display;
int counter = 0;

void loop() {
  
  // read the state of the pushbutton value:
  button1.update();
  button2.update();

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
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(20,10);
    display.setTextSize(2);
    display.println(result.Name);
/**/display.println(result.strength);
    show();
    Serial.println(result.Name);
/**/display.println(result.strength);
  }
    
  if (button2.pressed()){
    //button2 pressed
    //output the password field to PC
    Keyboard.print(result.password);
    delay(25);
    Keyboard.write(KEY_RETURN);
  }
  

}
#endif
