#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile

#define PIR_PIN 7

RH_ASK rf_driver(2000, 11, 12);

void setup()
{
    Serial.begin(9600);    // Debugging only
    if (! rf_driver.init())
      Serial.println("init failed");
    pinMode(PIR_PIN, INPUT);
}

void loop()
{
    int acionamento = digitalRead(PIR_PIN);
    if (acionamento == HIGH) {
      char msg[16] = "PRESENCA";
      rf_driver.send((uint8_t *)msg, strlen(msg));
      rf_driver.waitPacketSent();
    }
}
