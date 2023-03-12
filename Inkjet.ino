#include <AD9833.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <GravityTDS.h>
#include <OneWire.h> 
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 10
#define TdsSensorPin A0
#define FNC_PIN 49
#define rxPin A15
#define txPin A14
AD9833 gen(FNC_PIN);
SoftwareSerial mySerial =  SoftwareSerial(rxPin, txPin);
OneWire oneWire(ONE_WIRE_BUS); 
GravityTDS gravityTds;
DallasTemperature sensors(&oneWire);

//Variables
boolean auto_mode_state = false;
boolean fault_state = false;
boolean newData = false;
boolean timer_running = false;
boolean shutdown_in_progress = false;
const byte numChars = 5;
char receivedChars[numChars];
const byte clean_nozzle_pin = 9;
const byte vacuum_fault_pin = 8;
const byte ink_pressure_pin = 7;
const byte pump_pressure_pin = 6;
const byte vacuum_pin = 5;
const byte pump_vacuum_pin = 4;
const byte add_ink_pin = 3;
const byte add_makeup_pin = 2;
const byte air_pressure_sensor_pin = 15;
const byte vacuum_sensor_pin = 14;
const byte reservoir_lower_sensor_pin = 16;
const byte reservoir_upper_sensor_pin = 17;
const byte pump_lower_sensor_pin = 19;
const byte pump_upper_sensor_pin = 18;
const byte timer_lower_sensor_pin = 21;
const byte timer_upper_sensor_pin = 20;
int auto_mode_case = 0;
long previous_state_report_value = 0;
long state_report_value = 0;
unsigned int ink_pressure_value = 0;
unsigned int pump_pressure_value = 0;
unsigned int pump_vacuum_value = 0;
unsigned int add_ink_value = 0;
unsigned int add_makeup_value = 0;
unsigned int air_pressure_sensor_value = 0;
unsigned int vacuum_sensor_value = 0;
unsigned int reservoir_lower_sensor_value = 0;
unsigned int reservoir_upper_sensor_value = 0;
unsigned int pump_lower_sensor_value = 0;
unsigned int pump_upper_sensor_value = 0;
unsigned int timer_lower_sensor_value = 0;
unsigned int timer_upper_sensor_value = 0;
unsigned int fault_value = 0;
unsigned int vacuum_value = 0;
long clean_nozzle_value = 0;
unsigned int auto_mode_value = 0;
unsigned long start_timer_millis = 0;
unsigned long fault_millis = 0;
unsigned long ink_data_millis = 0;
unsigned long shutdown_millis = 0;
unsigned long ink_time = 0;
float tdsValue = 0;

//Setup
void setup() {
  gen.Begin();
  gen.ApplySignal(SINE_WAVE,REG0,46000);
  gen.EnableOutput(true);
  Serial.begin(9600);
  mySerial.begin(9600);
  mySerial.println("Connected...");
  mySerial.println("Printer ready...");
  mySerial.println("#" + String(state_report_value));
  sensors.begin();
  gravityTds.setPin(TdsSensorPin);
  gravityTds.setAref(5.0);
  gravityTds.setAdcRange(1024);
  gravityTds.begin();
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  pinMode(clean_nozzle_pin, OUTPUT);
  pinMode(vacuum_pin, OUTPUT);
  pinMode(ink_pressure_pin, OUTPUT);
  pinMode(pump_pressure_pin, OUTPUT);
  pinMode(pump_vacuum_pin, OUTPUT);
  pinMode(add_ink_pin, OUTPUT);
  pinMode(add_makeup_pin, OUTPUT);
  pinMode(timer_lower_sensor_pin, INPUT_PULLUP);
  pinMode(timer_upper_sensor_pin, INPUT_PULLUP);
  pinMode(pump_lower_sensor_pin, INPUT_PULLUP);
  pinMode(pump_upper_sensor_pin, INPUT_PULLUP);
  pinMode(reservoir_upper_sensor_pin, INPUT_PULLUP);
  pinMode(reservoir_lower_sensor_pin, INPUT_PULLUP);
  pinMode(air_pressure_sensor_pin, INPUT_PULLUP);
  pinMode(vacuum_sensor_pin, INPUT_PULLUP);
  pinMode(vacuum_fault_pin, INPUT_PULLUP);
  digitalWrite(ink_pressure_pin, HIGH);
  digitalWrite(pump_pressure_pin, HIGH);
  digitalWrite(pump_vacuum_pin, HIGH);
  digitalWrite(add_ink_pin, HIGH);
  digitalWrite(add_makeup_pin, HIGH);
  digitalWrite(vacuum_pin, HIGH);
  digitalWrite(clean_nozzle_pin, LOW);
  
}

