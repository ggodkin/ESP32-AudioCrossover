#include <Arduino.h>
#include <AudioTools.h>
#include <BluetoothA2DPSink.h>
#include <driver/i2s.h>

// Initialize two I2S streams
I2SStream i2s1;
I2SStream i2s2;

// Bluetooth A2DP sink
BluetoothA2DPSink a2dp_sink(i2s1);

void setup() { 
    // Configure first I2S channel
    auto cfg1 = i2s1.defaultConfig(); 
    cfg1.pin_bck = 14;   // First MAX98357 BCLK pin
    cfg1.pin_ws = 15;    // First MAX98357 LRC pin
    cfg1.pin_data = 23;  // First MAX98357 DIN pin
    i2s1.begin(cfg1); 

    // Configure second I2S channel
    auto cfg2 = i2s2.defaultConfig(); 
    cfg2.pin_bck = 26;   // Second MAX98357 BCLK pin
    cfg2.pin_ws = 25;    // Second MAX98357 LRC pin
    cfg2.pin_data = 22;  // Second MAX98357 DIN pin
    i2s2.begin(cfg2); 

    // Start Bluetooth A2DP sink
    a2dp_sink.start("ESP_BLE_XOver"); 
}

void loop() {
    // Duplicate audio output to both I2S channels (simple approach)
    while (a2dp_sink.is_connected()) { // Correct method name
        size_t bytes_read;
        uint8_t buffer[1024];

        bytes_read = i2s1.readBytes(buffer, sizeof(buffer)); // Correct function usage
        if (bytes_read > 0) {
            i2s2.write(buffer, bytes_read);
        }
    }
}
