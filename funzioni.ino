struct payload_Meteo
{
  time_t time;
  float inTemp;
  float inHum;
  float outTemp;
  float outHum;
  float outPress;
};


struct payload_Sensor_1
{
  float temp;
  float hum;
  float pres;
  float tempOfBmp;
  int battery_level; 
};

//
//void regolaOraFromNtp() {
//  unsigned long timeFromNtp = getNtpTime();
//  if(timeFromNtp != 0) {
//    setTime(timeFromNtp);
//    rtc.setTime(hour(),minute(),second());
//    rtc.setTime(day(),month(),year());
//  }
//}
//unsigned long getNtpTime() {  
// if(Ethernet::isLinkUp()) { 
//  unsigned long timeFromNTP;   
//  const unsigned long seventy_years = 2208988800UL;      
// 
//  ether.ntpRequest(ntpServer, ntpMyPort);   
//  while(true) {       
//    word length = ether.packetReceive();       
//    ether.packetLoop(length);       
//    if(length > 0 && ether.ntpProcessAnswer(&timeFromNTP, ntpMyPort)) {
//      Serial.print("Time from NTP: ");
//      Serial.println(timeFromNTP);
//      return UK.toLocal(timeFromNTP - seventy_years);
//    }
//  }
//  return 0;
// }
//}
//

void invioDati() {
  detachInterrupt(5);
  errorNetConnection = 0;  
  Serial.print("Sending...");
  //t=rtc.getTime();
  payload_Meteo payload = { now(), currInTemp, currInHum, currTemp, currHum, currPress};
  Serial.print(payload.outPress,2);
  RF24NetworkHeader header(/*to node*/ MASTER_ADDRESS);
  errorNetConnection = !network.write(header,&payload,sizeof(payload));
  attachInterrupt(5, buttonPressed, RISING);
  if(errorNetConnection)
  Serial.println("Errore Network");
}


/* descrizione: funzione per aggiornamento dei valori interni letti dai sensori 
 *              ed esterni se presenti nel buffer wifi.
 */
void aggiornaDati() {
  
  Serial.println("func. aggiorno dati");
  while (network.available()) 
  {
    recuperaDati();
  }
  touchInterface();
  recuperaDatiInterni();
  if(lcdActive) {
    if(schermata==3) 
      printMeteoAttuale();
    else if(schermata==1);
      printMain();
  }
  invioDati();
}

/* descrizione: funzione per il calcolo delle previsioni;
 * param[in] ora: valore 1 o 2.
 */ 
float calcolaPrevisione(uint8_t ora) {
  if(ora==1) {
    if(storico.getPress(-2)==0)
      return 0;
    float scartoUno = currPress-storico.getPress(-1);
    float scartoDue = storico.getPress(-1)-storico.getPress(-2);
    float prev = (scartoUno+scartoDue)/2;
    return currPress+prev; 
  }
  else if (ora==2) {
    if(storico.getPress(-2)==0)
      return 0;
    float prev1 = calcolaPrevisione(1);
    float scartoZero = prev1-currPress;
    float scartoUno = currPress-storico.getPress(-1);
    float scartoDue = storico.getPress(-1)-storico.getPress(-2);
    float prev = (scartoZero+scartoUno+scartoDue)/3;
    return prev1+prev;
  }
}

/* descrizione: funzione per il salvataggio dei valori attuali letti dai sensori esterni
 *              in un file testuale con nome GGMMAAAA.TXT su sd.
 */
//void saveToSd() {
//  Serial.println("save to SD");
//  if(sdAviable && storicoToSd) {
//    String nomeFile =  rtc.getDateStr();
//    nomeFile.replace(".","");
//    nomeFile.concat(".TXT");
//    char nome [13];
//    nomeFile.toCharArray(nome,13);
//    Serial.println(nome);
//    Serial.println(nomeFile);
//    if (!file.exists(nome))
//      file.create(nome);
//    byte res=file.openFile(nome, FILEMODE_TEXT_WRITE);
//     if (res==NO_ERROR)
//     {
//       String stringa = rtc.getTimeStr();
//       stringa.concat("t:");
//       stringa.concat(currStrTemp);
//       stringa.concat("h:");
//       stringa.concat(currStrHum);
//       stringa.concat("p:");
//       int intero = currPress;
//       int decimale = ((int)(currPress*10))-intero;
//       stringa.concat(intero);
//       stringa.concat(".");
//       stringa.concat(decimale);
//       char * buf = new char[27];
//       Serial.println(stringa);
//       stringa.toCharArray(buf,27);
//       Serial.println(buf);
//       file.writeLn(buf);
//       file.closeFile();
//       delete buf;
//     }
//     else
//     {
//       switch(res)
//       {
//         case ERROR_ANOTHER_FILE_OPEN:
//           Serial.println("** ERROR: Another file is already open...");
//           break;
//         default:
//           Serial.print("** ERROR: ");
//           Serial.println(res,HEX);
//           break;
//       }
//     }
//  }
//}

