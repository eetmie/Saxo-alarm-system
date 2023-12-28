//super car super safety system 1.1 24.1.2023 (super informative super trunk reset edition)
//by eddu
#define BUTTON_PIN1 12
#define BUTTON_PIN2 10
#define BUTTON_PIN3 8
#define RESET_PIN1 7
#define BUZZ_PIN 6
#define LED_PIN 4
#define RELAY_PIN 2
// all pins INPUT_PULLUP
#define pintype INPUT_PULLUP

const int waitBeforeAlarm = 9000; //wait time before alarm sounds in seconds
const int alarmRunTime = 10; //alarm run time in seconds

const int doorDelay = 500; //delay between checks in millis
const int flashDelay = 2000; // delay between flashes

bool ResetArmed = false;

int timesAlarmTriggered = 0; //how many times alarm has been triggered


//___________________________________________________________

//timeout
class TimeoutTimer {
  public:
    TimeoutTimer(unsigned long timeout) {
      _timeout = timeout;
      _startTime = millis();
      _isTimedOut = false;
    }
    bool isTimedOut() {
      if (_isTimedOut) {
        return true;
      }
      if (millis() - _startTime > _timeout) {
        _isTimedOut = true;
        return true;
      }
      return false;
    }
    void reset() {
      _startTime = millis();
      _isTimedOut = false;
    }
  private:
    unsigned long _timeout;
    unsigned long _startTime;
    bool _isTimedOut;
};



 // outputs for led and relays
class Output {
  private:
    byte pin;
  public:
    Output(byte pin) {
      this->pin = pin;
      init();
    }
    void init() {
      pinMode(pin, OUTPUT);
      off();
    }
    void on() {
      digitalWrite(pin, HIGH);
    }
    void off() {
      digitalWrite(pin, LOW);
    }
};



 // inputs for sensing the doors
class Input {
  private:
    byte pin;
    byte state;
    byte lastReading;
    unsigned long lastDebounceTime = 0;
    unsigned long debounceDelay = 20;
  public:
    Input(byte pin) {
      this->pin = pin;
      lastReading = LOW;
      init();
    }
    void init() {
      pinMode(pin, pintype);
      update();
    }
    void update() {
      byte newReading = digitalRead(pin);
      
      if (newReading != lastReading) {
        lastDebounceTime = millis();
      }
      if (millis() - lastDebounceTime > debounceDelay) {
        state = newReading;
      }
      lastReading = newReading;
    }
    byte getState() {
      update();
      return state;
    }
    bool isActive() {
      return (getState() == HIGH);
   }
};

// Inputs
Input Door1(BUTTON_PIN1);
Input Door2(BUTTON_PIN2);
Input Door3(BUTTON_PIN3);
Input ResetButton1(RESET_PIN1);

// Outputs
Output Buzzer(BUZZ_PIN);
Output Led(LED_PIN);
Output Relay(RELAY_PIN);

// Timers
TimeoutTimer flashTimer(flashDelay);
TimeoutTimer doorTimer(doorDelay);
TimeoutTimer alarmTimer(waitBeforeAlarm);


void setup() {
  Serial.begin(9600);
  Led.on();
  Buzzer.on();
  delay(100);
  Buzzer.off();
  delay(8000);
  while (!doorsClosed()) {
    //Serial.println("waiting all doors to be closed...");
    beep(3,80);
    delay(3000);
    
  }
  Led.off();
  delay(2000);
  //Serial.println("security active!");
  ResetArmed = false;
}

void loop() {
    
 if (doorTimer.isTimedOut()) {
    if (anyDoorOpen()) {
        Led.on();
        beep(2,200);
        delay(500);
        beep(timesAlarmTriggered,500);
      	ResetArmed = true;
      	while (ResetArmed) {
          
          	//Serial.println("kohta huutaa");	
        	//delay(waitBeforeAlarm*1000);
        	if (alarmTimer.isTimedOut()) {
        		alarm();
        	}
          	if (resetButtonPressed()) {
              	//Serial.println("resetoitu");
              	beep(1,1000);
              	delay(5000);
            	setup();
            }
          delay(100);
        }
    }
      doorTimer.reset();
 }   
  
 if (flashTimer.isTimedOut()) {
      flash(1,100);
      flashTimer.reset();
      }
  alarmTimer.reset();
  delay(20);
  }
//__________________________________________________


// check if any door is open
bool anyDoorOpen() {
  Door1.update();
  Door2.update();
  Door3.update();
  //Serial.println("checking if any door open...");
  if (!Door1.getState() || !Door2.getState() || !Door3.getState() ){
    return true; 
  }
  return false;
}
  
// check if reset button is pressed  
bool resetButtonPressed() {
  ResetButton1.update();
  //Serial.println("checking if any door open...");
  if (ResetButton1.getState()){
    return true; 
  }
  return false;
}

// check if all the doors are closed
bool doorsClosed() {
  Door1.update();
  Door2.update();
  Door3.update();
  //Serial.println("checking if all doors closed...");
  if (Door1.getState() && Door2.getState() && Door3.getState() ){
    return true; 
  }
  return false;
}

// alarm code  
void alarm() {
  Buzzer.on();
  Led.on();
  Relay.on();
  delay(alarmRunTime*1000);
  reset();
  timesAlarmTriggered++;
  delay(20);
 }

// beep code
void beep(int beeps, int beepsDelay) {
  for (int i=0; i<beeps; i++) {
    Buzzer.on();
    delay(beepsDelay);
    Buzzer.off();
    delay(beepsDelay);
  }
 }  

// LED flash code
void flash(int flashes, int flashesDelay) {
  //Serial.println("flashing...");
  for (int i=0; i<flashes; i++) {
    Led.on();
    delay(flashesDelay);
    Led.off();
    delay(flashesDelay);
  }
 }  

// reset code
void reset() {
  Buzzer.off();
  Led.off();
  Relay.off();
  Door1.update();
  Door2.update();
  Door3.update();
  ResetArmed = false;
}