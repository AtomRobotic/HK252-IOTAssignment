#include "tinyml.h"
#include "common.h"

const float T_MEAN = 32.4; 
const float T_STD = 7.3;  
const float H_MEAN = 56.0;
const float H_STD = 20.4;  

namespace {
tflite::ErrorReporter *error_reporter = nullptr;
const tflite::Model *model_ptr = nullptr;
tflite::MicroInterpreter *interpreter = nullptr;
TfLiteTensor *input = nullptr;
TfLiteTensor *output = nullptr;
constexpr int kTensorArenaSize =
    16 * 1024; // Tăng lên 16KB cho mạng nơ-ron 2 lớp ẩn
uint8_t tensor_arena[kTensorArenaSize];
} // namespace

void setupTinyML() {
  Serial.println("--- TinyML Classification Task Setup ---");
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  model_ptr = tflite::GetModel(model);

  static tflite::AllOpsResolver resolver;
  static tflite::MicroInterpreter static_interpreter(
      model_ptr, resolver, tensor_arena, kTensorArenaSize, error_reporter);
  interpreter = &static_interpreter;

  if (interpreter->AllocateTensors() != kTfLiteOk) {
    error_reporter->Report("AllocateTensors() failed");
    return;
  }

  input = interpreter->input(0);
  output = interpreter->output(0);
  Serial.println("TinyML Model Ready.");
}

void tiny_ml_task(void *pvParameters) {
  AppContext *app = (AppContext *)pvParameters;
  setupTinyML();

  if (input == nullptr) {
    vTaskDelete(NULL);
    return;
  }

  SensorData rcvSensorData;
  while (1) {
    if (xQueuePeek(app->xQueueSensor, &rcvSensorData, pdMS_TO_TICKS(100)) ==
        pdTRUE) {

      float normalized_temp = (rcvSensorData.temperature - T_MEAN) / T_STD;
      float normalized_humid = (rcvSensorData.humidity - H_MEAN) / H_STD;

      input->data.f[0] = normalized_temp;
      input->data.f[1] = normalized_humid;

      if (interpreter->Invoke() == kTfLiteOk) {
        int predicted_state = 0;
        float max_prob = output->data.f[0];

        for (int i = 1; i < 4; i++) {
          if (output->data.f[i] > max_prob) {
            max_prob = output->data.f[i];
            predicted_state = i;
          }
        }

        const char *states[] = {"NORMAL", "FAN_ONLY", "PUMP_ONLY", "BOTH_ON"};
        Serial.printf(
            "[TINYML] Input: T=%.1f, H=%.1f -> Pred: %s (Prob: %.2f)\n",
            rcvSensorData.temperature, rcvSensorData.humidity,
            states[predicted_state], max_prob);

        app->ml_predicted_state = predicted_state;
      }
    }
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}
