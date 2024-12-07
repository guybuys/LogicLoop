# LogicLoop
## _Library Documentatie_

## Overzicht

De `LogicLoop` library biedt een eenvoudige implementatie van een PLC (Programmable Logic Controller) voor Arduino. De library ondersteunt inputs, outputs en timers met verschillende modi zoals `ON_DELAY`, `OFF_DELAY` en `PULSE_TIMER`. 

## Klassen en Methoden
### Input Klasse
De `Input` klasse vertegenwoordigt een digitale input op een specifieke pin.

####  Constructor:

```cpp
Input(uint8_t pin, InputType type = NO);
```
- `pin`: De pin waarop de input is aangesloten.
- `type`: Het type input (`NO` voor Normally Open, `NC` voor Normally Closed).

#### Methoden:

```cpp
bool getState();
bool isRising();
bool isFalling();
```
- `getState()`: Geeft de huidige toestand van de input terug.
- `isRising()`: Geeft `true` terug tijdens de cyclus wanneer de input net is ingeschakeld.
- `isFalling()`: Geeft `true` terug tijdens de cyclus wanneer de input net is uitgeschakeld.

### Output Klasse
De `Output` klasse vertegenwoordigt een digitale output op een specifieke pin.

#### Constructor:

```cpp
Output(uint8_t pin);
```
- `pin`: De pin waarop de output is aangesloten.

#### Methoden:

```cpp
bool getState();
void setStatus(OutputStatus status);
void setState(bool state, bool immediate = false);
void setBlinkCount(int count);
```
- `getState()`: Geeft de huidige toestand van de output terug.
- `setStatus(OutputStatus status)`: Stelt de status van de output in (`OFF`, `SLOW`, `FAST`, `ON`, `LONG`).
- `setState(bool state, bool immediate = false)`: Stelt de toestand van de output in. Als `immediate` `true` is, wordt de toestand onmiddellijk gewijzigd; anders wordt de wijziging toegepast tijdens de volgende `update`.
- `setBlinkCount(int count)`: Stelt het aantal keren in dat de output moet knipperen.

### PLCtimer Klasse
De `PLCtimer` klasse vertegenwoordigt een timer met verschillende modi.

#### Constructor:
```cpp
PLCtimer(TimerType type);
```
- `type`: Het type timer (`PULSE_TIMER`, `ON_DELAY`, `OFF_DELAY`).
#### Methoden:

```cpp
bool getState();
unsigned long get_et();
unsigned long get_pt();
bool rising();
bool falling();
void set_pt(unsigned long program_time);
void processInput(bool input);
void setInput(Input* input);
void setOutput(Output* output);
```

- `getState()`: Geeft `true` of `false` terug afhankelijk van de toestand van de timer.
- `get_et()`: Geeft de verstreken tijd van de timer terug.
- `get_pt()`: Geeft de ingestelde tijd van de timer terug.
- `rising()`: Geeft `true` terug tijdens de eerste cyclus nadat de timer is gestart.
- `falling()`: Geeft `true` terug tijdens de eerste cyclus nadat de timer  de timer is gestopt.
- `set_pt(unsigned long program_time)`: Stelt de tijd in die de timer moet lopen voordat hij afgaat.
- `processInput(bool input)`: Verwerkt de ingangswaarde van de timer *(nodig wanneer er geen Input object gekoppeld is aan de timer)*.
- `setInput(Input* input)`: Koppelt een Input object aan de timer.
- `setOutput(Output* output)`: Koppelt een Output object aan de timer.

### Counter Klasse
De Counter klasse vertegenwoordigt een teller die kan worden gebruikt om gebeurtenissen te tellen en te controleren of een vooraf ingestelde waarde is bereikt.


