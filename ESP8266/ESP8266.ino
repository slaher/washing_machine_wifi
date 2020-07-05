#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

// output
#define POWER_BUTTON 5
#define START_BUTTON 15

// input
#define WASH_LED_INPUT 16           // pin 1, 2     pranie dioda 2 na pralce
#define RINSE_LED_INPUT 0           // pin 3, 4     płukanie dioda 3 na pralce
#define RUN_LED_INPUT 2             // pin 5, 6     start dioda 1 na pralce
#define SPIN_LED_INPUT 14           // pin 7, 8     wirowanie dioda 4 na pralce
#define DRAIN_LED_INPUT 12          // pin 9, 10    pompa wody dioda 5 na pralce
#define END_OF_WASH_LED_INPUT 13    // pin 11, 12   END dioda 6 na pralce
#define LOCK_LED_INPUT 4            // pin 13, 14   lock dioda 7 na pralce

// configure wifi
char ssid[] = ""; //Enter SSID
char password[] = ""; //Enter Password

//DDNS configuration
String domain = "slaherpralka.duckdns.org";
String token = "";
   
const String washmashine_name = "iwsd51252";

String send_wash_led_state = washmashine_name + "_ledwash_";
String send_rinse_led_state = washmashine_name + "_ledrinse_";
String send_run_led_state = washmashine_name + "_ledrun_";
String send_spin_led_state = washmashine_name + "_ledspin_";
String send_drain_led_state = washmashine_name + "_leddrain_";
String send_end_of_wash_led_state = washmashine_name + "_ledendofwash_";
String send_lock_led_state = washmashine_name + "_ledlock_";

String pause_string = washmashine_name + "_pause";
String start_string = washmashine_name + "_start";
String power_on_string = washmashine_name + "_power_on";
String power_off_string = washmashine_name + "_power_off";

String inData;
String local_ip;
char thisChar;
int wash_led_state = 0;
int rinse_led_state = 0;
int run_led_state = 0;
int spin_led_state = 0;
int drain_led_state = 0;
int end_of_wash_led_state = 0;
int lock_led_state = 0;


unsigned long previousMillis = 0;
const long interval_get_external_ip = 1*60*1000;           // interval 10 minutes
unsigned long currentMillis = 0;
 
String oldIp = "old";
String newIp = "new";

    
// Start a TCP Server on port 5045
WiFiServer server(5045);
HTTPClient http;


