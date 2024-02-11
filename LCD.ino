//LovyanGFX Library
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <lgfx/v1/platforms/esp32s3/Panel_RGB.hpp>
#include <lgfx/v1/platforms/esp32s3/Bus_RGB.hpp>
#include <driver/i2c.h>

//LovyanGFX Class
class LGFX : public lgfx::LGFX_Device {
public:
  lgfx::Bus_RGB _bus_instance;
  lgfx::Panel_RGB _panel_instance;
  lgfx::Light_PWM _light_instance;
  lgfx::Touch_GT911 _touch_instance;
  LGFX(void) {
    {
      auto cfg = _panel_instance.config();
      cfg.memory_width = 800;
      cfg.memory_height = 480;
      cfg.panel_width = 800;
      cfg.panel_height = 480;
      cfg.offset_x = 0;
      cfg.offset_y = 0;
      _panel_instance.config(cfg);
    }
    {
      auto cfg = _bus_instance.config();
      cfg.panel = &_panel_instance;
      cfg.pin_d0 = GPIO_NUM_15;   // B0
      cfg.pin_d1 = GPIO_NUM_7;    // B1
      cfg.pin_d2 = GPIO_NUM_6;    // B2
      cfg.pin_d3 = GPIO_NUM_5;    // B3
      cfg.pin_d4 = GPIO_NUM_4;    // B4
      cfg.pin_d5 = GPIO_NUM_9;    // G0
      cfg.pin_d6 = GPIO_NUM_46;   // G1
      cfg.pin_d7 = GPIO_NUM_3;    // G2
      cfg.pin_d8 = GPIO_NUM_8;    // G3
      cfg.pin_d9 = GPIO_NUM_16;   // G4
      cfg.pin_d10 = GPIO_NUM_1;   // G5
      cfg.pin_d11 = GPIO_NUM_14;  // R0
      cfg.pin_d12 = GPIO_NUM_21;  // R1
      cfg.pin_d13 = GPIO_NUM_47;  // R2
      cfg.pin_d14 = GPIO_NUM_48;  // R3
      cfg.pin_d15 = GPIO_NUM_45;  // R4
      cfg.pin_henable = GPIO_NUM_41;
      cfg.pin_vsync = GPIO_NUM_40;
      cfg.pin_hsync = GPIO_NUM_39;
      cfg.pin_pclk = GPIO_NUM_0;
      cfg.freq_write = 12000000;
      cfg.hsync_polarity = 0;
      cfg.hsync_front_porch = 40;
      cfg.hsync_pulse_width = 48;
      cfg.hsync_back_porch = 40;
      cfg.vsync_polarity = 0;
      cfg.vsync_front_porch = 1;
      cfg.vsync_pulse_width = 31;
      cfg.vsync_back_porch = 13;
      cfg.pclk_active_neg = 1;
      cfg.de_idle_high = 0;
      cfg.pclk_idle_high = 0;
      _bus_instance.config(cfg);
    }
    _panel_instance.setBus(&_bus_instance);
    {
      auto cfg = _light_instance.config();
      cfg.pin_bl = GPIO_NUM_2;
      _light_instance.config(cfg);
    }
    _panel_instance.light(&_light_instance);
    {
      auto cfg = _touch_instance.config();
      cfg.x_min = 0;
      cfg.x_max = 799;
      cfg.y_min = 0;
      cfg.y_max = 479;
      cfg.pin_int = -1;
      cfg.pin_rst = -1;
      cfg.bus_shared = false;
      cfg.offset_rotation = 0;
      cfg.i2c_port = I2C_NUM_1;
      cfg.pin_sda = GPIO_NUM_19;
      cfg.pin_scl = GPIO_NUM_20;
      cfg.freq = 400000;
      cfg.i2c_addr = 0x14;
      _touch_instance.config(cfg);
      _panel_instance.setTouch(&_touch_instance);
    }
    setPanel(&_panel_instance);
  }
};

//LovyanGFX
LGFX lcd;

//I2C IO
#include "Wire.h"
#define SDA 10
#define SCL 11

//I2C IO Registers
#define MCP23017_ADRESS 0x27  //I2C Adress
#define IODIRA 0x00           //Register for INPUT/OUTPUT A
#define IODIRB 0x01           //Register for INPUT/OUTPUT B
#define GPIOA 0x12            //Register for HIGH/LOW A
#define GPIOB 0x13            //Register for HIGH/LOW B
#define GPPUA 0x0C            //Register for Pullup A
#define GPPUB 0x0D            //Register for Pullup B

