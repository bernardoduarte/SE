#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile

#define PIR_PIN 2

RH_ASK driver;

void setup()
{
    Serial.begin(9600);    // Debugging only
    if (!driver.init())
      Serial.println("init failed");
    pinMode(PIR_PIN, INPUT);
}

void loop()
{
    char msg[12] = "PRESENCA";
    int acionamento = digitalRead(PIR_PIN);
    if (acionamento == HIGH) {
      strcpy(msg, "AUSENCIA");
    }
    driver.send((uint8_t *)msg, strlen(msg));
    driver.waitPacketSent();
    delay(1000);
}
