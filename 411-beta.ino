#include "functions.h"
#include <CSV_Parser.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Bounce2.h>

// INSTANTIATE A Button OBJECT FROM THE Bounce2 NAMESPACE
Bounce2::Button button1 = Bounce2::Button();
Bounce2::Button button2 = Bounce2::Button();

char **strings1;
char **strings2;
extern int buttonPin1;
extern int buttonPin2;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  setup_sd();
  //setup_button();
  button1.attach(buttonPin1 ,INPUT_PULLUP ); 
  button2.attach(buttonPin2 ,INPUT_PULLUP ); 
  // DEBOUNCE INTERVAL IN MILLISECONDS
  button1.interval(5);
  button2.interval(5); 
  // INDICATE THAT THE LOW STATE CORRESPONDS TO PHYSICALLY PRESSING THE BUTTON
  button1.setPressedState(LOW); 
  button2.setPressedState(LOW);

  setup_screen();    

  CSV_Parser cp(/*format*/ "ss", /*has_header*/ true);

  // The line below (readSDfile) wouldn't work if SD.begin wasn't called before.
  // readSDfile can be used as conditional, it returns 'false' if the file does not exist.
  if (cp.readSDfile("/manage.csv")) {
    strings1 = (char**)cp["Name"];    
    strings2 = (char**)cp["Password"];
    // output parsed values (allows to check that the file was parsed correctly)
    cp.print(); // assumes that "Serial.begin()" was called before (otherwise it won't work)
    
  } else {
  Serial.println("ERROR: File called '/file.csv' does not exist...");
  }

}

// button variables
int buttonState1;         // variable for reading the pushbutton status
int buttonState2;
int lastState1 = LOW;

extern Adafruit_SSD1306 display;
int counter = 0;

void loop() {
  
  // read the state of the pushbutton value:
  //buttonState1 = digitalRead(buttonPin1);
  //buttonState2 = digitalRead(buttonPin2);

  button1.update();
  button2.update();

  // check if the pushbutton is pressed. If it is, the buttonState is LOW:
  if (button1.pressed()) {
    //button pressed
    //increment name/password field in CSV file
    Serial.println("button press");
    if (counter < 2 && counter >= 0)
    {
      ++counter;
    }
    else{
      counter = 0;
    }
  } 
  //button not pressed, display the account name on OLED screen
  else {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(20,10);
    display.setTextSize(2);
    display.println(strings1[counter]);
    show();
    Serial.println(strings1[counter]);
  }
  
  
  if (button2.pressed()){
    //button2 pressed
    //output the password field to PC
  }
  else {}
  

}
