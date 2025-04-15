#include <Arduino.h>
#include <AudioTools.h>
#include <BluetoothA2DPSink.h>
#include <driver/i2s.h>

// Define cutoff frequency
const float CUTOFF_FREQ = 150.0;
const float SAMPLING_RATE = 44100.0;

// Initialize I2S streams
I2SStream i2s1;
I2SStream i2s2;

// Bluetooth A2DP sink
BluetoothA2DPSink a2dp_sink(i2s1);

// Define filter coefficients (example values)
float coef[] = {0.0209967345, 0.0960112308, 0.1460005493, 0.0960112308, 0.0209967345};

// Create filtered stream
FilteredStream<int16_t, float> filteredStream(i2s1, 2); // 2 channels

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

    // Apply BitwiseQuadFilter to each channel
    filteredStream.setFilter(0, new BitwiseQuadFilter<float>(coef)); // High-pass filter for channel 1
    filteredStream.setFilter(1, new BitwiseQuadFilter<float>(coef)); // Low-pass filter for channel 2
}

void loop() {
    while (a2dp_sink.is_connected()) {
        StreamCopy copier(i2s2, filteredStream); // Copy filtered audio to second I2S output
        copier.copy();
    }
}
