#include <stdlib.h>

void setup() 
{
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);

  Serial.setTimeout(100);
}

void loop() 
{
  while(Serial.available())
  {
    digitalWrite(LED_BUILTIN, (Serial.read()-'0'));
  }
}
