//Timer
static uint32_t current_millis = 0;

//PWM for Pressure Pump
#include "driver/mcpwm_prelude.h"
#define PWM_GPIO 12
#define TIMEBASE_0_RESOLUTION_HZ 1000000
#define TIMEBASE_0_PERIOD 20000
mcpwm_timer_handle_t timer0 = NULL;
mcpwm_gen_handle_t generator0 = NULL;

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
      cfg.freq_write = 8000000;  //12000000
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

// Button Config
// Button 1
#define BUTTON_1_LABEL "Pressure Pump"
#define BUTTON_1_ON_ACTION pressure_pump_on()
#define BUTTON_1_OFF_ACTION pressure_pump_off()
bool button_1_state = false;

// Button 2
#define BUTTON_2_LABEL "Return Pump"
#define BUTTON_2_ON_ACTION return_pump_on()
#define BUTTON_2_OFF_ACTION return_pump_off()
bool button_2_state = false;

// Button 3
#define BUTTON_3_LABEL "Gutter Line"
#define BUTTON_3_ON_ACTION gutter_line_on()
#define BUTTON_3_OFF_ACTION gutter_line_off()
bool button_3_state = false;

// Button 4
#define BUTTON_4_LABEL "Ink Line"
#define BUTTON_4_ON_ACTION ink_line_on()
#define BUTTON_4_OFF_ACTION ink_line_off()
bool button_4_state = false;

// Button 5
#define BUTTON_5_LABEL "Vacuum Line"
#define BUTTON_5_ON_ACTION vacuum_line_on()
#define BUTTON_5_OFF_ACTION vacuum_line_off()
bool button_5_state = false;

// Button 6
#define BUTTON_6_LABEL "Flush Line"
#define BUTTON_6_ON_ACTION flush_line_on()
#define BUTTON_6_OFF_ACTION flush_line_off()
bool button_6_state = false;

// Button 7
#define BUTTON_7_LABEL "Solvent Pump"
#define BUTTON_7_ON_ACTION solvent_pump_on()
#define BUTTON_7_OFF_ACTION solvent_pump_off()
bool button_7_state = false;

// Button 8
#define BUTTON_8_LABEL "Not Used"
#define BUTTON_8_ON_ACTION
#define BUTTON_8_OFF_ACTION
bool button_8_state = false;

// Button 9
#define BUTTON_9_LABEL "Not Used"
#define BUTTON_9_ON_ACTION
#define BUTTON_9_OFF_ACTION
bool button_9_state = false;

// Button 10
#define BUTTON_10_LABEL "Not Used"
#define BUTTON_10_ON_ACTION
#define BUTTON_10_OFF_ACTION
bool button_10_state = false;

// Button 11
#define BUTTON_11_LABEL "Not Used"
#define BUTTON_11_ON_ACTION
#define BUTTON_11_OFF_ACTION
bool button_11_state = false;

// Button 12
#define BUTTON_12_LABEL "Not Used"
#define BUTTON_12_ON_ACTION
#define BUTTON_12_OFF_ACTION
bool button_12_state = false;

//I2C
#include <Wire.h>
#define SDA 10
#define SCL 11

//I2C Communication
#define I2C_SLAVE_ADDR 0x42
#define CMD_GET_TIMER 0x01
uint32_t receivedValue = 0;
uint8_t sensor_0_status = 0;
uint8_t sensor_1_status = 0;
unsigned long i2c_request_millis = 0;
const long i2c_request_interval = 1000;
const float sensor_distance_meters = 0.03;
float time_delay_seconds = 0.0;
float jet_velocity = 0.0;
float pressure_velocity_ratio = 0.0;

//MCP23017
#define MCP23017_ADRESS 0x27  //MCP Adress
#define IODIRA 0x00           //Register for INPUT/OUTPUT A
#define IODIRB 0x01           //Register for INPUT/OUTPUT B
#define GPIOA 0x12            //Register for HIGH/LOW A
#define GPIOB 0x13            //Register for HIGH/LOW B
#define GPPUA 0x0C            //Register for Pullup A
#define GPPUB 0x0D            //Register for Pullup B
#define PINMODE_A 0x01        //A 00000001 0 = Output 1 = Input
#define PINMODE_B 0x01        //B 00000001 0 = Output 1 = Input
#define PULLUP_A 0x01         //A 00000001 0 = Output 1 = Input
#define PULLUP_B 0x01         //B 00000001 0 = Output 1 = Input

//MCP23017 Input Pinout
#define NOT_USED_INPUT_B 'B', 0
#define NOT_USED_INPUT_A 'A', 0

