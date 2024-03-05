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

//GT911 XY
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

//MCP23017 I2C
#include "Wire.h"
#define SDA 10
#define SCL 11
#define MCP23017_ADRESS 0x27  //MCP Adress
#define IODIRA 0x00           //Register for INPUT/OUTPUT A
#define IODIRB 0x01           //Register for INPUT/OUTPUT B
#define GPIOA 0x12            //Register for HIGH/LOW A
#define GPIOB 0x13            //Register for HIGH/LOW B
#define GPPUA 0x0C            //Register for Pullup A
#define GPPUB 0x0D            //Register for Pullup B
#define PINMODE_A 0x07        //A 00000111 0 = Output 1 = Input
#define PINMODE_B 0x07        //B 00000111 0 = Output 1 = Input
#define PULLUP_A 0x07         //A 00000111 0 = Output 1 = Input
#define PULLUP_B 0x07         //B 00000111 0 = Output 1 = Input

//MCP23017 Input Pinout
#define VISCOSIMETER_UPPER_SWITCH 'B', 2
#define VISCOSIMETER_LOWER_SWITCH 'A', 2
#define WASTE_TANKR_SWITCH 'A', 1

//MCP23017 Output Pinout
#define PUMP_PIN mcp_out_bit_16
#define GUTTER_PIN mcp_out_bit_7
#define MAKEUP_PIN mcp_out_bit_14
#define NOZZLE_INK_PIN mcp_out_bit_13
#define NOZZLE_CLEANING_PIN mcp_out_bit_12
#define NOZZLE_VACUUM_PIN mcp_out_bit_4
#define NOZZLE_MAKEUP_PIN mcp_out_bit_15
#define VENT_WASTE_TANK_PIN mcp_out_bit_6
#define DRAIN_WASTE_TANK_PIN mcp_out_bit_5
#define VISCOSIMETER_PIN mcp_out_bit_8
#define NO_OUTPUT_11 mcp_out_bit_9
#define NO_OUTPUT_12 mcp_out_bit_11
#define NO_OUTPUT_13 mcp_out_bit_2
#define NO_OUTPUT_14 mcp_out_bit_10
#define NO_OUTPUT_15 mcp_out_bit_3
#define NO_OUTPUT_16 mcp_out_bit_1

//MCP23017 Output Bits
int mcp_out_value_1;   //A0
int mcp_out_value_2;   //A1
int mcp_out_value_3;   //A2
int mcp_out_value_4;   //A3
int mcp_out_value_5;   //A4
int mcp_out_value_6;   //A5
int mcp_out_value_7;   //A6
int mcp_out_value_8;   //A7
int mcp_out_value_9;   //B0
int mcp_out_value_10;  //B1
int mcp_out_value_11;  //B2
int mcp_out_value_12;  //B3
int mcp_out_value_13;  //B4
int mcp_out_value_14;  //B5
int mcp_out_value_15;  //B6
int mcp_out_value_16;  //B7
bool mcp_out_bit_1 = false;
bool mcp_out_bit_2 = false;
bool mcp_out_bit_3 = false;
bool mcp_out_bit_4 = false;
bool mcp_out_bit_5 = false;
bool mcp_out_bit_6 = false;
bool mcp_out_bit_7 = false;
bool mcp_out_bit_8 = false;
bool mcp_out_bit_9 = false;
bool mcp_out_bit_10 = false;
bool mcp_out_bit_11 = false;
bool mcp_out_bit_12 = false;
bool mcp_out_bit_13 = false;
bool mcp_out_bit_14 = false;
bool mcp_out_bit_15 = false;
bool mcp_out_bit_16 = false;
int io_a_value = 0;
int previous_io_a_value = 0;
int io_b_value = 0;
int previous_io_b_value = 0;
byte io_input_a = 0;
byte io_input_b = 0;

//DS3231
#include "uRTCLib.h"
uRTCLib rtc(0x68);
unsigned long rtc_millis = 0;

//ADS1115
#include <Wire.h>
#define ADS1115 0x48
#define CONFIG_REGISTER 1
#define CONFIG_BYTE_1 0b10000011
#define CONVERSION_REGISTER 0
#define ADS_BUFFERCOUNT 16
const uint8_t  CONFIG_BYTE_0[4] = {0b11010000, 0b11100000, 0b11110000, 0b11000000};
const float    VOLTS_PER_STEP   = 6.144 / 32768.0;
uint16_t ADS_averages[4], ADS_buffers[4][ADS_BUFFERCOUNT];