/* descrizione: recupera valori salvati su sd.
 */
//void retriveFromSd() {
//  Serial.println("retrive from SD");
//  if(sdAviable && storicoToSd) {
//    byte res;
//    word result;
//    char textBuffer[27];
//    String nomeFile =  rtc.getDateStr();
//    nomeFile.replace(".","");
//    nomeFile.concat(".TXT");
//    char nome [nomeFile.length()+1];
//    nomeFile.toCharArray(nome,nomeFile.length()+1);
//    Serial.println(nome);
//    if (file.exists(nome))
//    {  
//      res=file.openFile(nome, FILEMODE_TEXT_READ);
//      if (res==NO_ERROR)
//      {
//        int maxHour = -1;
//        result=0;
//        while ((result!=EOF) and (result!=FILE_IS_EMPTY))
//        {
//          result=file.readLn(textBuffer, 27);
//          if (result!=FILE_IS_EMPTY)
//          {
//            if (result==BUFFER_OVERFLOW) {
//              Serial.print(textBuffer);
//            }
//            else{
//              Serial.println(textBuffer);
//              
//              char cOra[] = {textBuffer[0], textBuffer[1]};
//              Serial.print(textBuffer[0]);
//              Serial.println(textBuffer[1]);
//              Serial.println(cOra);
//              int ora = atoi(cOra);
//              Serial.print("ora: ");
//              Serial.println(ora);
//              if(maxHour<ora){
//                maxHour=ora;
//       
//                char cTemp[] = {textBuffer[10], textBuffer[11], textBuffer[12], textBuffer[13]};
//                float tTemp = atof(cTemp);
//                
//              
//                char cHum[] = {textBuffer[16], textBuffer[17]};
//                int tHum = atoi(cHum);
//           
//                char cPress[] = {textBuffer[20], textBuffer[21], textBuffer[22], textBuffer[23], textBuffer[24], textBuffer[25]};
//                float tPress = atof(cPress);
//                storico.saveCurrent(tTemp, tHum, tPress);
//              }
//              
//            }
//          }
//          else
//            Serial.println("** ERROR: File is empty...");
//        }
//        Serial.println();
//        file.closeFile();
//        int diffHour = t.hour - maxHour;
//        if(diffHour)
//          for(int i=0;i<diffHour;i++)
//            storico.saveCurrent(0, 0, 0);
//        else {
//            currPress=storico.getPress();
//            currTemp=storico.getTemp();
//            currHum=storico.getHum();
//        }
//      }
//      else
//      {
//        switch(res)
//        {
//          case ERROR_ANOTHER_FILE_OPEN:
//            Serial.println("** ERROR: Another file is already open...");
//            break;
//          default:
//            Serial.print("** ERROR: ");
//            Serial.println(res,HEX);
//            break;
//        }
//      }
//    }
//    else
//      Serial.println("** ERROR: file does not exist...");
//  }
//}

/* descrizione: salva i dati attuali nello storico e su sd.
 */
void storeData () {
  Serial.println("store data");
  storico.saveCurrent(currTemp, currHum, currPress);
  //saveToSd();
}

/* descrizione: funzione che recupera i valori dal sensore interno.
 */
void recuperaDatiInterni() {
  Serial.println("aggiorno dati interni");
  delay(dht.getMinimumSamplingPeriod());
  currInHum = dht.getHumidity();
  currInTemp = dht.getTemperature();
}

/* descrizione: funzione aggiornarna la data o l'ora solo se necessario
 */
void checkDataOra() {
   printDataOra(false); 
}

/* descrizione: funzione che recupera i dati letti dal sensore esterno dal buffer wifi.
 */
void recuperaDati() {
  Serial.println("aggiorno dati esterni");
  if (network.available()) {
    detachInterrupt(5);
    RF24NetworkHeader header;
    payload_Sensor_1 payload;
    network.read(header,&payload,sizeof(payload));
    currPress=sealevel(payload.pres,altitudine);
    currTemp=payload.temp;
    currHum= payload.hum;
    attachInterrupt(5, buttonPressed, RISING);
    //rendo reattiva la grafica
    touchInterface();
    //if(!lcdActive);
      //t = rtc.getTime();
    //printSerialTime(t);
    //printSerialTime(oldDataSaved);
    if(oldDataSaved <= now()-3600){
      storeData();
      oldDataSaved=now();
    }
  }
}


/* descrizione: funzione che, data una pressione P (mb) misurata a una speifica 
 *              altitudine A (meters), ritorna la pressione al livello del mare;
 * param[in] double P: pressione espressa in millibar;
 * param[in] double A: altitudine espressa in metri.
 */ 
