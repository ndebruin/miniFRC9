/**
 * Lights the ESP32's built-in LED when the "W" key is pressed in AlfredoConnect
 * connected to the ESP32's Bluetooth.
 * 
 * Detailed instructions can be found at https://github.com/AlfredoElectronics/AlfredoConnect-Receive/.
 */

#include <AlfredoConnect.h>
#include <BluetoothSerial.h>

const int LED_BUILTIN = 2;

BluetoothSerial bluetooth;

void setup() {
    bluetooth.begin("ESP32 Bluetooth");
    AlfredoConnect.begin(bluetooth);
    bluetooth.println("LEDControlBluetooth started");

    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
    
    if (AlfredoConnect.keyHeld(Key::W)) {
        digitalWrite(LED_BUILTIN, HIGH);
    } else {
        digitalWrite(LED_BUILTIN, LOW);
    }

    AlfredoConnect.update();
}