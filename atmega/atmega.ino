
// outputs pin definition
#define POWER_BUTTON 11
#define START_BUTTON 12

// inputs pin definitions
#define RUN_LED_INPUT 2             // pin 5, 6     start LED 1 (green)on washmachine
#define PAUSE_LED_INPUT 9           // pin 15, 16   pause LED 1 (amber) on washmachine
#define WASH_LED_INPUT 3            // pin 1, 2     wash LED 2 on washmachine
#define RINSE_LED_INPUT 4           // pin 3, 4     rinse LED 3 on washmachine
#define SPIN_LED_INPUT 5            // pin 7, 8     spin LED 4 on washmachine
#define DRAIN_LED_INPUT 6           // pin 9, 10    drain LED 5 on washmachine
#define END_OF_WASH_LED_INPUT 7     // pin 11, 12   end LED 6 on washmachine
#define LOCK_LED_INPUT 8            // pin 13, 14   lock LED 7 on washmachine

const String washmashine_name = "iwsd51252";

String inputString;           // atring to hold incoming data
bool stringComplete = false;  // whether the string is complete

// commands from ESP
String start_string = "command_start";
String pause_string = "command_pause";
String power_on_string = "command_powerOn";
String power_off_string = "command_powerOff";

// LED states to send to mobile application
String send_run_led_state = washmashine_name + "_ledrun_";
String send_pause_led_state = washmashine_name + "_ledpause_";
String send_wash_led_state = washmashine_name + "_ledwash_";
String send_rinse_led_state = washmashine_name + "_ledrinse_";
String send_spin_led_state = washmashine_name + "_ledspin_";
String send_drain_led_state = washmashine_name + "_leddrain_";
String send_end_of_wash_led_state = washmashine_name + "_ledendofwash_";
String send_lock_led_state = washmashine_name + "_ledlock_";

// led states declarations
int initialize_state = 2;
int current_led_state = initialize_state;
int run_led_state = initialize_state;
int pause_led_state = initialize_state;
int wash_led_state = initialize_state;
int rinse_led_state = initialize_state;
int spin_led_state = initialize_state;
int drain_led_state = initialize_state;
int end_of_wash_led_state = initialize_state;
int lock_led_state = initialize_state;

void setup() {
  // set inputs, outputs
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
  
  update_led_states();
  
  Serial.println("START");
}

void loop() {
  update_led_states();

  if (stringComplete) {
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
 
  if (current_led_state != led_state)
  {
    if (!current_led_state)
    {
      delay(100);
      Serial.println(message + "true");
    }
    else
    {
      delay(100);
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
    // if the incoming character is a newline, set a flag so the main loop can
    if (inChar == '\n') {
      stringComplete = true;
    }
    else {
      if ((' ' <= inChar) && (inChar <= '~'))
        inputString += inChar;
    }
  }
}

void update_led_states()
{
  run_led_state = read_led_state(RUN_LED_INPUT, run_led_state, send_run_led_state);
  pause_led_state = read_led_state(PAUSE_LED_INPUT, pause_led_state, send_pause_led_state);

  wash_led_state = read_led_state(WASH_LED_INPUT, wash_led_state, send_wash_led_state);
  rinse_led_state = read_led_state(RINSE_LED_INPUT, rinse_led_state, send_rinse_led_state);
  spin_led_state = read_led_state(SPIN_LED_INPUT, spin_led_state, send_spin_led_state);
  drain_led_state = read_led_state(DRAIN_LED_INPUT, drain_led_state, send_drain_led_state);
  end_of_wash_led_state = read_led_state(END_OF_WASH_LED_INPUT, end_of_wash_led_state, send_end_of_wash_led_state);
  lock_led_state = read_led_state(LOCK_LED_INPUT, lock_led_state, send_lock_led_state);
}
