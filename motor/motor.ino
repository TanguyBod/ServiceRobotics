int motor1pin1 = 5;
int motor1pin2 = 6;

int motor2pin1 = 10;
int motor2pin2 = 11;

void setup() {
  Serial.begin(115200);
  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(motor2pin1, OUTPUT);
  pinMode(motor2pin2, OUTPUT);
  
  analogWrite(motor1pin1, 125);
  digitalWrite(motor1pin2, LOW);
  analogWrite(motor2pin1, 125);
  digitalWrite(motor2pin2, LOW); 
}

void loop () {
  // Serial.println("Output motor 1");
  // Serial.println(digitalRead(3));
  // Serial.println(digitalRead(4));
  // Serial.println("Output motor 2");
  // Serial.println(digitalRead(12));
  // Serial.println(digitalRead(13));
  // delay(1000);
}