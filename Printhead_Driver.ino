//Variables n Stuff
#include <Wire.h>
#include "driver/rmt_tx.h"
#include "driver/mcpwm_prelude.h"
#define SENSOR_0 5
#define SENSOR_1 6
#define TOF_READING 7
#define SDA1 8
#define SCL1 9
#define SLAVE_ADDRESS 0x42
#define TIMEBASE_PERIOD 32
#define TIMEBASE_RESOLUTION_HZ 1000000
#define RMT_CLK_HZ 1000000
#define PWM_OUT_PIN_1 1
#define PWM_OUT_PIN_2 2
mcpwm_timer_handle_t timer = NULL;
mcpwm_oper_handle_t oper = NULL;
mcpwm_cmpr_handle_t comparatorA = NULL;
mcpwm_cmpr_handle_t comparatorB = NULL;
mcpwm_gen_handle_t generator1 = NULL;
mcpwm_gen_handle_t generator2 = NULL;
rmt_channel_handle_t channels[2];
rmt_encoder_handle_t encoders[2];
volatile int phaseShift = 0;
uint32_t tof_duration = 0;
uint32_t current_micros = 0;
bool sensor_0_error = false;
bool sensor_1_error = false;
bool missed = false;

//RMT Signals
const gpio_num_t tx_gpios[2] = { GPIO_NUM_10, GPIO_NUM_4 };
const rmt_symbol_word_t waveform_items[] = {
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 128, .level0 = 0, .duration1 = 130, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 128, .level0 = 0, .duration1 = 130, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 128, .level0 = 0, .duration1 = 130, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 128, .level0 = 0, .duration1 = 130, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 128, .level0 = 0, .duration1 = 130, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 128, .level0 = 0, .duration1 = 130, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 128, .level0 = 0, .duration1 = 130, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 128, .level0 = 0, .duration1 = 130, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 128, .level0 = 0, .duration1 = 130, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 128, .level0 = 0, .duration1 = 130, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 128, .level0 = 0, .duration1 = 130, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 128, .level0 = 0, .duration1 = 130, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 128, .level0 = 0, .duration1 = 130, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 128, .level0 = 0, .duration1 = 130, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 128, .level0 = 0, .duration1 = 130, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
  { .duration0 = 4240, .level0 = 0, .duration1 = 4242, .level1 = 0 }
};
const rmt_symbol_word_t waveform_items_ch1[] = {
  { .duration0 = 256, .level0 = 1, .duration1 = 16192, .level1 = 0 }
};

//Old
/* rmt_channel_handle_t tx_channels[4] = { NULL };
rmt_encoder_handle_t copyEncoder = NULL;
rmt_transmit_config_t transmitConfig = { .loop_count = -1 }; */

