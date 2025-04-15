#include <Arduino.h>
#include <AudioTools.h>
#include <BluetoothA2DPSink.h>
#include <driver/i2s.h>

// Cutoff frequency and sampling rate
const float CUTOFF_FREQ = 150.0f;
const float SAMPLING_RATE = 44100.0f;

// Initialize I2S streams
I2SStream i2s1;
I2SStream i2s2;

// Bluetooth A2DP sink
BluetoothA2DPSink a2dp_sink(i2s1);

// Define biquad filter coefficients as float (not double)
const float hp_b_coefficients[] = {1.0f, -2.0f, 1.0f};
const float hp_a_coefficients[] = {1.0f, -1.8f, 0.81f};

const float lp_b_coefficients[] = {0.02f, 0.04f, 0.02f};
const float lp_a_coefficients[] = {1.0f, -1.5f, 0.7f};

// Create filtered stream
FilteredStream<int16_t, float> inFiltered(i2s1, 2); // 2 channels

void setup() {
    Serial.begin(115200);

    // Configure first I2S channel
    auto cfg1 = i2s1.defaultConfig();
    cfg1.pin_bck = 14;
    cfg1.pin_ws = 15;
    cfg1.pin_data = 23;
    i2s1.begin(cfg1);

    // Configure second I2S channel
    auto cfg2 = i2s2.defaultConfig();
    cfg2.pin_bck = 26;
    cfg2.pin_ws = 25;
    cfg2.pin_data = 22;
    i2s2.begin(cfg2);

    // Start Bluetooth A2DP sink
    a2dp_sink.start("ESP_BLE_XOver");

    // Apply BiQuadDF2 filters with correct float type and gain
    inFiltered.setFilter(0, new BiQuadDF2<float>(hp_b_coefficients, hp_a_coefficients, 1.0f)); // High-pass filter for Channel 1
    inFiltered.setFilter(1, new BiQuadDF2<float>(lp_b_coefficients, lp_a_coefficients, 1.0f)); // Low-pass filter for Channel 2
}

void loop() {
    while (a2dp_sink.is_connected()) {
        StreamCopy copier(i2s2, inFiltered); // Copy filtered audio to second I2S output
        copier.copy();
    }
}
