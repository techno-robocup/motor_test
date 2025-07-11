#include <Arduino.h>
#include "motorio.hpp"
#include "mutex_guard.hpp"
#include "serial_io.hpp"

SerialIO serial;

constexpr int tyre_1 = 13, tyre_2 = 14, tyre_3 = 15, tyre_4 = 16;
constexpr int button_pin = 21;
constexpr int arm_feedback = 34, arm_pulse = 17;
constexpr int wire_SIG = 32;
constexpr int tyre_interval = 1500;

// Fixed array size to match actual usage (4 motors, not 2)
int tyre_values[4] = {1700, 1700, 1700, 1700};
int arm_value = 0;
bool wire = false;

MOTORIO tyre_1_motor(tyre_1, tyre_interval), tyre_2_motor(tyre_2, tyre_interval),
    tyre_3_motor(tyre_3, tyre_interval), tyre_4_motor(tyre_4, tyre_interval);

inline int readbutton() { return digitalRead(button_pin); }

TaskHandle_t motor_task;
SemaphoreHandle_t motor_sem = xSemaphoreCreateMutex();

long double current_speed = 1000.0;

void motor_task_func(void* arg) {
  while (true) {
    current_speed += 0.1;
    if (current_speed > 2000) current_speed = 1000.0;
    MutexGuard guard(motor_sem);
    // tyre_1_motor.run_msec((int)current_speed);
    // tyre_2_motor.run_msec((int)current_speed);
    // tyre_3_motor.run_msec((int)current_speed);
    // tyre_4_motor.run_msec((int)current_speed);
    tyre_1_motor.run_msec(1500);
    tyre_2_motor.run_msec(1500);
    tyre_3_motor.run_msec(1500);
    tyre_4_motor.run_msec(1500);

    // vTaskDelay(pdMS_TO_TICKS(100));
    // serial.sendMessage(Message(1, "motor_task_func running"));
    // vTaskDelay(pdMS_TO_TICKS(1));
  }
}

// Optimized string parsing without String operations
bool parseMotorCommand(const char* message, int* values, int max_values) {
  int idx = 0;
  const char* ptr = message;

  while (idx < max_values && *ptr) {
    // Skip leading spaces
    while (*ptr == ' ') ptr++;
    if (!*ptr) break;

    // Parse number
    int val = 0;
    bool negative = false;

    if (*ptr == '-') {
      negative = true;
      ptr++;
    }

    while (*ptr >= '0' && *ptr <= '9') {
      val = val * 10 + (*ptr - '0');
      ptr++;
    }

    if (negative) val = -val;
    values[idx++] = val;

    // Skip to next space or end
    while (*ptr == ' ') ptr++;
  }

  return idx == max_values;
}

void setup() {
  esp_task_wdt_deinit();
  serial.init();
  pinMode(button_pin, INPUT);
  /*
  ARGS for xTaskCreatePinnedToCore:
  - Task function
  - Task name
  - Stack size (reduced from 10000 to 2048)
  - Task parameter
  - Task priority
  - Task handle
  - Core ID
  */
  xTaskCreatePinnedToCore(motor_task_func, "MotorTask", 10000, nullptr, 1, &motor_task, 0);
}

void loop() {
  // for (int i = 1000; i <= 2000; i += 10) {
  //   MutexGuard guard(motor_sem);
  //   tyre_values[0] = tyre_values[1] = tyre_values[2] = tyre_values[3] = i;
  //   tyre_1_motor.run_msec(tyre_values[0]);
  //   tyre_2_motor.run_msec(tyre_values[1]);
  //   tyre_3_motor.run_msec(tyre_values[2]);
  //   tyre_4_motor.run_msec(tyre_values[3]);
  //   serial.sendMessage(Message(1, "MOTOR " + String(tyre_values[0]) + " " +
  //   String(tyre_values[1]) +
  //                                     " " + String(tyre_values[2]) + " " +
  //                                     String(tyre_values[3])));
  // }
  // vTaskDelay(100);
  Serial.println((int)current_speed);
}