//Viscosimeter
unsigned long viscosimeter_pause = 10000;
unsigned long viscosimeter_timeout = 60000;
unsigned int viscosimeter_pump_time = 3000;
unsigned long viscosimeter_counter_millis = 0;
unsigned long viscosimeter_pause_millis = 0;
unsigned long viscosimeter_pump_millis = 0;
unsigned long viscosimeter_falltime = 0;
unsigned int viscosimeter_case = 0;

//Warnings
unsigned long waste_tank_full_millis = 0;
bool waste_tank_warning_led = false;

//Setup
void setup() {

  //LCD
  lcd.init();
  lcd.setBrightness(255);
  lcd.fillScreen(TFT_BLACK);
  lcd.setFont(&fonts::AsciiFont8x16);
  lcd.setTextColor(TFT_WHITE, TFT_BLACK);
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
  lcd.fillRect(5, 445, 30, 30, TFT_GREEN);  // CIJ Printer
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
  lcd.drawString("    Sunday", 720, 455);
  lcd.drawString("Viscosity:     0000 ms", 610, 5);
  ////lcd.drawString("Pressure: 00.0 psi", 690, 95);
  ////lcd.drawString("Temperature: 00.0 `C", 670, 80);
  ////lcd.drawString("Conductivity: 000 ppm", 660, 110);
  
  //Serial
  Serial.begin(115200);
  Serial.println("START");

  //MCP IO
  Wire.begin(SDA, SCL);
  Wire.beginTransmission(MCP23017_ADRESS);
  Wire.write(IODIRA);
  Wire.write(PINMODE_A);
  Wire.endTransmission();
  Wire.beginTransmission(MCP23017_ADRESS);
  Wire.write(IODIRB);
  Wire.write(PINMODE_B);
  Wire.endTransmission();
  Wire.beginTransmission(MCP23017_ADRESS);
  Wire.write(GPPUA);
  Wire.write(PULLUP_A);
  Wire.endTransmission();
  Wire.beginTransmission(MCP23017_ADRESS);
  Wire.write(GPPUB);
  Wire.write(PULLUP_B);
  Wire.endTransmission();

  //RTC
  URTCLIB_WIRE.begin();

  //ADS1115
  InitAverages();
}

//Valve Functions
//ON
void pump_on() {
  if (pump_button == false) {
    Serial.println("Pump ON");
    lcd.fillRect(5, 5, 30, 30, TFT_GREEN);
    pump_button = true;
    PUMP_PIN = true;
  }
}

void gutter_on() {
  if (gutter_button == false) {
    Serial.println("Gutter ON");
    lcd.fillRect(5, 45, 30, 30, TFT_GREEN);
    gutter_button = true;
    GUTTER_PIN = true;
  }
}

void makeup_on() {
  if (makeup_button == false) {
    Serial.println("MakeUp ON");
    lcd.fillRect(5, 85, 30, 30, TFT_GREEN);
    makeup_button = true;
    MAKEUP_PIN = true;
  }
}

void nozzle_ink_on() {
  if (nozzle_ink_button == false) {
    Serial.println("Nozzle Ink ON");
    lcd.fillRect(5, 125, 30, 30, TFT_GREEN);
    nozzle_ink_button = true;
    NOZZLE_INK_PIN = true;
  }
}

void nozzle_cleaning_on() {
  if (nozzle_cleaning_button == false) {
    Serial.println("Nozzle Cleaning ON");
    lcd.fillRect(5, 165, 30, 30, TFT_GREEN);
    nozzle_cleaning_button = true;
    NOZZLE_CLEANING_PIN = true;
  }
}

void nozzle_vacuum_on() {
  if (nozzle_vacuum_button == false) {
    Serial.println("Nozzle Vacuum ON");
    lcd.fillRect(5, 205, 30, 30, TFT_GREEN);
    nozzle_vacuum_button = true;
    NOZZLE_VACUUM_PIN = true;
  }
}