double sealevel(double P, double A) {
  return(P/pow(1-(A/44330.0),5.255));
}

/* descrizione: funzione che gestisce il touchscreen dell'iterfaccia in base alla 
 *              schermata visualizzata e gestisce il valore "inattivita".
 */
void touchInterface()
{
  if (myTouch.dataAvailable())
  {
    int x,y;           //coordinate touchscreen
    inattivita=0;
    myTouch.read();
    x=myTouch.getX();
    y=myTouch.getY();
    // azioni diverse in base alla schermata
    switch(schermata) 
    {
      case 1: //caso uno schermata principale 
      if((y>=37) && (y<=177)) {  //out secondo rettangolo
        if((x>=106) && (x<=206) ) {
           myGLCD.setColor(255, 165, 0);
           myGLCD.drawRect(106, 37, 206, 177);
           printSituazioneEsterna();
        }
        else if((x>=5) && (x<=105) ) { //in primo rettangolo
           myGLCD.setColor(255, 165, 0);
           myGLCD.drawRect(5, 37, 106, 177);
           
        }
      }
      else if (((y>=184) && (y<=214)) && ((x>=15) && (x<=45))) //clicco su settings
      {
        myGLCD.setColor(255, 165, 0);
        myGLCD.drawRect(15, 184, 45, 214);
        setPressione();
      }
      else if(y>=220) {
         myGLCD.setColor(255, 165, 0);
         myGLCD.drawRect(0,221, 319, 239);
         setClock();
      }
    break;
    case 2:  // caso 2 schermata grafico pressione
      if (((y>=45) && (y<=75)) && ((x>=280) && (x<=310)))        //clicco su indietro
      {
        myGLCD.setColor(255, 165, 0);
        myGLCD.drawRect(280, 45, 310, 75);
        printSituazioneEsterna();
      }
//      else if (((y>=132) && (y<=162)) && ((x>=280) && (x<=310))) //clicco su settings
//      {
//        myGLCD.setColor(255, 165, 0);
//        myGLCD.drawRect(280, 132, 310, 162);
//        setPressione();
//      }
      else if(y>=220) {
         myGLCD.setColor(255, 165, 0);
         myGLCD.drawRect(0,221, 319, 239);
         setClock();
      }
      break;
    case 3: // schermata situazione esterna
      if (((y>=37) && (y<=219)) && ((x>=15) && (x<=303)))        //clicco sul primo rettangolo
      {
        myGLCD.setColor(255, 165, 0);
        myGLCD.drawRect(15, 37, 303,219);
        grafico();
      }
//      else if (((y>=37) && (y<=219)) && ((x>=112) && (x<=207)))  //clicco sul secondo rettangolo 
//      {
//        myGLCD.setColor(255, 165, 0);
//        myGLCD.drawRect(112, 37, 207, 219);
//        printMain();
//      }
//      else if (((y>=37) && (y<=219)) && ((x>=208) && (x<=303)))  //clicco sul terzo rettangolo 
//      {
//        myGLCD.setColor(255, 165, 0);
//        myGLCD.drawRect(208, 37, 303, 219);
//        grafico();
//      }
      else if(y>=220) {
         myGLCD.setColor(255, 165, 0);
         myGLCD.drawRect(0,221, 319, 239);
         setClock();
      }
      break;
    default:
      break;
    }
  }
  else
    inattivita++;
}

/* descrizione: funzione che salva sulla memoria EEPROM il valore dell'altitudine
 *              utilizzando gli indirizzi 1 e 2 e nella variabile globale "altitudine";
 * param[in] int alt: altitudine da salvare (max valore 9999).
 */
void saveAltitude(int alt) {
  int miglCent = (int)(alt/100);
  EEPROM.write(0, miglCent);
  EEPROM.write(1, (alt-(miglCent*100)));
  altitudine=alt;
}

//void savePressure(int ora) {
//  int pressione = storico.getPress(ora);
//  if(ora<0&ora>-25){
//    int indirizzo=2-ora;
//    int miglCent = (int)(pressione/100);
//    EEPROM.write(indirizzo, miglCent);
//    EEPROM.write(indirizzo+25, (pressione-(miglCent*100)));
//  }
//}
//
//int readPressure(int ora) {
//  if(ora<0&ora>-25){
//    int indirizzo=2-ora;
//    int miglCent = (int)(pressione/100);
//    int p = EEPROM.write(indirizzo)*100;
//    p += EEPROM.write(indirizzo+25);
//    return p; 
//  }
//}

/* descrizione: funzione che legge e ritorna il valore dell'altitudine salvato sul'EEPROM.
 */
int readAltitude() {
  int alt = EEPROM.read(0)*100;
  alt += EEPROM.read(1);
  return alt;
}

/* descrizione: funzione che accende la retroilluminazione dello schermo.
 */
