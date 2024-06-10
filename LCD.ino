//Timer
static uint32_t current_millis = 0;

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
bool pump_button = false;
bool gutter_button = false;
bool makeup_button = false;
bool nozzle_ink_button = false;
bool nozzle_cleaning_button = false;
bool nozzle_vacuum_button = false;
bool flush_viscosimeter_button = false;
bool circulation_pumps_button = false;
int temperature_regulation_button = 0;
bool viscosimeter_button = false;

//I2C
#include "Wire.h"
#define SDA 10
#define SCL 11

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
#define VISCOSIMETER_UPPER_SWITCH 'B', 0
#define VISCOSIMETER_LOWER_SWITCH 'A', 0

//MCP23017 Output Pinout
#define GUTTER_PIN mcp_out_bit_12
#define MAKEUP_PIN mcp_out_bit_14
#define NOZZLE_INK_PIN mcp_out_bit_13
#define NOZZLE_CLEANING_PIN mcp_out_bit_7
#define NOZZLE_VACUUM_PIN mcp_out_bit_4
#define FLUSH_VISCOSIMETER_PIN mcp_out_bit_15
#define CIRCULATION_TANKS_PIN mcp_out_bit_6
#define VISCOSIMETER_PIN mcp_out_bit_5
#define PRESSURE_PUMP_0_PIN mcp_out_bit_3
#define PRESSURE_PUMP_1_PIN mcp_out_bit_11
#define PRESSURE_PUMP_2_PIN mcp_out_bit_2
#define PRESSURE_PUMP_3_PIN mcp_out_bit_10
#define DIR_PIN_B mcp_out_bit_16
#define DIR_PIN_A mcp_out_bit_8
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
static char dts_string[19];
int16_t ds3231_int_temp;

//ADS1115
#define ADS1115 0x48
#define CONFIG_REGISTER 1
#define CONFIG_BYTE_1 0b10000011
#define CONVERSION_REGISTER 0
#define ADS_BUFFERCOUNT 16
const uint8_t CONFIG_BYTE_0[4] = { 0b11010000, 0b11100000, 0b11110000, 0b11000000 };
const float VOLTS_PER_STEP = 6.144 / 32768.0;
uint16_t ADS_buffers[4][ADS_BUFFERCOUNT];
static String pressure;
static String viscosimeter_temperature;
static String nozzle_temperature;
static String conductivity;

//Viscosimeter
unsigned long viscosimeter_pause = 10000;
unsigned long viscosimeter_timeout = 90000;
unsigned int viscosimeter_pump_time = 3000;
unsigned long viscosimeter_counter_millis = 0;
unsigned long viscosimeter_pause_millis = 0;
unsigned long viscosimeter_pump_millis = 0;
unsigned int viscosimeter_case = 0;

//Warnings
unsigned long waste_tank_full_millis = 0;
bool waste_tank_warning_led = false;

//PID Temperature Control
#include <PID_v1.h>
#define PELTIER_PWM_PIN 12
double Setpoint, Input, Output;
double Kp = 75.0, Ki = 1.0, Kd = 0.5;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, REVERSE);

//Wifi
#include <WiFi.h>
#define WIFI_SSID "FRITZ!Box Fon WLAN 7360"
#define WIFI_PASSWORD "333724476976386"

//Datalogger
//#include <WiFiClient.h>
//#include <FTPClient_Generic.h>
// char ftp_server[] = "192.168.188.1";
// char ftp_user[] = "ftpuser";
// char ftp_pass[] = "L6KW70";
// char dirName[] = "/";
// FTPClient_Generic ftp(ftp_server, ftp_user, ftp_pass, 60000);

//DB
typedef struct db_entry {
  uint32_t date_time;
  int16_t room_temperature;
  uint16_t nozzle_temperature;
  uint16_t viscosimeter_temperature;
  uint32_t fall_time;
  uint16_t pressure;
  uint16_t tds;
  uint16_t feature_bit;
} db_entry;

db_entry ftp_current;
uint64_t last_ftp_current = 0;
uint32_t timestamp = 0;

