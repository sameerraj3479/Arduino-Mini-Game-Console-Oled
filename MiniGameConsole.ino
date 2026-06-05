#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define W 128
#define H 64
Adafruit_SSD1306 display(W, H, &Wire, -1);

#define BTN_UP 2
#define BTN_DOWN 3
#define BTN_SELECT 4
#define BUZZER 7

int menu = 0;
bool inMenu = true;
bool menuIntroPlayed = false;

bool isDown(int pin) {
  return digitalRead(pin) == LOW;
}

bool press(int pin) {
  if (digitalRead(pin) == LOW) {
    delay(140);
    while (digitalRead(pin) == LOW);
    return true;
  }
  return false;
}

void beep(int f, int t) {
  tone(BUZZER, f, t);
}

void menuOpenSound() {
  tone(BUZZER, 784, 60);  delay(75);
  tone(BUZZER, 988, 60);  delay(75);
  noTone(BUZZER);
}

// Your uploaded sound approximation for Dino defeat
void dinoDefeatSound() {
  tone(BUZZER, 800, 120); delay(150);
  tone(BUZZER, 1000, 120); delay(150);
  tone(BUZZER, 1200, 150); delay(180);
  tone(BUZZER, 900, 100); delay(120);
  tone(BUZZER, 600, 200); delay(250);
  noTone(BUZZER);
}

void setup() {
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_SELECT, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.display();

  randomSeed(analogRead(A0));
  menuIntroPlayed = false;
}

void loop() {
  if (inMenu) showMenu();
  else {
    if (menu == 0) flappy();
    if (menu == 1) dino();
    if (menu == 2) pong();
    if (menu == 3) reaction();
  }
}

