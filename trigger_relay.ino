/* Description of sketch
*/

#ifndef cbi //clear
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi //set
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif 
#define DEFAULT_TRIGGER 133

volatile int trigger_number = DEFAULT_TRIGGER; //Default trigger
void setup() {

  Serial.begin(9600);

  DDRC = B11111111; //Set all of port C to output
  
  cbi(ADCSRB,ACME);//Select input AIN1 as the negative input of the Comparator
  cbi(ADCSRA,ADEN);//Turn ADC of

  ACSR = 
   (0<<ACD) |   // Analog Comparator: Enabled
   (0<<ACBG) |   // Analog Comparator Bandgap Select: AIN0 (pin D6) is used as positive comparator input
   (0<<ACO) |   // Analog Comparator Output: Off
   (0<<ACI) |   // Analog Comparator Interrupt Flag: interrupt sets value to 1 (manually reset to 0 in interrupt routine)
   (1<<ACIE) |   // Analog Comparator Interrupt: Enabled
   (0<<ACIC) |   // Analog Comparator Input Capture: Disabled
   (1<<ACIS1) | (1<<ACIS0);   // Analog Comparator Interrupt Mode: Comparator Interrupt on falling edge
    // ACIS1/ACIS0 = 0/0 for toggle, 1/0 for falling edge, 1/1 for rising edge

}

void loop() {

    if(Serial.available()>0){
      if (trigger_number!=DEFAULT_TRIGGER){
        Serial.print("Warning: previously loaded trigger overwritten.\n");
      }
      trigger_number=Serial.parseInt();  //Converts the string received into an integer
      Serial.print("Trigger received: ");
      Serial.print(trigger_number);
      Serial.print("\n");
    }

}

ISR(ANALOG_COMP_vect){ //ANALOG_COMP_vect is defined as _VECTOR(29)
  cbi(ACSR,ACIE); // disable further interrupts while we service this one
  
  PORTC = trigger_number; //8bit TTL output
  delayMicroseconds(2000); //2ms pause
  PORTC = 0; //Reset
  
  if(trigger_number == DEFAULT_TRIGGER){
    Serial.print("Default trigger sent\n");
  }
  else {
    Serial.print("Trigger sent: ");
    Serial.print(trigger_number);
    Serial.print("\n");
  }
  
  int i = 100; //100ms pause
  while(i > 0)
  {
    delayMicroseconds(1000); 
    i--;
  }
  
  trigger_number = DEFAULT_TRIGGER; //Reset to default trigger
  cbi(ACSR,ACI);  // clear interrupt flag
  sbi(ACSR,ACIE); // re-enable further interrupts
}
