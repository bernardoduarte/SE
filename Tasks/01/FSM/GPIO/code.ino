int STATE;
unsigned long led_changed_at;
unsigned long led_delay;
unsigned long b1_pressed_at, b2_pressed_at;
bool b1_pressed, b2_pressed;

void state_1(unsigned long led_change_time) {
  PORTD |= 1 << 7;
  led_changed_at = led_change_time;
  if (b1_pressed) led_delay -= 50;
  if (b2_pressed) led_delay += 50;
  b1_pressed = false;
  b2_pressed = false;
  STATE = 1;
}

void state_2(unsigned long led_change_time, unsigned long press_time) {
  PORTD |= 1 << 7;
  if (b2_pressed) led_delay += 50;
  b2_pressed = false;
  led_changed_at = led_change_time;
  b1_pressed_at = press_time;
  b1_pressed = true;
  STATE = 2;
}

void state_3(unsigned long led_change_time, unsigned long press_time) {
  PORTD |= 1 << 7;
  if (b1_pressed) led_delay -= 50;
  b1_pressed = false;
  led_changed_at = led_change_time;
  b2_pressed_at = press_time;
  b2_pressed = true;
  STATE = 3;
}

void state_4(unsigned long led_change_time) {
  PORTD |= 1 << 7;
  if (b2_pressed) led_delay += 50;
  b2_pressed = false;
  led_changed_at = led_change_time;
  STATE = 4;
}

void state_5(unsigned long led_change_time) {
  PORTD |= 1 << 7;
  if (b1_pressed) led_delay -= 50;
  b1_pressed = false;
  led_changed_at = led_change_time;
  STATE = 5;
}

void state_6(unsigned long led_change_time, unsigned long press_time) {
  PORTD |= 1 << 7;
  led_changed_at = led_change_time;
  b2_pressed_at = press_time;
  b2_pressed = true;
  STATE = 6;
}

void state_7(unsigned long led_change_time, unsigned long press_time) {
  PORTD |= 1 << 7;
  led_changed_at = led_change_time;
  b1_pressed = true;
  STATE = 7;
}

void state_8(void) {
  STATE = 8;
}

void state_9(unsigned long led_change_time) {
  PORTD &= ~(1 << 7);
  led_changed_at = led_change_time;
  if (b1_pressed) led_delay -= 50;
  if (b2_pressed) led_delay += 50;
  b1_pressed = false;
  b2_pressed = false;
  STATE = 9;
}

void state_10(unsigned long led_change_time, unsigned long press_time) {
  PORTD &= ~(1 << 7);
  if (b2_pressed) led_delay += 50;
  b2_pressed = false;
  led_changed_at = led_change_time;
  b1_pressed_at = press_time;
  b1_pressed = true;
  STATE = 10;
}

void state_11(unsigned long led_change_time, unsigned long press_time) {
  PORTD &= ~(1 << 7);
  if (b1_pressed) led_delay -= 50;
  b1_pressed = false;
  led_changed_at = led_change_time;
  b2_pressed_at = press_time;
  b2_pressed = true;
  STATE = 11;
}

void state_12(unsigned long led_change_time) {
  PORTD &= ~(1 << 7);
  if (b2_pressed) led_delay += 50;
  b2_pressed = false;
  led_changed_at = led_change_time;
  STATE = 12;
}

void state_13(unsigned long led_change_time) {
  PORTD &= ~(1 << 7);
  if (b1_pressed) led_delay -= 50;
  b1_pressed = false;
  led_changed_at = led_change_time;
  STATE = 13;
}

void state_14(unsigned long led_change_time, unsigned long press_time) {
  PORTD &= ~(1 << 7);
  led_changed_at = led_change_time;
  b2_pressed_at = press_time;
  b2_pressed = true;
  STATE = 14;
}

void state_15(unsigned long led_change_time, unsigned long press_time) {
  PORTD &= ~(1 << 7);
  led_changed_at = led_change_time;
  b1_pressed_at = press_time;
  b1_pressed = true;
  STATE = 15;
}

void state_16(void) {
  STATE = 16;
}

