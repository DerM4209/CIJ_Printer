#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <GravityTDS.h>
#include <OneWire.h>
#include <DallasTemperature.h>

//Variables
boolean auto_mode_running = false;
boolean fault_state = false;
boolean newData = false;
boolean shutdown_in_progress = false;
boolean viscosimeter_running = false;
const byte tds_sensor_pin = A0;
const byte one_wire_pin = 10;
const byte rx_pin = A15;
const byte tx_pin = A14;
const byte nozzle_ink_pin = 9;
const byte nozzle_vacuum_pin = 11;
const byte overflow_pin = 8;
const byte drain_pin = 7;
const byte pump_tank_pressure_pin = 6;
const byte vacuum_pin = 5;
const byte pump_tank_vacuum_pin = 4;
const byte add_ink_pin = 3;
const byte add_makeup_pin = 2;
const byte air_pressure_sensor_pin = 15;
const byte vacuum_sensor_pin = 14;
const byte vacuum_tank_lower_sensor_pin = 16;
const byte vacuum_tank_upper_sensor_pin = 17;
const byte pump_tank_lower_sensor_pin = 19;
const byte pump_tank_upper_sensor_pin = 18;
const byte pressure_tank_lower_sensor_pin = 21;
const byte pressure_tank_upper_sensor_pin = 20;
const byte viscosimeter_pump_pin = A3;
const byte viscosimeter_upper_sensor_pin = A1;
const byte viscosimeter_lower_sensor_pin = A2;
const byte numChars = 5;
char receivedChars[numChars];
unsigned int auto_mode_case = 0;
unsigned int drain_value = 0;
unsigned int pump_tank_pressure_value = 0;
unsigned int pump_tank_vacuum_value = 0;
unsigned int add_ink_value = 0;
unsigned int add_makeup_value = 0;
unsigned int air_pressure_sensor_value = 0;
unsigned int vacuum_sensor_value = 0;
unsigned int vacuum_tank_lower_sensor_value = 0;
unsigned int vacuum_tank_upper_sensor_value = 0;
unsigned int pump_tank_lower_sensor_value = 0;
unsigned int pump_tank_upper_sensor_value = 0;
unsigned int pressure_tank_lower_sensor_value = 0;
unsigned int pressure_tank_upper_sensor_value = 0;
unsigned int fault_value = 0;
unsigned int vacuum_value = 0;
unsigned int auto_mode_value = 0;
unsigned int viscosimeter_case = 0;
unsigned int viscosimeter_pump_time = 3000;
unsigned int viscosimeter_settle_time = 250;
unsigned long nozzle_ink_value = 0;
unsigned long nozzle_vacuum_value = 0;
unsigned long continuous_viscosity_testing_value = 0;
unsigned long viscosimeter_value = 0;
unsigned long previous_state_report_value = 0;
unsigned long state_report_value = 0;
unsigned long fault_millis = 0;
unsigned long ink_data_millis = 0;
unsigned long shutdown_millis = 0;
unsigned long viscosimeter_pause = 10000;
unsigned long viscosimeter_timeout = 60000;
unsigned long viscosimeter_counter_millis = 0;
unsigned long viscosimeter_pause_millis = 0;
unsigned long viscosimeter_pump_millis = 0;
unsigned long viscosimeter_falltime = 0;
float tdsValue = 0;
SoftwareSerial mySerial =  SoftwareSerial(rx_pin, tx_pin);
OneWire oneWire(one_wire_pin);
DallasTemperature sensors(&oneWire);
GravityTDS gravityTds;

