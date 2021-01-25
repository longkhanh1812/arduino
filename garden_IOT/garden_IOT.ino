#include <Arduino_FreeRTOS.h>
#include <RTClib.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"


#define DHTTYPE DHT11
#define APIN A0
#define DPIN 2
#define relay1PIN 3   //may bom
#define relay2PIN 7   //quat
#define r_ledPIN 4
#define g_ledPIN 5
#define dht11PIN 6

static int hr;
static int m;
static int s;
static int h;       //do am
static int t;     //nhiet do
static int value;   //do am dat
static int value1;
String guidulieu;

RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x27,20,4);
DHT dht(dht11PIN,DHTTYPE);

TaskHandle_t rd_ss;
TaskHandle_t hdl_evt;
void read_sensor(void *pr)
{
  lcd.init();                    
  lcd.backlight();
  rtc.begin();
  rtc.isrunning();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  while(1)
  {
  DateTime now = rtc.now();
  hr=now.hour();
  m=now.minute();
  s=now.second();
  value=analogRead(APIN);
  value1=digitalRead(DPIN);
  h=dht.readHumidity();
  t=dht.readTemperature();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("TODAY :");
  lcd.setCursor(8,0);
  lcd.print(hr);
  lcd.print(" : ");
  lcd.print(m);
  lcd.setCursor(0,1);
  lcd.print("Value : ");
  lcd.print(value);
  lcd.setCursor(0,2);
  lcd.print("Humi : ");
  lcd.print(h);
  lcd.setCursor(0,3);
  lcd.print("Temp : ");
  lcd.print(t);
  lcd.print("oC");
  vTaskDelay(10000/portTICK_PERIOD_MS);
  
  }
}
char state;

void handl_event(void *pr)
{
  pinMode(APIN,INPUT);
  pinMode(DPIN,INPUT);
  pinMode(relay1PIN,OUTPUT);
  pinMode(relay2PIN,OUTPUT);
  pinMode(r_ledPIN,OUTPUT);
  pinMode(g_ledPIN,OUTPUT);
  while(1)
  {
  vTaskDelay(733/portTICK_PERIOD_MS);
  String dlnhietdo = String(t);
  String dldoamdat =String(value);
  String guidulieu =dlnhietdo + dldoamdat;
  Serial.println(guidulieu);
  if(Serial.available() )
  {
    state = Serial.read();
    
  }
  Serial.println(state);
  if(state == '5')
  {
    Serial.println("a");
    digitalWrite(relay1PIN,HIGH);
  }else if(state=='6')
  {
    Serial.println("b");
    digitalWrite(relay1PIN,LOW);
  }else if(state=='7')
  {
    Serial.println("c");
    digitalWrite(relay2PIN,HIGH);
  }else if(state=='8')
  {
    Serial.println("d");
    digitalWrite(relay2PIN,LOW);
  }
  
  if(value>400)                 //do am dat chua du thi chua bat relay
  {
    
    digitalWrite(r_ledPIN,HIGH);
    digitalWrite(g_ledPIN,LOW);
    digitalWrite(relay1PIN,LOW);
  }else                       //do am dat da du thi bat relay -> bom nuoc
  {
    
    digitalWrite(r_ledPIN,LOW);
    digitalWrite(g_ledPIN,HIGH);
    digitalWrite(relay1PIN,HIGH);
  }
  if(t>35)
  {
   
    digitalWrite(relay2PIN,HIGH);
  }
  else
  {
    
    digitalWrite(relay2PIN,LOW);
  }
  if(hr==6 && m>=0)
  {
    if(m<=30)
    digitalWrite(relay2PIN,HIGH);
  }else if(hr==19 && (m>=0))
  {
    if(m<=30)
    {
    
    digitalWrite(relay2PIN,HIGH);
    }
  }else{
    
    digitalWrite(relay2PIN,LOW);
  }
  }
  
}


void setup() {

  
  Serial.begin(9600);
  xTaskCreate(read_sensor,"read sensor",256,NULL,6,&rd_ss);
  xTaskCreate(handl_event,"hdl evt",256,NULL,4,&hdl_evt);
  
  vTaskStartScheduler();
  

}

void loop() {

}
