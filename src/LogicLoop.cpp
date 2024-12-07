#include "LogicLoop.h"

// Input Klasse Implementatie
Input::Input(uint8_t pin, InputType type) : _pin(pin), _state(false), _lastState(false), _isActiveLow(type == ACTIVE_LOW) {
    pinMode(pin, INPUT);
}

Input::~Input() {
    // Lege destructor
}

void Input::update() {
    bool rawState = digitalRead(_pin);
    bool newState = _isActiveLow ? !rawState : rawState;
    _lastState = _state; // Update _lastState voordat _state wordt bijgewerkt
    _state = newState;
}

bool Input::getState() {
    return _state;
}

bool Input::isRising() {
    return (_state && !_lastState);
}

bool Input::isFalling() {
    return (!_state && _lastState);
}

// Output Klasse Implementatie
Output::Output(uint8_t pin) : _pin(pin), _status(OFF), _lastToggleTime(0), _blinkState(false), _blinkCount(0), _currentBlinkCount(0) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
}

Output::~Output() {
    // Lege destructor
}

bool Output::getState() {
    return _blinkState;
}

void Output::setState(bool state, bool immediate) {
    if (immediate) {
        if (state == true) {
            if (_status != ON) {
                setStatus(ON);
            }
        } else {
            if (_status != OFF) {
                setStatus(OFF);
            }
        }
    } else {
        _pendingState = state;
        _stateChanged = true;
    }
}

void Output::setStatus(OutputStatus newStatus) {
    if (_status != newStatus) {
        _status = newStatus;
        _lastToggleTime = millis();
        _blinkState = true; // Start with LED on
        if (_status == OFF) {
            digitalWrite(_pin, LOW);
            _blinkState = false;
        } else {
            digitalWrite(_pin, HIGH);
            _blinkState = true;
        }
        _blinkCount = 0;        // Default setting
        _currentBlinkCount = 0; // Reset blink count
    }
}

void Output::setBlinkCount(int count) {
    _blinkCount = count;
    _currentBlinkCount = 0;
}

void Output::update() {
    if (_stateChanged) {
        setState(_pendingState, true);
        _stateChanged = false;
    }
    
    unsigned long currentTime = millis();
    switch (_status) {
        case OFF:
            digitalWrite(_pin, LOW);
            _blinkState = false;
            break;
        case ON:
            digitalWrite(_pin, HIGH);
            _blinkState = true;
            break;
        case SLOW:
            if (currentTime - _lastToggleTime >= 500) { // Toggle every 500ms
                _blinkState = !_blinkState;
                digitalWrite(_pin, _blinkState ? HIGH : LOW);
                _lastToggleTime = currentTime;
                if (_blinkState == false && _blinkCount > 0) {
                    _currentBlinkCount++;
                    if (_currentBlinkCount >= _blinkCount) {
                        setStatus(OFF);
                    }
                }
            }
            break;
        case FAST:
            if (currentTime - _lastToggleTime >= 100) { // Toggle every 100ms
                _blinkState = !_blinkState;
                digitalWrite(_pin, _blinkState ? HIGH : LOW);
                _lastToggleTime = currentTime;
                if (_blinkState == false && _blinkCount > 0) {
                    _currentBlinkCount++;
                    if (_currentBlinkCount >= _blinkCount) {
                        setStatus(OFF);
                    }
                }
            }
            break;
        case LONG:
            if (currentTime - _lastToggleTime >= 1000) { // Hold for 1 second
                setStatus(OFF);
            }
            break;
    }
}

// PLCtimer Klasse Implementatie
PLCtimer::PLCtimer(TimerType type) : _mode(type), _in(false), _prev_in(false), _q(false), _prev_q(false), _running(false), _pt(0), _et(0), _last(0), _input(nullptr), _output(nullptr) {}

PLCtimer::~PLCtimer() {}

bool PLCtimer::getState() {
    return _q;
}

unsigned long PLCtimer::get_et() {
    return _et;
}

unsigned long PLCtimer::get_pt() {
    return _pt;
}

bool PLCtimer::isRising() {
    return (_q && !_prev_q);
}

bool PLCtimer::isFalling() {
    return (!_q && _prev_q);
}

void PLCtimer::set_pt(unsigned long program_time) {
    _pt = program_time;
}

void PLCtimer::processInput(bool input) {
    _in = input;
    _prev_q = _q;
    switch (_mode) {
        case ON_DELAY:
            if (_in && !_prev_in) {
                _start();
            }
            if (_in && _running) {
                _et = millis() - _last;
                if (_et >= _pt) {
                    _q = true;
                    _running = false;
                }
            } else {
                _q = _in;
            }
            break;
        case OFF_DELAY:
            if (!_in && _prev_in) {
                _start();
            }
            if (!_in && _running) {
                _et = millis() - _last;
                if (_et >= _pt) {
                    _q = false;
                    _running = false;
                }
            } else {
                _q = _in;
            }
            break;
        case PULSE_TIMER:
            if (_in && !_prev_in) {
                _start();
                _q = true;
            }
            if (_running) {
                _et = millis() - _last;
                if (_et >= _pt) {
                    _q = false;
                    _running = false;
                }
            }
            break;
    }
    _prev_in = _in;
}

