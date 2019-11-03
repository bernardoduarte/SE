#include <Servo.h>

int const ldrPin = A5;

int const servoPin = 9;
Servo s;
int pos;

int const sevsegPins[] = {0, 1, 2, 3, 4, 5, 7};

int const joystickXPin = A0;
int const joystickYPin = A1;
int const joystickButtonPin= 8;

short STATE;
unsigned long T0;
short current_speed;
unsigned long joystick_moved_at;
short current_pos;

unsigned long now;
unsigned long buttonPress;

typedef enum {
  A = 0x01,
  B = 0x02,
  C = 0x04,
  D = 0x08,
  E = 0x10,
  F = 0x20,
  G = 0x40
} SevSeg;

typedef enum {
  BLANK,
  ZERO = A|B|C|D|E|F,
  ONE = B|C,
  TWO = A|B|D|E|G,
  THREE = A|B|C|D|G,
  FOUR = B|C|F|G,
  FIVE = A|C|D|F|G,
  SIX = A|C|D|E|F|G,
  SEVEN = A|B|C,
  EIGHT = A|B|C|D|E|F|G,
  NINE = A|B|C|D|F|G
} Digit;

void sevsegDisplay(Digit digit) {
  digitalWrite(sevsegPins[0], !(A & digit));
  digitalWrite(sevsegPins[1], !(B & digit));
  digitalWrite(sevsegPins[2], !(C & digit));
  digitalWrite(sevsegPins[3], !(D & digit));
  digitalWrite(sevsegPins[4], !(E & digit));
  digitalWrite(sevsegPins[5], !(F & digit));
  digitalWrite(sevsegPins[6], !(G & digit));
}

Digit intToDigit(int i) {
  switch(i) {
    case 0: return ZERO;
    case 1: return ONE;
    case 2: return TWO;
    case 3: return THREE;
    case 4: return FOUR;
    case 5: return FIVE;
    case 6: return SIX;
    case 7: return SEVEN;
    case 8: return EIGHT;
    case 9: return NINE;
    default: return BLANK;
  }
}

void state_1(unsigned long now, int spd, unsigned long moved_at, short pos) {
  T0 = now;
  current_speed = spd;
  sevsegDisplay(intToDigit(current_speed));
  joystick_moved_at = moved_at;
  current_pos = pos;
  s.write(abs(current_pos));
  STATE = 1;
}

void state_2() {
  sevsegDisplay(BLANK);
  STATE = 2;
}

void setup() {
  pinMode(ldrPin, INPUT);
  s.attach(servoPin);
  for (int i = 0; i < 7; ++i) {
    pinMode(sevsegPins[i], OUTPUT);
  }

  // Configuração do timer0
  TCCR0A = 0; // Reseta registrador
  TCCR0B = 0; // Reseta registrador
  TCNT0 = 0; // Reseta temporizador
  OCR0A = 0xFF; // Carrega registrador de comparação
  TCCR0A |= (1 << WGM01); // Modo CTC
  TCCR0B |= (1 << CS00) | (1 << CS02); // Configura prescaler para 1024: CS12 = 1 e CS10 = 1
  TIMSK0 = 0; // Reseta resgistrador de máscara de interrupção, desligando as interrupções

  pinMode(8, INPUT_PULLUP);
  // Configuração do timer1
  TCCR1A = 0; // Reseta registrador
  TCCR1B = 0; // Reseta registrador
  TCNT1H = 0; // Reseta temporizador High byte
  TCNT1L = 0; // Reseta temporizador Low byte
  // Configura prescaler para 1024: CS12 = 1 e CS10 = 1
  // Configura input capture noise canceler: ICNC1 = 1
  // Configura input capture edge select: ICES1 = 1
  TCCR1B |= (1 << ICNC1) | (1 << ICES1) | (1 << CS12) | (1 << CS10);
  TIMSK1 = 0; // Reseta resgistrador de máscara de interrupção, desligando as interrupções

  now = 0;

  state_1(now, 0, now, 0);
}

void loop() {
  int ldrValue = analogRead(ldrPin);
  short x = analogRead(joystickXPin);
  short y = analogRead(joystickYPin);
  int btn = 0;

  if (TIFR0 & (1 << OCF0A)) {
    now++;
    TIFR0 |= (1 << OCF0A);
  }

  if (TIFR1 & (1 << ICF1)) {
    btn = 1;
    TIFR1 |= (1 << ICF1);
  }

  switch(STATE) {
    case 1: {
      if (ldrValue >= 900) {
        state_2();
      }
      else if (btn && now - buttonPress > 64) {
        state_2();
      }
      else if (now - T0 > 2) {
        short new_pos = current_pos + current_speed;
        if (new_pos > 180) new_pos = -180;
        state_1(now, current_speed, joystick_moved_at, new_pos);
      }
      else if (y < 40 && now - joystick_moved_at > 25) {
        int new_speed = (current_speed + 1) % 10;
        state_1(T0, new_speed, now, current_pos);
      }
      else if (y > 1000 && now - joystick_moved_at > 25) {
        int new_speed = (current_speed > 0) ? current_speed - 1 : 9;
        state_1(T0, new_speed, now, current_pos);
      }
      break;
    }
    case 2: {
      if (ldrValue < 900) {
        state_1(now, current_speed, joystick_moved_at, current_pos);
      }
      else if (btn && now - buttonPress > 64) {
        state_1(now, current_speed, joystick_moved_at, current_pos);
      }
      break;
    }
  }
}