#### Constructor:
```cpp
Counter(int preset);
```
#### Methoden:
```cpp
void increment();
void decrement();
int getCount();
void reset();
void setPreset(int preset);
bool isPresetReached();
```
- `increment()`: Verhoogt de teller met één.
- `decrement()`: Verlaagt de teller met één.
- `getCount()`: Retourneert de huidige waarde van de teller.
- `reset()`: Reset de teller naar nul.
- `setPreset(int preset)`: Stelt de vooraf ingestelde waarde van de teller in.
- `isPresetReached()`: Controleert of de teller de vooraf ingestelde waarde heeft bereikt.


### PLC Klasse
De `PLC` klasse beheert de inputs, outputs en timers.

#### Constructor:
```cpp
PLC(unsigned long interval);
```
- `interval`: Het scan-cyclus interval (in milliseconden) waarmee de `update` methode uitgevoerd wordt.
#### Methoden:
```cpp
void addInput(Input* input);
void addOutput(Output* output);
void addTimer(PLCtimer* timer);
bool update();
```

- `addInput(Input* input)`: Voegt een `Input` object toe aan de PLC.
- `addOutput(Output* output)`: Voegt een `Output` object toe aan de PLC.
- `addTimer(PLCtimer* timer)`: Voegt een `PLCtimer` object toe aan de PLC.
- `update()`: Werkt de toestand van alle inputs, outputs en timers bij. Deze methode moet periodiek worden aangeroepen (bijvoorbeeld in de `loop` functie). De methode geeft `true` terug nadat de toestand van de gekoppelde objecten bijgewerkt is en dit kan gebruikt worden om code ook periodiek uit te voeren.

### Output Klasse: `setState` vs `setStatus` methode 

Hier volgt een diepere uitleg over het verschil tussen de `setState` en `setStatus` methoden bij een `Output` object. Het is belangrijk dit klein verschil goed te begrijpen.

#### `setState` Methode
De setState methode wordt gebruikt om de toestand van de output direct of uitgesteld te wijzigen door een `boolean` mee te geven. Deze methode heeft een optionele parameter `immediate` die bepaalt of de wijziging onmiddellijk moet plaatsvinden of tijdens de volgende scan-cyclus (bij aanroep van de update methode). Wanneer de `immediate` parameter niet expliciet gegeven wordt, staat hij op `false` zodat de update bij de eerstvolgende scan zal plaatsvinden. Op die manier loopt het gelijk met eventuele timers die ook beinvloed worden door de toestand van de uitgang. 

#### Signatuur:
```cpp
void setState(bool state, bool immediate = true);
```
##### Parameters:

- `state`: De nieuwe toestand van de output (`boolean`: `true` voor aan, `false` voor uit).
- `immediate`: Een optionele parameter die bepaalt of de wijziging onmiddellijk moet plaatsvinden (`true`) of tijdens de volgende cyclus (`false`). De standaardwaarde, indien niet gespecifieerd, is `false`.

##### Gebruik:

Je wil een `Output` aan of uit zetten door middel van een `boolean` (`true` of `false`)

#### `setStatus` Methode

De `setStatus` methode wordt gebruikt om de status van de output in te stellen. De status bepaalt het gedrag van de output, zoals aan, uit, langzaam knipperen, snel knipperen of een lange puls.

#### Signatuur:
```cpp
void setStatus(OutputStatus status);
```
##### Parameters:

- `status`: De nieuwe status van de output. Dit kan een van de volgende waarden zijn:
    - `OFF`: De output is uit. Dit gedrag komt overeen met `setState(false)`
    - `SLOW`: De output knippert langzaam.
    - `FAST`: De output knippert snel.
    - `ON`: De output is aan.  Dit gedrag komt overeen met `setState(true)`
    - `LONG`: De output geeft een lange puls.

##### Gebruik:
Je wil een `Output` aan of uit zetten, laten knipperen of een lange puls laten geven. Dit kan niet rechtstreeks met een `boolean`.

