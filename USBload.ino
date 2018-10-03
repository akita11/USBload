   #include <avr/wdt.h>
#include <avr/sleep.h>

// Load active cycle [sec]
//#define T_CYCLE_AO_BO 28 // A=open,  B=open
//#define T_CYCLE_AC_BO 18 // A=close, B=open
//#define T_CYCLE_AO_BC 13 // A=open,  B=close
//#define T_CYCLE_AC_BC 8  // A=close, B=close --> 9.5sec

#define T_CYCLE_AO_BO 24 // A=open,  B=open  / 29s/1.2=24.2s / for 30s wake
#define T_CYCLE_AC_BO 15 // A=close, B=open  / 19s/1.2=15.8s / for 20s wake
#define T_CYCLE_AO_BC 11 // A=open,  B=close / 14s/1.2=11.7s / for 15s wake
#define T_CYCLE_AC_BC 7  // A=close, B=close / 9s/1.2=7.5s   / for 10s wake

#define LOAD 2
#define MD_A 0
#define MD_B 1

#define T_ON 1100 // 1.1sec

// ATtiny10 sleep (Power-down)
// SMCR = 0x40; // SM2:0=010
// SMCR |= 0x01; // SE(SleepEnable=1)
// SLEEP instruction

// Watchdog timer

uint8_t Tcycle;
uint8_t T = 0;

ISR(WDT_vect) {}

uint8_t i;

void setup() {
  pinMode(LOAD, OUTPUT);
  pinMode(MD_A, INPUT); // digitalWrite(MD_A, HIGH);
  pinMode(MD_B, INPUT); // digitalWrite(MD_B, HIGH);
  PUEB |= 0x03; // enable pull-ups
  if (digitalRead(MD_B) == 1) {
    if (digitalRead(MD_A) == 1) Tcycle = T_CYCLE_AO_BO;
    else Tcycle = T_CYCLE_AC_BO;
  }
  else {
    if (digitalRead(MD_A) == 1) Tcycle = T_CYCLE_AO_BC;
    else Tcycle = T_CYCLE_AC_BC;
  }
  // initial load for 1sec
  digitalWrite(LOAD, HIGH);
  for (i = 0; i < 1; i++) delay(T_ON);
  digitalWrite(LOAD, LOW);
  cli();
  SMCR |= (1 << SM1) | (1 << SE); // power down
  PRR |= (1 << PRADC); // disable ADC
  wdt_reset();
  //  WDTCSR =  0b01000000 | 0b00100001; // WDIE=1,  cycle=8s
  //  WDTCSR =  0b01000000 | 0b00000111; // WDIE=1,  cycle=2s
  WDTCSR =  0b01000000 | 0b00000110; // WDIE=1,  cycle=1s
  sei();
}

void loop() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_mode();
  // goes to sleep

  // wake from sleep
  sleep_disable();
  /*
    if (Tcycle == 0) { // continuous ON if Tcycle==0
      digitalWrite(LOAD, HIGH);
    }
    else {
    }
  */
  T++;
  if (T == Tcycle) {
    T = 0;
    digitalWrite(LOAD, HIGH);
    delay(T_ON);
    digitalWrite(LOAD, LOW);
  }
}

