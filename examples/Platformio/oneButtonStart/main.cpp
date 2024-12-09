#include <Arduino.h>
#include "LogicLoop.h"

PLC plc(50);                        // PLC met een scan-cyclus interval van 50 ms
Input startKnop(I0_0, ACTIVE_HIGH); // Normal  Open  input op I0_0 (ACTIVE_HIGH)
Input stopKnop(I0_1, ACTIVE_LOW);   // Normal Closed input op I0_1 (ACTIVE_LOW)
Output uitgang(Q0_1);               // Output op Q0_1

void setup_plc() {
  // Inputs
    plc.addInput(&startKnop);
    plc.addInput(&stopKnop);

    // Outputs
    plc.addOutput(&uitgang);
}

void setup() {
    Serial.begin(9600);
    setup_plc();
    
    uitgang.setStatus(OFF);
    Serial.println(F("PLC running..."));
}

void loop() {
  if (plc.update()) {
    uitgang.setState(!stopKnop.getState() && (startKnop.getState() || uitgang.getState()));
    
    if (startKnop.getState()) {
      Serial.println(F("P"));
    } else {
      Serial.println(F("R"));
    }
    
    if (startKnop.isRising()) {
      Serial.println(F("Start ingedrukt"));
    }
    if (stopKnop.isRising()) {
      Serial.println(F("Stop ingedrukt"));
    }
    if (startKnop.isFalling()) {
      Serial.println(F("Start losgelaten"));
    }
    if (stopKnop.isFalling()) {
      Serial.println(F("Stop losgelaten"));
    }
  }
}