void nozzle_makeup_on() {
  if (nozzle_makeup_button == false) {
    Serial.println("Nozzle MakeUp ON");
    lcd.fillRect(5, 245, 30, 30, TFT_GREEN);
    nozzle_makeup_button = true;
    NOZZLE_MAKEUP_PIN = true;
  }
}

void vent_waste_tank_on() {
  if (vent_waste_tank_button == false) {
    Serial.println("Vent Waste Tank ON");
    lcd.fillRect(5, 285, 30, 30, TFT_GREEN);
    vent_waste_tank_button = true;
    VENT_WASTE_TANK_PIN = true;
  }
}

void drain_waste_tank_on() {
  if (drain_waste_tank_button == false) {
    Serial.println("Drain Waste Tank ON");
    lcd.fillRect(5, 325, 30, 30, TFT_GREEN);
    drain_waste_tank_button = true;
    DRAIN_WASTE_TANK_PIN = true;
  }
}

void viscosimeter_on() {
  if (viscosimeter_button == false) {
    Serial.println("Viscosimeter ON");
    lcd.fillRect(5, 365, 30, 30, TFT_GREEN);
    viscosimeter_button = true;
  }
}

void viscosimeter_pump_on() {
  VISCOSIMETER_PIN = true;
}

//OFF
void pump_off() {
  if (pump_button == true) {
    Serial.println("Pump OFF");
    lcd.fillRect(5, 5, 30, 30, TFT_RED);
    pump_button = false;
    PUMP_PIN = false;
  }
}

void gutter_off() {
  if (gutter_button == true) {
    Serial.println("Gutter OFF");
    lcd.fillRect(5, 45, 30, 30, TFT_RED);
    gutter_button = false;
    GUTTER_PIN = false;
  }
}

void makeup_off() {
  if (makeup_button == true) {
    Serial.println("MakeUp OFF");
    lcd.fillRect(5, 85, 30, 30, TFT_RED);
    makeup_button = false;
    MAKEUP_PIN = false;
  }
}

void nozzle_ink_off() {
  if (nozzle_ink_button == true) {
    Serial.println("Nozzle Ink OFF");
    lcd.fillRect(5, 125, 30, 30, TFT_RED);
    nozzle_ink_button = false;
    NOZZLE_INK_PIN = false;
  }
}

void nozzle_cleaning_off() {
  if (nozzle_cleaning_button == true) {
    Serial.println("Nozzle Cleaning OFF");
    lcd.fillRect(5, 165, 30, 30, TFT_RED);
    nozzle_cleaning_button = false;
    NOZZLE_CLEANING_PIN = false;
  }
}

void nozzle_vacuum_off() {
  if (nozzle_vacuum_button == true) {
    Serial.println("Nozzle Vacuum OFF");
    lcd.fillRect(5, 205, 30, 30, TFT_RED);
    nozzle_vacuum_button = false;
    NOZZLE_VACUUM_PIN = false;
  }
}

void nozzle_makeup_off() {
  if (nozzle_makeup_button == true) {
    Serial.println("Nozzle MakeUp OFF");
    lcd.fillRect(5, 245, 30, 30, TFT_RED);
    nozzle_makeup_button = false;
    NOZZLE_MAKEUP_PIN = false;
  }
}

void vent_waste_tank_off() {
  if (vent_waste_tank_button == true) {
    Serial.println("Vent Waste Tank OFF");
    lcd.fillRect(5, 285, 30, 30, TFT_RED);
    vent_waste_tank_button = false;
    VENT_WASTE_TANK_PIN = false;
  }
}

void drain_waste_tank_off() {
  if (drain_waste_tank_button == true) {
    Serial.println("Drain Waste Tank OFF");
    lcd.fillRect(5, 325, 30, 30, TFT_RED);
    drain_waste_tank_button = false;
    DRAIN_WASTE_TANK_PIN = false;
  }
}

void viscosimeter_off() {
  if (viscosimeter_button == true) {
    Serial.println("Viscosimeter OFF");
    lcd.fillRect(5, 365, 30, 30, TFT_RED);
    viscosimeter_button = false;
  }
}

void viscosimeter_pump_off() {
  VISCOSIMETER_PIN = false;
}

