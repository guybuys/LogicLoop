#include <Arduino.h>
#include "LogicLoop.h"

PLC plc(50);                       // PLC met een scan-cyclus interval van 100 ms
Input startKnop(I0_0, ACTIVE_HIGH); // Normal  Open  input op I0_0 (ACTIVE_HIGH)
Output uitgang(Q0_1);               // Output op Q0_1
PLCtimer first_pulse_on_timer(ON_DELAY);     // Timer
PLCtimer second_pulse_on_timer(ON_DELAY);
PLCtimer between_pulse_timer(OFF_DELAY);
PLCtimer reset_timer(ON_DELAY);
Counter counter(2); // Counter

void setup_plc() {
  // Inputs
    plc.addInput(&startKnop);

    // Outputs
    plc.addOutput(&uitgang);

    // Timers
    plc.addTimer(&first_pulse_on_timer);
    plc.addTimer(&second_pulse_on_timer);
    plc.addTimer(&between_pulse_timer);
    plc.addTimer(&reset_timer);
    // Timer settings
    first_pulse_on_timer.set_pt(1000);
    second_pulse_on_timer.set_pt(2000);
    between_pulse_timer.set_pt(2000);
    reset_timer.set_pt(4000);

    // Counter settings
    counter.reset();
}

void setup() {
    Serial.begin(9600);
    setup_plc();
    
    uitgang.setStatus(OFF);
    Serial.println(F("PLC running..."));
}

void loop() {
  if (plc.update()) {
    if(!uitgang.getState()){
      // Machine staat uit
      reset_timer.processInput(!startKnop.getState());
      if(reset_timer.isRising()) {
        counter.reset();
      }
      if (!between_pulse_timer.getState()) {
        if(startKnop.getState()) {
          first_pulse_on_timer.processInput(true);
          between_pulse_timer.processInput(true);
          counter.increment();
        }
      }
      if(between_pulse_timer.getState() && (counter.getCount() == 1) && !first_pulse_on_timer.getState() && startKnop.getState()) {
        counter.increment();
      }
      if (counter.getCount() == 2) {
        second_pulse_on_timer.processInput(startKnop.getState());
        if(second_pulse_on_timer.isRising()) {
          uitgang.setState(true);
        }
      }
    } else {
      // Machine staat aan
      if(startKnop.isFalling()) {
        counter.reset();
      }
      if((counter.getCount() == 0) && startKnop.isRising()) {
        uitgang.setState(false);
      }
    }
  }
}