//Setup
void setup() {

  //Serial
  Serial.begin(115200);
  Serial.println("ESP32 I2C Slave Ready.");

  //I2C
  Wire.setPins(SDA1, SCL1);
  Wire.begin(SLAVE_ADDRESS);
  Wire.onRequest(requestEvent);

  //Pins for Testpoints
  pinMode(TOF_READING, OUTPUT);
  pinMode(SENSOR_0, INPUT);
  pinMode(SENSOR_1, INPUT);

  //RMT
  rmt_tx_channel_config_t tx_cfg0 = {
    .gpio_num = tx_gpios[0],
    .clk_src = RMT_CLK_SRC_DEFAULT,
    .resolution_hz = RMT_CLK_HZ,
    .mem_block_symbols = 144,
    .trans_queue_depth = 4,
    .flags = {
      .invert_out = false,
      .with_dma = false }
  };
  rmt_new_tx_channel(&tx_cfg0, &channels[0]);
  rmt_copy_encoder_config_t enc_cfg0 = {};
  rmt_new_copy_encoder(&enc_cfg0, &encoders[0]);
  rmt_enable(channels[0]);
  rmt_tx_channel_config_t tx_cfg1 = {
    .gpio_num = tx_gpios[1],
    .clk_src = RMT_CLK_SRC_DEFAULT,
    .resolution_hz = RMT_CLK_HZ,
    .mem_block_symbols = 48,
    .trans_queue_depth = 4,
    .flags = {
      .invert_out = false,
      .with_dma = false }
  };
  rmt_new_tx_channel(&tx_cfg1, &channels[1]);
  rmt_copy_encoder_config_t enc_cfg1 = {};
  rmt_new_copy_encoder(&enc_cfg1, &encoders[1]);
  rmt_enable(channels[1]);
  rmt_sync_manager_handle_t synchro = NULL;
  rmt_sync_manager_config_t synchro_config = {
    .tx_channel_array = channels,
    .array_size = sizeof(channels) / sizeof(channels[0]),
  };
  rmt_new_sync_manager(&synchro_config, &synchro);
  rmt_transmit_config_t tx_cfg = { .loop_count = -1 };
  rmt_transmit(channels[0], encoders[0], waveform_items, sizeof(waveform_items), &tx_cfg);
  rmt_transmit(channels[1], encoders[1], waveform_items_ch1, sizeof(waveform_items_ch1), &tx_cfg);

  //MCPWM
  mcpwm_timer_config_t timer_config = {
    .group_id = 0,
    .clk_src = MCPWM_TIMER_CLK_SRC_PLL160M,
    .resolution_hz = TIMEBASE_RESOLUTION_HZ,
    .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
    .period_ticks = TIMEBASE_PERIOD,
  };
  mcpwm_new_timer(&timer_config, &timer);
  mcpwm_operator_config_t operator_config = { .group_id = 0 };
  mcpwm_new_operator(&operator_config, &oper);
  mcpwm_operator_connect_timer(oper, timer);
  mcpwm_comparator_config_t cmp_config = { .flags = { .update_cmp_on_tep = true } };
  mcpwm_new_comparator(oper, &cmp_config, &comparatorA);
  mcpwm_comparator_set_compare_value(comparatorA, 16);
  mcpwm_new_comparator(oper, &cmp_config, &comparatorB);
  mcpwm_comparator_set_compare_value(comparatorB, 4);
  mcpwm_generator_config_t gen_config = {
    .gen_gpio_num = PWM_OUT_PIN_1,
    .flags = { .io_loop_back = true }
  };
  mcpwm_new_generator(oper, &gen_config, &generator1);
  gen_config.gen_gpio_num = PWM_OUT_PIN_2;
  mcpwm_new_generator(oper, &gen_config, &generator2);
  mcpwm_generator_set_action_on_timer_event(generator1, MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH));
  mcpwm_generator_set_action_on_compare_event(generator1, MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, comparatorA, MCPWM_GEN_ACTION_LOW));
  mcpwm_generator_set_action_on_timer_event(generator2, MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH));
  mcpwm_generator_set_action_on_compare_event(generator2, MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, comparatorB, MCPWM_GEN_ACTION_LOW));
  mcpwm_timer_enable(timer);
  mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP);

  //Old
  /*   gpio_num_t tx_gpio_number[4] = { GPIO_NUM_10, GPIO_NUM_4, GPIO_NUM_1, GPIO_NUM_2 };
  for (int i = 0; i < 4; i++) {
    rmt_tx_channel_config_t tx_chan_config = {
      .gpio_num = tx_gpio_number[i],
      .clk_src = RMT_CLK_SRC_DEFAULT,
      .resolution_hz = 1000000,
      .mem_block_symbols = 48,
      .trans_queue_depth = 1,
    };
    rmt_new_tx_channel(&tx_chan_config, &tx_channels[i]);
  }
  rmt_copy_encoder_config_t copyEncoderConfig = {};
  rmt_new_copy_encoder(&copyEncoderConfig, &copyEncoder);
  for (int i = 0; i < 4; i++) {
    rmt_enable(tx_channels[i]);
  }
  rmt_sync_manager_handle_t synchro = NULL;
  rmt_sync_manager_config_t synchro_config = {
    .tx_channel_array = tx_channels,
    .array_size = sizeof(tx_channels) / sizeof(tx_channels[0]),
  };
  rmt_new_sync_manager(&synchro_config, &synchro); */
}

