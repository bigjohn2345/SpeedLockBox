bool devicecharging=FALSE;

void setup() {
  
  pinMode(13, OUTPUT);

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