//Mainloop
void loop() {
  read_serial_data();
  on_data();
  state_report();
  auto_mode();
  handle_faults();
  ink_data();
  shutdown();
}

//Read mySerial Data
void read_serial_data() {
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;
  while (mySerial.available() > 0 && newData == false) {
    rc = mySerial.read();
    if (rc != endMarker) {
      receivedChars[ndx] = rc;
      ndx++;
      if (ndx >= numChars) {
        ndx = numChars - 1;
      }
    }
    else {
      receivedChars[ndx] = '\0'; // terminate the string
      ndx = 0;
      newData = true;
    }
  }
}

//State Report
void state_report() {
  if (digitalRead(ink_pressure_pin) == LOW) {
    ink_pressure_value = 1;
  }
  else {
    ink_pressure_value = 0;
  }
  if (digitalRead(pump_pressure_pin) == LOW) {
    pump_pressure_value = 2;
  }
  else {
    pump_pressure_value = 0;
  }
  if (digitalRead(pump_vacuum_pin) == LOW) {
    pump_vacuum_value = 4;
  }
  else {
    pump_vacuum_value = 0;
  }
  if (digitalRead(add_ink_pin) == LOW) {
    add_ink_value = 8;
  }
  else {
    add_ink_value = 0;
  }
  if (digitalRead(add_makeup_pin) == LOW) {
    add_makeup_value = 16;
  }
  else {
    add_makeup_value = 0;
  }
  if (digitalRead(air_pressure_sensor_pin) == HIGH) {
    air_pressure_sensor_value = 32;
  }
  else {
    air_pressure_sensor_value = 0;
  }
  if (digitalRead(vacuum_sensor_pin) == HIGH) {
    vacuum_sensor_value = 64;
  }
  else {
    vacuum_sensor_value = 0;
  }
  if (digitalRead(reservoir_lower_sensor_pin) == HIGH) {
    reservoir_lower_sensor_value = 128;
  }
  else {
    reservoir_lower_sensor_value = 0;
  }
  if (digitalRead(reservoir_upper_sensor_pin) == HIGH) {
    reservoir_upper_sensor_value = 256;
  }
  else {
    reservoir_upper_sensor_value = 0;
  }
  if (digitalRead(pump_lower_sensor_pin) == HIGH) {
    pump_lower_sensor_value = 512;
  }
  else {
    pump_lower_sensor_value = 0;
  }
  if (digitalRead(pump_upper_sensor_pin) == HIGH) {
    pump_upper_sensor_value = 1024;
  }
  else {
    pump_upper_sensor_value = 0;
  }
  if (digitalRead(timer_lower_sensor_pin) == HIGH) {
    timer_lower_sensor_value = 2048;
  }
  else {
    timer_lower_sensor_value = 0;
  }
  if (digitalRead(timer_upper_sensor_pin) == HIGH) {
    timer_upper_sensor_value = 4096;
  }
  else {
    timer_upper_sensor_value = 0;
  }
  if (fault_state == true) {
    fault_value = 8192;
  }
  else {
    fault_value = 0;
  }
  if (digitalRead(vacuum_pin) == LOW) {
    vacuum_value = 16384;
  }
  else {
    vacuum_value = 0;
  }
  if (auto_mode_state == true) {
    auto_mode_value = 32768;
  }
  else {
    auto_mode_value = 0;
  }
  if (digitalRead(clean_nozzle_pin) == LOW) {
    clean_nozzle_value = 65536;
  }
  else {
    clean_nozzle_value = 0;
  }
  
  state_report_value = (ink_pressure_value + pump_pressure_value + pump_vacuum_value +
                        add_ink_value + add_makeup_value + air_pressure_sensor_value + vacuum_sensor_value + reservoir_lower_sensor_value +
                        reservoir_upper_sensor_value + pump_lower_sensor_value + pump_upper_sensor_value + timer_lower_sensor_value +
                        timer_upper_sensor_value + fault_value + vacuum_value + auto_mode_value + clean_nozzle_value);
  if (previous_state_report_value != state_report_value) {
    mySerial.println("#" + String(state_report_value));
    previous_state_report_value = state_report_value;
  }
}

