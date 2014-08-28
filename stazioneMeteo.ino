
/*
*  Name:      Applicazione METEO 
*  Hardware:  Arduino MEGA 2560 
*             TFT01_3.2 Schermo TFT 320x240 with SD cardReader
*             TFT Shield v.1.2
*             NRF2401L+ on SPI connection (see below)
*  Data:      16/05/2014
*/

//Connect the nRF24L01 to Arduino like this. Use these same connections for Teensy 3.1.
// 
//                 Arduino      NRF2401L+
//                 3V3 or 5V----VCC   (3.3v) or (3.3V to 7V in only for Sparkfun WRL-00691) 
//             pin D8-----------CE    (chip enable in)
//          SS pin D53----------CSN   (chip select in) -------------- N.B.: Library default but in code below is set D49 pin!
//         SCK pin D52----------SCK   (SPI clock in)
//        MOSI pin D51----------SDI   (SPI Data in)
//        MISO pin D50----------SDO   (SPI data out)
//                              IRQ   (Interrupt output, not connected)
//                 GND----------GND   (ground in)
//
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <EEPROM.h>
#include <UTFT.h>
#include <UTouch.h>
#include <SPI.h>
#include <DS1307.h>
#include <RH_NRF24.h>
#include "DHT.h"
#include <StoricoDati.h>
#include <Time.h>
//#include <tinyFAT.h>
//#include <avr/pgmspace.h>


//Fonts
extern uint8_t BigFont[];
extern uint8_t SmallFont[];
extern uint8_t SevenSegNumFont[];

//Image Files
extern prog_uint16_t poconuv[0x1FA4];
extern prog_uint16_t sereno[0x1FA4];
extern prog_uint16_t pioggiadebole[0x1FA4];
extern prog_uint16_t settings[0x384];
extern unsigned int arrow[0x384];
//extern unsigned int serenonotte[0x1FA4];
//extern prog_uint16_t poconuvnotte[0x1FA4];
//extern prog_uint16_t neve[0x1FA4];

//VARIABILI
//set pin numbers:
const int buttonPin = 18;     // the number of the pushbutton pin /Interrupt 5
const int backlightPin = 9;   // number of backlight tft pin
const int wakePinWifi = 19;   // intrrupt from wifi - Interrupt 4    

//volatile boolean dataAviable = false;
int wakeCount =0;
int cycleNum;
#define STANBY_SEC 300

//Indirizzi EEPROM
//0,1 altitudine  

//richiesta di wakeUp 
volatile int wakeStatus = 0;  // variable to store a request for wakeUp
                              // 1 - wifi
                              // 2 - button
//parametri interfaccia
boolean buttonState = false;  //stato del pulsante true = premuto
boolean lcdActive = true;     //diventa false dopo un minuto di inattività
int inattivita = 0;           //tempo di inattività
int schermata = 0;            /*! 1 - Schermata principale
                               *  2 - Grafico pressione
                               *  3 - Situazione Attuale
                               */                               
//variabili per far lampeggiare la colonna della previsione +1
boolean flashBar = true;
int maxScala = 1000;
int mSecFlash = 0;

//dati meteo esterni
float secAggWifi = 0.0;       //secondi dall'ultimo aggiornamento dei dati meteo esterni
boolean firstConnection=true; //prima connessione con stazione esterna
boolean errorConnection=false; //errore nella connessione della stazione esterna
double altitudine;
float currPress=0.0;              //pressione corrente letta dai sensori esterni 
char* currStrHum="--";        //stringa umidità corrente  
float currHum = 0.0;          //umidita corrente letta dai sensori esterni 
char* currStrTemp="--.-";     //stringa temperatura corrente
float currTemp = 0.0;         //temperatura corrente letta dai sensori esterni 

StoricoDati storico; //Storico dati

//dati meteo interni
float currInHum=0.0;          //umidita corrente letta dal sensore interno 
float currInTemp=0.0;         //temperatura corrente letta dal sensoro interno 

//Variabili SD
//int sdAviable = 0;            //sd disponibile
//byte res;                     //byte di ritorno lettura e crittura su file

// Variabili orologio
boolean puntini = true;
int oldMin=0;                 //ultimo minuto stampato 
Time  t;                      // Init a Time-data structure
Time oldDataSaved;

