#include <Servo.h>
#include <AFMotor.h>
#include <NewPing.h>

#define TRIG_PIN A0
#define ECHO_PIN A1
#define LEFT_TURN_LED_PIN A3
#define RIGHT_TURN_LED_PIN A2
#define MAX_DISTANCE 200
#define GO_LEFT -1
#define GO_RIGHT 1
#define GO_BACKWARDS 0

Servo servo;
AF_DCMotor motorFL(1);
AF_DCMotor motorFR(2);

byte stopDist = 30;

NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE); 

int pos = 0;
int peek_angles[] = { 90, 45, 90, 135 };

void setup() {
  pinMode(LEFT_TURN_LED_PIN, OUTPUT);
  pinMode(RIGHT_TURN_LED_PIN, OUTPUT);  
  motorFL.setSpeed(255);
  motorFR.setSpeed(255);
  servo.attach(10);
  Serial.begin(9600);
}

void loop() {
  int distance = 255;
  int peek_index = 0;
  int array_length = sizeof peek_angles / sizeof peek_angles[0];

  while(distance >= stopDist && peek_index < array_length) {
    distance = peek(peek_angles[peek_index]);
    
    peek_index = peek_index + 1;
  }
   
  if (distance >= stopDist) {
    move_forward();
  } else {
   stop_moving();
   change_direction();
  }
}

void change_direction() {
  int new_direction = get_new_direction();

  look_ahead();
  
  if (new_direction == GO_LEFT) {
    turn_left();
  }

  if (new_direction == GO_RIGHT) {
    turn_right();
  }

  if (new_direction == GO_BACKWARDS) {
    move_backward();
    stop_moving();
    change_direction();
  }
}

int get_new_direction() {
   int left_distance = get_left_distance();
   int right_distance = get_right_distance();

  //  if (left_distance < stopDist && right_distance < stopDist) {
  //    return GO_BACKWARDS;
  //  }

   if (left_distance > right_distance) {
     return GO_LEFT;
   }

   if (right_distance > left_distance) {
     return GO_RIGHT;
   }

  return get_random_direction();
}

int get_random_direction() {
  int random_number = random(0, 10);

  if (random_number < 5) {
    return GO_LEFT;
  } else {
    return GO_RIGHT;
  }
}

int get_left_distance() {
    look_left();

    return readPing();
}

int get_right_distance() {
    look_right();

    return readPing();
}

void look_ahead() {
  look(90);
}

void look_left() {
  look(0);
}

void look_right() {
   look(180);
}

void look(int degrees) {
  servo.write(degrees);
  delay(500);
}

int peek(int degree) {
  servo.write(degree);
  delay(300);
  return readPing();
}

void move_forward() {
    motorFL.run(FORWARD);
    motorFR.run(FORWARD);
}

void move_backward() {
    motorFL.run(BACKWARD);
    motorFR.run(BACKWARD);
    delay(2500);
}

void stop_moving() {
  motorFL.run(RELEASE);
  motorFR.run(RELEASE);
  delay(300);
}

void turn_left() {
  set_led(LEFT_TURN_LED_PIN, HIGH);
  motorFL.run(BACKWARD);
  motorFR.run(FORWARD);
  //delay(1000);
  delay(800);
  stop_moving();

  if (turn_again()) {
    turn_left();
  }
  set_led(LEFT_TURN_LED_PIN, LOW);
}

void turn_right() {
  set_led(RIGHT_TURN_LED_PIN, HIGH);
  motorFR.setSpeed(200);
  motorFL.run(FORWARD);
  motorFR.run(BACKWARD);  
  //delay(1000);
  delay(800);
  stop_moving();
  
  if (turn_again()) {
    turn_right();
  }
  set_led(RIGHT_TURN_LED_PIN, LOW);
}

bool turn_again() {
  int distance = readPing();

  return (distance < stopDist);
}

void set_led(uint8_t name, uint8_t value) {
  digitalWrite(name, value);
  delay(200);
}

int readPing() { 
  delay(70);
  
  int distance = sonar.ping_cm();

  if (distance == 0) {
    return 255;
  }  else {
    return distance;
  }
}