void backlightOn() 
{
  digitalWrite(BACKLIGHT_PIN,LOW); 
}

/* descrizione: funzione che spegne la retroilluminazione dello schermo.
 */
void backlightOff() 
{
  digitalWrite(BACKLIGHT_PIN,HIGH);
}

/* descrizione: funzione che stampa su porta seriale l'ora indicata;
 * param[in] Time x: valore dell'orario da stampare.
 */
//void printSerialTime(const Time& x){
//  Serial.print('\n');
//  Serial.print(x.hour);
//  Serial.print(x.min);
//  Serial.print(x.sec);
//  Serial.print(":");
//}

/* descrizione: funzione che arrotorna il numero float indicato in intero e lo ritorna;
 * param[in] float num: valore da arrotondare.
 */
int nextInt(float num) {
  if(num>0)
    return (int)(num+0.5);
  else
    return (int)(num-0.5);
}

/* descrizione: funzione che calcola gli orari di alba e tramonto del sole e li salva
 *              nell'array globale "timeArray" utilizzando la variabile globale "t" (ora attuale).
 */
void calcolaOrariSole() {
  //if(!lcdActive);
    //t = rtc.getTime();
  // The date on which you want to calculate astronomical events
  int millennio = ((int)(year()/100))*100;
  uint16_t anno = year()-millennio;
  
  // This functions are used to set your geographical coordinates of your location
  mySun.setPosition(myLatitude, myLongitude);
  
  // This is the function that allows you to calculate sunrise and sunset
  boolean check = mySun.computeSR(timeArray, twilight_minutes, day(), month(), anno);
  //return true if all the data entered are correct
  
  if( !check == true )
    // Some parameter is incorrect and the function can not perform the calculation
    Serial.println("Something wrong in function computeSR...please check your input parameters");
  if(UK.locIsDST(now())) {
    timeArray[SUNSET_H]++;
    timeArray[SUNRISE_H]++;
  }
}

char *verboseError(byte err)
{
	switch (err)
	{
	case ERROR_MBR_READ_ERROR:
		return "Error reading MBR";
		break;
	case ERROR_MBR_SIGNATURE:
		return "MBR Signature error";
		break;
	case ERROR_MBR_INVALID_FS:
		return "Unsupported filesystem";
		break;
	case ERROR_BOOTSEC_READ_ERROR:
		return "Error reading Boot Sector";
		break;
	case ERROR_BOOTSEC_SIGNATURE:
		return "Boot Sector Signature error";
		break;
	default:
		return "Unknown error";
		break;
	}
}
void printListFile() {
      byte res;
      Serial.println("Listing Files...");
      res = file.findFirstFile(&file.DE);
      if (res==NO_ERROR)
      {
        Serial.print(file.DE.filename);
        Serial.print(".");
        Serial.print(file.DE.fileext);
        Serial.print("   ");
        printTime(file.DE.time>>11, ((file.DE.time>>5) & B00111111));
        Serial.print("   Size: ");
        hexPrintLong(file.DE.fileSize);
      }
      else
      {
        Serial.println("No files found...");
      }
      while (res==NO_ERROR)
      {
        res = file.findNextFile(&file.DE);
        if (res==NO_ERROR)
        {
          Serial.print(file.DE.filename);
          Serial.print(".");
          Serial.print(file.DE.fileext);
          Serial.print("   ");
          printTime(file.DE.time>>11, ((file.DE.time>>5) & B00111111));
          Serial.print("   Size: ");
          hexPrintLong(file.DE.fileSize);
        }
      }
      Serial.println(); 
}

void hexPrintLong(unsigned long val)
{
  Serial.print("0x");
  if (val < 0x10000000)
    Serial.print("0");
  if (val < 0x1000000)
    Serial.print("0");
  if (val < 0x100000)
    Serial.print("0");
  if (val < 0x10000)
    Serial.print("0");
  if (val < 0x1000)
    Serial.print("0");
  if (val < 0x100)
    Serial.print("0");
  if (val < 0x10)
    Serial.print("0");
  Serial.print(val, HEX);
  Serial.print(" [");
  Serial.print(val, DEC);
  Serial.println("] ");
}

void printTime(byte h, byte m)
{
  if (h<10)
    Serial.print("0");
  Serial.print(h, DEC);
  Serial.print(":");
  if (m<10)
    Serial.print("0");
  Serial.print(m, DEC);
}

boolean isDayTime() {
  if(hour() < timeArray[SUNSET_H]) {
    if (minute() < timeArray[SUNSET_M])
      return true;
    else
      return false;
  }
  else if(hour() > timeArray[SUNRISE_H]) {
    if(minute() > timeArray[SUNRISE_M])
      return true;
    else
      return false; 
  } 
  else
    return false;
      
  //Serial.println(now());
  //Serial.println(getTime(sunset));
  //Serial.println(getTime(sunrise));
}

