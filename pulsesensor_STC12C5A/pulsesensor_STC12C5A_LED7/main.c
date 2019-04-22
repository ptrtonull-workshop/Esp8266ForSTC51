/*************************************************************************
********                                                           *******            
********    网店：【星辉科技电子】http://shop108071095.taobao.com/  *******
********                                                            *******
********    旺旺： anning86525                 											******
********                                                            *******
**************************************************************************/
//******************************参数说明*********************************//
//MCU:STC12C5A60S2
//ADC PIN:P1.0
//SYSTEM CLOCK:11.0592MHz
//Baudrate:115200
//UART:P3.0 P3.1
//**********************************************************************//
#include <STC12C5A60S2.h>
#include "stdio.h"

#define false 0
#define true 1
#define FOSC 11059200L		//系统时钟
#define BAUD 115200				//波特率
#define T0MS (65536-FOSC/12/500)		//500HZ in 12T MODE

#define ADC_POWER 0x80			//ADC POWER CONTROL BIT
#define ADC_FLAG 0x10			//ADC COMPLETE FLAG
#define ADC_START 0x08;			//ADC START CONTROL BIT
#define ADC_SPEEDLL 0x00		//540 CLOCKS
#define ADC_SPEEDL 0x20			//360 CLOCKS
#define ADC_SPEEDH 0x40			//180 CLOCKS
#define ADC_SPEEDHH 0x60		//90 CLOCKS
#define ADC_MASK 0x01

//数码管段码显示：0~f,不亮
unsigned char code LED_Disp[] = {0xC0,0xF9,0xA4,0xB0,
                        0x99,0x92,0x82,0xF8,
						0x80,0x90,0x88,0x83,
						0xC6,0xA1,0x86,0x8E};

sfr LedPort = 0x80;	 //段选段P0
sfr LedCtrl = 0xa0;	 //位选段P2
unsigned char DisBuff[3];

void UART_init(void); 
void LED_Disp_Seg7(void);
void ADC_init(unsigned char channel);
void T0_init(void);
void sendDataToProcessing(char symbol, int dat);
void delay(unsigned int n);
void UART_send(char dat);

unsigned char PulsePin = 0;       // Pulse Sensor purple wire connected to analog pin 0(P1.0为AD口)
sbit blinkPin = P2^0;                // pin to blink led at each beat
sbit fadePin = P2^3;                  // pin to do fancy classy fading blink at each beat
sbit led1 = P2^1;
sbit led2 = P2^2;
int fadeRate = 0;                 // used to fade LED on with PWM on fadePin


// these variables are volatile because they are used during the interrupt service routine!
volatile unsigned int BPM;                   // used to hold the pulse rate
volatile unsigned int Signal;                // holds the incoming raw data
volatile unsigned int IBI = 600;             // holds the time between beats, must be seeded! 
volatile bit Pulse = false;     // true when pulse wave is high, false when it's low
volatile bit QS = false;        // becomes true when Arduoino finds a beat.
volatile int rate[10];                    // array to hold last ten IBI values
volatile unsigned long sampleCounter = 0;          // used to determine pulse timing
volatile unsigned long lastBeatTime = 0;           // used to find IBI
volatile int Peak =512;                      // used to find peak in pulse wave, seeded
volatile int Trough = 512;                     // used to find trough in pulse wave, seeded
volatile int thresh = 512;                // used to find instant moment of heart beat, seeded
volatile int amp = 100;                   // used to hold amplitude of pulse waveform, seeded
volatile bit firstBeat = true;        // used to seed rate array so we startup with reasonable BPM
volatile bit secondBeat = false;      // used to seed rate array so we startup with reasonable BPM
static unsigned char order=0;

void sys_init()
{
  //pinMode(blinkPin,OUTPUT);         // pin that will blink to your heartbeat!
  //pinMode(fadePin,OUTPUT);          // pin that will fade to your heartbeat!
  UART_init();             // we agree to talk fast!
	ADC_init(PulsePin);
  T0_init();                 // sets up to read Pulse Sensor signal every 2mS  
}