//I2C IO Pinmode
//0 = Output 1 = Input
#define PINMODE_A 0x00  //A 00000000
#define PINMODE_B 0x00  //B 00000000

//I2C IO Bits
int io_value_1;   //A0
int io_value_2;   //A1
int io_value_3;   //A2
int io_value_4;   //A3
int io_value_5;   //A4
int io_value_6;   //A5
int io_value_7;   //A6
int io_value_8;   //A7
int io_value_9;   //B0
int io_value_10;  //B1
int io_value_11;  //B2
int io_value_12;  //B3
int io_value_13;  //B4
int io_value_14;  //B5
int io_value_15;  //B6
int io_value_16;  //B7
bool io_bit_1 = false;
bool io_bit_2 = false;
bool io_bit_3 = false;
bool io_bit_4 = false;
bool io_bit_5 = false;
bool io_bit_6 = false;
bool io_bit_7 = false;
bool io_bit_8 = false;
bool io_bit_9 = false;
bool io_bit_10 = false;
bool io_bit_11 = false;
bool io_bit_12 = false;
bool io_bit_13 = false;
bool io_bit_14 = false;
bool io_bit_15 = false;
bool io_bit_16 = false;
int io_a_value = 0;
int previous_io_a_value = 0;
int io_b_value = 0;
int previous_io_b_value = 0;

//I2C IO Pinout
#define PUMP_PIN io_bit_16
#define GUTTER_PIN io_bit_7
#define MAKEUP_PIN io_bit_14
#define NOZZLE_INK_PIN io_bit_13
#define NOZZLE_CLEANING_PIN io_bit_8
#define NOZZLE_VACUUM_PIN io_bit_4
#define NOZZLE_MAKEUP_PIN io_bit_15
#define VENT_WASTE_TANK_PIN io_bit_6
#define DRAIN_WASTE_TANK_PIN io_bit_5
#define VISCOSIMETER_PIN io_bit_2
#define EMPTY_11 io_bit_9
#define EMPTY_12 io_bit_11
#define EMPTY_13 io_bit_12
#define EMPTY_14 io_bit_10
#define EMPTY_15 io_bit_3
#define EMPTY_16 io_bit_1

//Touch XY
int32_t touch_x;
int32_t touch_y;
int32_t last_x = 0;
int32_t last_y = 0;
bool pump_button = false;
bool gutter_button = false;
bool makeup_button = false;
bool nozzle_ink_button = false;
bool nozzle_cleaning_button = false;
bool nozzle_vacuum_button = false;
bool nozzle_makeup_button = false;
bool vent_waste_tank_button = false;
bool drain_waste_tank_button = false;
bool viscosimeter_button = false;

//RTC
#include "uRTCLib.h"
uRTCLib rtc(0x68);
unsigned long rtc_millis = 0;

