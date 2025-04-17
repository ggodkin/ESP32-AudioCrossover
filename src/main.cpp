#include "AudioTools.h"
#include "BluetoothA2DPSink.h"

I2SStream i2s1;
I2SStream i2s2; // Second I2S instance
BluetoothA2DPSink a2dp_sink(i2s1);

void setup() {
    Serial.begin(115200);
    Serial.println("Initializing system with dual I2S...");

    // Configure first I2S instance
    auto cfg1 = i2s1.defaultConfig();
    cfg1.pin_bck = 14;   // MAX98357 BCLK pin
    cfg1.pin_ws = 15;    // MAX98357 LRC pin
    cfg1.pin_data = 23;  // MAX98357 DIN pin 
    i2s1.begin(cfg1);

    // Configure second I2S instance
    auto cfg2 = i2s2.defaultConfig();
    cfg2.pin_bck = 26;   // Second I2S BCLK pin
    cfg2.pin_ws = 25;    // Second I2S LRC pin
    cfg2.pin_data = 22;  // Second I2S DIN pin
    i2s2.begin(cfg2);

    // Start Bluetooth A2DP sink
    a2dp_sink.start("ESP32BT-I2S");
}

void loop() {
    // No need for manual polling—Bluetooth handles audio streaming automatically
}