void showMenu() {
  if (!menuIntroPlayed) {
    menuOpenSound();
    menuIntroPlayed = true;
  }

  if (press(BTN_UP)) {
    menu--;
    if (menu < 0) menu = 3;
    beep(700, 40);
  }

  if (press(BTN_DOWN)) {
    menu++;
    if (menu > 3) menu = 0;
    beep(700, 40);
  }

  if (press(BTN_SELECT)) {
    inMenu = false;
    menuIntroPlayed = false;
    beep(1000, 80);
    delay(200);
    return;
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(18, 0);
  display.print("MINI GAME CONSOLE");

  display.setCursor(18, 16);
  display.print(menu == 0 ? "> FLAPPY BIRD" : "  FLAPPY BIRD");

  display.setCursor(18, 28);
  display.print(menu == 1 ? "> CHROME DINO" : "  CHROME DINO");

  display.setCursor(18, 40);
  display.print(menu == 2 ? "> PONG PRO" : "  PONG PRO");

  display.setCursor(18, 52);
  display.print(menu == 3 ? "> REACTION TEST" : "  REACTION TEST");

  display.display();
}

// ---------------- FLAPPY ----------------
void flappy() {
  int birdY = 30, vel = 0;
  int pipeX = 125, gapY = 20;
  int score = 0;
  bool over = false;

  while (!inMenu) {
    if (press(BTN_SELECT)) {
      inMenu = true;
      menuIntroPlayed = false;
      return;
    }

    if (!over) {
      if (isDown(BTN_UP)) {
        vel = -4;
        beep(900, 20);
      }

      vel++;
      birdY += vel;
      pipeX -= 2;

      if (pipeX < -10) {
        pipeX = 128;
        gapY = random(10, 36);
        score++;
        beep(1300, 35);
      }

      if (birdY < 0 || birdY > 63 ||
          (pipeX < 16 && pipeX > 4 && (birdY < gapY || birdY > gapY + 24))) {
        over = true;
        beep(180, 300);
      }
    }

    display.clearDisplay();
    display.fillCircle(10, birdY, 3, WHITE);
    display.fillRect(pipeX, 0, 9, gapY, WHITE);
    display.fillRect(pipeX, gapY + 24, 9, 64, WHITE);

    display.setCursor(78, 0);
    display.print("Score:");
    display.print(score);

    if (over) {
      display.setCursor(32, 22);
      display.print("GAME OVER");
      display.setCursor(14, 38);
      display.print("D2 Restart");
      display.setCursor(14, 50);
      display.print("D4 Menu");
    }

    display.display();

    if (over && press(BTN_UP)) {
      birdY = 30;
      vel = 0;
      pipeX = 125;
      score = 0;
      over = false;
    }

    delay(35);
  }
}

// ---------------- DINO ----------------
void dino() {
  int dinoY = 48;
  int vel = 0;
  bool jumping = false;

  int cactusX = 128;
  int cloudX = 100;
  int score = 0;
  int speed = 4;
  bool over = false;

  while (!inMenu) {
    if (press(BTN_SELECT)) {
      inMenu = true;
      menuIntroPlayed = false;
      return;
    }

    if (!over) {
      if (isDown(BTN_UP) && !jumping) {
        vel = -8;
        jumping = true;
        beep(900, 25);
      }

      if (jumping) {
        dinoY += vel;
        vel++;

        if (dinoY >= 48) {
          dinoY = 48;
          jumping = false;
        }
      }

      speed = 4 + (score / 4);
      if (speed > 10) speed = 10;

      cactusX -= speed;
      cloudX--;

      if (cloudX < -20) cloudX = 128;

      if (cactusX < -8) {
        cactusX = 128;
        score++;
        beep(1200, 20);
      }

      if (cactusX < 20 && cactusX > 7 && dinoY > 42) {
        over = true;
        dinoDefeatSound();
      }
    }

    display.clearDisplay();

    display.drawLine(0, 56, 128, 56, WHITE);
    for (int i = 0; i < 128; i += 12) {
      display.drawPixel(i, 59, WHITE);
    }

    display.drawCircle(cloudX, 12, 3, WHITE);
    display.drawCircle(cloudX + 5, 11, 4, WHITE);
    display.drawCircle(cloudX + 10, 12, 3, WHITE);

    display.fillRect(10, dinoY, 8, 8, WHITE);
    display.fillRect(15, dinoY - 4, 5, 5, WHITE);
    display.drawPixel(18, dinoY - 2, BLACK);
    display.drawLine(11, dinoY + 8, 8, dinoY + 11, WHITE);
    display.drawLine(16, dinoY + 8, 19, dinoY + 11, WHITE);

    display.fillRect(cactusX, 48, 5, 8, WHITE);
    display.drawLine(cactusX - 2, 51, cactusX, 51, WHITE);
    display.drawLine(cactusX + 5, 52, cactusX + 7, 52, WHITE);

    display.setCursor(72, 0);
    display.print("S:");
    display.print(score);
    display.print(" V:");
    display.print(speed);

    if (over) {
      display.setCursor(32, 22);
      display.print("GAME OVER");
      display.setCursor(14, 38);
      display.print("D2 Restart");
      display.setCursor(14, 50);
      display.print("D4 Menu");
    }

    display.display();

    if (over && press(BTN_UP)) {
      dinoY = 48;
      vel = 0;
      jumping = false;
      cactusX = 128;
      cloudX = 100;
      score = 0;
      speed = 4;
      over = false;
    }

    delay(max(14, 28 - speed));
  }
}

// ---------------- PONG ----------------
void pong() {
  int paddleY = 22;
  int ballX = 64, ballY = 32;
  int dx = 2, dy = 1;
  int score = 0;
  int speedDelay = 28;
  bool over = false;

  while (!inMenu) {
    if (press(BTN_SELECT)) {
      inMenu = true;
      menuIntroPlayed = false;
      return;
    }

    if (!over) {
      if (isDown(BTN_UP) && paddleY > 0) paddleY -= 3;
      if (isDown(BTN_DOWN) && paddleY < 46) paddleY += 3;

      ballX += dx;
      ballY += dy;

      if (ballY <= 1) {
        ballY = 1;
        dy = -dy;
        beep(700, 15);
      }

      if (ballY >= 62) {
        ballY = 62;
        dy = -dy;
        beep(700, 15);
      }

      if (ballX >= 126) {
        ballX = 126;
        dx = -dx;
        beep(700, 15);
      }

      if (ballX <= 6) {
        if (ballY >= paddleY && ballY <= paddleY + 18) {
          ballX = 6;
          dx = -dx;
          score++;
          beep(1100, 25);

          if (score % 4 == 0 && speedDelay > 16) {
            speedDelay -= 2;
          }

          if (ballY < paddleY + 6) dy = -2;
          else if (ballY > paddleY + 12) dy = 2;
          else dy = 1;
        } 
        else if (ballX < 0) {
          over = true;
          beep(180, 350);
        }
      }
    }

    display.clearDisplay();

    for (int y = 0; y < 64; y += 8) {
      display.drawLine(64, y, 64, y + 3, WHITE);
    }

    display.fillRect(0, paddleY, 4, 18, WHITE);
    display.fillCircle(ballX, ballY, 2, WHITE);

    display.setCursor(80, 0);
    display.print("S:");
    display.print(score);

    if (over) {
      display.setCursor(38, 22);
      display.print("LOST!");
      display.setCursor(14, 38);
      display.print("D2 Restart");
      display.setCursor(14, 50);
      display.print("D4 Menu");
    }

    display.display();

    if (over && press(BTN_UP)) {
      paddleY = 22;
      ballX = 64;
      ballY = 32;
      dx = 2;
      dy = 1;
      score = 0;
      speedDelay = 28;
      over = false;
    }

    delay(speedDelay);
  }
}

// ---------------- REACTION ----------------
void reaction() {
  unsigned long best = 9999;

  while (!inMenu) {
    display.clearDisplay();
    display.setCursor(20, 5);
    display.print("REACTION TEST");
    display.setCursor(8, 22);
    display.print("D2 Start");
    display.setCursor(8, 36);
    display.print("D4 Menu Anytime");
    display.setCursor(8, 50);
    display.print("Best:");
    if (best == 9999) display.print("--");
    else {
      display.print(best);
      display.print("ms");
    }
    display.display();

    if (press(BTN_SELECT)) {
      inMenu = true;
      menuIntroPlayed = false;
      return;
    }

    if (press(BTN_UP)) {
      display.clearDisplay();
      display.setCursor(36, 24);
      display.print("WAIT...");
      display.setCursor(12, 42);
      display.print("Don't press early");
      display.display();

      unsigned long waitTime = random(1000, 9000);
      unsigned long waitStart = millis();
      bool falseStart = false;

      while (millis() - waitStart < waitTime) {
        if (digitalRead(BTN_SELECT) == LOW) {
          delay(150);
          inMenu = true;
          menuIntroPlayed = false;
          return;
        }

        if (digitalRead(BTN_UP) == LOW) {
          falseStart = true;
          break;
        }
      }

      if (falseStart) {
        beep(200, 300);
        display.clearDisplay();
        display.setCursor(24, 22);
        display.print("Too Early!");
        display.setCursor(16, 42);
        display.print("Wait for beep");
        display.display();
        delay(1500);
        continue;
      }

      beep(1300, 80);
      unsigned long start = millis();

      display.clearDisplay();
      display.setCursor(26, 24);
      display.print("PRESS D2!");
      display.setCursor(20, 42);
      display.print("D4 Menu");
      display.display();

      while (digitalRead(BTN_UP) == HIGH) {
        if (digitalRead(BTN_SELECT) == LOW) {
          delay(150);
          inMenu = true;
          menuIntroPlayed = false;
          return;
        }
      }

      unsigned long t = millis() - start;
      if (t < best) best = t;

      display.clearDisplay();
      display.setCursor(20, 8);
      display.print("YOUR TIME:");
      display.setCursor(36, 25);
      display.print(t);
      display.print(" ms");

      display.setCursor(20, 44);
      if (t < 200) display.print("LEGEND SPEED!");
      else if (t < 300) display.print("VERY FAST!");
      else if (t < 450) display.print("GOOD!");
      else display.print("TRY AGAIN!");

      display.display();
      beep(1500, 100);
      delay(2000);
    }
  }
}