void main(void)
{
  sys_init();
	while(1)
	{
		sendDataToProcessing('S', Signal);     // send Processing the raw Pulse Sensor data
		if (QS == true){                       // Quantified Self flag is true when arduino finds a heartbeat
					fadeRate = 255;                  // Set 'fadeRate' Variable to 255 to fade LED with pulse
					sendDataToProcessing('B',BPM);   // send heart rate with a 'B' prefix
					sendDataToProcessing('Q',IBI);   // send time between beats with a 'Q' prefix
					QS = false;                      // reset the Quantified Self flag for next time    
			 }
  
  //ledFadeToBeat();

  delay(138);                             //  take a break 19.6ms

}
}


//void ledFadeToBeat(){
//    fadeRate -= 15;                         //  set LED fade value
//    fadeRate = constrain(fadeRate,0,255);   //  keep LED fade value from going into negative numbers!
//    analogWrite(fadePin,fadeRate);          //  fade LED
//  }


void sendDataToProcessing(char symbol, int dat ){
    putchar(symbol);                // symbol prefix tells Processing what type of data is coming
		printf("%d\r\n",dat);						// the data to send culminating in a carriage return
  }

void UART_init(void)
{
	 PCON &= 0x7f;  //波特率不倍速
   SCON = 0x50;  //8位数据，可变波特率
   BRT = 0xFD;    //独立波特率产生器初值
   AUXR |= 0x04;  //时钟设置为1T模式
   AUXR |= 0x01;  //选择独立波特率产生器
   AUXR |= 0x10;  //启动波特率产生
}
 char putchar(unsigned char dat)
{
	TI=0;
	SBUF=dat;
	while(!TI);
	TI=0;
	
	return SBUF;
}
void delay(unsigned int n)
{
	unsigned int i,j;
	for(i=0;i<n;i++)
		for(j=0;j<100;j++);
}
void LED_Disp_Seg7(void)
{

  LedCtrl = LedCtrl | 0xf0;
	switch(order)
	{
		case 0:
			LedPort = LED_Disp[DisBuff[0]];
			LedCtrl = LedCtrl & 0x7f;
			break;
		case 1:
			LedPort = LED_Disp[DisBuff[1]];
			LedCtrl = LedCtrl & 0xbf;
			break;
		case 2:
			if(DisBuff[2]==0)
				LedCtrl = LedCtrl | 0xf0;
			else
			{
				LedPort = LED_Disp[DisBuff[2]];
				LedCtrl = LedCtrl & 0xdf;
			}
			break;
		default:
			LedCtrl = LedCtrl | 0xf0;
	}
   if(++order>2)
			order=0;
}




void T0_init(void){     
  // Initializes Timer0 to throw an interrupt every 2mS.
	TMOD |= 0x01;	//16bit TIMER
	TL0=T0MS;
	TH0=T0MS>>8;
	TR0=1;		//start Timer 0
	ET0=1;		//enable Timer Interrupt
  EA=1;             // MAKE SURE GLOBAL INTERRUPTS ARE ENABLED      
} 

void ADC_init(unsigned char channel)
{
	P1ASF=ADC_MASK<<channel;	//enable PlusePin as ADC INPUT
	ADC_RES=0;	//clear former ADC result
	ADC_RESL=0;	//clear former ADC result
	AUXR1 |= 0x04;	//adjust the format of ADC result
	ADC_CONTR=channel|ADC_POWER|ADC_SPEEDLL|ADC_START;	//power on ADC and start conversion
}