//Setup
void setup() {

  //LCD
  lcd.init();
  lcd.setBrightness(255);
  lcd.fillScreen(TFT_WHITE);
  lcd.setTextSize(1);
  lcd.setTextColor(TFT_BLACK, TFT_WHITE);
  lcd.fillRect(5, 5, 30, 30, TFT_RED);      // Pump
  lcd.fillRect(5, 45, 30, 30, TFT_RED);     // Gutter
  lcd.fillRect(5, 85, 30, 30, TFT_RED);     // MakeUp
  lcd.fillRect(5, 125, 30, 30, TFT_RED);    // Nozzle Ink
  lcd.fillRect(5, 165, 30, 30, TFT_RED);    // Nozzle Cleaning
  lcd.fillRect(5, 205, 30, 30, TFT_RED);    // Nozzle Vacuum
  lcd.fillRect(5, 245, 30, 30, TFT_RED);    // Nozzle MakeUp
  lcd.fillRect(5, 285, 30, 30, TFT_RED);    // Vent Waste Tank
  lcd.fillRect(5, 325, 30, 30, TFT_RED);    // Drain Waste Tank
  lcd.fillRect(5, 365, 30, 30, TFT_RED);    // Viscosimeter
  lcd.fillRect(5, 405, 30, 30, TFT_GREEN);  // Waste Tank Full
  lcd.fillRect(5, 445, 30, 30, TFT_GREEN);  // Waste Tank Full
  lcd.drawString("Pump", 50, 15);
  lcd.drawString("Gutter", 50, 55);
  lcd.drawString("MakeUp", 50, 95);
  lcd.drawString("Nozzle Ink", 50, 135);
  lcd.drawString("Nozzle Cleaning", 50, 175);
  lcd.drawString("Nozzle Vacuum", 50, 215);
  lcd.drawString("Nozzle MakeUp", 50, 255);
  lcd.drawString("Vent Waste Tank", 50, 295);
  lcd.drawString("Drain Waste Tank", 50, 335);
  lcd.drawString("Viscosimeter", 50, 375);
  lcd.drawString("Waste Tank Full", 50, 415);
  lcd.drawString("CIJ Printer", 50, 455);

  //Serial
  Serial.begin(115200);
  Serial.println("START");

  //I2C IO
  Wire.begin(SDA, SCL);
  Wire.beginTransmission(MCP23017_ADRESS);
  Wire.write(IODIRA);
  Wire.write(PINMODE_A);
  Wire.endTransmission();
  Wire.beginTransmission(MCP23017_ADRESS);
  Wire.write(IODIRB);
  Wire.write(PINMODE_B);
  Wire.endTransmission();

  //RTC
  URTCLIB_WIRE.begin();
}

//Touch Buttons
void touch_buttons() {

  //Pump Button
  if (pump_button == false) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 5 && touch_y <= 35) {
          lcd.fillRect(5, 5, 30, 30, TFT_GREEN);
          last_x = touch_x;
          last_y = touch_y;
          pump_on();
        }
      }
    }
  }
  if (pump_button == true) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 5 && touch_y <= 35) {
          lcd.fillRect(5, 5, 30, 30, TFT_RED);
          last_x = touch_x;
          last_y = touch_y;
          pump_off();
        }
      }
    }
  }

  //Gutter Button
  if (gutter_button == false) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 45 && touch_y <= 75) {
          lcd.fillRect(5, 45, 30, 30, TFT_GREEN);
          last_x = touch_x;
          last_y = touch_y;
          gutter_on();
        }
      }
    }
  }
  if (gutter_button == true) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 45 && touch_y <= 75) {
          lcd.fillRect(5, 45, 30, 30, TFT_RED);
          last_x = touch_x;
          last_y = touch_y;
          gutter_off();
        }
      }
    }
  }

  // MakeUp Button
  if (makeup_button == false) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 85 && touch_y <= 115) {
          lcd.fillRect(5, 85, 30, 30, TFT_GREEN);
          last_x = touch_x;
          last_y = touch_y;
          makeup_on();
        }
      }
    }
  }
  if (makeup_button == true) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 85 && touch_y <= 115) {
          lcd.fillRect(5, 85, 30, 30, TFT_RED);
          last_x = touch_x;
          last_y = touch_y;
          makeup_off();
        }
      }
    }
  }

  // Nozzle Ink Button
  if (nozzle_ink_button == false) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 125 && touch_y <= 155) {
          lcd.fillRect(5, 125, 30, 30, TFT_GREEN);
          last_x = touch_x;
          last_y = touch_y;
          nozzle_ink_on();
        }
      }
    }
  }
  if (nozzle_ink_button == true) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 125 && touch_y <= 155) {
          lcd.fillRect(5, 125, 30, 30, TFT_RED);
          last_x = touch_x;
          last_y = touch_y;
          nozzle_ink_off();
        }
      }
    }
  }

  // Nozzle Cleaning Button
  if (nozzle_cleaning_button == false) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 165 && touch_y <= 195) {
          lcd.fillRect(5, 165, 30, 30, TFT_GREEN);
          last_x = touch_x;
          last_y = touch_y;
          nozzle_cleaning_on();
        }
      }
    }
  }
  if (nozzle_cleaning_button == true) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 165 && touch_y <= 195) {
          lcd.fillRect(5, 165, 30, 30, TFT_RED);
          last_x = touch_x;
          last_y = touch_y;
          nozzle_cleaning_off();
        }
      }
    }
  }

  // Nozzle Vacuum Button
  if (nozzle_vacuum_button == false) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 205 && touch_y <= 235) {
          lcd.fillRect(5, 205, 30, 30, TFT_GREEN);
          last_x = touch_x;
          last_y = touch_y;
          nozzle_vacuum_on();
        }
      }
    }
  }
  if (nozzle_vacuum_button == true) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 205 && touch_y <= 235) {
          lcd.fillRect(5, 205, 30, 30, TFT_RED);
          last_x = touch_x;
          last_y = touch_y;
          nozzle_vacuum_off();
        }
      }
    }
  }

  // Nozzle MakeUp Button
  if (nozzle_makeup_button == false) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 245 && touch_y <= 275) {
          lcd.fillRect(5, 245, 30, 30, TFT_GREEN);
          last_x = touch_x;
          last_y = touch_y;
          nozzle_makeup_on();
        }
      }
    }
  }
  if (nozzle_makeup_button == true) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 245 && touch_y <= 275) {
          lcd.fillRect(5, 245, 30, 30, TFT_RED);
          last_x = touch_x;
          last_y = touch_y;
          nozzle_makeup_off();
        }
      }
    }
  }

  // Vent Waste Tank Button
  if (vent_waste_tank_button == false) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 285 && touch_y <= 315) {
          lcd.fillRect(5, 285, 30, 30, TFT_GREEN);
          last_x = touch_x;
          last_y = touch_y;
          vent_waste_tank_on();
        }
      }
    }
  }
  if (vent_waste_tank_button == true) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 285 && touch_y <= 315) {
          lcd.fillRect(5, 285, 30, 30, TFT_RED);
          last_x = touch_x;
          last_y = touch_y;
          vent_waste_tank_off();
        }
      }
    }
  }

  // Drain Waste Tank Button
  if (drain_waste_tank_button == false) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 325 && touch_y <= 355) {
          lcd.fillRect(5, 325, 30, 30, TFT_GREEN);
          last_x = touch_x;
          last_y = touch_y;
          drain_waste_tank_on();
        }
      }
    }
  }
  if (drain_waste_tank_button == true) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 325 && touch_y <= 355) {
          lcd.fillRect(5, 325, 30, 30, TFT_RED);
          last_x = touch_x;
          last_y = touch_y;
          drain_waste_tank_off();
        }
      }
    }
  }

  // Viscosimeter Button
  if (viscosimeter_button == false) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 365 && touch_y <= 395) {
          lcd.fillRect(5, 365, 30, 30, TFT_GREEN);
          last_x = touch_x;
          last_y = touch_y;
          viscosimeter_on();
        }
      }
    }
  }
  if (viscosimeter_button == true) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 365 && touch_y <= 395) {
          lcd.fillRect(5, 365, 30, 30, TFT_RED);
          last_x = touch_x;
          last_y = touch_y;
          viscosimeter_off();
        }
      }
    }
  }
}

