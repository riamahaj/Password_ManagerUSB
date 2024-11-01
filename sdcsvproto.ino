#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <CSV_Parser.h>



void setup() {
  Serial.begin(115200);
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



  CSV_Parser cp(/*format*/ "ss", /*has_header*/ true);

  // The line below (readSDfile) wouldn't work if SD.begin wasn't called before.
  // readSDfile can be used as conditional, it returns 'false' if the file does not exist.
  if (cp.readSDfile("/manage.csv")) {
    char **strings1 = (char**)cp["Name"];    
    char **strings2 = (char**)cp["Password"];
  
    if (strings1 && strings2) {
        for(int row = 0; row < cp.getRowsCount(); row++) {
          Serial.print(row, DEC);
          Serial.print(". Name = ");
          Serial.println(strings1[row]);
          Serial.print(row, DEC);
          Serial.print(". Password = ");
          Serial.println(strings2[row]);
          }
    } else {
      Serial.println("ERROR: At least 1 of the columns was not found, something went wrong.");
    }
    
    // output parsed values (allows to check that the file was parsed correctly)
    cp.print(); // assumes that "Serial.begin()" was called before (otherwise it won't work)
    
  } else {
    Serial.println("ERROR: File called '/file.csv' does not exist...");
  }   
}

void loop() {
  // put your main code here, to run repeatedly:

}