//MCP23017 Output Pinout
#define RETURN_PUMP_PIN mcp_out_bit_7
#define GUTTER_LINE_PIN mcp_out_bit_12
#define INK_LINE_PIN mcp_out_bit_13
#define VACUUM_LINE_PIN mcp_out_bit_4
#define FLUSH_LINE_PIN mcp_out_bit_15
#define SOLVENT_LINE_PIN mcp_out_bit_14
#define DIR_PIN_B mcp_out_bit_16
#define DIR_PIN_A mcp_out_bit_8
#define NOT_USED_5_PIN mcp_out_bit_5
#define NOT_USED_6_PIN mcp_out_bit_6
#define NOT_USED_3_PIN mcp_out_bit_3
#define NOT_USED_11_PIN mcp_out_bit_11
#define NOT_USED_2_PIN mcp_out_bit_2
#define NOT_USED_10_PIN mcp_out_bit_10
#define NO_OUTPUT_15 mcp_out_bit_1  //Used as Input
#define NO_OUTPUT_16 mcp_out_bit_9  //Used as Input

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
#define DS3231_ADDRESS 0x68
int years, months, days, day_of_week, hours, minutes, seconds;
char time_buffer[30];
char date_buffer[30];
String ds3231_temp;
String fraction;
String current_day_of_week;
uint8_t fractions;
const byte number_of_chars = 129;
char received_chars[number_of_chars];
bool new_serial_data = false;
int16_t ds3231_int_temp;

//ADS1115
#define ADS1115 0x48
#define CONFIG_REGISTER 1
#define CONFIG_BYTE_1 0b10000011
#define CONVERSION_REGISTER 0
const uint8_t CONFIG_BYTE_0[4] = { 0b11010000, 0b11100000, 0b11110000, 0b11000000 };
const float VOLTS_PER_STEP = 6.144 / 32768.0;
unsigned long ads1115_millis = 0;
uint16_t ads1115_buffers[4];
uint16_t pressure_int = 0;
uint16_t tds_int = 0;
uint16_t system_temperature_int = 0;
uint16_t nozzle_temperature_int = 0;
uint16_t room_temperature_int;
double room_temperature_float;
double nozzle_temperature_float;
double system_temperature_float;
double pressure_float;
double tds_float;
double ink_pressure = 0;

//Setup
void setup() {

  //Serial
  Serial.begin(115200);
  Serial.println("START");

  //LCD
  lcd.init();
  lcd.setBrightness(255);
  lcd.fillScreen(TFT_BLACK);
  lcd.setFont(&fonts::AsciiFont8x16);
  lcd.setTextColor(TFT_WHITE, TFT_BLACK);

  //Left Items
  //Button 1
  lcd.fillRect(5, 5, 30, 30, TFT_RED);
  lcd.drawString(BUTTON_1_LABEL, 50, 15);

  //Button 2
  lcd.fillRect(5, 45, 30, 30, TFT_RED);
  lcd.drawString(BUTTON_2_LABEL, 50, 55);

  //Button 3
  lcd.fillRect(5, 85, 30, 30, TFT_RED);
  lcd.drawString(BUTTON_3_LABEL, 50, 95);

  //Button 4
  lcd.fillRect(5, 125, 30, 30, TFT_RED);
  lcd.drawString(BUTTON_4_LABEL, 50, 135);

  //Button 5
  lcd.fillRect(5, 165, 30, 30, TFT_RED);
  lcd.drawString(BUTTON_5_LABEL, 50, 175);

  //Button 6
  lcd.fillRect(5, 205, 30, 30, TFT_RED);
  lcd.drawString(BUTTON_6_LABEL, 50, 215);

  //Button 7
  lcd.fillRect(5, 245, 30, 30, TFT_RED);
  lcd.drawString(BUTTON_7_LABEL, 50, 255);

  //Button 8
  lcd.fillRect(5, 285, 30, 30, TFT_RED);
  lcd.drawString(BUTTON_8_LABEL, 50, 295);

  //Button 9
  lcd.fillRect(5, 325, 30, 30, TFT_RED);
  lcd.drawString(BUTTON_9_LABEL, 50, 335);

  //Button 10
  lcd.fillRect(5, 365, 30, 30, TFT_RED);
  lcd.drawString(BUTTON_10_LABEL, 50, 375);

  //Button 11
  lcd.fillRect(5, 405, 30, 30, TFT_RED);
  lcd.drawString(BUTTON_11_LABEL, 50, 415);

  //Button 12
  lcd.fillRect(5, 445, 30, 30, TFT_RED);
  lcd.drawString(BUTTON_12_LABEL, 50, 455);

  //Right Items
  lcd.drawString("Velocity:    0 m/s", 615, 105);
  lcd.drawString("S0:", 615, 145);
  lcd.fillRect(645, 147, 10, 10, TFT_RED);
  lcd.drawString("S1:", 705, 145);
  lcd.fillRect(735, 147, 10, 10, TFT_RED);
  lcd.drawString("Viscosity:   0 p/v", 615, 185);
  lcd.drawString("    Sunday", 720, 455);
  drawVelocityBar();
  drawViscosityBar();

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

  //BTS9760 H Bridge Direction
  DIR_PIN_A = true;
  DIR_PIN_B = false;

  //PWM for Pressure Pump
  mcpwm_timer_config_t timer0_config = {
    .group_id = 0,
    .clk_src = MCPWM_TIMER_CLK_SRC_PLL160M,
    .resolution_hz = TIMEBASE_0_RESOLUTION_HZ,
    .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
    .period_ticks = TIMEBASE_0_PERIOD
  };
  mcpwm_new_timer(&timer0_config, &timer0);
  mcpwm_oper_handle_t oper0 = NULL;
  mcpwm_operator_config_t operator_config = { .group_id = 0 };
  mcpwm_new_operator(&operator_config, &oper0);
  mcpwm_operator_connect_timer(oper0, timer0);
  mcpwm_cmpr_handle_t comparator0 = NULL;
  mcpwm_comparator_config_t comparator_config = { .flags = { .update_cmp_on_tep = true } };
  mcpwm_new_comparator(oper0, &comparator_config, &comparator0);
  mcpwm_comparator_set_compare_value(comparator0, 6000);
  mcpwm_generator_config_t generator_config = { .gen_gpio_num = PWM_GPIO };
  mcpwm_new_generator(oper0, &generator_config, &generator0);
  mcpwm_generator_set_action_on_timer_event(generator0, MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH));
  mcpwm_generator_set_action_on_compare_event(generator0, MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, comparator0, MCPWM_GEN_ACTION_LOW));
  mcpwm_timer_enable(timer0);
  mcpwm_timer_start_stop(timer0, MCPWM_TIMER_START_NO_STOP);
  mcpwm_generator_set_force_level(generator0, 0, true);
}

