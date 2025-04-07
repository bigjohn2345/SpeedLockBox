Servo myservo;
bool doorLocked;
bool phoneDetected;
bool IsAtSpeed;

void setup() {
myservo.attach(9);
}

void loop() {
if( phoneDetected && IsAtSpeed){
  myservo.write(100);
  doorLocked = true;
}
else{
  myservo.write(180);
  doorLocked = false;
}
}
