/*
*  Name:      Applicazione METEO 
 *  Hardware:  Arduino MEGA 2560 
 *             TFT01_3.2 Schermo TFT 320x240 with SD cardReader
 *             TFT Shield v.1.2
 *             NRF2401L+ on SPI connection (see below)
 *             ENC28J60 on SPI connection (see below)
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
//
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <EEPROM.h>
#include <UTFT.h>
#include <UTouch.h>
#include <SPI.h>
#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>
#include <RF24Network.h>
#include <RF24.h>
#include "DHT.h"
#include <StoricoDati.h>
#include <Timezone.h>
#include <SunLight.h>
#include <tinyFAT.h>
#include <UTFT_tinyFAT.h>
#include <SunLight.h>

//VARIABILI
//indirizzi rete
#define MY_ADDRESS 01
#define MASTER_ADDRESS 00

//set pin numbers:
#define BUTTON_PIN 18     // the number of the pushbutton pin /Interrupt 5
#define BACKLIGHT_PIN 9   // number of backlight tft pin
#define WAKE_WIFI_PIN 19   // intrrupt from wifi - Interrupt 4    

//tempo di ritorno alla home e poi di spegniento dello schermo 
#define STANBY_SEC 10

// posizione per il calcolo delle ore solari
// Setting the longitude of your location
// Will be used to estimate the average Noon
#define MY_LONGITUDE 11.80
// Setting the latitude of your location
#define MY_LATITUDE 45.39
// If you want to anticipate the sunrise and postpone the sunset.
// Permits data beetween 0 and 60 and the unit of measurement is minutes
#define TWILIGHT_MINUTES 10

uint8_t storicoToSd = 0;          // 1 se l'arduino deve salvare lo storico su sd, 0 altrimenti
//FINE VARIABILI  


//Fonts
extern uint8_t BigFont[];
extern uint8_t SmallFont[];
extern uint8_t DotMatrix_M_Slash[];
extern uint8_t franklingothic_normal[];
//Image Files
extern prog_uint16_t settings[0x384];
extern prog_uint16_t arrow[0x384];
static char* images[]={
  "MET0.RAW", "MET1.RAW", "MET2.RAW", "MET3.RAW", "MET4.RAW", "MET5.RAW" };
/*
 MET0.RAW --- sereno         giorno
 MET1.RAW --- poco nuvoloso  giorno
 MET2.RAW --- pioggia debole giorno
 MET3.RAW --- sereno         notte
 MET4.RAW --- poco nuvoloso  notte
 MET5.RAW --- pioggia debole notte
 */

// TimeZone : United Kingdom (London, Belfast)
TimeChangeRule BST = {
  "BST", Last, Sun, Mar, 3, 120};        //British Summer Time
TimeChangeRule GMT = {
  "GMT", Last, Sun, Oct, 3, 60};         //Standard Time
Timezone UK(BST, GMT);

//int wakeCount =0;
//int cycleNum;
//#define STANBY_SEC 150

//Indirizzi EEPROM
//0,1 altitudine  

//richiesta di wakeUp 
volatile uint8_t wakeStatus = 0;  // variable to store a request for wakeUp
// 1 - wifi
// 2 - button
//parametri interfaccia
static boolean buttonState = false;      //stato del pulsante true = premuto
static boolean lcdActive = true;         //diventa false dopo un minuto di inattività
//uint8_t inattivita = 0;           //tempo di inattività
static unsigned long prevMSecInat = 0;   
static uint8_t schermata = 0;            /*! 1 - Situazione Attuale
 *  2 - Grafico pressione
 *  3 - Schermata esterno
 *  4 - Impostazioni
 *  5 - Setting ora
 */

//variabili per far lampeggiare la colonna della previsione +1
static boolean flashBar = true;
static int maxScala = 1000;
static int mSecFlash = 0;

//dati meteo esterni
static boolean firstConnection=true; //prima connessione con stazione esterna
static boolean errorConnection=false; //errore nella connessione della stazione esterna
static double altitudine;
static float currPress= NAN;          //pressione corrente letta dai sensori esterni  
static float currHum = NAN;          //umidita corrente letta dai sensori esterni 
static float currTemp = NAN;         //temperatura corrente letta dai sensori esterni 

static StoricoDati storico; //Storico dati

//dati meteo interni
static float currInHum=NAN;          //umidita corrente letta dal sensore interno 
static float currInTemp=NAN;         //temperatura corrente letta dal sensoro interno 

//Variabili SD
static uint8_t sdAviable = 0;            //sd disponibile
static uint8_t errorNetConnection = 0;      //errore connessione mosulo rete

// Variabili orologio
static int oldMin=0;                 //ultimo minuto stampato 
static time_t oldDataSaved;

//COSTRUTTORI
UTFT myGLCD(TFT01_32,38,39,40,41); 	//Display TFT
UTouch  myTouch(6,5,4,3,2);        	//TouchScreen