//Relay Functions
//Pressure Pump
void pressure_pump_on() {
  Serial.println("Pressure Pump ON");
  mcpwm_generator_set_force_level(generator0, -1, true);
}
void pressure_pump_off() {
  Serial.println("Pressure Pump OFF");
  mcpwm_generator_set_force_level(generator0, 0, true);
}

//Return Pump
void return_pump_on() {
  Serial.println("Return Pump ON");
  RETURN_PUMP_PIN = true;
}
void return_pump_off() {
  Serial.println("Return Pump OFF");
  RETURN_PUMP_PIN = false;
}

//Gutter Line
void gutter_line_on() {
  Serial.println("Gutter Line ON");
  GUTTER_LINE_PIN = true;
}
void gutter_line_off() {
  Serial.println("Gutter Line OFF");
  GUTTER_LINE_PIN = false;
}

//Ink Line
void ink_line_on() {
  Serial.println("Ink Line ON");
  INK_LINE_PIN = true;
}
void ink_line_off() {
  Serial.println("Ink Line OFF");
  INK_LINE_PIN = false;
}

//Vacuum Line
void vacuum_line_on() {
  Serial.println("Vacuum Line ON");
  VACUUM_LINE_PIN = true;
}
void vacuum_line_off() {
  Serial.println("Vacuum Line OFF");
  VACUUM_LINE_PIN = false;
}

//Flush Line
void flush_line_on() {
  Serial.println("Flush Line ON");
  FLUSH_LINE_PIN = true;
}
void flush_line_off() {
  Serial.println("Flush Line OFF");
  FLUSH_LINE_PIN = false;
}

//Solvent Pump
void solvent_pump_on() {
  Serial.println("Solvent Pump ON");
  SOLVENT_LINE_PIN = true;
}
void solvent_pump_off() {
  Serial.println("Solvent Pump OFF");
  SOLVENT_LINE_PIN = false;
}

