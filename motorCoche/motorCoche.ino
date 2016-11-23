/* 
control motor DC
 */

int IN3 = 12;    // Input3 conectada al pin 12
int IN4 = 13;    // Input4 conectada al pin 13 
int ENB = 11;    // ENB conectada al pin 11 de Arduino para PWM
void setup()
{
  pinMode (ENB, OUTPUT); 
  pinMode (IN3, OUTPUT);
  pinMode (IN4, OUTPUT);
}
void loop()
{
  for (int i = 60; i<255; i++)
  {
    adelante(i);
    delay(20);
  }
  freno();
  //delay(1000);
  for (int i = 60; i<255; i++)
  {
    atras(i);
    delay(20);
  }
  freno();
}
void atras(int velocidad)
{
  digitalWrite (IN3, HIGH);
  digitalWrite (IN4, LOW);
  analogWrite(ENB,velocidad);
}
void adelante(int velocidad)
{
  digitalWrite (IN3, LOW);
  digitalWrite (IN4, HIGH);
  analogWrite(ENB,velocidad);
}
void freno()
{
  digitalWrite (IN3, LOW);
  digitalWrite (IN4, LOW);
}