//Valve Functions
//ON
void pump_on() {
  Serial.println("Pump ON");
  pump_button = true;
  PUMP_PIN = true;
}

void gutter_on() {
  Serial.println("Gutter ON");
  gutter_button = true;
  GUTTER_PIN = true;
}

void makeup_on() {
  Serial.println("MakeUp ON");
  makeup_button = true;
  MAKEUP_PIN = true;
}

void nozzle_ink_on() {
  Serial.println("Nozzle Ink ON");
  nozzle_ink_button = true;
  NOZZLE_INK_PIN = true;
}

void nozzle_cleaning_on() {
  Serial.println("Nozzle Cleaning ON");
  nozzle_cleaning_button = true;
  NOZZLE_CLEANING_PIN = true;
}

void nozzle_vacuum_on() {
  Serial.println("Nozzle Vacuum ON");
  nozzle_vacuum_button = true;
  NOZZLE_VACUUM_PIN = true;
}

void nozzle_makeup_on() {
  Serial.println("Nozzle MakeUp ON");
  nozzle_makeup_button = true;
  NOZZLE_MAKEUP_PIN = true;
}

void vent_waste_tank_on() {
  Serial.println("Vent Waste Tank ON");
  vent_waste_tank_button = true;
  VENT_WASTE_TANK_PIN = true;
}

void drain_waste_tank_on() {
  Serial.println("Drain Waste Tank ON");
  drain_waste_tank_button = true;
  DRAIN_WASTE_TANK_PIN = true;
}

