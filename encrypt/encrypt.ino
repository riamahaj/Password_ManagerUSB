#include "SimpleArduinoEncryption.h"

// Encryption keys
const byte ENCRYPTION_KEYS[] = {0xAA, 0xBB, 0xCC, 0xCB};
const size_t NUM_KEYS = sizeof(ENCRYPTION_KEYS) / sizeof(ENCRYPTION_KEYS[0]);

// Initialize encryption object with keys
SimpleArduinoEncryption encryption(ENCRYPTION_KEYS, NUM_KEYS);  
void encrypt_display(String messageString)
{
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
  
}

void setup() {
  
  Serial.begin(9600);
  String messageString = "Bank, , ,abc,";
  encrypt_display(messageString);
  messageString = "Instagram, , ,SupermanVsBatman:ElectricBungalow123:,";
  encrypt_display(messageString);
  messageString = "Snapchat, , ,password123,";
  encrypt_display(messageString);
  
}

void loop() {
  // No activity in loop
}