void PLCtimer::update() {
    if (_input) {
        processInput(_input->getState());
    } 
    /*
    else {
        processInput(_in);
    }
    */

    if (_output) {
        _output->setState(_q, true);
    }
}

void PLCtimer::setInput(Input* input) {
    _input = input;
}

void PLCtimer::setOutput(Output* output) {
    _output = output;
}

void PLCtimer::_start() {
    _last = millis();
    _running = true;
}

bool PLCtimer::isRunning() {
    return _running;
}

// Counter Klasse Implementatie
Counter::Counter(int preset) : _count(0), _preset(preset), _reset(false), _prevReset(false) {}

Counter::~Counter() {}

void Counter::increment() {
    _count++;
}

void Counter::decrement() {
    _count--;
}

int Counter::getCount() {
    return _count;
}

void Counter::reset() {
    _count = 0;
}

void Counter::setPreset(int preset) {
    _preset = preset;
}

bool Counter::isPresetReached() {
    return _count >= _preset;
}

// PLC Klasse Implementatie
PLC::PLC(unsigned long scanCycleInterval) 
    : _scanCycleInterval(scanCycleInterval),
     _lastScanCycleTime(millis()),
     _executionTime(0), 
     _totalExecutionTime(0), 
     _executionCount(0), 
     _worstCaseExecutionTime(0), 
     _maxHealthyScanCycleTime((3 * scanCycleInterval) >> 2) {
}

void PLC::addInput(Input* input) {
    _inputs.push_back(input);
}

void PLC::addOutput(Output* output) {
    _outputs.push_back(output);
}

void PLC::addTimer(PLCtimer* timer) {
    _timers.push_back(timer);
}

void PLC::addCounter(Counter* counter) {
    _counters.push_back(counter);
}

void PLC::updateCounters() {
    for (size_t i = 0; i < _counters.size(); ++i) {
        // Update logica voor counters indien nodig
    }
}

bool PLC::update() {
    unsigned long currentTime = millis();
    if (currentTime - _lastScanCycleTime >= _scanCycleInterval) {
        _lastScanCycleTime = currentTime;
    
        // Update alle inputs
        for (size_t i = 0; i < _inputs.size(); ++i) {
            _inputs[i]->update();
        }

        // Update alle outputs
        for (size_t i = 0; i < _outputs.size(); ++i) {
            _outputs[i]->update();
        }

        // Update alle timers
        for (size_t i = 0; i < _timers.size(); ++i) {
            _timers[i]->update();
        }

        // Update alle counters
        updateCounters();

        _executionTime = currentTime;
        _measureExecutionTime = true;
        return true;
    }
    if (_measureExecutionTime) {
        _totalExecutionTime += currentTime - _executionTime;
        _executionCount++;
        if (currentTime - _executionTime > _worstCaseExecutionTime) {
            _worstCaseExecutionTime = currentTime - _executionTime;
        }
        _measureExecutionTime = false;
    }
    return false;
}

unsigned long PLC::getAverageExecutionTime() {
    if (_executionCount == 0) return 0;
    return _totalExecutionTime / _executionCount;
}

unsigned long PLC::getWorstCaseExecutionTime() {
    return _worstCaseExecutionTime;
}

void PLC::resetWorstCaseExecutionTime() {
    _worstCaseExecutionTime = 0;
}

bool PLC::isHealthy() {
    if (this->getAverageExecutionTime() <= _maxHealthyScanCycleTime) {
        return true;
    }
    return false;
}

#if defined(MDUINO_21_PLUS)
uint16_t PLC::readInputsCompact() {
    uint16_t inputs = 0;

    // Lees PORTA (bits 0-4)
    inputs |= (PINA & 0x1F) << 0; // Bits 0-4

    // Lees PORTD (bits 2-3)
    inputs |= ((PIND & 0x0C) >> 2) << 5; // Bits 5-6

    // Lees PORTF (bits 0-5)
    inputs |= (PINF & 0x3F) << 7; // Bits 7-12

    return inputs;
}

uint8_t PLC::readOutputsCompact() {
    uint8_t outputStates = 0;

    // Lees de eerste 5 bits van PORTC (Q0_0 tot Q0_4)
    outputStates |= (PORTC & 0b00011111);

    // Lees de laatste 3 bits van PORTD (Q0_5 tot Q0_7)
    outputStates |= (PORTD & 0b11100000);

    return outputStates;
}

uint32_t PLC::readIOsCompact() {
    // Lees de status van outputs en inputs
    uint8_t outputStates = this->readOutputsCompact();
    uint16_t inputStates = this->readInputsCompact();

    // Combineer de waarden in een 32-bit variabele
    uint32_t ioStates = 0;
    ioStates |= (uint32_t)outputStates;         // Voeg outputs toe (bits 0–7)
    ioStates |= ((uint32_t)inputStates << 8);  // Voeg inputs toe (bits 8–23)

    return ioStates;
}

#endif