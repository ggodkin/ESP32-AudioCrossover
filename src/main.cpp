#include "AudioTools.h"
#include "BluetoothA2DPSink.h"

I2SStream i2s1;
I2SStream i2s2; // Second I2S instance
BluetoothA2DPSink a2dp_sink(i2s1);

// Create a filtered stream for i2s2
FilteredStream<int16_t, float> filteredI2S2(i2s2, 2);

// Define FIR filter coefficients for a 150Hz low-pass filter
float coef[] = {0.0209967345, 0.0960112308, 0.1460005493, 0.0960112308, 0.0209967345};

void setup() {
    Serial.begin(115200);
    Serial.println("Initializing system with dual I2S and filtering...");

    // Configure first I2S instance
    auto cfg1 = i2s1.defaultConfig();
    cfg1.pin_bck = 14;
    cfg1.pin_ws = 15;
    cfg1.pin_data = 23;
    i2s1.begin(cfg1);

    // Configure second I2S instance
    auto cfg2 = i2s2.defaultConfig();
    cfg2.pin_bck = 26;
    cfg2.pin_ws = 25;
    cfg2.pin_data = 22;
    i2s2.begin(cfg2);

    // Apply FIR filter to second I2S channel
    filteredI2S2.setFilter(0, new FIR<float>(coef));
    filteredI2S2.setFilter(1, new FIR<float>(coef));

    // Start Bluetooth A2DP sink
    a2dp_sink.start("ESP32BT-I2S");
}

void loop() {
    // Copy filtered audio to output
    StreamCopy copier(i2s2, filteredI2S2);
    copier.copy();
}
