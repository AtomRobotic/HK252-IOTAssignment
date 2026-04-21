#include "tinyml.h"
#include "environmental_logic.h"

extern QueueHandle_t xSensorQueue;
// Globals, for the convenience of one-shot setup.
namespace
{
    tflite::ErrorReporter *error_reporter = nullptr;
    const tflite::Model *model_ptr = nullptr;
    tflite::MicroInterpreter *interpreter = nullptr;
    TfLiteTensor *input = nullptr;
    TfLiteTensor *output = nullptr;
    constexpr int kTensorArenaSize = 10 * 1024; 
    uint8_t tensor_arena[kTensorArenaSize];
} // namespace

void setupTinyML()
{
    Serial.println("TensorFlow Lite Init....");
    static tflite::MicroErrorReporter micro_error_reporter;
    error_reporter = &micro_error_reporter;

    model_ptr = tflite::GetModel(model); 
    if (model_ptr->version() != TFLITE_SCHEMA_VERSION)
    {
        error_reporter->Report("Model provided is schema version %d, not equal to supported version %d.",
                               model_ptr->version(), TFLITE_SCHEMA_VERSION);
        return;
    }

    static tflite::AllOpsResolver resolver;
    static tflite::MicroInterpreter static_interpreter(
        model_ptr, resolver, tensor_arena, kTensorArenaSize, error_reporter);
    interpreter = &static_interpreter;

    TfLiteStatus allocate_status = interpreter->AllocateTensors();
    if (allocate_status != kTfLiteOk)
    {
        error_reporter->Report("AllocateTensors() failed");
        return;
    }

    input = interpreter->input(0);
    output = interpreter->output(0);

    Serial.println("TensorFlow Lite Micro initialized on ESP32.");
}

void tiny_ml_task(void *pvParameters)
{

    setupTinyML();
    SensorData_t data;
    while (1)
    {
        if (xQueueReceive(xSensorQueue, &data, portMAX_DELAY) == pdTRUE)
        {
            input->data.f[0] = data.temperature;
            input->data.f[1] = data.humidity;
            input->data.f[2] = data.soil_humidity;
            input->data.f[3] = data.light;

            TfLiteStatus invoke_status = interpreter->Invoke();
            if (invoke_status != kTfLiteOk)
            {
                error_reporter->Report("Invoke failed");
                continue;
            }

            int predicted_state = 0;
            float max_prob = output->data.f[0];
            
            for (int i = 1; i < 3; i++) {
                if (output->data.f[i] > max_prob) {
                    max_prob = output->data.f[i];
                    predicted_state = i;
                }
            }

            Serial.printf("[TINYML] Pred: %d (Prob: %.2f) -> T:%.1f, H:%.1f, S:%.1f, L:%.1f\n", 
                          predicted_state, max_prob, data.temperature, data.humidity, data.soil_humidity, data.light);

            // 4. Trigger environmental actions
            handle_environmental_actions((SystemState_t)predicted_state, data);

            vTaskDelay(pdMS_TO_TICKS(2000));
        }
    }
}