//Manual Control
void on_data() {
  if (newData == true) {
    newData = false;
    //ON
    if (strcmp(receivedChars, "F001") == 0) {
      if (auto_mode_state == false) {
        ink_pressure_on();
      }
      else {
        mySerial.println("Turn Auto Mode off, first!");
      }
    }
    if (strcmp(receivedChars, "F003") == 0) {
      if (auto_mode_state == false) {
        pump_pressure_on();
      }
      else {
        mySerial.println("Turn Auto Mode off, first!");
      }
    }
    if (strcmp(receivedChars, "F005") == 0) {
      if (auto_mode_state == false) {
        pump_vacuum_on();
      }
      else {
        mySerial.println("Turn Auto Mode off, first!");
      }
    }
    if (strcmp(receivedChars, "F007") == 0) {
      if (auto_mode_state == false) {
        add_ink_on();
      }
      else {
        mySerial.println("Turn Auto Mode off, first!");
      }
    }
    if (strcmp(receivedChars, "F009") == 0) {
      if (auto_mode_state == false) {
        add_makeup_on();
      }
      else {
        mySerial.println("Turn Auto Mode off, first!");
      }
    }
    if (strcmp(receivedChars, "F011") == 0) {
      if (auto_mode_state == false) {
        vacuum_on();
      }
      else {
        mySerial.println("Turn Auto Mode off, first!");
      }
    }
    if (strcmp(receivedChars, "F013") == 0) {
      auto_mode_on();
    }
	if (strcmp(receivedChars, "F015") == 0) {
      if (auto_mode_state == false) {
        clean_nozzle_on();
      }
      else {
        mySerial.println("Turn Auto Mode off, first!");
      }
    }
	
    //OFF
    if (strcmp(receivedChars, "F002") == 0) {
      if (auto_mode_state == false) {
        ink_pressure_off();
      }
      else {
        mySerial.println("Turn Auto Mode off, first!");
      }
    }
    if (strcmp(receivedChars, "F004") == 0) {
      if (auto_mode_state == false) {
        pump_pressure_off();
      }
      else {
        mySerial.println("Turn Auto Mode off, first!");
      }
    }
    if (strcmp(receivedChars, "F006") == 0) {
      if (auto_mode_state == false) {
        pump_vacuum_off();
      }
      else {
        mySerial.println("Turn Auto Mode off, first!");
      }
    }
    if (strcmp(receivedChars, "F008") == 0) {
      if (auto_mode_state == false) {
        add_ink_off();
      }
      else {
        mySerial.println("Turn Auto Mode off, first!");
      }
    }
    if (strcmp(receivedChars, "F010") == 0) {
      if (auto_mode_state == false) {
        add_makeup_off();
      }
      else {
        mySerial.println("Turn Auto Mode off, first!");
      }
    }
    if (strcmp(receivedChars, "F012") == 0) {
      if (auto_mode_state == false) {
        vacuum_off();
      }
      else {
        mySerial.println("Turn Auto Mode off, first!");
      }
    }
    if (strcmp(receivedChars, "F014") == 0) {
      auto_mode_off();
    }
	if (strcmp(receivedChars, "F016") == 0) {
      if (auto_mode_state == false) {
        clean_nozzle_off();
      }
      else {
        mySerial.println("Turn Auto Mode off, first!");
      }
    }
  }
}

