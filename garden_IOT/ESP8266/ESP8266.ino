#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
 #include <string.h>
MDNSResponder mdns;
 
ESP8266WebServer server(80);
String webPage;
const char* ssid     = "2704";      //Thay tên wifi ở đây
const char* password = "@dangmien@";  //Thay tên password ở đây
 String output5State ;
String output4State;
String hienthisensor;
int nhietdo;
int doamdat;
String nd;
String dad;
void setup() {
 
 
  
  webPage += "<!DOCTYPE html><html>";
  webPage += "<head><meta name=\"viewport\" content=\"width=device-width\", \"initial-scale=1\">";
  webPage += "<link rel=\"icon\" href=\"data:,\">";
  webPage += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}";
  webPage += ".button { background-color: #77878A; border: none; color: white; padding: 16px 40px;";
  webPage += "text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}";
  webPage += ".button2 {background-color: #195B6A;}</style></head>";
  webPage += "<body style=\"background-color:#ecf0f1\"><h1>Smart Garden</h1>";
  webPage += "<p> MAY BOM </p>";
  webPage += "<p><a href=\"/mb/on\"><button class=\"button button2\">ON</button></a></p>";
  webPage += "<p><a href=\"/mb/off\"><button class=\"button\">OFF</button></a></p>";
  webPage += "<p> QUAT GIO </p>";
  webPage += "<p><a href=\"/quat/on\"><button class=\"button button2\">ON</button></a></p>";
  webPage += "<p><a href=\"/quat/off\"><button class=\"button\">OFF</button></a></p>";
  webPage += "</body></html>";
  Serial.begin(9600);
  delay(100);
 
 

  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  if (mdns.begin("esp8266", WiFi.localIP())) 
    Serial.println("MDNS responder started");
 
  server.on("/", [](){
    server.send(200, "text/html", webPage);
  });
  server.on("/mb/on", [](){
    server.send(200, "text/html", webPage);
    Serial.println("5");

  });
  server.on("/mb/off", [](){
    server.send(200, "text/html", webPage);
    Serial.println("6");
    
  });
  server.on("/quat/on", [](){
    server.send(200, "text/html", webPage);
    Serial.println("7");
  });
  server.on("/quat/off", [](){
    server.send(200, "text/html", webPage);
   Serial.println("8");
  });
  server.on("/hienthi",hienthi);
  server.begin();
  Serial.println("HTTP server started");
}
  
String dulieu;
void loop() {
  char c;
  dulieu="";
  server.handleClient();
  while (Serial.available()) {
      delay(30);
      c = Serial.read();
      if (c == '#') break;
      dulieu += c;
  }  

  
  char a[4];
  char b[5];  
  
  int j=0;
  for(int i=2;i<dulieu.length();i++)
  {
    
    b[j]=dulieu[i];
    j++;
  }
  int k=0;
  for(int i=0;i<2;i++)
  {
    a[k]=dulieu[i];
    k++;
  }
  nd=String(a);
  dad=String(b);
  
  delay(500);
 
}

void hienthi()
{
  hienthisensor += "<p>NHIET DO <input name='nhietdo' value="+nd+"></p>";
  hienthisensor += "<p>DO AM DAT<input name='doamdat' value="+dad+"></p>";
  server.send(200 , "text/html" , hienthisensor);
  hienthisensor = "";
}