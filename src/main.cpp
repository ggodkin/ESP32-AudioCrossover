#include <Arduino.h>
#include <AudioTools.h>
#include <BluetoothA2DPSink.h>
#include <driver/i2s.h>

// Create Bluetooth A2DP sink
BluetoothA2DPSink a2dp_sink;

// Define pin configurations for subwoofer (low frequencies)
i2s_pin_config_t subwoofer_pin_config = {
    .bck_io_num = 26,  // Bit clock pin
    .ws_io_num = 25,   // Word select pin
    .data_out_num = 22 // Data output pin
};

// Define pin configurations for other frequencies (high frequencies)
i2s_pin_config_t other_audio_pin_config = {
    .bck_io_num = 14,  // Bit clock pin
    .ws_io_num = 15,   // Word select pin
    .data_out_num = 23 // Data output pin
};

// Split frequency parameter
int split_frequency = 200; // Set split frequency in Hz

// Process audio data and split into subwoofer and other streams
void process_audio_data(const uint8_t *data, uint32_t length) {
    size_t bytes_written;
    const int16_t *samples = (const int16_t *)data;
    uint32_t sample_count = length / sizeof(int16_t);

    for (uint32_t i = 0; i < sample_count; i++) {
        int16_t value = samples[i];
        if (value < split_frequency) {
            // Send to subwoofer stream
            i2s_write(I2S_NUM_0, &value, sizeof(value), &bytes_written, portMAX_DELAY);
        } else {
            // Send to other audio stream
            i2s_write(I2S_NUM_1, &value, sizeof(value), &bytes_written, portMAX_DELAY);
        }
    }
}

void setup() {
    Serial.begin(115200);

    // Configure I2S
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = 44100,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_MSB,
        .dma_buf_count = 8,
        .dma_buf_len = 64,
        .use_apll = false,
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0
    };

    // Install drivers for subwoofer pins
    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &subwoofer_pin_config);

    // Install drivers for other audio pins
    i2s_driver_install(I2S_NUM_1, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_1, &other_audio_pin_config);

    // Set the A2DP stream reader callback to process the incoming BLE audio
    a2dp_sink.set_stream_reader(process_audio_data);
    a2dp_sink.start("ESP_BLE_XOver");
}

void loop() {
    // Main loop remains empty for this setup
}
