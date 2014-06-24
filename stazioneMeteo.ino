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
#include <avr/sleep.h>
#include <UTFT.h>
#include <UTouch.h>
#include <SPI.h>
#include <DS1307.h>
#include <RH_NRF24.h>
#include "DHT.h"


//Fonts
extern uint8_t BigFont[];
extern uint8_t SmallFont[];
extern uint8_t SevenSegNumFont[];

//Image Files
extern prog_uint16_t poconuv[0x1FA4];
extern prog_uint16_t sereno[0x1FA4];
extern prog_uint16_t pioggiadebole[0x1FA4];
extern prog_uint16_t settings[0x384];
extern prog_uint16_t arrow[0x384];

//variabili
// set pin numbers:
const int buttonPin = 18;     // the number of the pushbutton pin /Interrupt 5
const int backlightPin = 9;   // number of backlight tft pin

int wakePinWifi = 19;          //Interrupt 4      
volatile int wakeStatus = 0;             // variable to store a request for wakeUp
                                          // 1 - wifi
                                          // 2 - button

//parametri interfaccia
boolean buttonState = false;  //stato del pulsante true = premuto
boolean lcdActive = true;     //diventa false dopo un minuto di inattività
int inattivita = 0;           //tempo di inattività
int schermata = 1;            /*! 1 - Schermata principale
                               *  2 - Grafico pressione
                               *  3 - Situazione Attuale
                               */
//dati meteo esterni
float secAggWifi = 0.0;       //secondi dall'ultimo aggiornamento dei dati meteo esterni
boolean firstConnection=true; //prima connessione con stazione esterna
double altitudine=30.0;       // altitudine manuale
int currPress=0;              //pressione corrente letta dai sensori esterni 
char* currStrHum="--";        //stringa umidità corrente  
float currHum = 0.0;          //umidita corrente letta dai sensori esterni 
char* currStrTemp="--.-";     //stringa temperatura corrente
float currTemp = 0.0;         //temperatura corrente letta dai sensori esterni 

//dati meteo interni
float currInHum=0.0;          //umidita corrente letta dal sensore interno 
float currInTemp=0.0;         //temperatura corrente letta dal sensoro interno 

boolean puntini = true;

//simulo orologio
int oldMin=0;
int milliSec=0;
int secondi=0;
int ora=14;
int minuti=15;

//costruttori
UTFT myGLCD(TFT01_32,38,39,40,41); //Display TFT
UTouch  myTouch(6,5,4,3,2);        //TouchScreen
RH_NRF24 nrf24(8,49);              // Singleton instance of the radio driver
DHT dht;

// Init the DS1307
DS1307 rtc(20, 21);

// Init a Time-data structure
Time  t;

void setup()
{
  randomSeed(analogRead(0));
  
  pinMode(backlightPin,OUTPUT);    // inizialize the backlight pin as output
  backlightOff();                  // set initial state of backlight
  
  pinMode(buttonPin, INPUT);       // initialize the pushbutton pin as an input 
  pinMode(wakePinWifi, INPUT_PULLUP);
  
  attachInterrupt(5, buttonPressed, RISING); // use interrupt 5 (pin 18) and run function
                                      // buttonPressed when pin 5 goes from low to high 
  attachInterrupt(4, wakeUpNowWifi, LOW); // use interrupt 4 (pin 19) and run function
                                      // wakeUpNow when pin 4 goes from low to high 

  
  
  //Setup the LCD
  myGLCD.InitLCD();
  //myGLCD.setFont(BigFont);
  myGLCD.clrScr();
  //myGLCD.print("METEO APP LOADING...", CENTER, 110);
  
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
  recuperaDatiInterni();
  
  printMain();      //paint schermata iniziale
  backlightOn();    //Accendo retroilluminazione
}

void loop()
{
  while(true) 
  {
    if(firstConnection) 
    {
      if(secAggWifi<305)
      {
        secAggWifi+=0.001;
        if (nrf24.available()) 
        {
          secAggWifi=0.0;
          recuperaDati();      //recupero dati
          recuperaDatiInterni(); 
          saveToSd();
          if(schermata==1)
              printMeteoAttuale();
        }
      }
      else 
      {
        myGLCD.setColor(255, 165, 0); //Arancione
        myGLCD.setBackColor(64, 64, 64); //Grigiet sotto
        myGLCD.print("Errore Connesione!!", CENTER, 110);
      }
    }
    else 
    {
      if(secAggWifi<305)
      {
        secAggWifi+=0.001;
        if(secAggWifi>295)
        {
          if (nrf24.available()) 
          {
            secAggWifi=0.0;
            recuperaDati();      //recupero dati
            recuperaDatiInterni(); 
            saveToSd();
            if(lcdActive & schermata==1)
              printMeteoAttuale();
          }
        }
      }
      else 
      {
        myGLCD.setColor(255, 165, 0); //Arancione
        myGLCD.setBackColor(64, 64, 64); //Grigiet sotto
        myGLCD.print("Errore Connesione!!", CENTER, 110);
      }
    }
    
    //buttonState = digitalRead(buttonPin);    // read the state of the pushbutton value
    if (wakeStatus == 2)
    {
      buttonPressed();
    }
    if(lcdActive) {
      checkDataOra();
      touchInterface();                    //eseguo interfaccia touch
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
    else 
      delay(100);
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
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here

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
    attachInterrupt(4, wakeUpNowWifi, LOW); // use interrupt 4 (pin 19) and run function
                                      // wakeUpNow when pin 4 goes from low to high 

    sleep_mode();            // here the device is actually put to sleep!!
                             // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP
    sleep_disable();         // first thing after waking from sleep:
                             // disable sleep...
    detachInterrupt(5);      // disables interrupt 5 on pin 18 so the 
                             // wakeUpNow code will not be executed 
                             // during normal running time.
    detachInterrupt(4);      // disables interrupt 4 on pin 19 so the 
                             // wakeUpNow code will not be executed 
                             // during normal running time.
    attachInterrupt(5, buttonPressed, RISING); // use interrupt 5 (pin 18) and run function
                                      // buttonPressed when pin 5 goes from low to high 
}

void buttonPressed() {
  wakeStatus=0;
  if(lcdActive)   
      { inattivita=0;
        printMain();
      }
  else
      {
        inattivita=0;
        myGLCD.lcdOn();
        lcdActive=true;
        printMain();
        backlightOn();
      }
  delay(100);
}
