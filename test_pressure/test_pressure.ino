int PIN_PRESSURE1 = 0; // TODO
int PIN_PRESSURE2 = 0; // TODO
int PIN_ARM_POS = 0; // TODO

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  Serial.print("{");
  Serial.print("\"pressure1\":");
  Serial.print(analogRead(PIN_PRESSURE1));
  Serial.print(",\"pressure2\":");
  Serial.print(analogRead(PIN_PRESSURE2));
  Serial.print(",\"arm_pos\":");
  Serial.print(digitalRead(PIN_ARM_POS));
  Serial.println("}");
  delay(1000);
}
