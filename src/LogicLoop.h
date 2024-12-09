#ifndef LOGICLOOP_H
#define LOGICLOOP_H

//#include <vector>
#include <Arduino.h>
//#include <ArduinoSTL.h>
#include "SimpleVector.h"

enum InputType {
    ACTIVE_HIGH, // Normally Open
    ACTIVE_LOW  // Normally Closed
};

enum OutputStatus {
    OFF,
    SLOW,
    FAST,
    ON,
    LONG
};

enum TimerType {
    PULSE_TIMER,
    ON_DELAY,
    OFF_DELAY
};

class Input {
private:
    uint8_t _pin;
    bool _state;
    bool _lastState;
    bool _isActiveLow;

public:
    Input(uint8_t pin, InputType type = ACTIVE_HIGH);
    ~Input(); // Destructor

    void update();
    bool getState();
    bool isRising();
    bool isFalling();
};

class Output {
private:
    uint8_t _pin;
    OutputStatus _status;
    unsigned long _lastToggleTime;
    bool _blinkState;
    int _blinkCount;
    int _currentBlinkCount;
    bool _pendingState;
    bool _stateChanged;

public:
    Output(uint8_t pin);
    ~Output(); // Destructor

    bool getState();
    void setStatus(OutputStatus status);
    void setState(bool state, bool immediate = false);
    void setBlinkCount(int count);
    void update();
};

class PLCtimer {
public:
    PLCtimer(TimerType type);   // Constructor
    ~PLCtimer();                // Destructor

    bool getState();            // Indicates time has elapsed
    unsigned long get_et();
    unsigned long get_pt();
    bool isRising();
    bool isFalling();
    void set_pt(unsigned long program_time);
    void processInput(bool input);
    void update();
    bool isRunning();

    void setInput(Input* input);
    void setOutput(Output* output);

private:
    TimerType _mode;
    bool _in;
    bool _prev_in;
    bool _q;
    bool _prev_q;
    bool _running;
    unsigned long _pt;
    unsigned long _et;
    unsigned long _last;
    Input* _input;
    Output* _output;

    void _start();
};

class Counter {
private:
    int _count;
    int _preset;
    bool _reset;
    bool _prevReset;
    bool _prevIncrement;
    bool _prevDecrement;

public:
    Counter(int preset);
    ~Counter();

    void increment();
    void increment(bool condition);
    void decrement();
    void decrement(bool condition);
    void reset();
    void reset(bool condition);
    int getCount();
    void setPreset(int preset);
    bool isPresetReached();
    bool isZero();

    void incrementOnCondition(bool condition);
    void decrementOnCondition(bool condition);
    void resetOnCondition(bool condition);

};

class PLC {
private:
    unsigned long _scanCycleInterval;
    unsigned long _lastScanCycleTime;

    unsigned long _executionTime;
    unsigned long _totalExecutionTime;
    unsigned long _executionCount;
    unsigned long _worstCaseExecutionTime;
    unsigned long _maxHealthyScanCycleTime;
    bool _measureExecutionTime;

    SimpleVector<Input*> _inputs;
    SimpleVector<Output*> _outputs;
    SimpleVector<PLCtimer*> _timers;
    SimpleVector<Counter*> _counters;

public:
    PLC(unsigned long scanCycleInterval);

    void addInput(Input* input);
    void addOutput(Output* output);
    void addTimer(PLCtimer* timer);
    void addCounter(Counter* counter);
    void updateCounters();
    bool update();

    unsigned long getAverageExecutionTime();
    unsigned long getWorstCaseExecutionTime();
    void resetWorstCaseExecutionTime();
    bool isHealthy();

#if defined(MDUINO_21_PLUS)
    uint32_t readIOsCompact();
    uint8_t readOutputsCompact();
    uint16_t readInputsCompact();
#endif

};

#endif