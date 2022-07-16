#ifdef TEST_joystick

#define x_remote_pin 34
#define y_remote_pin 35

void setup(){
  Serial.begin(115200);
  pinMode(x_remote_pin,INPUT);
  pinMode(y_remote_pin,INPUT);

}
int x;
int y;
void loop(){
  x=analogRead(x_remote_pin);
  y=analogRead(y_remote_pin);
  Serial.print("X:");
  Serial.print(x);
  Serial.print("|Y:");
  Serial.println(y);

}

#endif