//Google Sheets
#include <ESP_Google_Sheet_Client.h>
#define PROJECT_ID "109615531160123849169"
#define CLIENT_EMAIL "cij-printer-datalogger@cij-printer.iam.gserviceaccount.com"
#define USER_EMAIL "meffertdominik@gmail.com"
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----\nMIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQCem6jE8zVKK78Z\nbMkHfLypRXpIbCOgzHgesrns3dRbnxcSCOhFg3iP77FL5yDzLPe02qqtfjDdDPaa\nOjmyVEUNOPAVliYzy7mrPigM5muBmBUk5ZuFDikbiIsTTyBzZyoclQSrY5inv80M\nl+mqxHdwkTonLzh/Yf9EGK2myGpyjuXYOwXpsDciIBRVh3hLuMMEttVwLXjoWMFc\nSoUJkxxyWB2CPRhL+kL9VQ+2+5fHKHcHC4ckxabzb0mjhC5kgtSLWUuaGZ0upZdf\n/tMpjCSYYKvqmqjoh4ZPv0yIet5UIbFwAKLE4GPaPIwx6zm3DLFQDIPog3ULWLSl\nn+LyB0uTAgMBAAECggEAAzNb8AGCaUlBtXcjBWIognfy9EQuYd/OarwrkVKWpvSq\naARC/6gcBjeNid0TpqSZKQ6ilBawUL2iulEDegxGSnKEMcNSaQgq6oA6GayKrdZO\nUuzJOyGihbo3KEAYBQwnZuVA7152pafysNbd8cK+MEjV9hV3W+az2oFu+6cessoU\nRINemATM1AYcweMIM2k5uBnVg/EjZ7yc8RMIBahXl8+YLuRkTKDARrnXwIG/kqUS\nSc2ci6l+H9U5zwRfyDsexnghHY76Pitf/+5Op9JzFIGjPxtibBn0O5TKYOzUHGn4\np5A5itZDCikPGCf+TgxdeI/hC4XDItQz/zEMhWSIKQKBgQDZLwP7ja91cleRcN79\nN//I3Ir945LQ9y3xcVfIKzHpJDnpP1Q6M8zydjjo3YKbv+Met8kn88hMlpULyJf8\niReQ630uEK9pdJyGpvL+nY6WSVbJbH+XHZW7/HWZBCMITBlDf/gHxBgTOoWSPyZF\ndW/hDeRxoBTkz1vn75s68nkv2wKBgQC69JXGDMFkINdcqajBEIr6extdpEl7WQP3\nXs6FX52ktYvdQMQNOcCLkmkyr8HyLUeuR7q05CJFGnxtnuEzMRgG00WYnNnV0MHl\nA1GYtTN8HxZSO8geLyRxFNst0C78Nw4gZ69ZFF8JBFp+/Oo5c1Pt/GcPAuE5fv6s\nHUfI6rxcqQKBgHLsTQg0ni7gtXOxrV2C32fzQ907KfpHgdksJ841BTEgSUCikThH\nJuD6K/0VEcLG1DDyAZCRkGDuoandmIc6J+04n0Hb9GYyHY2nFBglWUkvP6xQLGfT\ncONLoRK6PKtsUUYFlkLNwguSwRF1NxQ0I2KMjmCN2+CD1Vcy1ApqOjRXAoGAX09/\n57brBOPlR11LJhoL8ETAn824lIS+C+eue7pSQP0477Qr/t4Zdk5PCUMnKhLBf20c\nF0pS2k31b2FXR+TYbepZicM1HtdNcjZt6988/yNOGuBVVmYMOwY58zI2MejTb9vM\nPiqvhIZOqHzB4EYWXLaXx8RdFBXVOeO3jAt3ufkCgYEArkkgCr6OQZU5l9u0/wQj\n3Mh0wfPdQ/NXLmRGS7SQpumwCEkb+L6IltdH6cIGyyheFAZ25bn0qw2Qn6+Q2vF4\nHHdpx3gZNrPjrYdplUbH/mHB1oDrzEB4dabW0lHhHq6OqBn4vEK52NJN/CewiWzm\nh6BPTuGKGOEgEbV81tmudiw=\n-----END PRIVATE KEY-----\n";
bool taskComplete = false;

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
  lcd.fillRect(5, 5, 30, 30, TFT_RED);    // Pump
  lcd.fillRect(5, 45, 30, 30, TFT_RED);   // Gutter
  lcd.fillRect(5, 85, 30, 30, TFT_RED);   // MakeUp
  lcd.fillRect(5, 125, 30, 30, TFT_RED);  // Nozzle Ink
  lcd.fillRect(5, 165, 30, 30, TFT_RED);  // Nozzle Cleaning
  lcd.fillRect(5, 205, 30, 30, TFT_RED);  // Nozzle Vacuum
  lcd.fillRect(5, 245, 30, 30, TFT_RED);  // Nozzle MakeUp
  lcd.fillRect(5, 285, 30, 30, TFT_RED);  // Vent Waste Tank
  lcd.fillRect(5, 325, 30, 30, TFT_RED);  // Temperature Regulation
  lcd.fillRect(5, 365, 30, 30, TFT_RED);  // Viscosimeter
  lcd.fillRect(5, 405, 30, 30, TFT_RED);  // Waste Tank Full
  lcd.fillRect(5, 445, 30, 30, TFT_RED);  // CIJ Printer
  lcd.drawString("Pressure Pumps", 50, 15);
  lcd.drawString("Gutter Valve", 50, 55);
  lcd.drawString("MakeUp Pump", 50, 95);
  lcd.drawString("Nozzle Ink Valve", 50, 135);
  lcd.drawString("Nozzle Vacuum Valve", 50, 175);
  lcd.drawString("Viscosimeter", 50, 215);
  lcd.drawString("Flush Line", 50, 255);
  lcd.drawString("Circulation Pumps", 50, 295);
  lcd.drawString("Temperature Regulation", 50, 335);
  lcd.drawString("Return Pump", 50, 375);
  lcd.drawString("Not Used", 50, 415);
  lcd.drawString("WiFi", 50, 455);
  lcd.drawString("    Sunday", 720, 455);
  lcd.drawString("Viscosity:     0000 ms", 615, 5);

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

  //ADS1115
  InitAverages();

  //PID Temperature Control
  //ledcSetup(0, 1000, 8);
  //ledcAttachPin(PELTIER_PWM_PIN, 0);
  ledcAttach(PELTIER_PWM_PIN, 1000, 8);
  Input = 25;
  Setpoint = 25;
  //myPID.SetMode(AUTOMATIC);

  //Wifi
  WiFi.disconnect(true);
  WiFi.onEvent(WiFiStationConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
  WiFi.onEvent(WiFiGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFi.onEvent(WiFiStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println();
  Serial.println();
  Serial.println("Wait for WiFi... ");

  //Google Sheets
  GSheet.setPrerefreshSeconds(10 * 60);
  GSheet.begin(CLIENT_EMAIL, PROJECT_ID, PRIVATE_KEY);
}

//Valve Functions
//ON
void pump_on() {
  if (pump_button == false) {
    Serial.println("Pump ON");
    lcd.fillRect(5, 5, 30, 30, TFT_GREEN);
    pump_button = true;
    bitset(0b0000000000000001);
  }
}

void gutter_on() {
  if (gutter_button == false) {
    Serial.println("Gutter ON");
    lcd.fillRect(5, 45, 30, 30, TFT_GREEN);
    gutter_button = true;
    GUTTER_PIN = true;
    bitset(0b0000000000000010);
  }
}

void makeup_on() {
  if (makeup_button == false) {
    Serial.println("MakeUp ON");
    lcd.fillRect(5, 85, 30, 30, TFT_GREEN);
    makeup_button = true;
    MAKEUP_PIN = true;
    bitset(0b0000000000000100);
  }
}

void nozzle_ink_on() {
  if (nozzle_ink_button == false) {
    Serial.println("Nozzle Ink ON");
    lcd.fillRect(5, 125, 30, 30, TFT_GREEN);
    nozzle_ink_button = true;
    NOZZLE_INK_PIN = true;
    bitset(0b0000000000001000);
  }
}

void nozzle_vacuum_on() {
  if (nozzle_vacuum_button == false) {
    Serial.println("Nozzle Vacuum ON");
    lcd.fillRect(5, 165, 30, 30, TFT_GREEN);
    nozzle_vacuum_button = true;
    NOZZLE_VACUUM_PIN = true;
    bitset(0b0000000000010000);
  }
}

void viscosimeter_on() {
  if (viscosimeter_button == false) {
    Serial.println("Viscosimeter ON");
    lcd.fillRect(5, 205, 30, 30, TFT_GREEN);
    viscosimeter_button = true;
    bitset(0b0000000000100000);
  }
}

void flush_viscosimeter_on() {
  if (flush_viscosimeter_button == false) {
    Serial.println("Flush Viscosimeter ON");
    lcd.fillRect(5, 245, 30, 30, TFT_GREEN);
    flush_viscosimeter_button = true;
    FLUSH_VISCOSIMETER_PIN = true;
    bitset(0b0000000001000000);
  }
}

void circulation_pumps_on() {
  if (circulation_pumps_button == false) {
    Serial.println("Circulation Pumps ON");
    lcd.fillRect(5, 285, 30, 30, TFT_GREEN);
    circulation_pumps_button = true;
    CIRCULATION_TANKS_PIN = true;
    bitset(0b0000000010000000);
  }
}

void temperature_regulation_heat() {
  if (temperature_regulation_button == 0) {
    lcd.fillRect(5, 325, 30, 30, TFT_ORANGE);
    Serial.println("Temperature Regulation Heating");
    temperature_regulation_button = 1;
    bitset(0b0000000100000000);
  }
}

void temperature_regulation_cool() {
  if (temperature_regulation_button == 2) {
    lcd.fillRect(5, 325, 30, 30, TFT_BLUE);
    Serial.println("Temperature Regulation Cooling");
    temperature_regulation_button = 3;
    bitset(0b0000001000000000);
  }
}

void return_pump_on() {
  if (nozzle_cleaning_button == false) {
    Serial.println("Not Used ON");
    lcd.fillRect(5, 365, 30, 30, TFT_GREEN);
    nozzle_cleaning_button = true;
    NOZZLE_CLEANING_PIN = true;
    bitset(0b0000100000000000);
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
    bitclear(0b0000000000000001);
  }
}

void gutter_off() {
  if (gutter_button == true) {
    Serial.println("Gutter OFF");
    lcd.fillRect(5, 45, 30, 30, TFT_RED);
    gutter_button = false;
    GUTTER_PIN = false;
    bitclear(0b0000000000000010);
  }
}

void makeup_off() {
  if (makeup_button == true) {
    Serial.println("MakeUp OFF");
    lcd.fillRect(5, 85, 30, 30, TFT_RED);
    makeup_button = false;
    MAKEUP_PIN = false;
    bitclear(0b0000000000000100);
  }
}

void nozzle_ink_off() {
  if (nozzle_ink_button == true) {
    Serial.println("Nozzle Ink OFF");
    lcd.fillRect(5, 125, 30, 30, TFT_RED);
    nozzle_ink_button = false;
    NOZZLE_INK_PIN = false;
    bitclear(0b0000000000001000);
  }
}

void nozzle_vacuum_off() {
  if (nozzle_vacuum_button == true) {
    Serial.println("Nozzle Vacuum OFF");
    lcd.fillRect(5, 165, 30, 30, TFT_RED);
    nozzle_vacuum_button = false;
    NOZZLE_VACUUM_PIN = false;
    bitclear(0b0000000000010000);
  }
}

void viscosimeter_off() {
  if (viscosimeter_button == true) {
    Serial.println("Viscosimeter OFF");
    lcd.fillRect(5, 205, 30, 30, TFT_RED);
    viscosimeter_button = false;
    bitclear(0b0000000000100000);
  }
}

void flush_viscosimeter_off() {
  if (flush_viscosimeter_button == true) {
    Serial.println("Flush Viscosimeter OFF");
    lcd.fillRect(5, 245, 30, 30, TFT_RED);
    flush_viscosimeter_button = false;
    FLUSH_VISCOSIMETER_PIN = false;
    bitclear(0b0000000001000000);
  }
}

void circulation_pumps_off() {
  if (circulation_pumps_button == true) {
    Serial.println("Circulation Pumps OFF");
    lcd.fillRect(5, 285, 30, 30, TFT_RED);
    circulation_pumps_button = false;
    CIRCULATION_TANKS_PIN = false;
    bitclear(0b0000000010000000);
  }
}

void temperature_regulation_heat_off() {
  if (temperature_regulation_button == 1) {
    Serial.println("Temperature Regulation OFF");
    lcd.fillRect(5, 325, 30, 30, TFT_RED);
    temperature_regulation_button = 2;
    bitclear(0b0000001100000000);
  }
}

void temperature_regulation_cool_off() {
  if (temperature_regulation_button == 3) {
    Serial.println("Temperature Regulation OFF");
    lcd.fillRect(5, 325, 30, 30, TFT_RED);
    temperature_regulation_button = 0;
    bitclear(0b0000001100000000);
  }
}

void return_pump_off() {
  if (nozzle_cleaning_button == true) {
    Serial.println("Not Used OFF");
    lcd.fillRect(5, 365, 30, 30, TFT_RED);
    nozzle_cleaning_button = false;
    NOZZLE_CLEANING_PIN = false;
    bitclear(0b0000100000000000);
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

  // Viscosimeter Button
  if (viscosimeter_button == false) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 205 && touch_y <= 235) {
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
        if (touch_y >= 205 && touch_y <= 235) {
          last_x = touch_x;
          last_y = touch_y;
          viscosimeter_off();
        }
      }
    }
  }

  // Nozzle Vacuum Button
  if (nozzle_vacuum_button == false) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 165 && touch_y <= 195) {
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
        if (touch_y >= 165 && touch_y <= 195) {
          last_x = touch_x;
          last_y = touch_y;
          nozzle_vacuum_off();
        }
      }
    }
  }

  // Nozzle MakeUp Button
  if (flush_viscosimeter_button == false) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 245 && touch_y <= 275) {
          last_x = touch_x;
          last_y = touch_y;
          flush_viscosimeter_on();
        }
      }
    }
  }
  if (flush_viscosimeter_button == true) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 245 && touch_y <= 275) {
          last_x = touch_x;
          last_y = touch_y;
          flush_viscosimeter_off();
        }
      }
    }
  }

  // Vent Waste Tank Button
  if (circulation_pumps_button == false) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 285 && touch_y <= 315) {
          last_x = touch_x;
          last_y = touch_y;
          circulation_pumps_on();
        }
      }
    }
  }
  if (circulation_pumps_button == true) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 285 && touch_y <= 315) {
          last_x = touch_x;
          last_y = touch_y;
          circulation_pumps_off();
        }
      }
    }
  }

  // Temperature Regulation
  if (temperature_regulation_button == 0) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 325 && touch_y <= 355) {
          last_x = touch_x;
          last_y = touch_y;
          temperature_regulation_heat();
        }
      }
    }
  }
  if (temperature_regulation_button == 1) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 325 && touch_y <= 355) {
          last_x = touch_x;
          last_y = touch_y;
          temperature_regulation_heat_off();
        }
      }
    }
  }
  if (temperature_regulation_button == 2) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 325 && touch_y <= 355) {
          last_x = touch_x;
          last_y = touch_y;
          temperature_regulation_cool();
        }
      }
    }
  }
  if (temperature_regulation_button == 3) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 325 && touch_y <= 355) {
          last_x = touch_x;
          last_y = touch_y;
          temperature_regulation_cool_off();
        }
      }
    }
  }

  // Nozzle Cleaning Button
  if (nozzle_cleaning_button == false) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 365 && touch_y <= 395) {
          last_x = touch_x;
          last_y = touch_y;
          return_pump_on();
        }
      }
    }
  }
  if (nozzle_cleaning_button == true) {
    if (touch_x != last_x || touch_y != last_y) {
      if (touch_x >= 5 && touch_x <= 35) {
        if (touch_y >= 365 && touch_y <= 395) {
          last_x = touch_x;
          last_y = touch_y;
          return_pump_off();
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
  sprintf(dts_string, "%04u%02u%02u%02u%02u%02u.dat", years, months, days, hours, minutes, seconds);
  //Serial.println(dts_string);
}

void ds3231_read_temp() {
  Wire.beginTransmission(DS3231_ADDRESS);
  Wire.write(17);
  Wire.endTransmission();
  Wire.requestFrom(DS3231_ADDRESS, 2);
  ds3231_int_temp = (Wire.read() << 2);   // Read & left-shift the high 8 bits of the temperature
  ds3231_int_temp += (Wire.read() >> 6);  // Read the low 2 bits (fraction) of the temperature into the two LSBs of ds3231_int_temp
  if (ds3231_int_temp & 0x0200) {         // Handle negative values by moving the "sign bit" left 6 places
    ds3231_int_temp -= 0x0200;
    ds3231_int_temp |= 0x8000;
  }
  ftp_current.room_temperature = ds3231_int_temp;  // Update the room_temperature variable in ftp_current
  ds3231_temp = "   ";
  ds3231_temp += String(ds3231_int_temp / 4.0f);  // Convert to String representation after float-dividing by 4, to get correct printed output.
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
      received_chars[incoming_chars_index] = '\0';  // terminate the string
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
    //Serial.println(time_buffer);
    lcd.drawString(time_buffer, 720, 410);
    snprintf(date_buffer, sizeof(date_buffer), "%02u.%02u.%04u", days, months, years);
    //Serial.println(date_buffer);
    lcd.drawString(date_buffer, 720, 425);
    //Serial.println(ds3231_temp);
    ds3231_temp.concat(ds3231_temp_end_string);
    lcd.drawString(ds3231_temp, 710, 440);
    //Serial.println(current_day_of_week);
    lcd.drawString(current_day_of_week, 720, 455);
    //Serial.println("");
    timestamp = seconds;
    timestamp += minutes * 60;                             // 60 seconds per minute
    timestamp += hours * 60 * 60;                          // 60 minutes per hour
    timestamp += days * 24 * 60 * 60;                      // 24 hours per day
    timestamp += months * 31 * 24 * 60 * 60;               // 31 days(max) per month
    timestamp += (years - 1990) * 12 * 31 * 24 * 60 * 60;  // 12 months per year
    ftp_current.date_time = timestamp;
    //Serial.println(timestamp);
    //Serial.println(ftp_current.date_time);
    last_millis = current_millis;
  }
  set_clock();
}

//ADS1115
// This function just saves a lot of manual typing to initialize ADS_averages and ADS_buffers arrays to all '0's
void InitAverages() {
  for (uint8_t i = 0; i < 4; i++) {
    /*ADS_averages[i] = 0;*/
    for (uint8_t j = 0; j < 16; j++) {
      ADS_buffers[i][j] = 0;
    }
  }
}

// This function reads 4 analog values from the ADS1115; adds them to their respective ring buffers; then updates the values of ADS_averages to the new average values
// NOTE: this function is NOT thread-safe and can cause collisions if used improperly
void UpdateAverages() {
  static uint8_t ADS_bufferIndex = 0;  // Tracks current position in the ring buffers
  static uint8_t inputIndex = 0;       // Tracks which analog input we're sampling this cycle'
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
  for (uint8_t j = 0; j < ADS_BUFFERCOUNT; j++) {
    accumulator += ADS_buffers[inputIndex][j];
  }
  switch (inputIndex) {  // This is where you'll have to change the averaging code if you modify ADS_BUFFERCOUNT
    case 0:
      ftp_current.pressure = accumulator >> 4;
      break;
    case 1:
      ftp_current.tds = accumulator >> 4;
      break;
    case 2:
      ftp_current.viscosimeter_temperature = accumulator >> 4;
      break;
    case 3:
      ftp_current.nozzle_temperature = accumulator >> 4;
      break;
  }

  inputIndex++;
  if (inputIndex >= 4) {  // Reset the inputIndex to 0 after reading the fourth input (A3) and increment the bufferIndex for the ring buffers
    inputIndex = 0;
    ADS_bufferIndex++;                         // Increment the ring-buffer position for the next read
    if (ADS_bufferIndex >= ADS_BUFFERCOUNT) {  // Reset ring-buffer position if we've reached the end'
      ADS_bufferIndex = 0;
    }
  }
}

// This function will send values to the serial console, based on the current values in the ADS_averages[] array
void SendAverages() {
  static const String viscosimeterString = "Viscosimeter:  ";
  static const String nozzleString = "Nozzle:        ";
  static const String tdsString = "Conductivity:  ";
  static const String prsString = "Pressure:      ";
  static const char tempend[5] = { ' ', 247, 'C', ' ', 0 };
  static const String tempEnd = tempend;
  static const String tdsEnd = " ppm     ";
  static const String prsEnd = " psi     ";
  static String outString;


  // Pressure sensor == A0
  double ink_pressure = ftp_current.pressure * VOLTS_PER_STEP;  // Convert raw ADC data to voltage
  ink_pressure -= 0.5;                                          // 0.5v = 0psi
  ink_pressure *= 20;                                           // 20 psi change per volt

  // Format for constant-size output String
  if (ink_pressure >= 10) {
    outString = prsString + String(ink_pressure, 1) + prsEnd;
  } else {
    outString = prsString + " " + String(ink_pressure, 1) + prsEnd;
  }
  lcd.drawString(outString, 615, 35);  // Enable this in final code; commented out for testing without the display lib.
  //Serial.println(outString);

  // Temperature sensors == A2 & A3
  double temperature = CalculateTemperature(ftp_current.viscosimeter_temperature);
  if (temperature >= 10) {
    outString = viscosimeterString + String(temperature, 1) + tempEnd;
  } else {
    outString = viscosimeterString + " " + String(temperature, 1) + tempEnd;
  }
  lcd.drawString(outString, 615, 20);  // Enable this in final code; commented out for testing without the display lib.
  //Serial.println(outString);
  temperature = CalculateTemperature(ftp_current.nozzle_temperature);
  if (temperature >= 10) {
    outString = nozzleString + String(temperature, 1) + tempEnd;
  } else {
    outString = nozzleString + " " + String(temperature, 1) + tempEnd;
  }
  lcd.drawString(outString, 615, 65);  // Enable this in final code; commented out for testing without the display lib.
  //Serial.println(outString);


  // TDS sensor == A1
  double tdsVoltage = ftp_current.tds * VOLTS_PER_STEP;                // Calculate TDS sensor voltage from the raw average ADC reading
  double compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0);  // Temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
  tdsVoltage = tdsVoltage / compensationCoefficient;                   // Compensate reading for the actual measured temperature
  //convert voltage value to tds value
  double tdsValue = ((133.42 * tdsVoltage * tdsVoltage * tdsVoltage) - (255.86 * tdsVoltage * tdsVoltage) + (857.39 * tdsVoltage)) * 0.5;
  if (tdsValue >= 100) {
    outString = tdsString + " " + String(tdsValue, 0) + tdsEnd;
  } else if (tdsValue >= 10) {
    outString = tdsString + "  " + String(tdsValue, 0) + tdsEnd;
  } else {
    outString = tdsString + "   " + String(tdsValue, 0) + tdsEnd;
  }
  lcd.drawString(outString, 615, 50);  // Enable this in final code; commented out for testing without the display lib.
  //Serial.println(outString);
  //Serial.println("\n");  // Insert 2 blank lines, to make values easier to read.
}

// This function converts a raw average value into a calculated centigrade temperature
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

//ADS1115 Read
void ads1115_read() {
  static const uint16_t ADS_INTERVAL = 25;      // Timeout (in ms) between ADS1115 reads (per input, so effectively this number gets multiplied by 4)
  static const uint16_t SERIAL_INTERVAL = 250;  // Timeout (in ms) betweeen serial updates

  static unsigned long last_ADS_read_millis = 0;
  static unsigned long last_serial_update_millis = 0;

  if (current_millis > last_ADS_read_millis + ADS_INTERVAL) {
    UpdateAverages();
    last_ADS_read_millis = current_millis;
  }

  if (current_millis > last_serial_update_millis + SERIAL_INTERVAL) {
    SendAverages();
    last_serial_update_millis = current_millis;
  }
}

//Viscosimeter
void viscosimeter() {
  switch (viscosimeter_case) {
    case 0:  //Viscosimeter running?
      if (viscosimeter_button == true) {
        viscosimeter_pause_millis = current_millis;
        viscosimeter_case = 1;
      }
      //Serial.println("Case 0");
      break;
    case 1:  //Pause
      if (current_millis - viscosimeter_pause_millis >= viscosimeter_pause) {
        viscosimeter_pump_millis = current_millis;
        viscosimeter_case = 2;
      }
      if (viscosimeter_button == false) {
        viscosimeter_case = 0;
      }
      //Serial.println("Case 1");
      break;
    case 2:  //Lift Ball;
      viscosimeter_pump_on();
      if (current_millis - viscosimeter_pump_millis >= viscosimeter_pump_time) {
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
        viscosimeter_counter_millis = current_millis;
        viscosimeter_case = 4;
      }
      if (current_millis - viscosimeter_pause_millis >= viscosimeter_timeout) {
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
        ftp_current.fall_time = current_millis - viscosimeter_counter_millis;
        Serial.println("Viscosity: " + String(ftp_current.fall_time) + " ms");
        lcd.drawString("Viscosity:     " + String(ftp_current.fall_time) + " ms", 615, 5);
        viscosimeter_case = 0;
      }
      if (current_millis - viscosimeter_pause_millis >= viscosimeter_timeout) {
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

//Temperature Regulation
unsigned long heat_cool_transition_millis = 0;
void temperature_regulation() {
  switch (temperature_regulation_button) {
    case 0:  //Cooling Off
      myPID.SetMode(MANUAL);
      ledcWrite(PELTIER_PWM_PIN, 0);
      //Serial.println("Case 0");
      heat_cool_transition_millis = current_millis;
      break;
    case 1:  //Heating On
      if (circulation_pumps_button == true) {
        if (current_millis - heat_cool_transition_millis > 10000) {
          myPID.SetMode(AUTOMATIC);
          DIR_PIN_A = true;
          DIR_PIN_B = false;
          Input = CalculateTemperature(ftp_current.viscosimeter_temperature);
          myPID.SetControllerDirection(DIRECT);
          myPID.Compute();
          ledcWrite(PELTIER_PWM_PIN, Output);
          //Serial.println("Case 1");
        }
      } else {
        temperature_regulation_button = 0;
        lcd.fillRect(5, 325, 30, 30, TFT_RED);
      }
      break;
    case 2:  //Heating Off
      if (circulation_pumps_button == true) {
        myPID.SetMode(MANUAL);
        ledcWrite(PELTIER_PWM_PIN, 0);
        //Serial.println("Case 2");
        heat_cool_transition_millis = current_millis;
      } else {
        temperature_regulation_button = 0;
        lcd.fillRect(5, 325, 30, 30, TFT_RED);
      }
      break;
    case 3:  //Cooling On
      if (circulation_pumps_button == true) {
        if (current_millis - heat_cool_transition_millis > 10000) {
          myPID.SetMode(AUTOMATIC);
          DIR_PIN_A = false;
          DIR_PIN_B = true;
          Input = CalculateTemperature(ftp_current.viscosimeter_temperature);
          myPID.SetControllerDirection(REVERSE);
          myPID.Compute();
          ledcWrite(PELTIER_PWM_PIN, Output);
          //Serial.println("Case 3");
        }
      } else {
        temperature_regulation_button = 0;
        lcd.fillRect(5, 325, 30, 30, TFT_RED);
      }
      break;
  }
}

//WiFi
void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println("Connected to AP successfully!");
}

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info) {
  lcd.fillRect(5, 445, 30, 30, TFT_GREEN);
  bitset(0b0000010000000000);
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  lcd.fillRect(5, 445, 30, 30, TFT_RED);
  bitclear(0b0000010000000000);
  Serial.println("Disconnected from WiFi access point");
  Serial.print("WiFi lost connection. Reason: ");
  Serial.println(info.wifi_sta_disconnected.reason);
  Serial.println("Trying to Reconnect");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

//DB
static db_entry ftp_buffer[600];
void update_db() {
  static uint16_t ftp_index = 0;
  if (current_millis >= last_ftp_current + 1000) {
    last_ftp_current = current_millis;
    ftp_buffer[ftp_index] = ftp_current;  // Copies all data from ftp_current structure into ftp_buffer[ftp_index]
    ftp_index++;
    if (ftp_index >= 60) {  // Check to see if we've filled our buffer and, if so, reset our index & flush the buffer to ftp
      ftp_index = 0;
      //ftpSend(ftp_buffer, 600):            // Call a function to actually append the data from ftp_buffer[] to the logfile
      //datalogger();
    }
    //Serial.println("Mode " + String(myPID.GetMode()));
    //Serial.println("Dir " + String(myPID.GetDirection()));
    //Serial.println("Output " + String(Output));
    //Serial.println("");
  }
}

// //Datalogger
// unsigned long loggermillis = 0;
// bool file_created = false;
// static char current_logfile[19];
// void datalogger() {
// if (WiFi.status() == WL_CONNECTED) {
// ftp.OpenConnection();
// ftp.ChangeWorkDir(dirName);
// if (file_created == false) {
// for (uint8_t i = 0; i < 19; i++) {
// current_logfile[i] = dts_string[i];
// }
// file_created = true;
// }
// ftp.InitFile(COMMAND_XFER_TYPE_BINARY);
// ftp.AppendFile(current_logfile);
// for (uint16_t i = 0; i < 60; i++) {
// ftp.WriteData((uint8_t *)&ftp_buffer[i], sizeof(db_entry));
// }
// Serial.println("Appended");
// printDbEntry(ftp_current);
// ftp.CloseFile();
// ftp.CloseConnection();
// }
// }

//Check DB Entry
void printDbEntry(const db_entry record) {
  static uint16_t year, month, day, hour, minute, second;
  static uint32_t datestamp;
  static double temp_double;
  static char temp_string[128];

  // Re-format our 32-bit datestamp into human-readable format, then print it to Serial
  datestamp = record.date_time;
  second = datestamp % 60;  // Extract seconds first
  datestamp /= 60;
  minute = datestamp % 60;  // Then minutes
  datestamp /= 60;
  hour = datestamp % 24;  // Next hours
  datestamp /= 24;
  day = datestamp % 31;  // Next days
  datestamp /= 31;
  month = datestamp % 12;  // And months
  datestamp /= 12;
  year = datestamp + 1990;  // Finally years
  sprintf(temp_string, "Time & Date: %02u:%02u:%02u %02u/%02u/%04u\n", hour, minute, second, day, month, year);
  Serial.print(temp_string);

  // Print the viscometer fall time
  sprintf(temp_string, "Fall Time: %u ms\n", record.fall_time);
  Serial.print(temp_string);

  // Room Temperature
  temp_double = record.room_temperature / 4.0f;
  sprintf(temp_string, "Room Temperature: %.2fC\n", temp_double);
  Serial.print(temp_string);

  // Nozzle Temperature
  temp_double = CalculateTemperature(record.nozzle_temperature);
  sprintf(temp_string, "Nozzle Temperature: %.2fC\n", temp_double);
  Serial.print(temp_string);

  // Viscosimeter Temperature
  temp_double = CalculateTemperature(record.viscosimeter_temperature);
  sprintf(temp_string, "Viscosimeter Temperature: %.2fC\n", temp_double);
  Serial.print(temp_string);

  // Print the tds reading
  temp_double -= 25;
  temp_double *= 0.02;
  temp_double += 1.0;
  temp_double = record.tds / temp_double;
  temp_double *= VOLTS_PER_STEP;
  temp_double = ((133.42 * temp_double * temp_double * temp_double) - (255.86 * temp_double * temp_double) + (857.39 * temp_double)) * 0.5;
  sprintf(temp_string, "Conductivity: %.2f ppm\n", temp_double);
  Serial.print(temp_string);

  // Print the pressure
  temp_double = record.pressure * VOLTS_PER_STEP;
  temp_double -= 0.5;
  temp_double *= 20;
  sprintf(temp_string, "Pressure: %.2f psi\n\n", temp_double);
  Serial.print(temp_string);

  //Check Features
  //Pump
  if (ftp_current.feature_bit & 0b0000000000000001) {
    Serial.println("ON: Pump");
  } else {
    Serial.println("OFF: Pump");
  }

  //Gutter
  if (ftp_current.feature_bit & 0b0000000000000010) {
    Serial.println("ON: Gutter");
  } else {
    Serial.println("OFF: Gutter");
  }

  //MakeUp
  if (ftp_current.feature_bit & 0b000000000000100) {
    Serial.println("ON: MakeUp");
  } else {
    Serial.println("OFF: MakeUp");
  }

  //Nozzle Ink
  if (ftp_current.feature_bit & 0b0000000000001000) {
    Serial.println("ON: Nozzle Ink");
  } else {
    Serial.println("OFF: Nozzle Ink");
  }

  //Nozzle Vacuum
  if (ftp_current.feature_bit & 0b0000000000010000) {
    Serial.println("ON: Nozzle Vacuum");
  } else {
    Serial.println("OFF: Nozzle Vacuum");
  }

  //Viscosimeter
  if (ftp_current.feature_bit & 0b0000000000100000) {
    Serial.println("ON: Viscosimeter");
  } else {
    Serial.println("OFF: Viscosimeter");
  }

  //Flush Viscosimeter
  if (ftp_current.feature_bit & 0b0000000001000000) {
    Serial.println("ON: Flush Viscosimeter");
  } else {
    Serial.println("OFF: Flush Viscosimeter");
  }

  //Circulation Pumps
  if (ftp_current.feature_bit & 0b0000000010000000) {
    Serial.println("ON: Circulation Pumps");
  } else {
    Serial.println("OFF: Circulation Pumps");
  }

  //Heating
  if (ftp_current.feature_bit & 0b0000000100000000) {
    Serial.println("ON: Heating");
  } else {
    Serial.println("OFF: Heating");
  }

  //Cooling
  if (ftp_current.feature_bit & 0b0000001000000000) {
    Serial.println("ON: Cooling");
  } else {
    Serial.println("OFF: Cooling");
  }

  //WiFi
  if (ftp_current.feature_bit & 0b0000010000000000) {
    Serial.println("ON: WiFi");
  } else {
    Serial.println("OFF: WiFi");
  }

  //Return Pump
  if (ftp_current.feature_bit & 0b0000100000000000) {
    Serial.println("ON: Return Pump");
  } else {
    Serial.println("OFF: Return Pump");
  }

  Serial.println("");
}

void bitclear(const uint16_t bitmask) {
  ftp_current.feature_bit &= ~bitmask;  // AND feature_bit against inverted bitmask to leave all bits EXCEPT the bit being cleared in previous state, but force the bit being cleared to 0.
}

void bitset(const uint16_t bitmask) {
  ftp_current.feature_bit |= bitmask;
}

uint8_t pump_number = 1;
unsigned long pump_switching_millis = 0;
void pressure_pump_switching() {
  switch (pump_number) {
    case 0:  //Pump 0
      if (pump_button == true) {
        //Turn Pump 0 on
        PRESSURE_PUMP_0_PIN = true;
        if (current_millis - pump_switching_millis > 10000) {
          //Turn Pump 3 off
          PRESSURE_PUMP_3_PIN = false;
          if (current_millis - pump_switching_millis > 50000) {
            pump_switching_millis = current_millis;
            pump_number = 1;
          }
        }
      } else {
        pump_number = 4;
      }
      break;

    case 1:  //Pump 1
      if (pump_button == true) {
        //Turn Pump 1 on
        PRESSURE_PUMP_1_PIN = true;
        if (current_millis - pump_switching_millis > 10000) {
          //Turn Pump 0 off
          PRESSURE_PUMP_0_PIN = false;
          if (current_millis - pump_switching_millis > 50000) {
            pump_switching_millis = current_millis;
            pump_number = 2;
          }
        }
      } else {
        pump_number = 4;
      }
      break;

    case 2:  //Pump 2
      if (pump_button == true) {
        //Turn Pump 2 on
        PRESSURE_PUMP_2_PIN = true;
        if (current_millis - pump_switching_millis > 10000) {
          //Turn Pump 1 off
          PRESSURE_PUMP_1_PIN = false;
          if (current_millis - pump_switching_millis > 50000) {
            pump_switching_millis = current_millis;
            pump_number = 3;
          }
        }
      } else {
        pump_number = 4;
      }
      break;

    case 3:  //Pump 3
      if (pump_button == true) {
        //Turn Pump 3 on
        PRESSURE_PUMP_3_PIN = true;
        if (current_millis - pump_switching_millis > 10000) {
          //Turn Pump 2 off
          PRESSURE_PUMP_2_PIN = false;
          if (current_millis - pump_switching_millis > 50000) {
            pump_switching_millis = current_millis;
            pump_number = 0;
          }
        }
      } else {
        pump_number = 4;
      }
      break;

    case 4:  //Standby
      if (pump_button == false) {
        PRESSURE_PUMP_0_PIN = false;
        PRESSURE_PUMP_1_PIN = false;
        PRESSURE_PUMP_2_PIN = false;
        PRESSURE_PUMP_3_PIN = false;
      } else {
        pump_number = 0;
      }
      break;
  }
}

//Create Update Sheet
void create_update_sheet() {
  bool ready = GSheet.ready();
  if (ready == true && taskComplete == false) {
    String spreadsheetId;
    String spreadsheetURL;
    bool success = false;
    FirebaseJson response;
    FirebaseJson spreadsheet;
    FirebaseJsonData result;
    FirebaseJson valueRange;

    //Create Sheet
    spreadsheet.set("properties/title", "Test - Create Update and Read");  //Filename
    success = GSheet.create(&response, &spreadsheet, USER_EMAIL);

    //Update Sheet
    if (success == true) {
      response.get(result, FPSTR("spreadsheetId"));
      if (result.success == true) {
        spreadsheetId = result.to<const char *>();
        result.clear();
        response.get(result, FPSTR("spreadsheetUrl"));  // parse or deserialize the JSON response
      }
      if (result.success == true) {
        spreadsheetURL = result.to<const char *>();
        Serial.println("\nThe spreadsheet URL");
        Serial.println(spreadsheetURL);
      }
      valueRange.add("range", "Sheet1!A1:C3");
      valueRange.add("majorDimension", "COLUMNS");
      valueRange.set("values/[0]/[0]", "A1");  // column 1/row 1
      valueRange.set("values/[0]/[1]", "A2");  // column 1/row 2
      valueRange.set("values/[0]/[2]", "A3");  // column 1/row 3
      valueRange.set("values/[1]/[0]", "B1");  // column 2/row 1
      valueRange.set("values/[1]/[1]", "B2");  // column 2/row 2
      valueRange.set("values/[1]/[2]", "B3");  // column 2/row 3
      valueRange.set("values/[2]/[0]", "C1");  // column 3/row 1
      valueRange.set("values/[2]/[1]", "C2");  // column 3/row 2
      valueRange.set("values/[2]/[2]", "C3");  // column 3/row 3
      success = GSheet.values.update(&response, spreadsheetId, "Sheet1!A1:C3", &valueRange);
      response.toString(Serial, true);
      valueRange.clear();
      valueRange.add("range", "Sheet1!G1:I3");
      valueRange.add("majorDimension", "ROWS");
      valueRange.set("values/[0]/[0]", "G1");  // row 1/column 7
      valueRange.set("values/[1]/[0]", "G2");  // row 2/column 7
      valueRange.set("values/[2]/[0]", "G3");  // row 3/column 7
      valueRange.set("values/[0]/[1]", "H1");  // row 1/column 8
      valueRange.set("values/[1]/[1]", "H2");  // row 2/column 8
      valueRange.set("values/[2]/[1]", "H3");  // row 3/column 8
      valueRange.set("values/[0]/[2]", "I1");  // row 1/column 9
      valueRange.set("values/[1]/[2]", "I2");  // row 2/column 9
      valueRange.set("values/[2]/[2]", "I3");  // row 3/column 9
      success = GSheet.values.update(&response, spreadsheetId, "Sheet1!G1:I3", &valueRange);
      response.toString(Serial, true);
      valueRange.clear();
    }
    taskComplete = true;
  }
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
  viscosimeter();
  temperature_regulation();
  update_db();
  pressure_pump_switching();
  create_update_sheet();
}