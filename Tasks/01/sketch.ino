#define LED_PIN 7
#define BUT1_PIN 9
#define BUT2_PIN 10

#define PRESS_THRESHOLD 100

unsigned long wavelength;
bool do_blink;

typedef struct button {
  unsigned long pressed_at;
  bool beign_pressed;
  unsigned long released_at;
  bool was_pressed;
  unsigned short pin;
} Button;

Button buttons[2];

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUT1_PIN, INPUT);
  pinMode(BUT2_PIN, INPUT);

  do_blink = true;
  wavelength = 2000;

  reset_button(&buttons[0]);
  buttons[0].pin = BUT1_PIN;
  reset_button(&buttons[1]);
  buttons[1].pin = BUT2_PIN;
}

void loop() {
  // put your main code here, to run repeatedly:
  if (do_blink)
    blink();

  check_buttons();
  manage_blink();
}

void blink() {
  unsigned long now = millis();
  if ((now / (wavelength / 2)) % 2) {
    digitalWrite(LED_PIN, HIGH); 
  }
  else {
    digitalWrite(LED_PIN, LOW);
  }
}

void check_buttons() {
  short i;
  for (i = 0; i < 2; ++i) {
    int val = digitalRead(buttons[i].pin);
    if (val == 0 && !buttons[i].beign_pressed) {
      buttons[i].pressed_at = millis();
      buttons[i].beign_pressed = true;
    }
    else if (val == 1 && buttons[i].beign_pressed){
      buttons[i].released_at = millis();
      buttons[i].beign_pressed = false;
      buttons[i].was_pressed = true;
    }
  }
}

void manage_blink() {
  if (buttons[0].was_pressed && buttons[1].was_pressed) {
    if (delta_press(buttons[0], buttons[1]) < 500) {
      do_blink = !do_blink;
      reset_button(&buttons[0]);
      reset_button(&buttons[1]); 
    }
  }
  else if (!(buttons[0].beign_pressed || buttons[1].beign_pressed)) {
    if (buttons[0].was_pressed) {
      if (wavelength > 50)
        wavelength -= 50;
      reset_button(&buttons[0]);
    }
    else if (buttons[1].was_pressed) {
      wavelength += 50;
      reset_button(&buttons[1]);
    } 
  }
}

void reset_button(Button *but) {
  but->pressed_at = 0;
  but->released_at = 0;
  but->beign_pressed = false;
  but->was_pressed = false;
}

unsigned long delta_press(Button but1, Button but2) {
  return but1.pressed_at > but2.pressed_at ?
    but1.pressed_at - but2.pressed_at :
    but2.pressed_at - but1.pressed_at;
}