RF24 radio(8,49);             	        //Singleton instance of the radio driver
// Network uses that radio
RF24Network network(radio);

DHT dht;                                //Sensore Temperatura
//DS1307 rtc(20, 21);			//Orologio

UTFT_tinyFAT myFiles(&myGLCD);

SunLight mySun; // Declaration of the object

// The array where will be saved variables of sunrise and sunset
// with the following form:
// timeArray[ Rise_hours, Rise_minutes, Set_hours, Set_minutes ]
// if you want you can use specially created index:
// SUNRISE_H SUNRISE_M SUNSET_H SUNSET_M
static uint8_t timeArray[4];


void setup()
{
  randomSeed(analogRead(0));
  SPI.begin();
  
  // Set the clock to run-mode
  //rtc.halt(false);
  setSyncProvider(RTC.get); 
  
  pinMode(53,OUTPUT);        //Pin sd ss
  file.setSSpin(53);
  
  pinMode(BACKLIGHT_PIN,OUTPUT);    // inizialize the backlight pin as output
  backlightOff();                  // set initial state of backlight

  pinMode(BUTTON_PIN, INPUT);       // initialize the pushbutton pin as an input 
  pinMode(WAKE_WIFI_PIN, INPUT_PULLUP);

  //cycleNum = (int)(STANBY_SEC/8-STANBY_SEC/95);
  //wakeCount=0;

  //Setup the LCD
  myGLCD.InitLCD();
  myGLCD.setFont(franklingothic_normal);
  myGLCD.clrScr();
  backlightOn();    //Accendo retroilluminazione
  myGLCD.print("LOADING...", CENTER, 110);

  altitudine=readAltitude();
  //altitudine=30;

  //inizializza comunicazione seriale
  Serial.begin(115200);
  Serial.println("***** REBOOT... *****");
  Serial.println("");

  //Setup SD
  byte res = file.initFAT(); //SPISPEED_VERYHIGH
  if (res==NO_ERROR) {
    sdAviable=1;
    Serial.println("SD correctly inizialized");
  }
  else {
    Serial.print("***** ERROR: ");
    Serial.println(verboseError(res));
  }

  //Setup TouchScreen
  myTouch.InitTouch();
  myTouch.setPrecision(PREC_HI);

  //setup DHT
  dht.setup(45);

  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_HIGH);
  network.begin(/*channel*/ 125, /*node address*/ MY_ADDRESS);


  oldDataSaved = now();
  oldDataSaved-=3600;

  //retriveFromSd();
  //printListFile();
  //Setup Ethernet
  //ethAviable = inizializeEth();
  //regolo ora da internet
  //regolaOraFromNtp(); 

  attachInterrupt(5, wakeUpNowButton, RISING); // use interrupt 5 (pin 18) and run function
  // buttonPressed when pin 5 goes from low to high 
  //attachInterrupt(4, wifiInterrupt, RISING); // use interrupt 4 (pin 19) and run function
  // wakeUpNow when pin 4 goes from low to high
  
  //paint schermata iniziale
  recuperaDatiInterni();
  printMain();
  Serial.print("Setup eseguito in :");
  Serial.print(millis()/1000);
  Serial.println("s");
  Serial.println();
}

void loop()
{
  while(true) 
  {
    // Pump the network regularly
    network.update();
    if(wakeStatus==2) {
      wakeStatus=0;
      buttonPressed();
    }
    if(lcdActive) {
      if(network.available()) {
        aggiornaDati();
      }
      checkDataOra();
      touchInterface();         //eseguo interfaccia touch
      flashPrevision();
      if(millis() > prevMSecInat) {
        if ( ((millis()/1000 - prevMSecInat) >= STANBY_SEC) & (schermata!=1)) {
          printMain();
        }
        else if ((millis()/1000 - prevMSecInat) >= STANBY_SEC*2) {
          backlightOff();                    //spengo retroilluminazione 
          myGLCD.lcdOff();                   //metto in stand-By lo schermo
          lcdActive=false;
          //sleepNow();
        }
      }
//      if((inattivita==STANBY_SEC) & (schermata!=1) )    //dopo 3000 di inattivita se sono su una schermata diversa dalla principale
//        
//      else if(inattivita==(STANBY_SEC*2))              //dopo 5000 di inattivita
//      {
//        
//      }
      //delay(10);
    }
    else {
        if(network.available()) {
          //Serial.println("Dati disp");
          //wakeCount=0;
          aggiornaDati();
          //delay(100);
        }
        //sleepNow();
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
  wdt_reset();
  myWatchdogEnable();
  //Serial.println("...going to sleep.");
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
  attachInterrupt(5, wakeUpNowButton, RISING); // use interrupt 5 (pin 18) and run function
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
    { 
      prevMSecInat = millis()/1000;
      printMain();
    }
  }
  else
  {
    prevMSecInat = millis()/1000;
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
  //Serial.println("wakeup!");
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