//GT911 Buttons
void touch_buttons() {

  //Pump Button
  if (pump_button == false) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 5 && touch_y <= 35) {
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
          last_x = touch_x;
          last_y = touch_y;
          viscosimeter_off();
        }
      }
    }
  }
}

//GT911 Touchscreen
void touchscreen() {
  if (lcd.getTouch(&touch_x, &touch_y)) {
  }
}

//MCP23017 Write
void mcp23017_write() {

  if (mcp_out_bit_1 == true) {
    mcp_out_value_1 = 0;
  } else {
    mcp_out_value_1 = 1;
  }

  if (mcp_out_bit_2 == true) {
    mcp_out_value_2 = 0;
  } else {
    mcp_out_value_2 = 2;
  }

  if (mcp_out_bit_3 == true) {
    mcp_out_value_3 = 0;
  } else {
    mcp_out_value_3 = 4;
  }

  if (mcp_out_bit_4 == true) {
    mcp_out_value_4 = 0;
  } else {
    mcp_out_value_4 = 8;
  }

  if (mcp_out_bit_5 == true) {
    mcp_out_value_5 = 0;
  } else {
    mcp_out_value_5 = 16;
  }

  if (mcp_out_bit_6 == true) {
    mcp_out_value_6 = 0;
  } else {
    mcp_out_value_6 = 32;
  }

  if (mcp_out_bit_7 == true) {
    mcp_out_value_7 = 0;
  } else {
    mcp_out_value_7 = 64;
  }

  if (mcp_out_bit_8 == true) {
    mcp_out_value_8 = 128;
  } else {
    mcp_out_value_8 = 0;
  }

  if (mcp_out_bit_9 == true) {
    mcp_out_value_9 = 0;
  } else {
    mcp_out_value_9 = 1;
  }

  if (mcp_out_bit_10 == true) {
    mcp_out_value_10 = 0;
  } else {
    mcp_out_value_10 = 2;
  }

  if (mcp_out_bit_11 == true) {
    mcp_out_value_11 = 0;
  } else {
    mcp_out_value_11 = 4;
  }

  if (mcp_out_bit_12 == true) {
    mcp_out_value_12 = 0;
  } else {
    mcp_out_value_12 = 8;
  }

  if (mcp_out_bit_13 == true) {
    mcp_out_value_13 = 0;
  } else {
    mcp_out_value_13 = 16;
  }

  if (mcp_out_bit_14 == true) {
    mcp_out_value_14 = 0;
  } else {
    mcp_out_value_14 = 32;
  }

  if (mcp_out_bit_15 == true) {
    mcp_out_value_15 = 0;
  } else {
    mcp_out_value_15 = 64;
  }

  if (mcp_out_bit_16 == true) {
    mcp_out_value_16 = 128;
  } else {
    mcp_out_value_16 = 0;
  }

  io_a_value = (mcp_out_value_1 + mcp_out_value_2 + mcp_out_value_3 + mcp_out_value_4 + mcp_out_value_5 + mcp_out_value_6 + mcp_out_value_7 + mcp_out_value_8);
  if (previous_io_a_value != io_a_value) {
    Wire.beginTransmission(MCP23017_ADRESS);
    Wire.write(GPIOA);
    Wire.write(io_a_value);
    Wire.endTransmission();
    Serial.println(io_a_value, BIN);
    previous_io_a_value = io_a_value;
  }

  io_b_value = (mcp_out_value_9 + mcp_out_value_10 + mcp_out_value_11 + mcp_out_value_12 + mcp_out_value_13 + mcp_out_value_14 + mcp_out_value_15 + mcp_out_value_16);
  if (previous_io_b_value != io_b_value) {
    Wire.beginTransmission(MCP23017_ADRESS);
    Wire.write(GPIOB);
    Wire.write(io_b_value);
    Wire.endTransmission();
    Serial.println(io_b_value, BIN);
    previous_io_b_value = io_b_value;
  }
}

