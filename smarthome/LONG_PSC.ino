#include <DHT.h>
#include <Servo.h>
#include <SPI.h>
#include <RFID.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Arduino_FreeRTOS.h>
#include <task.h>
#include <queue.h>

QueueHandle_t queue;

#define SS_PIN 10
#define RST_PIN 9
#define LED2 8
#define servo 5
#define DHT_PIN 4
#define DHTTYPE DHT11
#define PIR 6
#define LED 3
#define BUZZER 17
#define QUAT 7
#define LED3 16

Servo myServo;
LiquidCrystal_I2C lcd(0x27, 16, 2);
RFID rfid(SS_PIN, RST_PIN);
DHT dht(DHT_PIN, DHTTYPE);


static int compare;
String data;
unsigned char reading_card[4];
unsigned char master[4] = { 0x93, 0x78, 0x71, 0x1A };
unsigned char slave[4] = { 0x09, 0xA5, 0xED, 0x5C };


void checkrfid_init()
{
  if (rfid.isCard()) {
    if (rfid.readCardSerial())
    {

      for (int i = 0; i < 4; i++) {
        memcpy(&reading_card, &rfid.serNum, sizeof(rfid.serNum));
      }
      if (memcmp(&master, &reading_card, sizeof(master)) == 0) compare = 1;
      if (memcmp(&slave, &reading_card, sizeof(slave)) == 0)  compare = 2;
      Serial.println(compare);
    }
    rfid.halt();
  }

}
TaskHandle_t Task1Handler1;
TaskHandle_t Task1Handler2;
TaskHandle_t TasklHandler3;



/**/

/*Task1*/
void checkrfid_door(void *pr)
{
  int count=0;
  pinMode(LED, OUTPUT);
  SPI.begin();
  rfid.init();
  myServo.write(0);
  int send;
  pinMode(BUZZER,OUTPUT);
  while (1)
  {
    
    int value = digitalRead(PIR);
    if (value == HIGH)digitalWrite(LED, HIGH);
    else  digitalWrite(LED, LOW);
    checkrfid_init();
    if (compare == 1)
    {
      send = 1;
      xQueueSend(queue, &send, 0);
      myServo.write(150);
      vTaskDelay(500 / portTICK_PERIOD_MS);
//      myServo.write(0);
      compare = 0;
      count =0;
      
    } else if (compare == 2)
    {
      Serial.println("count =");
      Serial.println(count);
      count++;
      if(count ==3)
      {
        Serial.println("count =");
        for(int i=0;i<5;i++)
        {
        digitalWrite(BUZZER,HIGH);
        vTaskDelay(250 / portTICK_PERIOD_MS);
        digitalWrite(BUZZER,LOW);
        vTaskDelay(250 / portTICK_PERIOD_MS);
        }
        count =0;
      }
      send = 2;
      xQueueSend(queue, &send, 0);
      vTaskDelay(500 / portTICK_PERIOD_MS);
      compare = 0;
    }
  }
  vTaskDelete(NULL);

}
/*Task2*/
void lcd_process(void *pr)
{
  
  int receive;
  lcd.init();
  lcd.backlight();
  dht.begin();
  while (1)
  {
    vTaskDelay(200/ portTICK_PERIOD_MS);
    xQueueReceive(queue, &receive, 0);
    vTaskSuspend(Task1Handler1);
    vTaskSuspend(TasklHandler3);
   float h = dht.readHumidity();
   float t = dht.readTemperature();
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print("Temp : ");
   lcd.print(t);
   lcd.print("oC");
   lcd.setCursor(0, 1);
   lcd.print("Humi : ");
   lcd.print(h);
   
   vTaskResume(Task1Handler1);
   vTaskResume(TasklHandler3);
    switch (receive)
    {
      case  1:
        
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("SMARTHOME vKMA");
        lcd.setCursor(1, 1);
        lcd.print("WELCOME HOME");
        receive = 0;
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        myServo.write(0);
        break;
      case 2:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("SMARTHOME vKMA");
        lcd.setCursor(1, 1);
        lcd.print("UUID Failed");
        receive = 0;
        vTaskDelay(1500 / portTICK_PERIOD_MS);
        break;
      default:
        break;
    }

  }
  vTaskDelete(NULL);
}

/*Task 3*/
void blt_process(void *pr)
{
  pinMode(LED2, OUTPUT);
  pinMode(QUAT,OUTPUT);
  pinMode(LED3, OUTPUT);
  char c;
  while (1)
  {
    
    while (Serial.available()) {
      vTaskDelay(30 / portTICK_PERIOD_MS);
      c = Serial.read();
      if (c == '#') break;
      data += c;
    }
    if (data.length() > 0) {
      Serial.println(data);
      if (data == "bật đèn" || data == "Bật Đèn" || c == '1')
      {
        digitalWrite(LED2, HIGH);
      } else if (data == "tắt đèn" || data == "Tắt Đèn" || c == '2')
      { 
        digitalWrite(LED2, LOW);
      }else if(data =="mở cửa" || c=='3')
      {
        myServo.write(140);
      }else if(data =="đóng cửa" || c=='4')
      {
        myServo.write(0);
      }
      else if(data =="bật quạt" || c=='5')
      {
        digitalWrite(QUAT,HIGH);
      }
      else if(data =="tắt quạt" || c=='6')
      {
        digitalWrite(QUAT,LOW);
      }

      data = "";

    }

  }
}

void setup()
{
  pinMode(PIR, INPUT);
  myServo.attach(servo);
  Serial.begin(9600);
  xTaskCreate(checkrfid_door, "Task1", 128, NULL, 6, &Task1Handler1);
  xTaskCreate(lcd_process, "Task2", 128, NULL, 4, &Task1Handler2);
  xTaskCreate(blt_process, "Task3", 128, NULL, 5, &TasklHandler3);
  queue = xQueueCreate(2, sizeof(int));
  vTaskStartScheduler();

}

void loop()
{



}