unsigned int analogRead(unsigned char channel)
{
	unsigned int result;

	ADC_CONTR &=!ADC_FLAG;	//clear ADC FLAG
	result=ADC_RES;
	result=result<<8;
	result+=ADC_RESL;
	ADC_CONTR|=channel|ADC_POWER|ADC_SPEEDLL|ADC_START;
	return result;
}
// Timer 0中断子程序，每2MS中断一次，读取AD值，计算心率值
void Timer0_rountine(void) interrupt 1
{                       
  int N;
	unsigned char i;
	// keep a running total of the last 10 IBI values
  unsigned int runningTotal = 0;                  // clear the runningTotal variable    

	EA=0;                                      // disable interrupts while we do this
	TL0=T0MS;
	TH0=T0MS>>8;				//reload 16 bit TIMER0
  Signal = analogRead(PulsePin);              // read the Pulse Sensor 
  sampleCounter += 2;                         // keep track of the time in mS with this variable
  N = sampleCounter - lastBeatTime;       // monitor the time since the last beat to avoid noise
	LED_Disp_Seg7();


    //  find the peak and trough of the pulse wave
  if(Signal < thresh && N > (IBI/5)*3){       // avoid dichrotic noise by waiting 3/5 of last IBI
    if (Signal < Trough){                        // T is the trough
      Trough = Signal;                         // keep track of lowest point in pulse wave 
    }
  }

  if(Signal > thresh && Signal > Peak){          // thresh condition helps avoid noise
    Peak = Signal;                             // P is the peak
  }                                        // keep track of highest point in pulse wave

  //  NOW IT'S TIME TO LOOK FOR THE HEART BEAT
  // signal surges up in value every time there is a pulse
  if (N > 250){                                   // avoid high frequency noise
    if ( (Signal > thresh) && (Pulse == false) && (N > (IBI/5)*3) ){        
      Pulse = true;                               // set the Pulse flag when we think there is a pulse
      blinkPin=0;               // turn on pin 13 LED
      IBI = sampleCounter - lastBeatTime;         // measure time between beats in mS
      lastBeatTime = sampleCounter;               // keep track of time for next pulse

      if(secondBeat){                        // if this is the second beat, if secondBeat == TRUE
        secondBeat = false;                  // clear secondBeat flag
        for(i=0; i<=9; i++){             // seed the running total to get a realisitic BPM at startup
          rate[i] = IBI;                      
        }
      }

      if(firstBeat){                         // if it's the first time we found a beat, if firstBeat == TRUE
        firstBeat = false;                   // clear firstBeat flag
        secondBeat = true;                   // set the second beat flag
        EA=1;                               // enable interrupts again
        return;                              // IBI value is unreliable so discard it
      }   



      for(i=0; i<=8; i++){                // shift data in the rate array
        rate[i] = rate[i+1];                  // and drop the oldest IBI value 
        runningTotal += rate[i];              // add up the 9 oldest IBI values
      }

      rate[9] = IBI;                          // add the latest IBI to the rate array
      runningTotal += rate[9];                // add the latest IBI to runningTotal
      runningTotal /= 10;                     // average the last 10 IBI values 
      BPM = 60000/runningTotal;               // how many beats can fit into a minute? that's BPM!
			if(BPM>200)BPM=200;			//限制BPM最高显示值
			if(BPM<30)BPM=30;				//限制BPM最低显示值
			DisBuff[0]   = BPM%10;//取个位数
			DisBuff[1]   = BPM%100/10; //取十位数
			DisBuff[2]   = BPM/100;	   //百位数
      QS = true;                              // set Quantified Self flag 
      // QS FLAG IS NOT CLEARED INSIDE THIS ISR
    }                       
  }

  if (Signal < thresh && Pulse == true){   // when the values are going down, the beat is over
    blinkPin=1;            // turn off pin 13 LED
    Pulse = false;                         // reset the Pulse flag so we can do it again
    amp = Peak - Trough;                           // get amplitude of the pulse wave
    thresh = amp/2 + Trough;                    // set thresh at 50% of the amplitude
    Peak = thresh;                            // reset these for next time
    Trough = thresh;
  }

  if (N > 2500){                           // if 2.5 seconds go by without a beat
    thresh = 512;                          // set thresh default
    Peak = 512;                               // set P default
    Trough = 512;                               // set T default
    lastBeatTime = sampleCounter;          // bring the lastBeatTime up to date        
    firstBeat = true;                      // set these to avoid noise
    secondBeat = false;                    // when we get the heartbeat back
  }

  EA=1;                                   // enable interrupts when youre done!
}// end isr
