// ------------------------------------------------
// MCE202 -PROJECT
// AUTOMATIC CLEANER
// ------------------------------------------------

//required libraries
#include <Wire.h>

// Fan parameter
int fan_pin = 6;  // pin for the fan

//buzzer pin
int buzzer_pin=7;

//vehicle motion parameters
// Four pins below determine the location of the wheels at the vehicle
int right1 = 13;
int left1 = 8;
int right2 = 12;
int left2 = 9;
// Two pins below allows operation of the pins above
int right_en = 11;
int left_en = 10;
// Other data coming from arduino (speed and direction)
char com_direc;
byte com_speed = 0;
char last_direc;  // This one is for turning right or left
int rotate_speed = 0; // This is for speed adjustement when turning left or right


//Distance  parameters
//  Pins echo and trig are used for distance sensor
int echo = 2;
int trig = 2;
int last_distance = 999;
int change_or_notchange = 1;
enum stat {READY, DONE};
byte state = READY;

// ------------------------------------------------
// Setup
// ------------------------------------------------
void setup() {
  // Pins must be used as outputs
  pinMode(right1, OUTPUT);
  pinMode(right2, OUTPUT);
  pinMode(right_en, OUTPUT);
  pinMode(left1, OUTPUT);
  pinMode(left2, OUTPUT);
  pinMode(left_en, OUTPUT);
  pinMode(fan_pin, OUTPUT);
  pinMode(buzzer_pin, OUTPUT);
  Serial.begin(9600);
  Wire.begin(2);  // Determined as slave
  Wire.onReceive(receiveEvent);
  attachInterrupt(digitalPinToInterrupt(echo), again_distance, CHANGE); // interrupt

}

// ------------------------------------------------
// Loop
// ------------------------------------------------
void loop() {

  long distance, times;
  cli();
  pinMode(trig, OUTPUT);
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  pinMode(echo, INPUT);
  state = READY;
  sei();
  times = pulseIn(echo, HIGH);
  if (state == DONE ) {
    distance = (times / 2) / 29.1;
    if (last_distance != distance) {
      last_distance = distance;
      change_or_notchange = 1;
      if (distance > 200 || distance < 0) {
        Serial.println("out of range");
      }
      else {
        Serial.print(distance); Serial.println(" cm");
      }
    }
    if (distance < 30) {
      stopp();
      digitalWrite(buzzer_pin,HIGH);
      delay(500);
      digitalWrite(buzzer_pin,LOW);
    }
  }

  delay(500);
  //time delay for distance sensor

}

// ------------------------------------------------
// Functions
// ------------------------------------------------
void receiveEvent(int bytesReady)
{
  // Part for reading when massage arrives
  while (Wire.available())
  {
    Serial.println("Message came ");
    char a = Wire.read();
    byte b = Wire.read();
    // Below, printed what the data coming are 
    display(a, b);
    com_direc = a;
    com_speed = b;
    coming_mesage_analyze();
  }
}

// ------------------------------------------------
// ------------------------------------------------
void display(char a, byte b) {
  // Printing of the data about speed and direction
  Serial.print("The direction is : ");
  if (a == 'A') {
    Serial.println("FORWARD");
  } else if (a == 'B') {
    Serial.println("BACK");
  } else if (a == 'C') {
    Serial.println("LEFT");
  } else if (a == 'D') {
    Serial.println("RIGHT");
  }
  Serial.print("The speed is : "); Serial.println(b);
}


// ------------------------------------------------
// ------------------------------------------------
void coming_mesage_analyze() {

  // This is the part where the data and massages coming from other arduino is processed and evaluated
  fan(); // The fan start to run 
  // Below operation direction and speed of the motors
  // For enabling rotation to the left or right other motors speed is decreased to half
  switch (com_direc) {
    case 'A' :
      last_direc = com_direc;
      forward();
      break;
    case 'B' :
      last_direc = com_direc;
      back();
      break;
    case 'C' :
      rotate_speed = com_speed / 2;
      if (last_direc == 'A') {
        forward_left();
      } else {
        back_left();
      }
      break;
    case 'D' :
      rotate_speed = com_speed / 2;
      if (last_direc == 'A') {
        forward_right();
      } else {
        back_right();
      }

      break;
  }
}

// ------------------------------------------------
// ------------------------------------------------
void fan() {
  analogWrite(fan_pin, 255);

}

// ------------------------------------------------
// ------------------------------------------------
void again_distance() {
  // Added interrupt for detection of distance changes while other operations are being done
  
  if (state == READY) {
    if (digitalRead(echo)) {
      state = DONE;

    }
  }
}

// ------------------------------------------------
// ------------------------------------------------

// Functions below transmits vehicles operating direction and speed to dc motors
// Function's name determine vehicle's direction
// According to these direction and speed of the motors are entered
void forward() {
  digitalWrite(right1, HIGH);
  digitalWrite(left1, HIGH);
  digitalWrite(right2, LOW);
  digitalWrite(left2, LOW);
  analogWrite(right_en, com_speed);
  analogWrite(left_en, com_speed);

}
// ------------------------------------------------
// ------------------------------------------------
void back() {
  digitalWrite(right1, LOW);
  digitalWrite(left1, LOW);
  digitalWrite(right2, HIGH);
  digitalWrite(left2, HIGH);
  analogWrite(right_en, com_speed);
  analogWrite(left_en, com_speed);

}
// ------------------------------------------------
// ------------------------------------------------
void forward_left() {
  digitalWrite(right1, HIGH);
  digitalWrite(left1, HIGH);
  digitalWrite(right2, LOW);
  digitalWrite(left2, LOW);
  analogWrite(right_en, com_speed);
  analogWrite(left_en, rotate_speed);

}
// ------------------------------------------------
// ------------------------------------------------
void forward_right() {
  digitalWrite(right1, HIGH);
  digitalWrite(left1, HIGH);
  digitalWrite(right2, LOW);
  digitalWrite(left2, LOW);
  analogWrite(right_en, rotate_speed);
  analogWrite(left_en, com_speed);

}
// ------------------------------------------------
// ------------------------------------------------
void back_left() {
  digitalWrite(right1, LOW);
  digitalWrite(left1, LOW);
  digitalWrite(right2, HIGH);
  digitalWrite(left2, HIGH);
  analogWrite(right_en, com_speed);
  analogWrite(left_en, rotate_speed);

}
// ------------------------------------------------
// ------------------------------------------------
void back_right() {
  digitalWrite(right1, LOW);
  digitalWrite(left1, LOW);
  digitalWrite(right2, HIGH);
  digitalWrite(left2, HIGH);
  analogWrite(right_en, rotate_speed);
  analogWrite(left_en, com_speed);

}
// ------------------------------------------------
// ------------------------------------------------
void stopp() {
  analogWrite(fan_pin, 0);
  digitalWrite(right1, LOW);
  digitalWrite(left1, LOW);
  digitalWrite(right2, LOW);
  digitalWrite(left2, LOW);
  analogWrite(right_en, 0);
  analogWrite(left_en, 0);

}