// GT911
void touch_buttons() {
  // Button 1
  if (button_1_state == false) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 5 && touch_y <= 35) {
          last_x = touch_x;
          last_y = touch_y;
          BUTTON_1_ON_ACTION;
          lcd.fillRect(5, 5, 30, 30, TFT_GREEN);
          button_1_state = true;
        }
      }
    }
  }
  if (button_1_state == true) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 5 && touch_y <= 35) {
          last_x = touch_x;
          last_y = touch_y;
          BUTTON_1_OFF_ACTION;
          lcd.fillRect(5, 5, 30, 30, TFT_RED);
          button_1_state = false;
        }
      }
    }
  }

  // Button 2
  if (button_2_state == false) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 45 && touch_y <= 75) {
          last_x = touch_x;
          last_y = touch_y;
          BUTTON_2_ON_ACTION;
          lcd.fillRect(5, 45, 30, 30, TFT_GREEN);
          button_2_state = true;
        }
      }
    }
  }
  if (button_2_state == true) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 45 && touch_y <= 75) {
          last_x = touch_x;
          last_y = touch_y;
          BUTTON_2_OFF_ACTION;
          lcd.fillRect(5, 45, 30, 30, TFT_RED);
          button_2_state = false;
        }
      }
    }
  }

  // Button 3
  if (button_3_state == false) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 85 && touch_y <= 115) {
          last_x = touch_x;
          last_y = touch_y;
          BUTTON_3_ON_ACTION;
          lcd.fillRect(5, 85, 30, 30, TFT_GREEN);
          button_3_state = true;
        }
      }
    }
  }
  if (button_3_state == true) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 85 && touch_y <= 115) {
          last_x = touch_x;
          last_y = touch_y;
          BUTTON_3_OFF_ACTION;
          lcd.fillRect(5, 85, 30, 30, TFT_RED);
          button_3_state = false;
        }
      }
    }
  }

  // Button 4
  if (button_4_state == false) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 125 && touch_y <= 155) {
          last_x = touch_x;
          last_y = touch_y;
          BUTTON_4_ON_ACTION;
          lcd.fillRect(5, 125, 30, 30, TFT_GREEN);
          button_4_state = true;
        }
      }
    }
  }
  if (button_4_state == true) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 125 && touch_y <= 155) {
          last_x = touch_x;
          last_y = touch_y;
          BUTTON_4_OFF_ACTION;
          lcd.fillRect(5, 125, 30, 30, TFT_RED);
          button_4_state = false;
        }
      }
    }
  }

  // Button 5
  if (button_5_state == false) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 165 && touch_y <= 195) {
          last_x = touch_x;
          last_y = touch_y;
          BUTTON_5_ON_ACTION;
          lcd.fillRect(5, 165, 30, 30, TFT_GREEN);
          button_5_state = true;
        }
      }
    }
  }
  if (button_5_state == true) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 165 && touch_y <= 195) {
          last_x = touch_x;
          last_y = touch_y;
          BUTTON_5_OFF_ACTION;
          lcd.fillRect(5, 165, 30, 30, TFT_RED);
          button_5_state = false;
        }
      }
    }
  }

  // Button 6
  if (button_6_state == false) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 205 && touch_y <= 235) {
          last_x = touch_x;
          last_y = touch_y;
          BUTTON_6_ON_ACTION;
          lcd.fillRect(5, 205, 30, 30, TFT_GREEN);
          button_6_state = true;
        }
      }
    }
  }
  if (button_6_state == true) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 205 && touch_y <= 235) {
          last_x = touch_x;
          last_y = touch_y;
          BUTTON_6_OFF_ACTION;
          lcd.fillRect(5, 205, 30, 30, TFT_RED);
          button_6_state = false;
        }
      }
    }
  }

  // Button 7
  if (button_7_state == false) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 245 && touch_y <= 275) {
          last_x = touch_x;
          last_y = touch_y;
          BUTTON_7_ON_ACTION;
          lcd.fillRect(5, 245, 30, 30, TFT_GREEN);
          button_7_state = true;
        }
      }
    }
  }
  if (button_7_state == true) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 245 && touch_y <= 275) {
          last_x = touch_x;
          last_y = touch_y;
          BUTTON_7_OFF_ACTION;
          lcd.fillRect(5, 245, 30, 30, TFT_RED);
          button_7_state = false;
        }
      }
    }
  }

  // Button 8
  if (button_8_state == false) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 285 && touch_y <= 315) {
          last_x = touch_x;
          last_y = touch_y;
          BUTTON_8_ON_ACTION;
          lcd.fillRect(5, 285, 30, 30, TFT_GREEN);
          button_8_state = true;
        }
      }
    }
  }
  if (button_8_state == true) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 285 && touch_y <= 315) {
          last_x = touch_x;
          last_y = touch_y;
          BUTTON_8_OFF_ACTION;
          lcd.fillRect(5, 285, 30, 30, TFT_RED);
          button_8_state = false;
        }
      }
    }
  }

  // Button 9
  if (button_9_state == false) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 325 && touch_y <= 355) {
          last_x = touch_x;
          last_y = touch_y;
          BUTTON_9_ON_ACTION;
          lcd.fillRect(5, 325, 30, 30, TFT_GREEN);
          button_9_state = true;
        }
      }
    }
  }
  if (button_9_state == true) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 325 && touch_y <= 355) {
          last_x = touch_x;
          last_y = touch_y;
          BUTTON_9_OFF_ACTION;
          lcd.fillRect(5, 325, 30, 30, TFT_RED);
          button_9_state = false;
        }
      }
    }
  }

  // Button 10
  if (button_10_state == false) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 365 && touch_y <= 395) {
          last_x = touch_x;
          last_y = touch_y;
          BUTTON_10_ON_ACTION;
          lcd.fillRect(5, 365, 30, 30, TFT_GREEN);
          button_10_state = true;
        }
      }
    }
  }
  if (button_10_state == true) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 365 && touch_y <= 395) {
          last_x = touch_x;
          last_y = touch_y;
          BUTTON_10_OFF_ACTION;
          lcd.fillRect(5, 365, 30, 30, TFT_RED);
          button_10_state = false;
        }
      }
    }
  }

  // Button 11
  if (button_11_state == false) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 405 && touch_y <= 435) {
          last_x = touch_x;
          last_y = touch_y;
          BUTTON_11_ON_ACTION;
          lcd.fillRect(5, 405, 30, 30, TFT_GREEN);
          button_11_state = true;
        }
      }
    }
  }
  if (button_11_state == true) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 405 && touch_y <= 435) {
          last_x = touch_x;
          last_y = touch_y;
          BUTTON_11_OFF_ACTION;
          lcd.fillRect(5, 405, 30, 30, TFT_RED);
          button_11_state = false;
        }
      }
    }
  }

  // Button 12
  if (button_12_state == false) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 445 && touch_y <= 475) {
          last_x = touch_x;
          last_y = touch_y;
          BUTTON_12_ON_ACTION;
          lcd.fillRect(5, 445, 30, 30, TFT_GREEN);
          button_12_state = true;
        }
      }
    }
  }
  if (button_12_state == true) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 445 && touch_y <= 475) {
          last_x = touch_x;
          last_y = touch_y;
          BUTTON_12_OFF_ACTION;
          lcd.fillRect(5, 445, 30, 30, TFT_RED);
          button_12_state = false;
        }
      }
    }
  }
}