//Setup
void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  mySerial.println("Connected...");
  mySerial.println("Printer ready...");
  mySerial.println("#" + String(state_report_value));
  sensors.begin();
  gravityTds.setPin(tds_sensor_pin);
  gravityTds.setAref(5.0);
  gravityTds.setAdcRange(1024);
  gravityTds.begin();
  pinMode(rx_pin, INPUT);
  pinMode(tx_pin, OUTPUT);
  pinMode(nozzle_ink_pin, OUTPUT);
  pinMode(nozzle_vacuum_pin, OUTPUT);
  pinMode(vacuum_pin, OUTPUT);
  pinMode(drain_pin, OUTPUT);
  pinMode(pump_tank_pressure_pin, OUTPUT);
  pinMode(pump_tank_vacuum_pin, OUTPUT);
  pinMode(add_ink_pin, OUTPUT);
  pinMode(add_makeup_pin, OUTPUT);
  pinMode(pressure_tank_lower_sensor_pin, INPUT_PULLUP);
  pinMode(pressure_tank_upper_sensor_pin, INPUT_PULLUP);
  pinMode(pump_tank_lower_sensor_pin, INPUT_PULLUP);
  pinMode(pump_tank_upper_sensor_pin, INPUT_PULLUP);
  pinMode(vacuum_tank_upper_sensor_pin, INPUT_PULLUP);
  pinMode(vacuum_tank_lower_sensor_pin, INPUT_PULLUP);
  pinMode(air_pressure_sensor_pin, INPUT_PULLUP);
  pinMode(vacuum_sensor_pin, INPUT_PULLUP);
  pinMode(overflow_pin, INPUT_PULLUP);
  digitalWrite(drain_pin, HIGH);
  digitalWrite(pump_tank_pressure_pin, HIGH);
  digitalWrite(pump_tank_vacuum_pin, HIGH);
  digitalWrite(add_ink_pin, HIGH);
  digitalWrite(add_makeup_pin, HIGH);
  digitalWrite(vacuum_pin, HIGH);
  digitalWrite(nozzle_ink_pin, HIGH);
  digitalWrite(nozzle_vacuum_pin, HIGH);
  pinMode(viscosimeter_upper_sensor_pin, INPUT_PULLUP);
  pinMode(viscosimeter_lower_sensor_pin, INPUT_PULLUP);
  pinMode(viscosimeter_pump_pin, OUTPUT);
  digitalWrite(viscosimeter_pump_pin, LOW);
}

//Mainloop
void loop() {
  read_serial_data();
  remote_control();
  state_report();
  auto_mode();
  handle_faults();
  ink_data();
  shutdown();
  viscosimeter();
}

//Functions
//ON
void drain_on() {
  if (digitalRead(vacuum_pin) == HIGH) {
    digitalWrite(drain_pin, LOW);
  }
  else {
    mySerial.println("Turn Vacuum off, first!");
  }
}
void pump_tank_pressure_on() {
  if (digitalRead(pump_tank_vacuum_pin) == HIGH) {
    digitalWrite(pump_tank_pressure_pin, LOW);
  }
  else {
    mySerial.println("Turn Pump Tank Vacuum off, first!");
  }
}
void pump_tank_vacuum_on() {
  if (digitalRead(pump_tank_pressure_pin) == HIGH && digitalRead(add_ink_pin) == HIGH && digitalRead(add_makeup_pin) == HIGH && digitalRead(nozzle_vacuum_pin) == HIGH) {
    digitalWrite(pump_tank_vacuum_pin, LOW);
  }
  else {
    mySerial.println("Turn Pump Tank Pressure, Add Ink, Add MakeUp, Nozzle Vacuum off, first!");
  }
}
void add_ink_on() {
  if (digitalRead(pump_tank_vacuum_pin) == HIGH && digitalRead(add_makeup_pin) == HIGH && digitalRead(nozzle_vacuum_pin) == HIGH) {
    digitalWrite(add_ink_pin, LOW);
  }
  else {
    mySerial.println("Turn Pump Tank Vacuum, Add MakeUp, Nozzle Vacuum off, first!");
  }
}
void add_makeup_on() {
  if (digitalRead(pump_tank_vacuum_pin) == HIGH && digitalRead(add_ink_pin) == HIGH && digitalRead(nozzle_vacuum_pin) == HIGH) {
    digitalWrite(add_makeup_pin, LOW);
  }
  else {
    mySerial.println("Pump Tank Vacuum, Add Ink, Nozzle Vacuum off, first!");
  }
}
void vacuum_on() {
  if (digitalRead(drain_pin) == HIGH) {
    digitalWrite(vacuum_pin, LOW);
  }
  else {
    mySerial.println("Turn Drain off, first!");
  }
}
void nozzle_ink_on() {
  if (digitalRead(nozzle_vacuum_pin) == HIGH) {
    digitalWrite(nozzle_ink_pin, LOW);
  }
  else {
    mySerial.println("Turn Nozzle Vacuum off, first!");
  }
}
void nozzle_vacuum_on() {
  if (digitalRead(nozzle_ink_pin) == HIGH && digitalRead(add_ink_pin) == HIGH && digitalRead(add_makeup_pin) == HIGH && digitalRead(pump_tank_vacuum_pin) == HIGH) {
    digitalWrite(nozzle_vacuum_pin, LOW);
  }
  else {
    mySerial.println("Turn Nozzle Ink, Add Ink, Add MakeUp, Pump Tank Vacuum off, first!");
  }
}
void auto_mode_on() {
  drain_off();
  vacuum_on();
  add_ink_off();
  add_makeup_off();
  nozzle_vacuum_off();
  nozzle_ink_on();
  viscosimeter_on();
  auto_mode_running = true;
  mySerial.println("Printer in Auto Mode!");
}
void viscosimeter_on() {
  viscosimeter_running = true;
}
void continuous_viscosity_testing_on() {
  viscosimeter_pause = 0;
}

