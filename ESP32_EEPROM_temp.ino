#include <EEPROM.h>
void setup() 
{
  EEPROM.begin(512);
  clearEEPROM(128,256);
}

void loop() 
{
}

void clearEEPROM(int st, int ed) {
    for (int i = st; i < ed; i++) {
        EEPROM.write(i, 0);
    }
    EEPROM.commit();
    //Serial.println("EEPROM Cleared!");
    Serial.printf("clearEEPROM (%d, %d).\n", st, ed);
}
