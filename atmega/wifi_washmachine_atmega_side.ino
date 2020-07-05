

// output
#define POWER_BUTTON 11
#define START_BUTTON 12

// input
#define RUN_LED_INPUT 2             // pin 5, 6     start dioda 1 na pralce
#define PAUSE_LED_INPUT 9
#define WASH_LED_INPUT 3          // pin 1, 2     pranie dioda 2 na pralce
#define RINSE_LED_INPUT 4          // pin 3, 4     pĹ‚ukanie dioda 3 na pralce
#define SPIN_LED_INPUT 5           // pin 7, 8     wirowanie dioda 4 na pralce
#define DRAIN_LED_INPUT 6          // pin 9, 10    pompa wody dioda 5 na pralce
#define END_OF_WASH_LED_INPUT 7    // pin 11, 12   END dioda 6 na pralce
#define LOCK_LED_INPUT 8            // pin 13, 14   lock dioda 7 na pralce

int invalid_state = 2;
String inputString;         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete
String start_string = "command_start";
String pause_string = "command_pause";
String power_on_string = "command_powerOn";
String power_off_string = "command_powerOff";

const String washmashine_name = "iwsd51252";

String send_run_led_state = washmashine_name + "_ledrun_";
String send_pause_led_state = washmashine_name + "_ledpause_";
String send_wash_led_state = washmashine_name + "_ledwash_";
String send_rinse_led_state = washmashine_name + "_ledrinse_";
String send_spin_led_state = washmashine_name + "_ledspin_";
String send_drain_led_state = washmashine_name + "_leddrain_";
String send_end_of_wash_led_state = washmashine_name + "_ledendofwash_";
String send_lock_led_state = washmashine_name + "_ledlock_";

int current_led_state = 5;
int run_led_state = 2;
int pause_led_state = 2;
int wash_led_state = 2;
int rinse_led_state = 2;
int spin_led_state = 0;
int drain_led_state = 0;
int end_of_wash_led_state = 0;
int lock_led_state = 0;
// Variables will change:


unsigned long previousMillis = 0;
const long interval = 3*1000;           // interval 1 seconds
unsigned long currentMillis = 0;

String msg = "";
void setup() {
  pinMode(POWER_BUTTON, OUTPUT);
  digitalWrite(POWER_BUTTON, LOW);
  pinMode(START_BUTTON, OUTPUT);
  digitalWrite(START_BUTTON, LOW);

  pinMode(RUN_LED_INPUT, INPUT);
  digitalWrite (RUN_LED_INPUT, HIGH);

  pinMode(PAUSE_LED_INPUT, INPUT);
  digitalWrite (PAUSE_LED_INPUT, HIGH);

  pinMode(WASH_LED_INPUT, INPUT);
  digitalWrite (WASH_LED_INPUT, HIGH);

  pinMode(RINSE_LED_INPUT, INPUT);
  digitalWrite (RINSE_LED_INPUT, HIGH);

  pinMode(SPIN_LED_INPUT, INPUT);
  digitalWrite (SPIN_LED_INPUT, HIGH);

  pinMode(DRAIN_LED_INPUT, INPUT);
  digitalWrite (DRAIN_LED_INPUT, HIGH);

  pinMode(END_OF_WASH_LED_INPUT, INPUT);
  digitalWrite (END_OF_WASH_LED_INPUT, HIGH);

  pinMode(LOCK_LED_INPUT, INPUT);
  digitalWrite (LOCK_LED_INPUT, HIGH);

  // initialize serial:
  Serial.begin(9600);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
  initialize_led_status();
  Serial.println("START");

}

void loop() {
  //               Serial.println("GO TO");
  //Serial.println(run_led_state);

 currentMillis = millis();
        if (currentMillis - previousMillis >= interval)
        {
          // save the last time you blinked the LED
          previousMillis = currentMillis;
          initialize_led_status();

          
        }
  run_led_state = read_led_state(RUN_LED_INPUT, run_led_state, send_run_led_state);
  pause_led_state = read_led_state(PAUSE_LED_INPUT, pause_led_state, send_pause_led_state);

  wash_led_state = read_led_state(WASH_LED_INPUT, wash_led_state, send_wash_led_state);
  rinse_led_state = read_led_state(RINSE_LED_INPUT, rinse_led_state, send_rinse_led_state);
  spin_led_state = read_led_state(SPIN_LED_INPUT, spin_led_state, send_spin_led_state);
  drain_led_state = read_led_state(DRAIN_LED_INPUT, drain_led_state, send_drain_led_state);
  end_of_wash_led_state = read_led_state(END_OF_WASH_LED_INPUT, end_of_wash_led_state, send_end_of_wash_led_state);
  lock_led_state = read_led_state(LOCK_LED_INPUT, lock_led_state, send_lock_led_state);

  // print the string when a newline arrives:
  if (stringComplete) {
    //Serial.println(inputString);

    if (inputString == start_string)
    {
      digitalWrite(START_BUTTON, HIGH);
      delay(2000);
      digitalWrite(START_BUTTON, LOW);
    }
    else if (inputString == power_on_string)
    {
      digitalWrite(POWER_BUTTON, HIGH);
      delay(3000);
      digitalWrite(POWER_BUTTON, LOW);
    }
    else if (inputString == power_off_string)
    {
      digitalWrite(POWER_BUTTON, HIGH);
      delay(4000);
      digitalWrite(POWER_BUTTON, LOW);
    }
    else if (inputString == pause_string)
    {
      digitalWrite(START_BUTTON, HIGH);
      delay(2000);
      digitalWrite(START_BUTTON, LOW);
    }
    else
    {}

    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}



int read_led_state(int led_name, int led_state, String message)
{
  current_led_state = digitalRead(led_name);
  //Serial.println("current");

  // Serial.println(current_led_state);
  //    Serial.println("global");

  //  Serial.println(led_state);

  //   delay(100);

  if (current_led_state != led_state)
  {
    if (!current_led_state)
    {
      //client.write(const_cast<char*>(msg.c_str()));
      delay(100);
      //Serial.println(const_cast<char*>(msg.c_str()));
      Serial.println(message + "true");
    }
    else
    {
      //client.write(const_cast<char*>(msg.c_str()));
      delay(100);
      //Serial.println(const_cast<char*>(msg.c_str()));
      Serial.println(message + "false");
    }
    
   return current_led_state;
  }
}



/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
    else {
      if ((' ' <= inChar) && (inChar <= '~'))
        inputString += inChar;
    }
  }
}


void initialize_led_status()
{
  //int invalid_state = 2;
  wash_led_state = read_led_state(WASH_LED_INPUT, invalid_state, send_wash_led_state);
  rinse_led_state = read_led_state(RINSE_LED_INPUT, invalid_state, send_rinse_led_state);
  run_led_state = read_led_state(RUN_LED_INPUT, invalid_state, send_run_led_state);
  pause_led_state = read_led_state(PAUSE_LED_INPUT, invalid_state, send_pause_led_state);
  spin_led_state = read_led_state(SPIN_LED_INPUT, invalid_state, send_spin_led_state);
  drain_led_state = read_led_state(DRAIN_LED_INPUT, invalid_state, send_drain_led_state);
  end_of_wash_led_state = read_led_state(END_OF_WASH_LED_INPUT, invalid_state, send_end_of_wash_led_state);
  lock_led_state = read_led_state(LOCK_LED_INPUT, invalid_state, send_lock_led_state);
}