void viscosimeter_on() {
  Serial.println("Viscosimeter ON");
  viscosimeter_button = true;
  VISCOSIMETER_PIN = true;
}

//OFF
void pump_off() {
  Serial.println("Pump OFF");
  pump_button = false;
  PUMP_PIN = false;
}

void gutter_off() {
  Serial.println("Gutter OFF");
  gutter_button = false;
  GUTTER_PIN = false;
}

void makeup_off() {
  Serial.println("MakeUp OFF");
  makeup_button = false;
  MAKEUP_PIN = false;
}

void nozzle_ink_off() {
  Serial.println("Nozzle Ink OFF");
  nozzle_ink_button = false;
  NOZZLE_INK_PIN = false;
}

void nozzle_cleaning_off() {
  Serial.println("Nozzle Cleaning OFF");
  nozzle_cleaning_button = false;
  NOZZLE_CLEANING_PIN = false;
}

void nozzle_vacuum_off() {
  Serial.println("Nozzle Vacuum OFF");
  nozzle_vacuum_button = false;
  NOZZLE_VACUUM_PIN = false;
}

void nozzle_makeup_off() {
  Serial.println("Nozzle MakeUp OFF");
  nozzle_makeup_button = false;
  NOZZLE_MAKEUP_PIN = false;
}

void vent_waste_tank_off() {
  Serial.println("Vent Waste Tank OFF");
  vent_waste_tank_button = false;
  VENT_WASTE_TANK_PIN = false;
}

void drain_waste_tank_off() {
  Serial.println("Drain Waste Tank OFF");
  drain_waste_tank_button = false;
  DRAIN_WASTE_TANK_PIN = false;
}

void viscosimeter_off() {
  Serial.println("Viscosimeter OFF");
  viscosimeter_button = false;
  VISCOSIMETER_PIN = false;
}

//Touchscreen
void touchscreen() {
  if (lcd.getTouch(&touch_x, &touch_y)) {
  }
}

//MCP23017 Write
void mcp23017_write() {

  if (io_bit_1 == true) {
    io_value_1 = 0;
  } else {
    io_value_1 = 1;
  }

  if (io_bit_2 == true) {
    io_value_2 = 2;
  } else {
    io_value_2 = 0;
  }

  if (io_bit_3 == true) {
    io_value_3 = 0;
  } else {
    io_value_3 = 4;
  }

  if (io_bit_4 == true) {
    io_value_4 = 0;
  } else {
    io_value_4 = 8;
  }

  if (io_bit_5 == true) {
    io_value_5 = 0;
  } else {
    io_value_5 = 16;
  }

  if (io_bit_6 == true) {
    io_value_6 = 0;
  } else {
    io_value_6 = 32;
  }

  if (io_bit_7 == true) {
    io_value_7 = 0;
  } else {
    io_value_7 = 64;
  }

  if (io_bit_8 == true) {
    io_value_8 = 0;
  } else {
    io_value_8 = 128;
  }

  if (io_bit_9 == true) {
    io_value_9 = 0;
  } else {
    io_value_9 = 1;
  }

  if (io_bit_10 == true) {
    io_value_10 = 0;
  } else {
    io_value_10 = 2;
  }

  if (io_bit_11 == true) {
    io_value_11 = 0;
  } else {
    io_value_11 = 4;
  }

  if (io_bit_12 == true) {
    io_value_12 = 0;
  } else {
    io_value_12 = 8;
  }

  if (io_bit_13 == true) {
    io_value_13 = 0;
  } else {
    io_value_13 = 16;
  }

  if (io_bit_14 == true) {
    io_value_14 = 0;
  } else {
    io_value_14 = 32;
  }

  if (io_bit_15 == true) {
    io_value_15 = 0;
  } else {
    io_value_15 = 64;
  }

  if (io_bit_16 == true) {
    io_value_16 = 128;
  } else {
    io_value_16 = 0;
  }

  io_a_value = (io_value_1 + io_value_2 + io_value_3 + io_value_4 + io_value_5 + io_value_6 + io_value_7 + io_value_8);
  if (previous_io_a_value != io_a_value) {
    Wire.beginTransmission(MCP23017_ADRESS);
    Wire.write(GPIOA);
    Wire.write(io_a_value);
    Wire.endTransmission();
    Serial.println(io_a_value, BIN);
    previous_io_a_value = io_a_value;
  }

  io_b_value = (io_value_9 + io_value_10 + io_value_11 + io_value_12 + io_value_13 + io_value_14 + io_value_15 + io_value_16);
  if (previous_io_b_value != io_b_value) {
    Wire.beginTransmission(MCP23017_ADRESS);
    Wire.write(GPIOB);
    Wire.write(io_b_value);
    Wire.endTransmission();
    Serial.println(io_b_value, BIN);
    previous_io_b_value = io_b_value;
  }
}

