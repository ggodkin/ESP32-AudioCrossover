#include <Arduino.h>
#include <AudioTools.h>
#include <BluetoothA2DPSink.h>
#include <driver/i2s.h>

// Bluetooth A2DP sink
I2SStream i2s; 
BluetoothA2DPSink a2dp_sink(i2s);

void setup() { 
    auto cfg = i2s.defaultConfig(); 
    cfg.pin_bck = 14;   // MAX98357 BCLK pin
    cfg.pin_ws = 15;    // MAX98357 LRC pin
    cfg.pin_data = 23;  // MAX98357 DIN pin (shared for left & right) 
    i2s.begin(cfg); 
    a2dp_sink.start("ESP_BLE_XOver"); 
}
    

void loop() {
    // No need for continuous processing in the loop
}