//Touchscreen
void touchscreen() {
  if (lcd.getTouch(&touch_x, &touch_y)) {
  }
}

//MCP23017
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

//DS3231
void ds3231_read_time(int &years, int &months, int &days, int &day_of_week, int &hours, int &minutes, int &seconds) {
  Wire.beginTransmission(DS3231_ADDRESS);
  Wire.write(0);
  Wire.endTransmission();
  Wire.requestFrom(DS3231_ADDRESS, 7);
  seconds = bcd_to_dec(Wire.read() & 0x7f);
  minutes = bcd_to_dec(Wire.read());
  hours = bcd_to_dec(Wire.read() & 0x3f);
  day_of_week = bcd_to_dec(Wire.read());
  days = bcd_to_dec(Wire.read());
  months = bcd_to_dec(Wire.read());
  years = bcd_to_dec(Wire.read()) + 2000;
  switch (day_of_week) {
    case 1:
      current_day_of_week = "    Monday";
      break;
    case 2:
      current_day_of_week = "   Tuesday";
      break;
    case 3:
      current_day_of_week = " Wednesday";
      break;
    case 4:
      current_day_of_week = "  Thursday";
      break;
    case 5:
      current_day_of_week = "    Friday";
      break;
    case 6:
      current_day_of_week = " Satureday";
      break;
    case 7:
      current_day_of_week = "    Sunday";
      break;
  }
}

void ds3231_read_temp() {
  Wire.beginTransmission(DS3231_ADDRESS);
  Wire.write(17);
  Wire.endTransmission();
  Wire.requestFrom(DS3231_ADDRESS, 2);
  ds3231_int_temp = (Wire.read() << 2);
  ds3231_int_temp += (Wire.read() >> 6);
  if (ds3231_int_temp & 0x0200) {
    ds3231_int_temp -= 0x0200;
    ds3231_int_temp |= 0x8000;
  }
  room_temperature_int = ds3231_int_temp;
  room_temperature_float = ds3231_int_temp / 4.0f;
  ds3231_temp = "   ";
  ds3231_temp += String(ds3231_int_temp / 4.0f);
}

void ds3231_write_time(int years, int months, int days, int day_of_week, int hours, int minutes, int seconds) {
  Wire.beginTransmission(DS3231_ADDRESS);
  Wire.write(0);
  Wire.write(dec_to_bcd(seconds));
  Wire.write(dec_to_bcd(minutes));
  Wire.write(dec_to_bcd(hours));
  Wire.write(dec_to_bcd(day_of_week));
  Wire.write(dec_to_bcd(days));
  Wire.write(dec_to_bcd(months));
  Wire.write(dec_to_bcd(years - 2000));
  Wire.endTransmission();
}

byte dec_to_bcd(byte val) {
  return ((val / 10 * 16) + (val % 10));
}

byte bcd_to_dec(byte val) {
  return ((val / 16 * 10) + (val % 16));
}

