#include "FspTimer.h"
#include "pwm.h"

FspTimer audio_timer;
uint64_t count=0;
uint64_t start_time=0;

// callback method used by timer
void timer_callback(timer_callback_args_t __attribute((unused)) *p_args) {
  count++;
}

bool beginTimer(float rate) {
  uint8_t timer_type = GPT_TIMER;
  int8_t tindex = FspTimer::get_available_timer(timer_type);
  if (tindex < 0){
    tindex = FspTimer::get_available_timer(timer_type, true);
  }
  if (tindex < 0){
    return false;
  }

  FspTimer::force_use_of_pwm_reserved_timer();

  if(!audio_timer.begin(TIMER_MODE_PERIODIC, timer_type, tindex, rate, 0.0f, timer_callback)){
    return false;
  }

  if (!audio_timer.setup_overflow_irq()){
    return false;
  }

  if (!audio_timer.open()){
    return false;
  }

  if (!audio_timer.start()){
    return false;
  }
  return true;
}


PwmOut pwm(LED_BUILTIN);

int sensVal = 0;
int led = MISO;
//there are 7 GPT timers which are also used by PWM.
void setup() {
  //pwm.begin(20000.0f, 0.0f);     //period 50us = 20000hz; pulse 0 us = 0% 
  pwm.begin(10000.0f, 0.0f);     //period 100us = 10000hz; pulse 0 us = 0% 
  pwm.pulse_perc(100.0f);        // set 100%
  pinMode(D2,OUTPUT); 
  pinMode(led,OUTPUT);
  pinMode(D1, OUTPUT);  // set 100%
 //  analogWrite(D1,255)  // to create a PWM signal

  Serial.begin(2400);   //Initialize serial and wait for port to open:
  Serial1.begin(2400); 
  Serial.println("Arduino UNO R4 TEST");
  while (!Serial) {
    Serial.print(". ");               ;  // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Start TEST");
  beginTimer(1000);     //executes a callback method at a defined interval rate e.g. 1000 hz.
  start_time = millis();
}

void loop() {
  int sensorValue = analogRead(A0);
  int freq = 1000 * count / (millis()-start_time);        // calculate the effective frequency
  Serial.print("timeR = ");   Serial.println(freq);
  Serial1.print("timeR = ");  Serial1.println(freq);
  count = 0;
  start_time = millis();
 // delay(1000);
Serial.print("Read Analog = ");   Serial.println(sensorValue);
Serial1.print("Read Analog = ");  Serial1.println(sensorValue);
digitalWrite(D2, HIGH);  delay(50);
digitalWrite(D2, LOW);   delay(50);
Serial.println(sensVal);    Serial1.println(sensVal);
sensVal++;
if (sensVal == 128)
{
  sensVal = 0;
}
}