//COSTRUTTORI
UTFT myGLCD(TFT01_32,38,39,40,41); 	//Display TFT
UTouch  myTouch(6,5,4,3,2);        	//TouchScreen
RH_NRF24 nrf24(8,49);              	//Singleton instance of the radio driver
DHT dht;						   	//Sensore Temperatura
DS1307 rtc(20, 21);					//Orologio

void setup()
{
  randomSeed(analogRead(0));
  
  pinMode(backlightPin,OUTPUT);    // inizialize the backlight pin as output
  backlightOff();                  // set initial state of backlight
  
  pinMode(buttonPin, INPUT);       // initialize the pushbutton pin as an input 
  pinMode(wakePinWifi, INPUT_PULLUP);
  
  attachInterrupt(5, buttonPressed, RISING); // use interrupt 5 (pin 18) and run function
                                      // buttonPressed when pin 5 goes from low to high 
  //attachInterrupt(4, wifiInterrupt, RISING); // use interrupt 4 (pin 19) and run function
                                      // wakeUpNow when pin 4 goes from low to high 

  cycleNum = (int)(STANBY_SEC/8-STANBY_SEC/95);
  wakeCount=(cycleNum-1);
  //Setup the LCD
  myGLCD.InitLCD();
  myGLCD.setFont(BigFont);
  myGLCD.clrScr();
  backlightOn();    //Accendo retroilluminazione
  myGLCD.print("LOADING...", CENTER, 110);
  
  altitudine=readAltitude();
  //altitudine=30;
  
  //inizializza comunicazione seriale
  Serial.begin(115200);
  Serial.println("REBOOT");
  
  //Setup TouchScreen
  myTouch.InitTouch();
  myTouch.setPrecision(PREC_HI);
  
  // Set the clock to run-mode
  rtc.halt(false);
  
  //setup DHT
  dht.setup(45);
  
  //Setup SD
  //pinMode(53,OUTPUT);
  //file.setSSpin(53);
  //file.initFAT();
  
  //Setup NRF24 Wifi board
  if (!nrf24.init())
    Serial.println("init failed");
  // Defaults after init are 2.402 GHz (channel 2), 2Mbps, 0dBm
  if (!nrf24.setChannel(10))
    Serial.println("setChannel failed");
  if (!nrf24.setRF(RH_NRF24::DataRate250kbps, RH_NRF24::TransmitPower0dBm))
    Serial.println("setRF failed");  
   
  t = rtc.getTime();
  oldDataSaved=t;
  if(t.hour==0)
    oldDataSaved.hour=23;
  else
    oldDataSaved.hour=t.hour-1;
  aggiornaDati();
  printMain();      //paint schermata iniziale
}

void loop()
{
    while(true) 
    {
      
      if(wakeStatus==2) {
        wakeStatus=0;
        buttonPressed();
      }
      if(lcdActive) {
        if(nrf24.available()) {
          //dataAviable=false;
          aggiornaDati();
        }
        checkDataOra();
        touchInterface();         //eseguo interfaccia touch
        flashPrevision();
        if(inattivita==5000&schermata!=1)    //dopo 3000 di inattivita se sono su una schermata diversa dalla principale
          printMain();
        else if(inattivita==8000)              //dopo 5000 di inattivita
        {
          backlightOff();                    //spengo retroilluminazione 
          myGLCD.lcdOff();                   //metto in stand-By lo schermo
          lcdActive=false;
          sleepNow();
        }
        delay(10);
      }
      else {
        if(wakeCount==(cycleNum-1)) {
          if(nrf24.available()) {
            Serial.println("Dati disp");
            wakeCount=0;
            aggiornaDati();
            sleepNow();
          }
        }
        else {
          Serial.print("No Dati disp: ");
          Serial.println(wakeCount);
          delay(100);
          wakeCount++;
          sleepNow();
        }
      }
    }
}

void wakeUpNowWifi()        // here the interrupt is handled after wakeup
{
  wakeStatus = 1;
  // execute code here after wake-up before returning to the loop() function
  // timers and code using timers (serial.print and more...) will not work here.
  // we don't really need to execute any special functions here, since we
  // just want the thing to wake up
}
void wakeUpNowButton()        // here the interrupt is handled after wakeup
{
  wakeStatus = 2;
  //buttonState = digitalRead(buttonPin);    // read the state of the pushbutton value
    //if (wakeStatus == 2)
    //{
  
    //}
  // execute code here after wake-up before returning to the loop() function
  // timers and code using timers (serial.print and more...) will not work here.
  // we don't really need to execute any special functions here, since we
  // just want the thing to wake up
}

