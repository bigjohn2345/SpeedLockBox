void setup() {
  
  pinMode(13, OUTPUT);
  bool devicecharging=FALSE;

}

void loop() {
  
  if(doorlatched) {    // check that door is latched

    digitalWrite(13, HIGH);
    devicecharging=TRUE;

  }

  else if(!doorlatched) {

    digitalWrite(13, LOW);
    devicecharging=FALSE;

  }

}
