#include "functions.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <CSV_Parser.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>



#define REASSIGN_PINS
int sck = 42;
int miso = 1;
int mosi = 2;
int cs = 41;

void setup_sd()
{
  while (!Serial) {
    delay(10);
  }
  
#ifdef REASSIGN_PINS
  SPI.begin(sck, miso, mosi, cs);
  if (!SD.begin(cs)) {
#else
  if (!SD.begin()) {
#endif
    Serial.println("Card Mount Failed");
    return;
  }
   
}

// constants won't change. They're used here to set pin numbers:
int buttonPin1 = 13;     // the number of the pushbutton pin
int buttonPin2 = 10;
int buttonPin3 = 15;
int buttonPin4 = 14;

// variables will change:
//extern int buttonState = 0;         // variable for reading the pushbutton status

void setup_button()
{
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
}

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

void setup_screen()
{
  //SDA(blue), SCL(yellow)
  Wire.begin(4,5);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  //delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Draw a single pixel in white
  display.println("Loading...");

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  //delay(2000);
}

void show()
{
  display.display();
  //delay(2000);
  display.fillScreen(SSD1306_BLACK);
}
