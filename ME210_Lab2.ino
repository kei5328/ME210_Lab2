/*************************************************************
  File:      ME210Lab2.ino
  Contents:  
  Notes:     Target: Arduino Uno
             
			 
*************************************************************/
/*---------------Module Defines-----------------------------*/

// Pinout
#define PIN_SIGNAL_IN 2
#define PIN_POT 3
#define PIN_SIGNAL_OUT 9

// Interrupts
#define INT_SIGNAL_IN 0

// Timers
#define TIMER_0            0
#define TIME_INTERVAL      1000

void SetupTimerInterrupt(void);
void UpdateCompareMatchRegister(void);
void UpdateOutputFrequency(void);
void UpdateCompareMatchRegister(uint8_t);
void CheckForKey(void);

static uint8_t signalToggle;
int potValue;
int keyPressed;

void setup(){
	  Serial.begin(9600);
  signalToggle = 0;
  SetupPins();
  SetupTimerInterrupt();
  keyPressed = 0;
}
void loop(){
	if(keyPressed==0){
	  UpdateOutputFrequency();
	 CheckForKey();
	Serial.println(OCR1AL);
	Serial.println(OCR1AH);
	}
}

void SetupPins() {
  pinMode(PIN_SIGNAL_OUT, OUTPUT);
  pinMode(PIN_POT, INPUT);
}

void CheckForKey(){
	if (Serial.available() > 0) {
				cli();  
				OCR1A = 0;
				Serial.print("Pressed");
				sei();  
				keyPressed = 1;
        }
		
}

void UpdateOutputFrequency() {
	potValue = analogRead(PIN_POT);
	Serial.println(potValue);
	uint16_t newOCR1A =  (analogRead(PIN_POT));
	UpdateCompareMatchRegister(newOCR1A);
}

void UpdateCompareMatchRegister(uint16_t newVal) {
  //if (newVal<15) newVal=16;
  uint16_t highRegister = newVal & 0b0000001100000000;
  highRegister = highRegister >> 8;
  cli();  
  OCR1AH = highRegister;
  OCR1AL = newVal;
  sei();  
}

void SetupTimerInterrupt() {
  cli();                               // Stop interrupts

  //Set Timer2 interrupt at 8kHz
  TCCR1A = _BV(COM1A1)|_BV(COM1B1)|_BV(WGM11)|_BV(WGM10);                          // Set TCCR1A Register
  TCCR1B = _BV(WGM12)|_BV(CS12)|_BV(CS10);                          // Same for TCCR2B
  TCNT1  = 0;                          // Initialize counter value to 0

  // Set compare match register to some initial value
  // 199 = (16*10^6) / (1250*64) - 1 (must be <256)
  OCR1A = 511;
  /*
  TCCR2A |= (1 << WGM21);              // Turn on CTC mode
  TCCR2B |= (1 << CS22);               // Set CS22 bit for 64 prescaler
  TIMSK2 |= (1 << OCIE2A);             // Enable timer compare interrupt
*/
  sei();                               //Allow interrupts
}

ISR(TIMER1_COMPA_vect) {
  if (signalToggle) {
    digitalWrite(PIN_SIGNAL_OUT, HIGH);
    signalToggle = 0;
  }
  else {
    digitalWrite(PIN_SIGNAL_OUT, LOW);
    signalToggle = 1;
  }
}