//TOF Counter
void tof_counter() {
  const uint32_t tof_pulse_periode = 16448;
  static uint32_t sensor_0_last_time = 0;
  static uint32_t sensor_1_last_time = 0;
  static uint32_t sensor_0_on_time = 0;
  static uint32_t sensor_1_on_time = 0;
  static uint32_t sensor_0_off_time = 256;
  static uint32_t sensor_1_off_time = 256;
  static bool sensor_0_previous_state = HIGH;
  static bool sensor_1_previous_state = HIGH;
  static uint32_t sensor_0_on_start = 0;
  static uint32_t sensor_1_on_start = 0;
  static uint32_t sensor_0_off_start = 0;
  static uint32_t sensor_1_off_start = 0;
  static uint32_t sensor_0_mid_time = 0;
  static uint32_t sensor_1_mid_time = 0;
  static bool sensor_0_counter_active = false;
  static bool sensor_1_counter_active = false;
  static uint32_t sensor_1_off_times[5] = { 256, 256, 256, 256, 256 };
  static uint32_t sensor_0_off_times[5] = { 256, 256, 256, 256, 256 };
  static uint8_t sensor_1_index = 0;
  static uint8_t sensor_0_index = 0;
  static uint32_t sensor_1_stable_off_time = 256;
  static uint32_t sensor_0_stable_off_time = 256;
  static uint32_t sensor_0_timeout = 32896;
  static uint32_t sensor_1_timeout = 32896;
  static uint32_t tof_reading_high = 0;
  static uint32_t tof_reading_low = 0;
  static uint32_t since_last_sensor_0_reading = 0;
  static uint32_t since_last_sensor_1_reading = 0;
  bool sensor_0_current_state = REG_READ(GPIO_IN_REG) & (1 << SENSOR_0);
  bool sensor_1_current_state = REG_READ(GPIO_IN_REG) & (1 << SENSOR_1);
  if (sensor_0_current_state != sensor_0_previous_state) {
    if (sensor_0_current_state == HIGH) {
      sensor_0_on_time = current_micros - sensor_0_last_time;
      sensor_0_off_time = current_micros - sensor_0_on_start;
      sensor_0_off_times[sensor_0_index] = sensor_0_off_time;
      sensor_0_index = (sensor_0_index + 1) % 5;
      uint32_t sorted_off_times[5];
      memcpy(sorted_off_times, sensor_0_off_times, sizeof(sensor_0_off_times));
      for (int i = 0; i < 4; i++) {
        for (int j = i + 1; j < 5; j++) {
          if (sorted_off_times[i] > sorted_off_times[j]) {
            uint32_t temp = sorted_off_times[i];
            sorted_off_times[i] = sorted_off_times[j];
            sorted_off_times[j] = temp;
          }
        }
      }
      sensor_0_stable_off_time = sorted_off_times[2];
    } else {
      sensor_0_off_start = current_micros;
      sensor_0_on_start = current_micros;
      sensor_0_mid_time = sensor_0_on_start + (sensor_0_stable_off_time / 2);
      sensor_0_counter_active = true;
    }
    sensor_0_last_time = current_micros;
  }
  if (sensor_0_counter_active && (current_micros >= sensor_0_mid_time)) {
    REG_WRITE(GPIO_OUT_W1TS_REG, (1 << TOF_READING));
    tof_reading_high = current_micros;
    sensor_0_counter_active = false;
  }
  if (sensor_1_current_state != sensor_1_previous_state) {
    if (sensor_1_current_state == HIGH) {
      sensor_1_on_time = current_micros - sensor_1_last_time;
      sensor_1_off_time = current_micros - sensor_1_on_start;
      sensor_1_off_times[sensor_1_index] = sensor_1_off_time;
      sensor_1_index = (sensor_1_index + 1) % 5;
      uint32_t sorted_off_times[5];
      memcpy(sorted_off_times, sensor_1_off_times, sizeof(sensor_1_off_times));
      for (int i = 0; i < 4; i++) {
        for (int j = i + 1; j < 5; j++) {
          if (sorted_off_times[i] > sorted_off_times[j]) {
            uint32_t temp = sorted_off_times[i];
            sorted_off_times[i] = sorted_off_times[j];
            sorted_off_times[j] = temp;
          }
        }
      }
      sensor_1_stable_off_time = sorted_off_times[2];
    } else {
      sensor_1_off_start = current_micros;
      sensor_1_on_start = current_micros;
      sensor_1_mid_time = sensor_1_on_start + (sensor_1_stable_off_time / 2);
      sensor_1_counter_active = true;
    }
    sensor_1_last_time = current_micros;
  }
  if (sensor_1_counter_active && (current_micros >= sensor_1_mid_time)) {
    REG_WRITE(GPIO_OUT_W1TC_REG, (1 << TOF_READING));
    tof_reading_low = current_micros;
    sensor_1_counter_active = false;
  }
  if (!sensor_0_error && !sensor_1_error) {
    if (tof_reading_low > tof_reading_high) {
      tof_duration = tof_reading_low - tof_reading_high;
      missed = false;
    } else {
      tof_duration = tof_reading_high - tof_reading_low;
      tof_duration = tof_pulse_periode - tof_duration;
      missed = true;
    }
  } else {
    tof_duration = 0;
  }
  if (sensor_0_last_time != 0) {
    since_last_sensor_0_reading = current_micros - sensor_0_last_time;
    if (since_last_sensor_0_reading > sensor_0_timeout) {
      sensor_0_error = true;
    } else {
      sensor_0_error = false;
    }
  }
  if (sensor_1_last_time != 0) {
    since_last_sensor_1_reading = current_micros - sensor_1_last_time;
    if (since_last_sensor_1_reading > sensor_1_timeout) {
      sensor_1_error = true;
    } else {
      sensor_1_error = false;
    }
  }
  sensor_0_previous_state = sensor_0_current_state;
  sensor_1_previous_state = sensor_1_current_state;
}

