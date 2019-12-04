#include "RTClib.h"
#include <SPI.h>
#include <SD.h>
#include <RH_ASK.h>

//Pino CS do cartao SD
int Pino_CS_SD = 10;
int Pino_CS_RF = 7;

RTC_DS1307 rtc;
File file;
RH_ASK rf_driver(2000, 6, 7);

void setup()
{
  Serial.begin(115200);
  Serial.println("Data logger");
  Serial.println();

  //Inicia o cartao SD
  Serial.println("Iniciando cartao SD...");
  if (!SD.begin(Pino_CS_SD))
  {
    Serial.println("Falha na inicializacao do SD!");
    return;
  }
  Serial.println("Cartao SD iniciado. OK");
  Serial.println();

  //Verifica as condicoes do RTC
  if (! rtc.begin())
  {
    Serial.println("RTC nao encontrado!");
    while (1);
  }
  if (! rtc.isrunning())
  {
    Serial.println("RTC nao operante!");
    //A linha abaixo ajusta o RTC com a data e hora do momento da compilacao
//    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    //A linha abaixo permite ajustar manualmente a data e hora do RTC
    //Formato: DateTime(ano, mes, dia, hora, minuto, segundo)
    //rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  if (! rf_driver.init())
      Serial.println("RF nao operante!");
}

void loop(void)
{
  // Set buffer to size of expected message
  uint8_t buf[16];
  uint8_t buflen = sizeof(buf);
  // Check if received packet is correct size
  if (rf_driver.recv(buf, &buflen))
  {
    // Message received with valid checksum
    Serial.print("Message Received: ");
    Serial.println((char*)buf);
    Serial.println("Gravando dados no SD...");
    grava_cartao_SD((char *)buf);
  }
//  delay(10000);
}

void grava_cartao_SD(char *mensagem)
{
  //Abre arquivo no SD para gravacao
  file = SD.open("log.txt", FILE_WRITE);

  //Escreve mensagem
  file.print(mensagem);
  
  //Le as informacoes de data e hora
  DateTime now = rtc.now();
  //Grava os dados no cartao SD
  file.print("Data/hora: ");
  file.print(now.day() < 10 ? "0" : "");
  file.print(now.day(), DEC);
  file.print('/');
  file.print(now.month() < 10 ? "0" : "");
  file.print(now.month(), DEC);
  file.print('/');
  file.print(now.year(), DEC);
  file.print(' ');
  file.print(now.hour() < 10 ? "0" : "");
  file.print(now.hour(), DEC);
  file.print(':');
  file.print(now.minute() < 10 ? "0" : "");
  file.print(now.minute(), DEC);
  file.print(':');
  file.print(now.second() < 10 ? "0" : "");
  file.print(now.second(), DEC);
  file.println();
  //Fecha arquivo
  file.close();
}
