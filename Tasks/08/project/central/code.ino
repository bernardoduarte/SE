#include "RTClib.h"
#include <SPI.h>
#include <SD.h>
#include <RH_ASK.h>

//Pino CS do cartao SD
int Pino_CS_SD = 10;
//Pino CS do módulo RF
int Pino_CS_RF_RX = 7;
int Pino_CS_RF_TX = 6;

//Pinos LED RGB
int Pino_RGB_RED = 4;
int Pino_RGB_GREEN = 3;
int Pino_RGB_BLUE = 2;

//Modulos
RTC_DS1307 rtc;
File file;
RH_ASK rf_driver(5000, Pino_CS_RF_RX, Pino_CS_RF_TX);

//Protocolo
struct mymessage {
  uint8_t sender;
  uint8_t receiver;
  uint8_t source;
};
typedef struct mymessage myMsg;

//FSM
int STATE;
unsigned int time0;

void state_1(unsigned int now, unsigned int red, unsigned int green, unsigned int blue) {
  digitalWrite(Pino_RGB_RED, red);
  digitalWrite(Pino_RGB_GREEN, green);
  digitalWrite(Pino_RGB_BLUE, blue);
  time0 = now;
  STATE = 1;
}

void setup() {
  // put your setup code here, to run once:
  bool erro = false;
  pinMode(Pino_RGB_RED, OUTPUT);
  pinMode(Pino_RGB_GREEN, OUTPUT);
  pinMode(Pino_RGB_BLUE, OUTPUT);
  
  Serial.begin(115200);
  
  Serial.println("Iniciando cartao SD...");
  if (! SD.begin(Pino_CS_SD)) {
    erro = true;
    Serial.println("Falha na inicializacao do SD!");
  }
  else {
    Serial.println("Cartao SD iniciado. OK");
  }

  Serial.println("Iniciando modulo RTC...");
  if (! rtc.begin()) {
    erro = true;
    Serial.println("Falha na inicializacao do RTC!");
  }
  else {
    Serial.println("Modulo RTC iniciado. OK");
    Serial.println("Verificando se o RTC esta rodando...");
    if (! rtc.isrunning()) {
      erro = true;
      Serial.println("RTC nao esta rodando!");
      //A linha abaixo ajusta o RTC com a data e hora do momento da compilacao
      //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
      //A linha abaixo permite ajustar manualmente a data e hora do RTC
      //Formato: DateTime(ano, mes, dia, hora, minuto, segundo)
      //rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    }
    else {
      Serial.println("Modulo RTC rodando. OK");
    }
  }

  Serial.println("Iniciando driver RF...");
  if (! rf_driver.init()) {
    erro = true;
    Serial.println("Falha ao iniciar driver RF!");
  }
  else {
    Serial.println("Driver RF inicializado. OK");
  }

  if (erro) {
    state_1(millis(), HIGH, LOW, LOW);
  }
  else {
    state_1(millis(), LOW, HIGH, LOW);
  }
}

void loop() {
  myMsg msg;
  uint8_t buflen = 3;
  bool msgReceived = false;
  unsigned long now = millis();
  int isRedOn = digitalRead(Pino_RGB_RED);
  int isGreenOn = digitalRead(Pino_RGB_GREEN);
  int isBlueOn = digitalRead(Pino_RGB_BLUE);

  msgReceived = rf_driver.recv((uint8_t *) &msg, &buflen);
  delay(15);

  switch(STATE) {
    case 1: {
      if (!isRedOn && msgReceived) {
        state_1(now, LOW, LOW, HIGH);
        Serial.println("Message received!");
        logMessage(msg);
      }
      else if (!isRedOn && isBlueOn && now - time0 > 5000) {
        state_1(now, LOW, HIGH, LOW);
        Serial.println("Blue OFF");
      }
      break;
    }
  }
}

String datetimeToString(DateTime date) {
  String result = String("");
  result += date.day() < 10 ? "0" : "";
  result += String(date.day(), DEC);
  result.concat("/");
  result.concat(date.month() < 10 ? "0" : "");
  result.concat(String(date.month(), DEC));
  result.concat("/");
  result.concat(String(date.year(), DEC));
  result.concat("T");
  result.concat(date.hour() < 10 ? "0": "");
  result.concat(String(date.hour(), DEC));
  result.concat(":");
  result.concat(date.minute() < 10 ? "0": "");
  result.concat(String(date.minute(), DEC));
  result.concat(":");
  result.concat(date.second() < 10 ? "0": "");
  result.concat(String(date.second(), DEC));

  return result;
}

void logMessage(myMsg msg) {
//  String dateString = datetimeToString(rtc.now());
//  Serial.println(dateString);
  file = SD.open("log.txt", FILE_WRITE);
  DateTime now = rtc.now();
  Serial.println("Gravando dados");
  //Grava os dados no cartao SD
  Serial.print(msg.source, DEC);
  Serial.print(";");
  Serial.print(now.day() < 10 ? "0" : "");
  Serial.print(now.day(), DEC);
  Serial.print('/');
  Serial.print(now.month() < 10 ? "0" : "");
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.year(), DEC);
  Serial.print('T');
  Serial.print(now.hour() < 10 ? "0" : "");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute() < 10 ? "0" : "");
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second() < 10 ? "0" : "");
  Serial.print(now.second(), DEC);
  Serial.println();

  delay(50);
  file.print(msg.source, DEC);
  file.print(";");
  file.print(now.day() < 10 ? "0" : "");
  file.print(now.day(), DEC);
  file.print('/');
  file.print(now.month() < 10 ? "0" : "");
  file.print(now.month(), DEC);
  file.print('/');
  file.print(now.year(), DEC);
  file.print('T');
  file.print(now.hour() < 10 ? "0" : "");
  file.print(now.hour(), DEC);
  file.print(':');
  file.print(now.minute() < 10 ? "0" : "");
  file.print(now.minute(), DEC);
  file.print(':');
  file.print(now.second() < 10 ? "0" : "");
  file.print(now.second(), DEC);
  file.println();
  file.close();
  delay(100);
}