//Valve Functions
//ON
void ink_pressure_on() {
  if (digitalRead(vacuum_pin) == LOW) {
    digitalWrite(ink_pressure_pin, LOW);
  }
  else {
    mySerial.println("Turn Vacuum on, first!");
  }
}
void pump_pressure_on() {
  if (digitalRead(ink_pressure_pin) == LOW && digitalRead(pump_vacuum_pin) == HIGH && digitalRead(add_ink_pin) == HIGH && digitalRead(add_makeup_pin) == HIGH) {
    digitalWrite(pump_pressure_pin, LOW);
  }
  else {
    mySerial.println("Turn Ink Pressure on and Pump Vacuum, Add Ink, Add MakeUP off, first!");
  }
}
void pump_vacuum_on() {
  if (digitalRead(pump_pressure_pin) == HIGH && digitalRead(ink_pressure_pin) == LOW && digitalRead(add_ink_pin) == HIGH && digitalRead(add_makeup_pin) == HIGH) {
    digitalWrite(pump_vacuum_pin, LOW);
  }
  else {
    mySerial.println("Turn Ink Pressure on and Pump Pressure, Add Ink, Add MakeUp off, first!");
  }
}
void add_ink_on() {
  if (digitalRead(ink_pressure_pin) == LOW && digitalRead(pump_vacuum_pin) == HIGH && digitalRead(add_makeup_pin) == HIGH && digitalRead(pump_pressure_pin) == HIGH) {
    digitalWrite(add_ink_pin, LOW);
  }
  else {
    mySerial.println("Turn Ink Pressure on and Pump Vacuum, Pump Pressure, Add MakeUp off, first!");
  }
}
void add_makeup_on() {
  if (digitalRead(ink_pressure_pin) == LOW && digitalRead(pump_vacuum_pin) == HIGH && digitalRead(add_ink_pin) == HIGH && digitalRead(pump_pressure_pin) == HIGH) {
    digitalWrite(add_makeup_pin, LOW);
  }
  else {
    mySerial.println("Turn Ink Pressure on and Pump Vacuum, Pump Pressure, Add Ink off, first!");
  }
}
void vacuum_on() {
  digitalWrite(vacuum_pin, LOW);
}
void clean_nozzle_on() {
  digitalWrite(clean_nozzle_pin, LOW);
}
void auto_mode_on() {
  vacuum_on();
  ink_pressure_on();
  clean_nozzle_off();
  auto_mode_state = true;
  auto_mode_case = 1;
}

//OFF
void ink_pressure_off() {
  digitalWrite(ink_pressure_pin, HIGH);
  digitalWrite(pump_pressure_pin, HIGH);
  digitalWrite(pump_vacuum_pin, HIGH);
  digitalWrite(add_ink_pin, HIGH);
  digitalWrite(add_makeup_pin, HIGH);
}
void pump_pressure_off() {
  digitalWrite(pump_pressure_pin, HIGH);
}
void pump_vacuum_off() {
  digitalWrite(pump_vacuum_pin, HIGH);
}
void add_ink_off() {
  digitalWrite(add_ink_pin, HIGH);
}
void add_makeup_off() {
  digitalWrite(add_makeup_pin, HIGH);
}
void vacuum_off() {
  digitalWrite(vacuum_pin, HIGH);
  digitalWrite(ink_pressure_pin, HIGH);
  digitalWrite(pump_pressure_pin, HIGH);
  digitalWrite(pump_vacuum_pin, HIGH);
  digitalWrite(add_ink_pin, HIGH);
  digitalWrite(add_makeup_pin, HIGH);
}
void clean_nozzle_off() {
  digitalWrite(clean_nozzle_pin, HIGH);
}
void auto_mode_off() {
  ink_pressure_off();
  clean_nozzle_on();
  shutdown_millis = millis();
  shutdown_in_progress = true;
}