//OFF
void drain_off() {
  digitalWrite(drain_pin, HIGH);
}
void pump_tank_pressure_off() {
  digitalWrite(pump_tank_pressure_pin, HIGH);
}
void pump_tank_vacuum_off() {
  digitalWrite(pump_tank_vacuum_pin, HIGH);
}
void add_ink_off() {
  digitalWrite(add_ink_pin, HIGH);
}
void add_makeup_off() {
  digitalWrite(add_makeup_pin, HIGH);
}
void vacuum_off() {
  digitalWrite(vacuum_pin, HIGH);
  digitalWrite(pump_tank_vacuum_pin, HIGH);
  digitalWrite(add_ink_pin, HIGH);
  digitalWrite(add_makeup_pin, HIGH);
  digitalWrite(nozzle_vacuum_pin, HIGH);
  digitalWrite(nozzle_ink_pin, HIGH);
}
void nozzle_ink_off() {
  digitalWrite(nozzle_ink_pin, HIGH);
}
void nozzle_vacuum_off() {
  digitalWrite(nozzle_vacuum_pin, HIGH);
}
void auto_mode_off() {
  if (shutdown_in_progress == true && auto_mode_running == true) {
    mySerial.println("Shutdown in Progress!");
  }
  if (shutdown_in_progress == false && auto_mode_running == true) {
    nozzle_ink_off();
	viscosimeter_off();
    shutdown_millis = millis();
    shutdown_in_progress = true;
    mySerial.println("Shutdown in Progress!");
  }
}
void viscosimeter_off() {
  viscosimeter_running = false;
}
void continuous_viscosity_testing_off() {
  viscosimeter_pause = 10000;
}

//Read Serial Data
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
      receivedChars[ndx] = '\0';
      ndx = 0;
      newData = true;
    }
  }
}