//MCP23017 Read
bool mcp23017_read(char gpio, int bit) {
  int gpio_register = 0;
  Wire.beginTransmission(MCP23017_ADRESS);
  Wire.write(GPIOA);
  Wire.endTransmission();
  Wire.requestFrom(MCP23017_ADRESS, 1);
  io_input_a = Wire.read();
  Wire.beginTransmission(MCP23017_ADRESS);
  Wire.write(GPIOB);
  Wire.endTransmission();
  Wire.requestFrom(MCP23017_ADRESS, 1);
  io_input_b = Wire.read();
  if (gpio == 'A') {
    gpio_register = io_input_a;
  }
  if (gpio == 'B') {
    gpio_register = io_input_b;
  }
  if (bitRead(gpio_register, bit) == 0) {
    return 1;
  }
  if (bitRead(gpio_register, bit) == 1) {
    return 0;
  }
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

//DS3231 Add leading Zeros and Day of Week
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
  lcd.drawString(time_zero, 720, 410);

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
  lcd.drawString(date_zero, 720, 425);

  //Temperature
  temperature_zero.concat("      ");
  if ((rtc.temp() / 100) > 9) {
    temperature_zero.concat(String((rtc.temp() / 100)));
  } else {
    temperature_zero.concat("0");
    temperature_zero.concat(String((rtc.temp() / 100)));
  }
  static const char roomtempend[5]     = { ' ', 247, 'C', ' ', 0 };
  static const String roomtempEnd      = roomtempend;
  temperature_zero.concat(roomtempEnd);
  Serial.println(temperature_zero);
  lcd.drawString(temperature_zero, 710, 440);

  //Day of Week
  switch (day_of_week) {
    case 1:
      Serial.println("    Monday");
      lcd.drawString("    Monday", 720, 455);
      break;
    case 2:
      Serial.println("   Tuesday");
      lcd.drawString("   Tuesday", 720, 455);
      break;
    case 3:
      Serial.println(" Wednesday");
      lcd.drawString(" Wednesday", 720, 455);
      break;
    case 4:
      Serial.println("  Thursday");
      lcd.drawString("  Thursday", 720, 455);
      break;
    case 5:
      Serial.println("    Friday");
      lcd.drawString("    Friday", 720, 455);
      break;
    case 6:
      Serial.println("  Saturday");
      lcd.drawString("  Saturday", 720, 455);
      break;
    case 7:
      Serial.println("    Sunday");
      lcd.drawString("    Sunday", 720, 455);
      break;
  }
  Serial.println("");
}

//ADS1115
// This function just saves a lot of manual typing to initialize ADS_averages and ADS_buffers arrays to all '0's
void InitAverages(){
    for(uint8_t i=0; i<4; i++){
        ADS_averages[i] = 0;
        for(uint8_t j=0; j<16; j++){
            ADS_buffers[i][j] = 0;
        }
    }
}

// This function reads 4 analog values from the ADS1115; adds them to their respective ring buffers; then updates the values of ADS_averages to the new average values
// NOTE: this function is NOT thread-safe and can cause collisions if used improperly
void UpdateAverages(){
    static uint8_t ADS_bufferIndex = 0;         // Tracks current position in the ring buffers
    static uint8_t inputIndex = 0;              // Tracks which analog input we're sampling this cycle'
    static uint32_t accumulator;
    static uint8_t ads1115_read_buffer[2];

    Wire.beginTransmission(ADS1115);
    Wire.write(CONFIG_REGISTER);
    Wire.write(CONFIG_BYTE_0[inputIndex]);
    Wire.write(CONFIG_BYTE_1);
    Wire.endTransmission();
    Wire.beginTransmission(ADS1115);
    Wire.write(CONVERSION_REGISTER);
    Wire.endTransmission();
    Wire.requestFrom(ADS1115, 2);
    ads1115_read_buffer[0] = Wire.read();
    ads1115_read_buffer[1] = Wire.read();
    Wire.endTransmission();
    ADS_buffers[inputIndex][ADS_bufferIndex] = ads1115_read_buffer[0] << 8 | ads1115_read_buffer[1];  // convert display results
    if (ADS_buffers[inputIndex][ADS_bufferIndex] > 32768) {
    ADS_buffers[inputIndex][ADS_bufferIndex] = 0;
    }

    // Now it's time to update the average read value for analog input'i'
    accumulator = 0;
    for(uint8_t j=0; j<ADS_BUFFERCOUNT; j++){
        accumulator += ADS_buffers[inputIndex][j];
    }
    ADS_averages[inputIndex] = accumulator >> 4;     // This is where you'll have to change the averaging code if you modify ADS_BUFFERCOUNT

    inputIndex++;
    if(inputIndex >= 4){                            // Reset the inputIndex to 0 after reading the fourth input (A3) and increment the bufferIndex for the ring buffers
        inputIndex = 0;
        ADS_bufferIndex++;                          // Increment the ring-buffer position for the next read
        if(ADS_bufferIndex >= ADS_BUFFERCOUNT){     // Reset ring-buffer position if we've reached the end'
            ADS_bufferIndex = 0;
        }
    }
}