void sleepNow()         // here we put the arduino to sleep
{
    /* Now is the time to set the sleep mode. In the Atmega8 datasheet
     * http://www.atmel.com/dyn/resources/prod_documents/doc2486.pdf on page 35
     * there is a list of sleep modes which explains which clocks and 
     * wake up sources are available in which sleep mode.
     *
     * In the avr/sleep.h file, the call names of these sleep modes are to be found:
     *
     * The 5 different modes are:
     *     SLEEP_MODE_IDLE         -the least power savings 
     *     SLEEP_MODE_ADC
     *     SLEEP_MODE_PWR_SAVE
     *     SLEEP_MODE_STANDBY
     *     SLEEP_MODE_PWR_DOWN     -the most power savings
     *
     * For now, we want as much power savings as possible, so we 
     * choose the according 
     * sleep mode: SLEEP_MODE_PWR_DOWN
     * 
     */  
    set_sleep_mode(SLEEP_MODE_PWR_SAVE);   // sleep mode is set here

    sleep_enable();          // enables the sleep bit in the mcucr register
                             // so sleep is possible. just a safety pin 

    /* Now it is time to enable an interrupt. We do it here so an 
     * accidentally pushed interrupt button doesn't interrupt 
     * our running program. if you want to be able to run 
     * interrupt code besides the sleep function, place it in 
     * setup() for example.
     * 
     * In the function call attachInterrupt(A, B, C)
     * A   can be either 0 or 1 for interrupts on pin 2 or 3.   
     * 
     * B   Name of a function you want to execute at interrupt for A.
     *
     * C   Trigger mode of the interrupt pin. can be:
     *             LOW        a low level triggers
     *             CHANGE     a change in level triggers
     *             RISING     a rising edge of a level triggers
     *             FALLING    a falling edge of a level triggers
     *
     * In all but the IDLE sleep modes only LOW can be used.
     */
    
    attachInterrupt(5, wakeUpNowButton, RISING); // use interrupt 5 (pin 18) and run function
                                      // wakeUpNow when pin 5 goes from low to high 
    //attachInterrupt(4, wakeUpNowWifi, RISING); // use interrupt 4 (pin 19) and run function
                                      // wakeUpNow when pin 4 goes from low to high 
    wdt_reset();
    myWatchdogEnable();
    Serial.println("...going to sleep.");
    delay(100);
    sleep_mode();            // here the device is actually put to sleep!!
                             // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP
    sleep_disable();         // first thing after waking from sleep:
                             // disable sleep...
    detachInterrupt(5);      // disables interrupt 5 on pin 18 so the 
                             // wakeUpNow code will not be executed 
                             // during normal running time.
    //detachInterrupt(4);      // disables interrupt 4 on pin 19 so the 
                             // wakeUpNow code will not be executed 
                             // during normal running time.
    attachInterrupt(5, buttonPressed, RISING); // use interrupt 5 (pin 18) and run function
                                      // buttonPressed when pin 5 goes from low to high 
    //attachInterrupt(4, wifiInterrupt, RISING); // use interrupt 4 (pin 19) and run function
                                      // wakeUpNow when pin 4 goes from low to high 
}

void wifiInterrupt() {
  //dataAviable=true;
}

void buttonPressed() {
  if(lcdActive){
    if(schermata!=1) 
      { inattivita=0;
        printMain();
      }
  }
  else
  {
        inattivita=0;
        myGLCD.lcdOn();
        lcdActive=true;
        backlightOn();
        printMain();  
  }
}

ISR(WDT_vect)
{
  cli();
  wdt_disable();
  Serial.println("wakeup!");
  sei();
}

void myWatchdogEnable() {  // turn on watchdog timer; interrupt mode every 2.0s
  cli();
  MCUSR = 0;
  WDTCSR |= B00011000;
  // WDTCSR = B01000111; // 2 second WDT 
  //WDTCSR = B01100000;    // 4 second WDT 
  WDTCSR = B01100001;  // 8 second WDT 
  sei();
}