//Remote Control
void remote_control() {
  if (newData == true) {
    newData = false;
    //ON
    if (strcmp(receivedChars, "F001") == 0) {
      if (auto_mode_running == false) {
        drain_on();
      }
      else {
        mySerial.println("Turn Auto Mode off, first!");
      }
    }
    if (strcmp(receivedChars, "F003") == 0) {
      if (auto_mode_running == false) {
        pump_tank_pressure_on();
      }
      else {
        mySerial.println("Turn Auto Mode off, first!");
      }
    }
    if (strcmp(receivedChars, "F005") == 0) {
      if (auto_mode_running == false) {
        pump_tank_vacuum_on();
      }
      else {
        mySerial.println("Turn Auto Mode off, first!");
      }
    }
    if (strcmp(receivedChars, "F007") == 0) {
      add_ink_on();
    }
    if (strcmp(receivedChars, "F009") == 0) {
      add_makeup_on();
    }
    if (strcmp(receivedChars, "F011") == 0) {
      if (auto_mode_running == false) {
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
      if (auto_mode_running == false) {
        nozzle_ink_on();
      }
      else {
        mySerial.println("Turn Auto Mode off, first!");
      }
    }
    if (strcmp(receivedChars, "F017") == 0) {
      if (auto_mode_running == false) {
        nozzle_vacuum_on();
      }
      else {
        mySerial.println("Turn Auto Mode off, first!");
      }
    }
    if (strcmp(receivedChars, "F019") == 0) {
      continuous_viscosity_testing_on();
    }
    if (strcmp(receivedChars, "F021") == 0) {
      viscosimeter_on();
    }

    //OFF
    if (strcmp(receivedChars, "F002") == 0) {
      if (auto_mode_running == false) {
        drain_off();
      }
      else {
        mySerial.println("Turn Auto Mode off, first!");
      }
    }
    if (strcmp(receivedChars, "F004") == 0) {
      if (auto_mode_running == false) {
        pump_tank_pressure_off();
      }
      else {
        mySerial.println("Turn Auto Mode off, first!");
      }
    }
    if (strcmp(receivedChars, "F006") == 0) {
      if (auto_mode_running == false) {
        pump_tank_vacuum_off();
      }
      else {
        mySerial.println("Turn Auto Mode off, first!");
      }
    }
    if (strcmp(receivedChars, "F008") == 0) {
      add_ink_off();
    }
    if (strcmp(receivedChars, "F010") == 0) {
      add_makeup_off();
    }
    if (strcmp(receivedChars, "F012") == 0) {
      if (auto_mode_running == false) {
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
      if (auto_mode_running == false) {
        nozzle_ink_off();
      }
      else {
        mySerial.println("Turn Auto Mode off, first!");
      }
    }
    if (strcmp(receivedChars, "F018") == 0) {
      if (auto_mode_running == false) {
        nozzle_vacuum_off();
      }
      else {
        mySerial.println("Turn Auto Mode off, first!");
      }
    }

    if (strcmp(receivedChars, "F020") == 0) {
      continuous_viscosity_testing_off();
    }
    if (strcmp(receivedChars, "F022") == 0) {
      viscosimeter_off();
    }
  }
}

//State Report
void state_report() {
  if (digitalRead(drain_pin) == LOW) {
    drain_value = 1;
  }
  else {
    drain_value = 0;
  }
  if (digitalRead(pump_tank_pressure_pin) == LOW) {
    pump_tank_pressure_value = 2;
  }
  else {
    pump_tank_pressure_value = 0;
  }
  if (digitalRead(pump_tank_vacuum_pin) == LOW) {
    pump_tank_vacuum_value = 4;
  }
  else {
    pump_tank_vacuum_value = 0;
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
  if (digitalRead(vacuum_tank_lower_sensor_pin) == HIGH) {
    vacuum_tank_lower_sensor_value = 128;
  }
  else {
    vacuum_tank_lower_sensor_value = 0;
  }
  if (digitalRead(vacuum_tank_upper_sensor_pin) == HIGH) {
    vacuum_tank_upper_sensor_value = 256;
  }
  else {
    vacuum_tank_upper_sensor_value = 0;
  }
  if (digitalRead(pump_tank_lower_sensor_pin) == HIGH) {
    pump_tank_lower_sensor_value = 512;
  }
  else {
    pump_tank_lower_sensor_value = 0;
  }
  if (digitalRead(pump_tank_upper_sensor_pin) == HIGH) {
    pump_tank_upper_sensor_value = 1024;
  }
  else {
    pump_tank_upper_sensor_value = 0;
  }
  if (digitalRead(pressure_tank_lower_sensor_pin) == HIGH) {
    pressure_tank_lower_sensor_value = 2048;
  }
  else {
    pressure_tank_lower_sensor_value = 0;
  }
  if (digitalRead(pressure_tank_upper_sensor_pin) == HIGH) {
    pressure_tank_upper_sensor_value = 4096;
  }
  else {
    pressure_tank_upper_sensor_value = 0;
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
  if (auto_mode_running == true) {
    auto_mode_value = 32768;
  }
  else {
    auto_mode_value = 0;
  }
  if (digitalRead(nozzle_ink_pin) == LOW) {
    nozzle_ink_value = 65536;
  }
  else {
    nozzle_ink_value = 0;
  }
  if (digitalRead(nozzle_vacuum_pin) == LOW) {
    nozzle_vacuum_value = 131072;
  }
  else {
    nozzle_vacuum_value = 0;
  }
  if (viscosimeter_pause == 0) {
    continuous_viscosity_testing_value = 262144;
  }
  else {
    continuous_viscosity_testing_value = 0;
  }
  if (viscosimeter_running == true) {
    viscosimeter_value = 524288;
  }
  else {
    viscosimeter_value = 0;
  }
  state_report_value = (drain_value + pump_tank_pressure_value + pump_tank_vacuum_value +
                        add_ink_value + add_makeup_value + air_pressure_sensor_value + vacuum_sensor_value + vacuum_tank_lower_sensor_value +
                        vacuum_tank_upper_sensor_value + pump_tank_lower_sensor_value + pump_tank_upper_sensor_value + pressure_tank_lower_sensor_value +
                        pressure_tank_upper_sensor_value + fault_value + vacuum_value + auto_mode_value + nozzle_ink_value + nozzle_vacuum_value +
                        continuous_viscosity_testing_value + viscosimeter_value);
  if (previous_state_report_value != state_report_value) {
    mySerial.println("#" + String(state_report_value));
    previous_state_report_value = state_report_value;
  }
}

//Handle Faults
void handle_faults() {
  if (digitalRead(overflow_pin) == HIGH) {
    auto_mode_off();
    if (millis() - fault_millis > 1000) {
      fault_state = true;
    }
    if (millis() - fault_millis > 2000) {
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
    case 0: //Auto Mode running?
      if (auto_mode_running == true) {
        auto_mode_case = 1;
      }
      break;
    case 1: //Wait
      if (digitalRead(pressure_tank_lower_sensor_pin) == LOW) {
        auto_mode_case = 2;
      }
      break;
    case 2: //Check Vacuum Tank
      if (digitalRead(vacuum_tank_lower_sensor_pin) == HIGH || digitalRead(pump_tank_upper_sensor_pin) == HIGH) {
        mySerial.println("Vacuum Tank filled or Pump Tank already filled!");
        auto_mode_case = 4;
      }
      else {
        mySerial.println("Filling Vacuum Tank!");
        auto_mode_case = 3;
      }
      break;
    case 3: //Fill Vacuum Tank (only with Ink for now)
      if (digitalRead(vacuum_tank_lower_sensor_pin) == LOW) {
        add_ink_on();
      }
      else {
        add_ink_off();
        auto_mode_case = 2;
      }
      break;
    case 4: //Check Pump Tank
      if (digitalRead(pump_tank_upper_sensor_pin) == HIGH) {
        mySerial.println("Pump Tank filled!");
        auto_mode_case = 6;
      }
      else {
        mySerial.println("Filling Pump Tank!");
        auto_mode_case = 5;
      }
      break;
    case 5: //Fill Pump Tank
      if (digitalRead(pump_tank_upper_sensor_pin) == LOW) {
        pump_tank_vacuum_on();
      }
      else {
        pump_tank_vacuum_off();
        auto_mode_case = 4;
      }
      break;
    case 6: //Check Pressure Tank
      if (digitalRead(pressure_tank_upper_sensor_pin) == HIGH) {
        mySerial.println("Pressure Tank filled!");
        auto_mode_case = 0;
      }
      else {
        mySerial.println("Filling Pressure Tank!");
        auto_mode_case = 7;
      }
      break;
    case 7: //Fill Pressure Tank
      if (digitalRead(pressure_tank_upper_sensor_pin) == LOW) {
        pump_tank_pressure_on();
      }
      else {
        pump_tank_pressure_off();
        auto_mode_case = 6;
      }
      break;
  }
}

//Ink Data
void ink_data() {
  if (millis() - ink_data_millis > 5000) {
    sensors.requestTemperatures();
    gravityTds.setTemperature(sensors.getTempCByIndex(0));
    gravityTds.update();
    tdsValue = gravityTds.getTdsValue();
    mySerial.println("@C" + String(tdsValue, 0));
    mySerial.println("@T" + String(sensors.getTempCByIndex(0)));
    ink_data_millis = millis();
  }
}

//Shutdown
void shutdown() {
  if (shutdown_in_progress == true) {
    if (millis() - shutdown_millis > 30000) {
      vacuum_off();
      auto_mode_running = false;
      shutdown_in_progress = false;
      mySerial.println("Printer Off!");
    }
  }
}

//Viscosimeter
void viscosimeter() {
  switch (viscosimeter_case) {
    case 0:  //Viscosimeter running?
      if (viscosimeter_running == true) {
        viscosimeter_pause_millis = millis();
        viscosimeter_case = 1;
      }
      break;
    case 1:  //Pause
      //mySerial.println("case1");
      if (millis() - viscosimeter_pause_millis >= viscosimeter_pause) {
        viscosimeter_pump_millis = millis();
        viscosimeter_case = 2;
      }
      break;
    case 2:  //Lift Ball;
      digitalWrite(viscosimeter_pump_pin, HIGH);
      if (millis() - viscosimeter_pump_millis >= viscosimeter_pump_time) {
        digitalWrite(viscosimeter_pump_pin, LOW);
        viscosimeter_case = 3;
      }
      break;
    case 3:  //Reset Counter
      if (digitalRead(viscosimeter_upper_sensor_pin) == LOW) {
        viscosimeter_counter_millis = millis();
        viscosimeter_case = 4;
      }
      if (millis() - viscosimeter_pause_millis >= viscosimeter_timeout) {
        mySerial.println("Timeout!");
        viscosimeter_case = 0;
      }
      break;
    case 4:  //Stop Counter and report Falltime
      if (digitalRead(viscosimeter_lower_sensor_pin) == LOW) {
        viscosimeter_falltime = millis() - viscosimeter_counter_millis;
        mySerial.println("$" + String(viscosimeter_falltime));
        viscosimeter_case = 0;
      }
      if (millis() - viscosimeter_pause_millis >= viscosimeter_timeout) {
        mySerial.println("Timeout!");
        viscosimeter_case = 0;
      }
      break;
  }
}