int get_int_from_string(char *string_with_int, byte num) {
  char *tail;
  while (num > 0) {
    num--;
    while ((!isdigit(*string_with_int)) && (*string_with_int != 0)) string_with_int++;
    tail = string_with_int;
    while ((isdigit(*tail)) && (*tail != 0)) tail++;
    if (num > 0) string_with_int = tail;
  }
  return (strtol(string_with_int, &tail, 10));
}

void read_serial_input() {
  static byte incoming_chars_index = 0;
  char end_marker = '\n';
  char received_incoming_chars;
  while (Serial.available() > 0 && new_serial_data == false) {
    received_incoming_chars = Serial.read();
    if (received_incoming_chars != end_marker) {
      received_chars[incoming_chars_index] = received_incoming_chars;
      incoming_chars_index++;
      if (incoming_chars_index >= number_of_chars) {
        incoming_chars_index = number_of_chars - 1;
      }
    } else {
      received_chars[incoming_chars_index] = '\0';
      incoming_chars_index = 0;
      new_serial_data = true;
    }
  }
}

void set_clock() {
  if (new_serial_data == true) {
    if (strstr(received_chars, "set") == received_chars) {
      days = get_int_from_string(received_chars, 1);
      months = get_int_from_string(received_chars, 2);
      years = get_int_from_string(received_chars, 3);
      hours = get_int_from_string(received_chars, 4);
      minutes = get_int_from_string(received_chars, 5);
      seconds = get_int_from_string(received_chars, 6);
      day_of_week = get_int_from_string(received_chars, 7);
      ds3231_write_time(years, months, days, day_of_week, hours, minutes, seconds);
      Serial.println("New Day, Date and Time set");
      memset(received_chars, 0, sizeof(received_chars));
    } else {
      Serial.println("To set the clock send something like 'set 08.03.2024 02:30:00 5' via serial");
      memset(received_chars, 0, sizeof(received_chars));
    }
    new_serial_data = false;
  }
}

void ds3231_read() {
  static const char ds3231_temp_end[5] = { ' ', 247, 'C', ' ', 0 };
  static const String ds3231_temp_end_string = ds3231_temp_end;
  static unsigned long last_millis;
  int years, months, days, day_of_week, hours, minutes, seconds;
  if (current_millis - last_millis > 1000) {
    ds3231_read_time(years, months, days, day_of_week, hours, minutes, seconds);
    ds3231_read_temp();
    snprintf(time_buffer, sizeof(time_buffer), "  %02u:%02u:%02u", hours, minutes, seconds);
    lcd.drawString(time_buffer, 720, 410);
    snprintf(date_buffer, sizeof(date_buffer), "%02u.%02u.%04u", days, months, years);
    lcd.drawString(date_buffer, 720, 425);
    ds3231_temp.concat(ds3231_temp_end_string);
    lcd.drawString(ds3231_temp, 710, 440);
    lcd.drawString(current_day_of_week, 720, 455);
    last_millis = current_millis;
  }
  set_clock();
}

//ADS1115 //change needed
void ads1115_read() {
  if (current_millis - ads1115_millis > 1000) {
    static uint16_t inputIndex = 0;
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
    ads1115_buffers[inputIndex] = ads1115_read_buffer[0] << 8 | ads1115_read_buffer[1];
    if (ads1115_buffers[inputIndex] > 32768) {
      ads1115_buffers[inputIndex] = 0;
    }
    switch (inputIndex) {
      case 0:
        pressure_int = ads1115_buffers[inputIndex];
        break;
      case 1:
        tds_int = ads1115_buffers[inputIndex];
        break;
      case 2:
        system_temperature_int = ads1115_buffers[inputIndex];
        break;
      case 3:
        nozzle_temperature_int = ads1115_buffers[inputIndex];
        break;
    }
    inputIndex++;
    if (inputIndex >= 4) {
      inputIndex = 0;
    }
    ads1115_millis = current_millis;
  }
  convertrawreadings();
}

