#define PIR 8

void setup()
{
   pinMode(PIR, INPUT);
   Serial.begin(9600);
}


void loop()
{
   int value = digitalRead(PIR);
   if(value ==HIGH)
   {
    Serial.println("HIGH");
   }else Serial.println("LOW");
}