//Handle Faults
void handle_faults() {
if (digitalRead(vacuum_fault_pin) == LOW){
auto_mode_off();
if (millis() - fault_millis > 1000){
fault_state = true;
}
if (millis() - fault_millis > 2000){
fault_state = false;
fault_millis = millis();
}
}
else {
fault_state = false;
}
}

//Auto Mode
void auto_mode() {
  switch (auto_mode_case) {
    case 0: //Auto Mode OFF
      timer_running = false;
      break;
    case 1: //Stop Timer
      if (digitalRead(timer_lower_sensor_pin) == LOW) {
        if (timer_running == true) {
		  mySerial.println("Timer stopped!");
          ink_time = millis() - start_timer_millis;
          mySerial.println("$" + String(ink_time / 1000));
        }
        auto_mode_case = 2;
      }
      break;
    case 2: //Check reservoir
      if (digitalRead(reservoir_lower_sensor_pin) == HIGH) {
        mySerial.println("Rerservoir filled!");
        auto_mode_case = 4;
      }
      else {
        mySerial.println("Filling Reservoir!");
        auto_mode_case = 3;
      }
      break;
    case 3: //Fill Reservoir (only with Ink for now)
      if (digitalRead(reservoir_lower_sensor_pin) == LOW) {
        add_ink_on();
      }
      else {
        add_ink_off();
        auto_mode_case = 2;
      }
      break;
    case 4: //Check pump
      if (digitalRead(pump_upper_sensor_pin) == HIGH) {
        mySerial.println("Pump filled!");
        auto_mode_case = 6;
      }
      else {
        mySerial.println("Filling Pump!");
        auto_mode_case = 5;
      }
      break;
    case 5: //Fill Pump
      if (digitalRead(pump_upper_sensor_pin) == LOW) {
        pump_vacuum_on();
      }
      else {
        pump_vacuum_off();
        auto_mode_case = 4;
      }
      break;
    case 6: //Check Timer
      if (digitalRead(timer_upper_sensor_pin) == HIGH) {
        mySerial.println("Timer filled!");
        auto_mode_case = 8;
      }
      else {
        mySerial.println("Filling Timer!");
        auto_mode_case = 7;
      }
      break;
    case 7: //Fill Timer
      if (digitalRead(timer_upper_sensor_pin) == LOW) {
        pump_pressure_on();
      }
      else {
        pump_pressure_off();
        auto_mode_case = 6;
      }
      break;
    case 8: //Start Timer
      if (digitalRead(timer_upper_sensor_pin) == LOW) {
        start_timer_millis = millis();
        timer_running = true;
        mySerial.println("Timer started!");
        auto_mode_case = 1;
      }
      break;
  }
}

//Ink Data
void ink_data() {
if (millis() - ink_data_millis > 5000){
sensors.requestTemperatures();
gravityTds.setTemperature(sensors.getTempCByIndex(0));
gravityTds.update();
tdsValue = gravityTds.getTdsValue();
mySerial.println("@C" + String(tdsValue,0));
mySerial.println("@T" + String(sensors.getTempCByIndex(0)));
ink_data_millis = millis();
}
}

//Shutdown
void shutdown() {
if (shutdown_in_progress == true){
if (millis() - shutdown_millis > 30000){
  vacuum_off();
  auto_mode_state = false;
  auto_mode_case = 0;
  shutdown_in_progress = false;
}
}
}