void setup() {
  pinMode(POWER_BUTTON, OUTPUT);
  digitalWrite(POWER_BUTTON,LOW);
  pinMode(START_BUTTON, OUTPUT);
  digitalWrite(START_BUTTON,  LOW);

  Serial.begin(9600);
  Serial.println("Configuration...");
  
  // output configuration
  pinMode(POWER_BUTTON, OUTPUT);
  digitalWrite(POWER_BUTTON,LOW);
  pinMode(START_BUTTON, OUTPUT);
  digitalWrite(START_BUTTON,  LOW);

  // input configuration
  pinMode(WASH_LED_INPUT, INPUT);
  digitalWrite(WASH_LED_INPUT, LOW);

  pinMode(RINSE_LED_INPUT,INPUT);
  pinMode(RUN_LED_INPUT,INPUT);
  pinMode(SPIN_LED_INPUT,INPUT);
  pinMode(DRAIN_LED_INPUT,INPUT);
  pinMode(END_OF_WASH_LED_INPUT,INPUT);
  pinMode(LOCK_LED_INPUT,INPUT);

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
//  WiFi.setDNS(dns1, dns2);
 
  // Start the TCP server
  server.begin();

  // initialize_led_status
  initialize_led_status();
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
        if (currentMillis - previousMillis >= interval_get_external_ip)
        {
          // save the last time you blinked the LED
          previousMillis = currentMillis;
          GetExternalIP();
        }
     } 
     //else{
    //  ESP.restart();
    // }
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
            initialize_led_status();
          }
        }
    } else {
      wash_led_state = read_led_state(WASH_LED_INPUT, wash_led_state, send_wash_led_state);
      rinse_led_state = read_led_state(RINSE_LED_INPUT, rinse_led_state, send_rinse_led_state);
      run_led_state = read_led_state(RUN_LED_INPUT, run_led_state, send_run_led_state);
      spin_led_state = read_led_state(SPIN_LED_INPUT, spin_led_state, send_spin_led_state);
      drain_led_state = read_led_state(DRAIN_LED_INPUT, drain_led_state, send_drain_led_state);
      end_of_wash_led_state = read_led_state(END_OF_WASH_LED_INPUT, end_of_wash_led_state, send_end_of_wash_led_state);
      lock_led_state = read_led_state(LOCK_LED_INPUT, lock_led_state, send_lock_led_state);

       
        // read data from the connected client
        if (client.available() > 0) 
        {
          thisChar = client.read();
          inData += thisChar;
        }
        else
        {
          Serial.print(inData);
          if(inData !="")
          {
            Serial.print("\n");
          }
          if (inData == start_string)
          {
            Serial.println("start!");
            digitalWrite(START_BUTTON, HIGH);
            delay(2000);
            digitalWrite(START_BUTTON, LOW);
          }
          else if (inData == power_on_string)
          {
            Serial.println("power ON!");
            digitalWrite(POWER_BUTTON, HIGH);
            delay(3000);
            digitalWrite(POWER_BUTTON, LOW);
          }    
          else if (inData == power_off_string)
          {
            Serial.println("power OFF!");
            digitalWrite(POWER_BUTTON, HIGH);
            delay(4000);
            digitalWrite(POWER_BUTTON, LOW);
          }
          else if (inData == pause_string)
          {
            Serial.println("pause!");
            digitalWrite(START_BUTTON, HIGH);
            delay(2000);
            digitalWrite(START_BUTTON, LOW);
          }
          else
          {}
         
          inData = "";
        }
      }
    }
    
int read_led_state(int led_name, int led_state, String message)
{
  int current_led_state = digitalRead(led_name);

       if(current_led_state != led_state)
          {
            if (!current_led_state)
            {
              String msg = message + "true";
              client.write(const_cast<char*>(msg.c_str()));
              delay(100);
              Serial.println(const_cast<char*>(msg.c_str()));
            }
            else
            {
              String msg = message + "false";
              client.write(const_cast<char*>(msg.c_str()));
              delay(100);
              Serial.println(const_cast<char*>(msg.c_str()));
            } 
            return current_led_state;
          }
  }

void initialize_led_status()
{
      int invalid_state = 2;
      wash_led_state = read_led_state(WASH_LED_INPUT, invalid_state, send_wash_led_state);
      rinse_led_state = read_led_state(RINSE_LED_INPUT, invalid_state, send_rinse_led_state);
      run_led_state = read_led_state(RUN_LED_INPUT, invalid_state, send_run_led_state);
      spin_led_state = read_led_state(SPIN_LED_INPUT, invalid_state, send_spin_led_state);
      drain_led_state = read_led_state(DRAIN_LED_INPUT, invalid_state, send_drain_led_state);
      end_of_wash_led_state = read_led_state(END_OF_WASH_LED_INPUT, invalid_state, send_end_of_wash_led_state);
      lock_led_state = read_led_state(LOCK_LED_INPUT, invalid_state, send_lock_led_state);
}

void GetExternalIP()
{
  String link = "http://api.ipify.org";
  Serial.print("[HTTP] begin...\n");
  if (http.begin(link)) 
    {  // HTTP
      Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();

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
   String update_url = "http://www.duckdns.org/update?domains="+domain+"&token="+token+"&ip="+ip+"";

    HTTPClient http2;
    Serial.println("start update ip");
    http2.begin(update_url);

    int httpCode = http2.GET();
        Serial.println("http code url update = "+httpCode);
       if(httpCode > 0) {
         oldIp = ip;
         Serial.println("success update ip to: "+ip);
        }
       http2.end();
}
