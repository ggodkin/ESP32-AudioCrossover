#include <Arduino.h>
#include <AudioTools.h>
#include <BluetoothA2DPSink.h>
#include <driver/i2s.h>

// Bluetooth A2DP sink
BluetoothA2DPSink a2dp_sink;

// Define shared I2S pin configuration for stereo output (both amplifiers)
i2s_pin_config_t stereo_pin_config = {
    .bck_io_num = 14,  // MAX98357 BCLK pin
    .ws_io_num = 15,   // MAX98357 LRC pin
    .data_out_num = 23  // MAX98357 DIN pin (shared for left & right)
};

// Define I2S pin configuration for subwoofer output (separate MAX98357)
i2s_pin_config_t subwoofer_pin_config = {
    .bck_io_num = 26,  
    .ws_io_num = 25,   
    .data_out_num = 22  
};

// Low-pass filter for subwoofer isolation (basic averaging)
int16_t low_pass_filter(int16_t sample) {
    static int16_t prev_sample = 0;
    int16_t filtered = (sample + prev_sample) / 2; 
    prev_sample = sample;
    return filtered;
}

// Process incoming stereo audio and split frequencies
void process_audio_data(const uint8_t *data, uint32_t length) {
    size_t bytes_written;
    const int16_t *samples = (const int16_t *)data;
    uint32_t sample_count = length / sizeof(int16_t);

    for (uint32_t i = 0; i < sample_count; i += 2) { // Process stereo
        int16_t left_sample = samples[i];     // Left channel
        int16_t right_sample = samples[i+1];  // Right channel

        // Low-pass filter for subwoofer output
        int16_t sub_sample = low_pass_filter((left_sample + right_sample) / 2);

        // Send subwoofer data to I2S_NUM_1
        i2s_write(I2S_NUM_1, &sub_sample, sizeof(sub_sample), &bytes_written, portMAX_DELAY);

        // Send stereo data through a single I2S output (I2S_NUM_0)
        i2s_write(I2S_NUM_0, &samples[i], sizeof(int16_t) * 2, &bytes_written, portMAX_DELAY);
    }
}

void setup() {
    Serial.begin(115200);

    // Stereo I2S configuration (shared across both left & right amplifiers)
    i2s_config_t i2s_config_stereo = {
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

    // Subwoofer I2S configuration (same settings but separate I2S instance)
    i2s_config_t i2s_config_subwoofer = i2s_config_stereo;
    i2s_config_subwoofer.channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT;

    // Install stereo I2S driver for left/right MAX98357
    i2s_driver_install(I2S_NUM_0, &i2s_config_stereo, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &stereo_pin_config);

    // Install subwoofer I2S driver
    i2s_driver_install(I2S_NUM_1, &i2s_config_subwoofer, 0, NULL);
    i2s_set_pin(I2S_NUM_1, &subwoofer_pin_config);

    // Set Bluetooth A2DP stream processing function
    a2dp_sink.set_stream_reader(process_audio_data);
    a2dp_sink.start("ESP_BLE_XOver");
}

void loop() {
    // No need for continuous processing in the loop
}