//Add leading Zeros and Day of Week
void print_rtc_reading() {
  String time_zero;
  String date_zero;
  String temperature_zero;
  int day_of_week = rtc.dayOfWeek();

  //Time
  time_zero.concat("  ");
  if (rtc.hour() > 9) {
    time_zero.concat(String(rtc.hour()));
  } else {
    time_zero.concat("0");
    time_zero.concat(String(rtc.hour()));
  }
  time_zero.concat(":");
  if (rtc.minute() > 9) {
    time_zero.concat(String(rtc.minute()));
  } else {
    time_zero.concat("0");
    time_zero.concat(String(rtc.minute()));
  }
  time_zero.concat(":");
  if (rtc.second() > 9) {
    time_zero.concat(String(rtc.second()));
  } else {
    time_zero.concat("0");
    time_zero.concat(String(rtc.second()));
  }
  Serial.println(time_zero);
  lcd.drawString(time_zero, 735, 5);

  //Date
  if (rtc.day() > 9) {
    date_zero.concat(String(rtc.day()));
  } else {
    date_zero.concat("0");
    date_zero.concat(String(rtc.day()));
  }
  date_zero.concat(".");
  if (rtc.month() > 9) {
    date_zero.concat(String(rtc.month()));
  } else {
    date_zero.concat("0");
    date_zero.concat(String(rtc.month()));
  }
  date_zero.concat(".20");
  if (rtc.year() > 9) {
    date_zero.concat(String(rtc.year()));
  } else {
    date_zero.concat("0");
    date_zero.concat(String(rtc.year()));
  }
  Serial.println(date_zero);
  lcd.drawString(date_zero, 735, 20);

  //Temperature
  temperature_zero.concat("      ");
  if ((rtc.temp() / 100) > 9) {
    temperature_zero.concat(String((rtc.temp() / 100)));
  } else {
    temperature_zero.concat("0");
    temperature_zero.concat(String((rtc.temp() / 100)));
  }
  temperature_zero.concat(" C");
  Serial.println(temperature_zero);
  lcd.drawString(temperature_zero, 735, 35);

  //Day of Week
  switch (day_of_week) {
    case 1:
      Serial.println("    Monday");
      lcd.drawString("    Monday", 735, 50);
      break;
    case 2:
      Serial.println("   Tuesday");
      lcd.drawString("   Tuesday", 735, 50);
      break;
    case 3:
      Serial.println(" Wednesday");
      lcd.drawString(" Wednesday", 735, 50);
      break;
    case 4:
      Serial.println("  Thursday");
      lcd.drawString("  Thursday", 735, 50);
      break;
    case 5:
      Serial.println("    Friday");
      lcd.drawString("    Friday", 735, 50);
      break;
    case 6:
      Serial.println("  Saturday");
      lcd.drawString("  Saturday", 735, 50);
      break;
    case 7:
      Serial.println("    Sunday");
      lcd.drawString("    Sunday", 735, 50);
      break;
  }
  Serial.println("");
}

//DS3231 Read
void ds3231_read() {
  if ((millis() - rtc_millis) > 1000) {
    rtc.refresh();
    print_rtc_reading();
    rtc_millis = millis();
  }
  if (Serial.available()) {
    int year = Serial.parseInt();
    int month = Serial.parseInt();
    int day = Serial.parseInt();
    int dayOfWeek = Serial.parseInt();
    int hour = Serial.parseInt();
    int minute = Serial.parseInt();
    int second = Serial.parseInt();
    rtc.set(second, minute, hour, dayOfWeek, day, month, year);
    while (Serial.available()) {
      Serial.read();
    }
  }
}

//Loop
void loop() {
  touchscreen();
  touch_buttons();
  mcp23017_write();
  ds3231_read();
}