void setup() {
  // put your setup code here, to run once:
  DDRD |= 1 << 7;
  DDRB &= ~(1 << 1);
  DDRB &= ~(1 << 2);
  led_delay = 500;
  state_1(millis());
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long now = millis();
  char but1 = PINB & (1 << 1);
  char but2 = PINB & (1 << 2);

  switch (STATE) {
    case 1: {
      if (now - led_changed_at >= led_delay) {
        state_9(now);
      } else if (!but1) {
        state_2(led_changed_at, now);
      } else if (!but2) {
        state_3(led_changed_at, now);
      }
      break;
    }
    case 2: {
      if (now - led_changed_at >= led_delay) {
        state_10(now, b1_pressed_at);
      } else if (but1) {
        state_1(led_changed_at);
      } else if (!but2) {
        state_8();
      } else if (now - b1_pressed_at >= 500) {
        state_4(led_changed_at);
      }
      break;
    }
    case 3: {
      if (now - led_changed_at >= led_delay) {
        state_11(now, b2_pressed_at);
      } else if (!but1) {
        state_8();
      } else if (but2) {
        state_1(led_changed_at);
      } else if (now - b2_pressed_at >= 500) {
        state_5(led_changed_at);
      }
      break;
    }
    case 4: {
      if (now - led_changed_at >= led_delay) {
        state_12(now);
      } else if (but1) {
        state_1(led_changed_at);
      } else if (!but2) {
        state_6(led_changed_at, now);
      }
      break;
    }
    case 5: {
      if (now - led_changed_at >= led_delay) {
        state_13(now);
      } else if (!but1) {
        state_7(led_changed_at, now);
      } else if (but2) {
        state_1(led_changed_at);
      }
      break;
    }
    case 6: {
      if (now - led_changed_at >= led_delay) {
        state_14(now, b2_pressed_at);
      } else if (but1) {
        state_3(led_changed_at, b2_pressed_at);
      } else if (but2) {
        state_4(led_changed_at);
      }
      break;
    }
    case 7: {
      if (now - led_changed_at >= led_delay) {
        state_15(now, b1_pressed_at);
      } else if (but1) {
        state_5(led_changed_at);
      } else if (but2) {
        state_2(led_changed_at, b1_pressed_at);
      }
      break;
    }
    case 9: {
      if (now - led_changed_at >= led_delay) {
        state_1(now);
      } else if (!but1) {
        state_10(led_changed_at, now);
      } else if (!but2) {
        state_11(led_changed_at, now);
      }
      break;
    }
    case 10: {
      if (now - led_changed_at >= led_delay) {
        state_2(now, b1_pressed_at);
      } else if (but1) {
        state_9(led_changed_at);
      } else if (!but2) {
        state_16();
      } else if (now - b1_pressed_at >= 500) {
        state_12(led_changed_at);
      }
      break;
    }
    case 11: {
      if (now - led_changed_at >= led_delay) {
        state_3(now, b2_pressed_at);
      } else if (!but1) {
        state_16();
      } else if (but2) {
        state_9(led_changed_at);
      } else if (now - b2_pressed_at >= 500) {
        state_13(led_changed_at);
      }
      break;
    }
    case 12: {
      if (now - led_changed_at >= led_delay) {
        state_4(now);
      } else if (but1) {
        state_9(led_changed_at);
      } else if (!but2) {
        state_14(led_changed_at, now);
      }
      break;
    }
    case 13: {
      if (now - led_changed_at >= led_delay) {
        state_5(now);
      } else if (!but1) {
        state_15(led_changed_at, now);
      } else if (but2) {
        state_9(led_changed_at);
      }
      break;
    }
    case 14: {
      if (now - led_changed_at >= led_delay) {
        state_6(now, b2_pressed_at);
      } else if (but1) {
        state_11(led_changed_at, b2_pressed_at);
      } else if (but2) {
        state_12(led_changed_at);
      }
      break;
    }
    case 15: {
      if (now - led_changed_at >= led_delay) {
        state_7(now, b1_pressed_at);
      } else if (but1) {
        state_13(led_changed_at);
      } else if (but2) {
        state_10(led_changed_at, b1_pressed_at);
      }
      break;
    }
  }
}

