#include <Arduino.h>
#include <Encoder.h>

Encoder tempEncoder = Encoder(14, 15);
Encoder frontLeft = tempEncoder;
Encoder frontRight = tempEncoder;
Encoder backLeft = tempEncoder;
Encoder backRight = tempEncoder;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  // disable serial port rx
  UCSR1B &= ~(1<<RXEN1);

  frontLeft = Encoder(0, 4);
  frontRight = Encoder(2, 5);
  backLeft = Encoder(3, 6);
  backRight = Encoder(7, 8);
}

unsigned long lastSend = millis();

unsigned long lastBlink = millis();
bool blinkState = true;

void loop() {
  String sendStr = (String(frontLeft.read()) + "," + String(frontRight.read()) + "," + String(backLeft.read()) + "," + String(backRight.read()));
  // send every 50ms
  if(millis() - lastSend > 50){
    lastSend = millis();
    Serial.println(sendStr);
  }
  // dogwatch led blink every 0.5s
  if(millis() - lastBlink > 500){
    lastBlink = millis();
    digitalWrite(LED_BUILTIN_RX, blinkState);
    blinkState = !blinkState;
  }

}