De `setStatus` methode kan uitgebreidt worden met de methode `setBlinkCount` om een uitgang een bepaald aantal keer te laten knipperen:
```cpp
led.setStatus(FAST);
led.setBlinkCount(5);
```
In het bovenstaande voorbeeld zal de `output` met naam `led` 5 keer snel knipperen. Daarna gaat de `status` automatisch over naar `OFF`, wat overeenkomt met `state` `false`
De `setStatus` methode is dus vooral handig wanneer je gebruik wilt maken van knipper functies maar het is niet mogelijk om het type `boolean` mee te geven met deze functie.

### LogicLoop library

#### Voorbeeld
Hier is een voorbeeld van hoe je de IS_PLC library kunt gebruiken in een Arduino sketch:

```cpp
#include <Arduino.h>
#include "IS_PLC.h"

PLC plc(200);               // PLC met een scan-cyclus van 200 ms
Input startKnop(2, NO);     // Normaal open input op pin 2 
Input stopKnop(3, NC);      // Normaal gesloten input op pin 3
Output rodeLamp(4);         // Output op pin 4
Output groeneLamp(5);       // Output op pin 5
PLCtimer timer1(ON_DELAY);  // ON_DELAY timer

void setup() {
    Serial.begin(9600);
    Serial.println(F("PLC programma"));

    // Voeg inputs en outputs toe aan de PLC
    plc.addInput(&startKnop);
    plc.addInput(&stopKnop);
    plc.addOutput(&rodeLamp);
    plc.addOutput(&groeneLamp);

    // Stel de preset time van de timer in
    timer1.set_pt(5000); // 5 seconden

    // Koppel de input en output aan de timer
    timer1.setInput(&startKnop);
    timer1.setOutput(&rodeLamp);

    // Voeg de timer toe aan de PLC
    plc.addTimer(&timer1);
}

void loop() {
    // Update de PLC logica
    if (plc.update()) {
        groeneLamp.setState(!stopKnop.getState() && (startKnop.getState() || groeneLamp.getState()); 
    }
}
```

## Addendum
- De library maakt gebruik van een eenvoudige vector-implementatie (`SimpleVector`) om afhankelijkheden van externe libraries te vermijden.
- Alle klassen beschikken over de methode `update()`. Deze werd echter bewust enkel vernoemd bij de `PLC` klasse omdat de gebruiker niet verondersteld wordt de `update()` methode voor de andere klassen op te roepen. Dit gebeurd immers automatisch in de `update()` methode van de `PLC` klasse wanneer de ojecten aan het `PLC` object gekoppeld werden. Het is echter perfect mogelijk om `Input`, `Output` of `Timer` objecten te gebruiken zonder een `PLC` object. In dat geval moet de `update()` methode van ieder object apart aangeroepen worden.
- **Compacte I/O Leesmethoden** voor visualisatie via USB poort (momenteel enkel voor MDUINO_21_PLUS):
    - uint32_t readIOsCompact(): Leest de status van de inputs en outputs in een compacte vorm.
    - uint8_t readOutputsCompact(): Leest de status van de outputs in een compacte vorm.
    - uint16_t readInputsCompact(): Leest de status van de inputs in een compacte vorm.
- Er zijn in de PLC klasse tools voorzien om de processortijd op te kunnen volgen:
    - `unsigned long getAverageExecutionTime()`: Retourneert de gemiddelde uitvoeringstijd van de scan-cyclus.
    - `unsigned long getWorstCaseExecutionTime()`: Retourneert de slechtste uitvoeringstijd van de scan-cyclus.
    - `void resetWorstCaseExecutionTime()`: Reset de slechtste uitvoeringstijd.
    - `bool isHealthy()`: Geeft `true` terug indien de gemiddelde uitvoeringstijd onder 75% van de scan-cyclus blijft.
- Wishlist voor nieuw features:
    - Function overloading gebruiken voor met setState van een output om zowel met bool als ON/OFF/SLOW/FAST/LONG parameters mee te geven als dit duidelijk is voor de gebruikers
    - Knipper en long tijd instelbaar maken met public method