void convertrawreadings() {
  static const String system_temp_string = "T System:      ";
  static const String nozzle_temp_string = "T Nozzle:      ";
  static const String tds_string = "Conductivity:  ";
  static const String pressure_string = "Pressure:      ";
  static const char tempend[5] = { ' ', 247, 'C', ' ', 0 };
  static const String tempEnd = tempend;
  static const String tdsEnd = " ppm     ";
  static const String prsEnd = " psi     ";
  static String outString;

  // Pressure sensor == A0
  ink_pressure = pressure_int * VOLTS_PER_STEP;
  ink_pressure -= 0.5;
  ink_pressure *= 20;
  pressure_float = ink_pressure;
  if (ink_pressure >= 10) {
    outString = pressure_string + String(ink_pressure, 1) + prsEnd;
  } else {
    outString = pressure_string + " " + String(ink_pressure, 1) + prsEnd;
  }
  lcd.drawString(outString, 615, 35);

  // Temperature sensors == A2 & A3
  double temperature = CalculateTemperature(system_temperature_int);
  system_temperature_float = temperature;
  if (temperature >= 10) {
    outString = system_temp_string + String(temperature, 1) + tempEnd;
  } else {
    outString = system_temp_string + " " + String(temperature, 1) + tempEnd;
  }
  lcd.drawString(outString, 615, 5);
  temperature = CalculateTemperature(nozzle_temperature_int);
  nozzle_temperature_float = temperature;
  if (temperature >= 10) {
    outString = nozzle_temp_string + String(temperature, 1) + tempEnd;
  } else {
    outString = nozzle_temp_string + " " + String(temperature, 1) + tempEnd;
  }
  lcd.drawString(outString, 615, 20);

  // TDS sensor == A1
  double tdsVoltage = tds_int * VOLTS_PER_STEP;
  double compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0);
  tdsVoltage = tdsVoltage / compensationCoefficient;
  double tdsValue = ((133.42 * tdsVoltage * tdsVoltage * tdsVoltage) - (255.86 * tdsVoltage * tdsVoltage) + (857.39 * tdsVoltage)) * 0.5;
  tds_float = tdsValue;
  if (tdsValue >= 100) {
    outString = tds_string + " " + String(tdsValue, 0) + tdsEnd;
  } else if (tdsValue >= 10) {
    outString = tds_string + "  " + String(tdsValue, 0) + tdsEnd;
  } else {
    outString = tds_string + "   " + String(tdsValue, 0) + tdsEnd;
  }
  lcd.drawString(outString, 615, 50);
}

double CalculateTemperature(uint16_t rawAvg) {
  const uint16_t Rref_resistance = 10000;
  const uint16_t thermistor_Rnominal = 10000;
  const uint16_t beta = 3435;
  const uint8_t nominal_temperature = 25;
  const double kelvin_offset = 273.15;
  double voltage = rawAvg * VOLTS_PER_STEP;                                                                                                                            // ADC voltage
  double current = (voltage) / Rref_resistance;                                                                                                                        // Determine current through the voltage divider
  double readResistance = (5 / current) - Rref_resistance;                                                                                                             // Determine the thermistor's resistance based on 5V vcc, Rref_resistance, and current
  double temperature = (beta * (nominal_temperature + kelvin_offset)) / (beta + ((nominal_temperature + kelvin_offset) * log(readResistance / thermistor_Rnominal)));  // Calculate temperature (in Kelvin)
  temperature -= kelvin_offset;                                                                                                                                        // Convert to Centigrade
  return temperature;
}

//I2C Request
void i2c_request() {
  if (current_millis - i2c_request_millis >= i2c_request_interval) {
    Serial.println("i2c request sent");
    Wire.requestFrom(I2C_SLAVE_ADDR, sizeof(receivedValue) + 2);
    if (Wire.available() == sizeof(receivedValue) + 2) {
      Wire.readBytes((char *)&receivedValue, sizeof(receivedValue));
      sensor_0_status = Wire.read();
      sensor_1_status = Wire.read();
      Serial.print("Raw Data Received - tof_duration: ");
      Serial.print(receivedValue);
      if (receivedValue != 0) {
        time_delay_seconds = receivedValue / 1000000.0;
        jet_velocity = sensor_distance_meters / time_delay_seconds;
        pressure_velocity_ratio = ink_pressure / jet_velocity;
      } else {
        jet_velocity = 0;
        pressure_velocity_ratio = 0;
      }
      lcd.drawString("Velocity:    " + String(jet_velocity) + " m/s", 615, 105);
      lcd.drawString("Viscosity:    " + String(pressure_velocity_ratio) + " p/v    ", 615, 185);
      Serial.print(", S0 Status: ");
      Serial.print(sensor_0_status);
      if (sensor_0_status == 1) {
        lcd.fillRect(645, 147, 10, 10, TFT_GREEN);
      } else {
        lcd.fillRect(645, 147, 10, 10, TFT_RED);
      }
      Serial.print(", S1 Status: ");
      Serial.println(sensor_1_status);
      if (sensor_1_status == 1) {
        lcd.fillRect(735, 147, 10, 10, TFT_GREEN);
      } else {
        lcd.fillRect(735, 147, 10, 10, TFT_RED);
      }
      Serial.println(sensor_0_status == 1 ? "S0:iO" : "S0:NiO");
      Serial.println(sensor_1_status == 1 ? "S1:iO" : "S1:NiO");
    }
    i2c_request_millis = current_millis;
  }
}

