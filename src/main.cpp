#include "AudioTools.h"
#include "BluetoothA2DPSink.h"

I2SStream i2s;
BluetoothA2DPSink a2dp_sink(i2s);

void setup() {
    Serial.begin(115200);
    Serial.println("Initializing system with I2SStream...");

    // Configure I2SStream
    auto cfg = i2s.defaultConfig();
    cfg.pin_bck = 14;   // MAX98357 BCLK pin
    cfg.pin_ws = 15;    // MAX98357 LRC pin
    cfg.pin_data = 23;  // MAX98357 DIN pin 
    i2s.begin(cfg);

    // Set up Bluetooth audio handling
    a2dp_sink.set_stream_reader([](const uint8_t* data, uint32_t length) {
        Serial.printf("Received %d bytes of audio data.\n", length);
        i2s.write(data, length);
    });

    a2dp_sink.start("ESP32BT-I2S");
}

void loop() {
    // No need for manual polling—Bluetooth handles audio streaming automatically
}