// This function will send values to the serial console, based on the current values in the ADS_averages[] array
void SendAverages(){
    static const String tempString = "Temperature:   ";
    static const String tdsString  = "Conductivity:  ";
    static const String prsString  = "Pressure:      ";
	static const char tempend[5]     = { ' ', 247, 'C', ' ', 0 };
	static const String tempEnd      = tempend;
    static const String tdsEnd     = " ppm     ";
    static const String prsEnd     = " psi     ";
    static String outString;


    // Pressure sensor == A0
    double ink_pressure = ADS_averages[0] * VOLTS_PER_STEP;     // Convert raw ADC data to voltage
    ink_pressure -= 0.5;                                        // 0.5v = 0psi
    ink_pressure *= 20;                                         // 20 psi change per volt

    // Format for constant-size output String
    if(ink_pressure >= 10){
        outString = prsString + String(ink_pressure, 1) + prsEnd;
    }else{
        outString = prsString + " " + String(ink_pressure, 1) + prsEnd;
    }
    lcd.drawString(outString, 610, 35);                         // Enable this in final code; commented out for testing without the display lib.
    Serial.println(outString);

    // Temperature sensor == A2
    double temperature = CalculateTemperature(ADS_averages[2]);
    if(temperature >= 10){
        outString = tempString + String(temperature, 1) + tempEnd;
    }else{
        outString = tempString + " " + String(temperature, 1) + tempEnd;
    }
    lcd.drawString(outString, 610, 20);      // Enable this in final code; commented out for testing without the display lib.
    Serial.println(outString);

    // TDS sensor == A1
    double tdsVoltage = ADS_averages[1] * VOLTS_PER_STEP;                   // Calculate TDS sensor voltage from the raw average ADC reading
    double compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0);     // Temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
    tdsVoltage = tdsVoltage / compensationCoefficient;                      // Compensate reading for the actual measured temperature
    //convert voltage value to tds value
    double tdsValue = ((133.42 * tdsVoltage * tdsVoltage * tdsVoltage) - (255.86 * tdsVoltage * tdsVoltage) + (857.39 * tdsVoltage)) * 0.5;
    if(tdsValue >= 100){
        outString = tdsString + " " + String(tdsValue, 0) + tdsEnd;
    }else if(tdsValue >= 10){
        outString = tdsString + "  " + String(tdsValue, 0) + tdsEnd;
    }else{
        outString = tdsString + "   " + String(tdsValue, 0) + tdsEnd;
    }
    lcd.drawString(outString, 610, 50);      // Enable this in final code; commented out for testing without the display lib.
    Serial.println(outString);

    Serial.println("\n");           // Insert 2 blank lines, to make values easier to read.
}

// This function converts a raw average value into a calculated centigrade temperature
double CalculateTemperature(uint16_t rawAvg){
    const uint16_t Rref_resistance       = 10000;
    const uint16_t thermistor_Rnominal   = 10000;
    const uint16_t beta                  = 3435;
    const uint8_t  nominal_temperature   = 25;
    const double   kelvin_offset         = 273.15;

    double voltage = rawAvg * VOLTS_PER_STEP;                   // ADC voltage
    double current = (voltage) / Rref_resistance;               // Determine current through the voltage divider
    double readResistance = (5 / current) - Rref_resistance;    // Determine the thermistor's resistance based on 5V vcc, Rref_resistance, and current
    double temperature = (beta * (nominal_temperature + kelvin_offset)) /
                         (beta + ((nominal_temperature + kelvin_offset) * log(readResistance / thermistor_Rnominal)));  // Calculate temperature (in Kelvin)
    temperature -= kelvin_offset;                               // Convert to Centigrade

    return temperature;
}

