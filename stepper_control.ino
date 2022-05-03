#include <avr/interrupt.h>
#include <GyverTimers.h>
#include <SPI.h>
#include "LD_Protocentral_ADS1220.h"
Protocentral_ADS1220 ads1220;

#define dirPin 8
#define stepPin 3
#define ENAPin 4
#define SPI_CLK  13
#define SPI_MISO 12
#define SPI_MOSI 11
#define ADC_CS   10
#define ADC_DRDY 6
#define AI2 A2

int32_t adc_data;
uint8_t  Td = 5;
uint32_t ADC_t = 0;

volatile byte pinA2state;
volatile byte pinA0state;
volatile byte pinD3count = 0;


void setup() {
  // put your setup code here, to run once:
    Serial.begin(115200);
    Serial.setTimeout(100);
    

    //while (!Serial) {  ; } // Ожидание подключения для настоящих USB
    ads1220.begin(ADC_CS,ADC_DRDY);
    ads1220.set_data_rate(DRT_2000SPS);// Частота АЦП для турбо режима
    ads1220.set_pga_gain(PGA_GAIN_1); // Усиление 1,2,4,8,16,32,64,128
    ads1220.set_FIR_Filter(FIR_OFF);//FIR_OFF, FIR_5060, FIR_50Hz, FIR_60Hz    
    ads1220.set_conv_mode_continuous(); // Непрерывное преобразование
    //ads1220.set_conv_mode_single_shot(); // Одиночное преобразование
    ads1220.set_OperationMode(MODE_TURBO); //Нормальный режим
    ads1220.Start_Conv(); // запуск преобразований
    ads1220.select_mux_channels(MUX_SE_CH2); // Выюор 3 канала

    pinMode(dirPin, OUTPUT);
    pinMode(stepPin, OUTPUT);//CHANEL_B
    pinMode(ENAPin, OUTPUT);
    
    pinMode(A0, INPUT);
    pinMode(A2, INPUT);
    pinA0state=digitalRead(A0);
    pinA2state=digitalRead(A2);
    
    digitalWrite(ENAPin,HIGH);
    delay(1);
    digitalWrite(dirPin,HIGH);
    delay(1);
    cli(); // отключить глобальные прерывания
//PCINT9=A1 pin, PCINT8=A0 pin
    PCICR=0b00000010; //enable Interuupts for PCINT 14-8
    PCMSK1=0b00000101;  //0bit=PCINT8 1bit=PCIN9 etc
   
    TCCR1A = 0; // установить TCCR1A регистр в 0
    TCCR1B = 0;
    OCR1A = 100; // установка регистра совпадения
    TCCR1B = 0b00000110; //count on falling edge
    TCCR1B |= (1 << WGM12); // включение в CTC режим
    TIMSK1 |= (1 << OCIE1A); // включить прерывание Timer1 on compare:
    

     Timer2.setFrequencyFloat(15000);
     //Timer1.enableISR(CHANNEL_A); 
     Timer2.outputEnable(CHANNEL_B, TOGGLE_PIN);//CHANEL_B
     Timer2.restart();
     interrupts();
     sei();  // включить глобальные прерывания
}

ISR(PCINT1_vect){
  pinA0state=digitalRead(A0);
  pinA2state=digitalRead(A2);
  if ((pinA0state==LOW) or (pinA2state==LOW)){
    digitalWrite(ENAPin,LOW);    
  }
  //Serial.print(pinA0state);Serial.print(", "); Serial.println(pinA2state);
  
}
ISR(TIMER1_COMPA_vect){
  //pinD3count+=1;
  //adc_data=ads1220.Read_WaitForData();
  //adc_data=ads1220.Read_SingleShot_SingleEnded_WaitForData(MUX_SE_CH2);  // Чтение 3 канала в одиночном режиме
}

void autochangedir(){
   if (pinA0state==LOW){
    digitalWrite(dirPin,LOW);
    digitalWrite(ENAPin,HIGH);
   }else if (pinA2state==LOW){
    digitalWrite(dirPin,HIGH);
    digitalWrite(ENAPin,HIGH);
   }else {
    digitalWrite(ENAPin,HIGH);
   }
}
void loop() {
  adc_data=ads1220.Read_WaitForData();
  Serial.println(adc_data*2.048/pow(2,23),4);// Serial.print(", "); Serial.print(pinD3count); Serial.print(", "); Serial.print(pinA0state);Serial.print(", "); Serial.println(pinA2state);
  
  pinA0state=digitalRead(A0);
  pinA2state=digitalRead(A2);
  autochangedir();

}