//I2C Communication
void requestEvent() {
  Wire.write((uint8_t*)&tof_duration, sizeof(tof_duration));
  uint8_t s0_status = sensor_0_error ? 0 : 1;
  uint8_t s1_status = sensor_1_error ? 0 : 1;
  Wire.write(s0_status);
  Wire.write(s1_status);
}
void report_status() {
  static uint32_t lastPrintTime = 0;
  uint32_t current_millis = millis();
  if (current_millis - lastPrintTime >= 1000) {
    Serial.println(tof_duration);
    if (sensor_0_error == false) {
      Serial.println("S0:iO");
    } else {
      Serial.println("S0:NiO");
    }
    if (sensor_1_error == false) {
      Serial.println("S1:iO");
    } else {
      Serial.println("S1:NiO");
    }
    if (missed == true) {
      Serial.println("off time used");
    } else {
      Serial.println("on time used");
    }
    lastPrintTime = current_millis;
    Serial.println("");
  }
}

//RMT Signal Generation
/* void rmt_signal() {
  static unsigned long lastMillis = 0;
  if (millis() - lastMillis >= 1000) {
    lastMillis = millis();
    phaseShift++;
    if (phaseShift >= 16) {
      phaseShift = 0;
    }
    const rmt_symbol_word_t pulsePattern1[] = {
      { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
      { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
      { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
      { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
      { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
      { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
      { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
      { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
      { .duration0 = 7744, .level0 = 0, .duration1 = 0, .level1 = 0 },
    };
    rmt_symbol_word_t pulsePattern0WithPhaseShift[10];
    int pulseCount = 0;
    if (phaseShift < 16) {
      pulsePattern0WithPhaseShift[pulseCount++] = {
        .duration0 = phaseShift, .level0 = 0, .duration1 = phaseShift, .level1 = 0
      };
      for (int i = 0; i < 8 && pulseCount < 10; i++) {
        pulsePattern0WithPhaseShift[pulseCount++] = {
          .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0
        };
      }
      if (pulseCount < 10) {
        pulsePattern0WithPhaseShift[pulseCount++] = {
          .duration0 = 7744 - (phaseShift * 2), .level0 = 0, .duration1 = 0, .level1 = 0
        };
      }
    }
    const rmt_symbol_word_t pulsePattern2[] = {
      { .duration0 = 256, .level0 = 1, .duration1 = 7744, .level1 = 0 },
    };
    const rmt_symbol_word_t pulsePattern3[] = {
      { .duration0 = 16, .level0 = 1, .duration1 = 16, .level1 = 0 },
    };
    const rmt_symbol_word_t pulsePattern4[] = {
      { .duration0 = 4, .level0 = 1, .duration1 = 28, .level1 = 0 },
    };
    rmt_disable(tx_channels[0]);
    rmt_disable(tx_channels[1]);
    rmt_disable(tx_channels[2]);
    rmt_disable(tx_channels[3]);
    rmt_enable(tx_channels[0]);
    rmt_enable(tx_channels[1]);
    rmt_enable(tx_channels[2]);
    rmt_enable(tx_channels[3]);
    if (phaseShift != 0) {
      rmt_transmit(tx_channels[0], copyEncoder, pulsePattern0WithPhaseShift, pulseCount * sizeof(rmt_symbol_word_t), &transmitConfig);
    } else {
      rmt_transmit(tx_channels[0], copyEncoder, pulsePattern1, sizeof(pulsePattern1), &transmitConfig);
    }
    rmt_transmit(tx_channels[1], copyEncoder, pulsePattern2, sizeof(pulsePattern2), &transmitConfig);
    rmt_transmit(tx_channels[2], copyEncoder, pulsePattern3, sizeof(pulsePattern3), &transmitConfig);
    rmt_transmit(tx_channels[3], copyEncoder, pulsePattern4, sizeof(pulsePattern4), &transmitConfig);
  }
} */

//Loop
void loop() {
  tof_counter();
  report_status();
  current_micros = micros();
  //rmt_signal();
}