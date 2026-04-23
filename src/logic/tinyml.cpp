#include "common.h"

#include "environmental_logic.h"

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

  // Đảm bảo tên mảng model trong dht_anomaly_model.h khớp ở đây
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
    // Đọc dữ liệu từ Queue (đợi tối đa 100ms)
    if (xQueuePeek(app->xQueueSensor, &rcvSensorData, pdMS_TO_TICKS(100)) ==
        pdTRUE) {

      // 1. TIỀN XỬ LÝ: Chuẩn hóa dữ liệu y = (x - mean) / std
      float normalized_temp = rcvSensorData.temperature;
      float normalized_humid = rcvSensorData.humidity;

      input->data.f[0] = normalized_temp;
      input->data.f[1] = normalized_humid;

      // 2. CHẠY INFERENCE
      if (interpreter->Invoke() == kTfLiteOk) {
        // 3. HẬU XỬ LÝ: Tìm nhãn có xác suất cao nhất (Softmax output)
        int predicted_state = 0;
        float max_prob = output->data.f[0];

        for (int i = 1; i < 4; i++) { // Duyệt qua 4 nhãn 0, 1, 2, 3
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

        // Cập nhật trạng thái ML ra biến toàn cục để điều khiển Quạt/Bơm
        ml_predicted_state = predicted_state;
      }
    }
    // Luôn delay để tránh lỗi Watchdog và nhường CPU cho task khác
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}