// Velocity Bar
#define BAR_WIDTH 175
#define BAR_HEIGHT 10
#define MID_VELOCITY 1600
#define VELOCITY_RANGE 100
#define MIN_VELOCITY (MID_VELOCITY - VELOCITY_RANGE)
#define MAX_VELOCITY (MID_VELOCITY + VELOCITY_RANGE)
#define VELOCITY_BAR_X 615
#define VELOCITY_BAR_Y 125
int lastVelocityX = -1;
unsigned long lastVelocityUpdate = 0;
const int velocityUpdateInterval = 50;

void drawVelocityBar() {
  int greenStart = VELOCITY_BAR_X + (BAR_WIDTH * 0.25);
  int greenWidth = BAR_WIDTH * 0.5;
  lcd.fillRect(VELOCITY_BAR_X, VELOCITY_BAR_Y, greenStart - VELOCITY_BAR_X, BAR_HEIGHT, TFT_ORANGE);
  lcd.fillRect(greenStart, VELOCITY_BAR_Y, greenWidth, BAR_HEIGHT, TFT_GREEN);
  lcd.fillRect(greenStart + greenWidth, VELOCITY_BAR_Y, BAR_WIDTH - greenWidth - (greenStart - VELOCITY_BAR_X), BAR_HEIGHT, TFT_ORANGE);
}

void drawVelocityNeedle(int value) {
  value = (value == 0) ? MID_VELOCITY : constrain(value, MIN_VELOCITY, MAX_VELOCITY);
  int needleX = map(value, MAX_VELOCITY, MIN_VELOCITY, VELOCITY_BAR_X, VELOCITY_BAR_X + BAR_WIDTH);
  if (lastVelocityX != -1) {
    lcd.fillRect(lastVelocityX - 1, VELOCITY_BAR_Y - 2, 3, BAR_HEIGHT + 4, TFT_BLACK);
  }
  drawVelocityBar();
  lcd.fillRect(needleX - 1, VELOCITY_BAR_Y - 2, 3, BAR_HEIGHT + 4, TFT_RED);
  lastVelocityX = needleX;
}

void updateVelocityNeedle() {
  if (millis() - lastVelocityUpdate < velocityUpdateInterval) return;
  lastVelocityUpdate = millis();
  drawVelocityNeedle(receivedValue);
}

// Viscosity Bar
#define MID_VISCOSITY 2.0f
#define VISCOSITY_RANGE 0.125f
#define MIN_VISCOSITY (MID_VISCOSITY - VISCOSITY_RANGE)
#define MAX_VISCOSITY (MID_VISCOSITY + VISCOSITY_RANGE)
#define VISCOSITY_BAR_X 615
#define VISCOSITY_BAR_Y 205
float lastViscosityX = -1.0f;
unsigned long lastViscosityUpdate = 0;
const int viscosityUpdateInterval = 50;

void drawViscosityBar() {
  int greenStart = VISCOSITY_BAR_X + (BAR_WIDTH * 0.25);
  int greenWidth = BAR_WIDTH * 0.5;
  lcd.fillRect(VISCOSITY_BAR_X, VISCOSITY_BAR_Y, greenStart - VISCOSITY_BAR_X, BAR_HEIGHT, TFT_ORANGE);
  lcd.fillRect(greenStart, VISCOSITY_BAR_Y, greenWidth, BAR_HEIGHT, TFT_GREEN);
  lcd.fillRect(greenStart + greenWidth, VISCOSITY_BAR_Y, BAR_WIDTH - greenWidth - (greenStart - VISCOSITY_BAR_X), BAR_HEIGHT, TFT_ORANGE);
}

void drawViscosityNeedle(float value) {
  value = (value == 0) ? MID_VISCOSITY : constrain(value, MIN_VISCOSITY, MAX_VISCOSITY);
  float needleX = VISCOSITY_BAR_X + ((value - MIN_VISCOSITY) * BAR_WIDTH) / (MAX_VISCOSITY - MIN_VISCOSITY);
  needleX = constrain(needleX, VISCOSITY_BAR_X, VISCOSITY_BAR_X + BAR_WIDTH);
  if (lastViscosityX != -1.0f) {
    lcd.fillRect(lastViscosityX - 1, VISCOSITY_BAR_Y - 2, 3, BAR_HEIGHT + 4, TFT_BLACK);
  }
  drawViscosityBar();
  lcd.fillRect(needleX - 1, VISCOSITY_BAR_Y - 2, 3, BAR_HEIGHT + 4, TFT_RED);
  lastViscosityX = needleX;
}

void updateViscosityNeedle() {
  if (millis() - lastViscosityUpdate < viscosityUpdateInterval) return;
  lastViscosityUpdate = millis();
  drawViscosityNeedle(pressure_velocity_ratio);
}

//Loop
void loop() {
  current_millis = millis();
  touchscreen();
  touch_buttons();
  mcp23017_write();
  ds3231_read();
  read_serial_input();
  ads1115_read();
  i2c_request();
  updateVelocityNeedle();
  updateViscosityNeedle();
}