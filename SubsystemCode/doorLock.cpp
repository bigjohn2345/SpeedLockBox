Servo myservo;
bool doorLocked;
bool phoneDetected;
bool motionDetected;

void setup() {
myservo.attach(9);
}

void loop() {
if( phoneDetected && motionDetected){
  myservo.write(100);
  doorLocked = true;
}
else{
  myservo.write(180);
  doorLocked = false;
}
}
