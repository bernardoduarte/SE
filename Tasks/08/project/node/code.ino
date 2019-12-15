#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile
#include <ServoTimer2.h>

//Pino CS do cartao SD
int Pino_CS_SD = 10;
//Pino CS do módulo RF
int Pino_CS_RF_RX = 7;
int Pino_CS_RF_TX = 6;
//Pino módulo PIR
int Pino_PIR = 2;
//Pino módulo Servo
int Pino_Servo = 3;

//Modulos
RH_ASK rf_driver(5000, Pino_CS_RF_RX, Pino_CS_RF_TX);
ServoTimer2 servo;

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
unsigned int timeIn;
const int servoStop = 90;
int dAngle;

//Node ID
int nodeID = 1;

void state_1(unsigned int now) {
  time0 = now;
  STATE = 1;
}

void state_2(unsigned int now) {
  time0 = now;
  STATE = 2;
}

void state_3(unsigned int now, int angle, unsigned int startedAt) {
  time0 = now;
  dAngle = angle;
  setDegrees(servo, getDegrees(servo) + angle);
  timeIn = startedAt;
  STATE = 3;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Iniciando driver RF...");
  if (! rf_driver.init()) {
    Serial.println("Falha ao iniciar driver RF!");
  }
  else {
    Serial.println("Driver RF inicializado. OK");
  }

  servo.attach(Pino_Servo);

  pinMode(Pino_PIR, INPUT);

  state_1(millis());
}

unsigned int getDegrees(ServoTimer2 s) {
  int pulse = s.read();
  return (pulse - MIN_PULSE_WIDTH)/(DEFAULT_PULSE_WIDTH / 180);
}

void setDegrees(ServoTimer2 s, unsigned int degree) {
  int pulse = degree * (DEFAULT_PULSE_WIDTH/180) + MIN_PULSE_WIDTH;
  s.write(pulse);
}

void sendMessage(uint8_t source, uint8_t receiver, uint8_t sender) {
  myMsg msg;
  
  msg.source = source;
  msg.receiver = receiver;
  msg.sender = sender;

  rf_driver.send((uint8_t *) &msg, 3);
  rf_driver.send((uint8_t *) &msg, 3);
  rf_driver.send((uint8_t *) &msg, 3);
  rf_driver.waitPacketSent();
}

void routeMessage(myMsg msg, uint8_t sender) {
  msg.sender = sender;

  rf_driver.send((uint8_t *) &msg, 3);
  rf_driver.send((uint8_t *) &msg, 3);
  rf_driver.send((uint8_t *) &msg, 3);
  rf_driver.waitPacketSent();
}

void loop() {
  myMsg msg;
  uint8_t buflen = sizeof(myMsg);
  int detection = digitalRead(Pino_PIR);
  unsigned int now = millis();
  bool msgReceived = false;

  msgReceived = rf_driver.recv((uint8_t *) &msg, &buflen);
  if (msgReceived) Serial.println("Mensagem");
  
//  Serial.print("now - time0: ");
//  Serial.println(now - time0, DEC);
//  Serial.println(getDegrees(servo), DEC);
  switch(STATE) {
    case 1: {
      if (detection) {
        sendMessage(nodeID, 0, nodeID);
        state_2(now);
        Serial.println("1->2");
      }
      else if (msgReceived) {
        routeMessage(msg, nodeID);
        state_1(time0);
      }
      break;
    }
    case 2: {
      if (now - time0 > 15000) {
        state_3(now, 1, now);
        Serial.println("2->3");
      }
      else if (msgReceived){
        routeMessage(msg, nodeID);
        state_2(time0);
      }
      break;
    }
    case 3: {
      if (detection) {
        state_2(now);
        sendMessage(nodeID, 0, nodeID);
        Serial.println("3->2");
      }
      else if (now - time0 > 50) {
        int angle = dAngle;
        unsigned int current = getDegrees(servo);
        if (current <= 85) {
          angle = 1;
        }
        else if (current >= 95) {
          angle = -1;
        }
        state_3(now, angle, timeIn);
        Serial.println("3->3");
      }
      else if (now - timeIn > 10000) {
        state_1(now);
        Serial.println("3->1");
      }
      break;
    }
  }
}