//ADS1115 Read
void ads1115_read() {
    static const uint16_t ADS_INTERVAL    = 25;   // Timeout (in ms) between ADS1115 reads (per input, so effectively this number gets multiplied by 4)
    static const uint16_t SERIAL_INTERVAL = 250;   // Timeout (in ms) betweeen serial updates

    static unsigned long last_ADS_read_millis = 0;
    static unsigned long last_serial_update_millis = 0;
    static unsigned long currMillis = 0;

    currMillis = millis();
    if(currMillis > last_ADS_read_millis + ADS_INTERVAL){
        UpdateAverages();
        last_ADS_read_millis = currMillis;
    }

    if(currMillis > last_serial_update_millis + SERIAL_INTERVAL){
        SendAverages();
        last_serial_update_millis = currMillis;
    }
}

//Viscosimeter
void viscosimeter() {
  switch (viscosimeter_case) {
    case 0:  //Viscosimeter running?
      if (viscosimeter_button == true) {
        viscosimeter_pause_millis = millis();
        viscosimeter_case = 1;
      }
      //Serial.println("Case 0");
      break;
    case 1:  //Pause
      if (millis() - viscosimeter_pause_millis >= viscosimeter_pause) {
        viscosimeter_pump_millis = millis();
        viscosimeter_case = 2;
      }
      if (viscosimeter_button == false) {
        viscosimeter_case = 0;
      }
      //Serial.println("Case 1");
      break;
    case 2:  //Lift Ball;
      viscosimeter_pump_on();
      if (millis() - viscosimeter_pump_millis >= viscosimeter_pump_time) {
        viscosimeter_pump_off();
        viscosimeter_case = 3;
      }
      if (viscosimeter_button == false) {
        viscosimeter_case = 0;
      }
      //Serial.println("Case 2");
      break;
    case 3:  //Reset Counter
      if (mcp23017_read(VISCOSIMETER_UPPER_SWITCH) == true) {
        viscosimeter_counter_millis = millis();
        viscosimeter_case = 4;
      }
      if (millis() - viscosimeter_pause_millis >= viscosimeter_timeout) {
        Serial.println("Timeout!");
        viscosimeter_case = 0;
      }
      if (viscosimeter_button == false) {
        viscosimeter_case = 0;
      }
      //Serial.println("Case 3");
      break;
    case 4:  //Stop Counter and report Falltime
      if (mcp23017_read(VISCOSIMETER_LOWER_SWITCH) == true) {
        viscosimeter_falltime = millis() - viscosimeter_counter_millis;
        Serial.println("Viscosity: " + String(viscosimeter_falltime) + " ms");
        lcd.drawString("Viscosity:     " + String(viscosimeter_falltime) + " ms", 610, 5);
        viscosimeter_case = 0;
      }
      if (millis() - viscosimeter_pause_millis >= viscosimeter_timeout) {
        Serial.println("Timeout!");
        viscosimeter_case = 0;
      }
      if (viscosimeter_button == false) {
        viscosimeter_case = 0;
      }
      //Serial.println("Case 4");
      break;
  }
}

//Check Warnings
void check_warnings() {

  //Waste Tank
  if (mcp23017_read(WASTE_TANKR_SWITCH) == false) {
    pump_off();
    gutter_off();
    makeup_off();
    nozzle_ink_off();
    nozzle_cleaning_off();
    nozzle_vacuum_off();
    nozzle_makeup_off();
    viscosimeter_off();
    if (millis() - waste_tank_full_millis > 1000 && waste_tank_warning_led == false) {
      lcd.fillRect(5, 405, 30, 30, TFT_PURPLE);
      waste_tank_warning_led = true;
      waste_tank_full_millis = millis();
    }
    if (millis() - waste_tank_full_millis > 1000 && waste_tank_warning_led == true) {
      lcd.fillRect(5, 405, 30, 30, TFT_GREEN);
      waste_tank_warning_led = false;
      waste_tank_full_millis = millis();
    }
  }
  if (mcp23017_read(WASTE_TANKR_SWITCH) == true) {
    lcd.fillRect(5, 405, 30, 30, TFT_GREEN);
    waste_tank_warning_led = false;
  }
}

//Loop
void loop() {
  touchscreen();
  touch_buttons();
  mcp23017_write();
  ds3231_read();
  ads1115_read();
  viscosimeter();
  check_warnings();
}