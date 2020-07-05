
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

// DDNS configuration
String domain = "";
String token = "";

// get current external IP
String link = "http://api.ipify.org";

// configure wifi
char ssid[] = ""; //Enter SSID
char password[] = ""; //Enter Password


int httpCode;
const String washmashine_name = "iwsd51252";

String pause_string = washmashine_name + "_pause";
String start_string = washmashine_name + "_start";
String power_on_string = washmashine_name + "_power_on";
String power_off_string = washmashine_name + "_power_off";


unsigned long previousMillis = 0;
const long interval = 10*60*1000;           // interval 10 minutes
unsigned long currentMillis = 0;
 
String oldIp = "old";
String newIp = "new";
String inData;
String local_ip;
char thisChar;
String inputString;         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete

char inChar;


// Start a TCP Server on port 5045
WiFiServer server(5045);
HTTPClient http;
HTTPClient http2;


void setup() {
  Serial.begin(9600);
    Serial.begin(9600);

  Serial.println("Configuration...");
  inputString.reserve(200);


  // WiFi configuration
  IPAddress ip(192,168,0,129);
  IPAddress gateway(192,168,0,1);
  IPAddress subnet(255,255,255,0);
  IPAddress dns1(8,8,8,8);
  IPAddress dns2(8,8,4,4);


  WiFi.config(ip, gateway, subnet,dns1, dns2);
  WiFi.begin(ssid,password);
  Serial.println("");
  
  //Wait for connection
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("Connected to "); Serial.println(ssid);
  Serial.print("IP Address: "); Serial.println(WiFi.localIP());
  Serial.print("Subnet: "); Serial.println(WiFi.subnetMask());
  Serial.print("Gateway: "); Serial.println(WiFi.gatewayIP());
 
  // Start the TCP server
  server.begin();

  // initialize_led_status
  if( WiFi.status() == WL_CONNECTED)
  {
    GetExternalIP();
  }
}

WiFiClient client;
void loop() 
{   
    if (WiFi.status() == WL_CONNECTED);
      {
        currentMillis = millis();
        if (currentMillis - previousMillis >= interval)
        {
          // save the last time you blinked the LED
          previousMillis = currentMillis;
          GetExternalIP();
        }
     } 
     
    if (!client.connected()) {
        // try to connect to a new client
        client = server.available();
        if (client)
        {
          if (client.connected())
          {
            Serial.println("Connected to client");
            delay(3000);

            // initialize_led_status
          }
        }
    } else {
      //KODZIK
 if (Serial.available()) {
    // get the new byte:
    inChar = (char)Serial.read();
    // add it to the inputString:
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    //Serial.println(inChar);

    if (inChar == '\n') {
      stringComplete = true;
    }
    else{
        if ((' ' <= inChar) && (inChar <= '~'))
        {
          inputString += inChar;
        }
      }
    }
    if (stringComplete) {
        //Serial.println(inputString);
         client.write(const_cast<char*>(inputString.c_str()));

          // clear the string:
        inputString = "";
        stringComplete = false;
      }

       
        // read data from the connected client
        if (client.available() > 0) 
        {
          thisChar = client.read();
          inData += thisChar;
        }
        else
        {
          //Serial.print(inData);
          if(inData !="")
          {
            Serial.print("\n");
          }
          if (inData == start_string)
          {
            Serial.println("command_start");
          }
          else if (inData == power_on_string)
          {
            Serial.println("command_powerOn");
          }    
          else if (inData == power_off_string)
          {
            Serial.println("command_powerOff");
          }
          else if (inData == pause_string)
          {
            Serial.println("command_pause");
          }
          else
          {}
         
          inData = "";
        }
      }
    }
    
void GetExternalIP()
{
  Serial.print("[HTTP] begin...\n");
  if (http.begin(link)) 
    {  // HTTP
      Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) 
        {
          newIp = http.getString();

          Serial.println(newIp);

          if (oldIp != newIp )
          {
            UpdateDynamicDnsIp(newIp);
            Serial.println("New IP available!");
          } 
        }
        
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    } else {
      Serial.printf("[HTTP} Unable to connect\n");
    }  
}

void UpdateDynamicDnsIp(String ip)
{
    Serial.println("start update ip");

    http2.begin("http://www.duckdns.org/update?domains="+domain+"&token="+token+"&ip="+ip+"");

    httpCode = http2.GET();
        Serial.println("http code url update = "+httpCode);
       if(httpCode > 0) {
         oldIp = ip;
         Serial.println("success update ip to: "+ip);
        }
       http2.end();
}


/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvents() {
  if (Serial.available()) {
    // get the new byte:
    inChar = (char)Serial.read();
    // add it to the inputString:
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    Serial.println(inChar);

    if (inChar == '\n') {
      stringComplete = true;
    }
    else{
        if ((' ' <= inChar) && (inChar <= '~'))
        {
          inputString += inChar;
        }
      }
    }
  }
