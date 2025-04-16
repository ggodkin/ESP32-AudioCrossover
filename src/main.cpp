#include <Arduino.h>
#include <AudioTools.h>
#include <BluetoothA2DPSink.h>
#include <driver/i2s.h>

// Bluetooth A2DP sink
BluetoothA2DPSink a2dp_sink;

// I2S configuration for first channel (Master)
i2s_config_t i2s_cfg1 = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = 44100,  
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, 
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LOWMED,
    .dma_buf_count = 8,  
    .dma_buf_len = 512,
    .use_apll = false,  
    .tx_desc_auto_clear = true
};

// I2S configuration for second channel (Slave)
i2s_config_t i2s_cfg2 = {
    .mode = (i2s_mode_t)(I2S_MODE_SLAVE | I2S_MODE_TX),
    .sample_rate = 44100,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LOWMED,
    .dma_buf_count = 8,
    .dma_buf_len = 512,
    .use_apll = false,
    .tx_desc_auto_clear = true
};

// I2S pin settings
i2s_pin_config_t i2s_pins1 = {
    .bck_io_num = 14,
    .ws_io_num = 15,
    .data_out_num = 23,
    .data_in_num = I2S_PIN_NO_CHANGE
};

i2s_pin_config_t i2s_pins2 = {
    .bck_io_num = 26,
    .ws_io_num = 25,
    .data_out_num = 22,
    .data_in_num = I2S_PIN_NO_CHANGE
};

void setup() { 
    Serial.begin(115200);
    Serial.println("Initializing system with DMA-based I2S...");

    // Install and configure I2S0 (Master)
    esp_err_t result1 = i2s_driver_install(I2S_NUM_0, &i2s_cfg1, 0, NULL);
    if (result1 != ESP_OK) {
        Serial.printf("I2S0 install failed: %d\n", result1);
    }

    esp_err_t result2 = i2s_driver_install(I2S_NUM_1, &i2s_cfg2, 0, NULL);
    if (result2 != ESP_OK) {
        Serial.printf("I2S1 install failed: %d\n", result2);
    }

    i2s_set_pin(I2S_NUM_0, &i2s_pins1);
    i2s_set_pin(I2S_NUM_1, &i2s_pins2);

    // Set Bluetooth audio state callback
    a2dp_sink.set_on_audio_state_changed([](esp_a2d_audio_state_t state, void* obj) {
        if (state == ESP_A2D_AUDIO_STATE_STARTED) {
            Serial.println("Bluetooth audio streaming started!");
        }
    });

    // Start Bluetooth A2DP sink
    a2dp_sink.start("ESP_BLE_XOver"); 
}

void loop() {
    if (!a2dp_sink.is_connected()) {
        return;
    }

    size_t bytes_read;
    uint8_t buffer[512];

    // No need to call i2s_read(); just send Bluetooth audio directly to I2S
    bytes_read = sizeof(buffer);
    memset(buffer, 0, sizeof(buffer));  // Initialize buffer to avoid unexpected noise

    if (bytes_read > 0 && buffer != nullptr) {
        i2s_write(I2S_NUM_0, buffer, bytes_read, &bytes_read, portMAX_DELAY);
        i2s_write(I2S_NUM_1, buffer, bytes_read, &bytes_read, portMAX_DELAY);
    }
}
