void aggiornaDati() {
  Serial.println("func. aggiorno dati");
  while (nrf24.available()) 
  {
    recuperaDati();
  }
    touchInterface();
    recuperaDatiInterni();
//    if(lcdActive & schermata==3) 
//      printMeteoAttuale();
}



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

void saveToSd() {
  Serial.println("save to SD");
  if(sdAviable) {
    String nomeFile =  rtc.getDateStr();
    nomeFile.replace(".","");
    nomeFile.concat(".TXT");
    char nome [13];
    nomeFile.toCharArray(nome,13);
    Serial.println(nome);
    Serial.println(nomeFile);
    if (!file.exists(nome))
      file.create(nome);
    byte res=file.openFile(nome, FILEMODE_TEXT_WRITE);
     if (res==NO_ERROR)
     {
       String stringa = rtc.getTimeStr();
       stringa.concat("t:");
       stringa.concat(currStrTemp);
       stringa.concat("h:");
       stringa.concat(currStrHum);
       stringa.concat("p:");
       int intero = currPress;
       int decimale = ((int)(currPress*10))-intero;
       stringa.concat(intero);
       stringa.concat(".");
       stringa.concat(decimale);
       char * buf = new char[27];
       Serial.println(stringa);
       stringa.toCharArray(buf,27);
       Serial.println(buf);
       file.writeLn(buf);
       file.closeFile();
       delete buf;
     }
     else
     {
       switch(res)
       {
         case ERROR_ANOTHER_FILE_OPEN:
           Serial.println("** ERROR: Another file is already open...");
           break;
         default:
           Serial.print("** ERROR: ");
           Serial.println(res,HEX);
           break;
       }
     }
  }
}

void retriveFromSd() {
  Serial.println("retrive from SD");
  byte res;
  word result;
  char textBuffer[27];
  String nomeFile =  rtc.getDateStr();
  nomeFile.replace(".","");
  nomeFile.concat(".TXT");
  char nome [nomeFile.length()+1];
  nomeFile.toCharArray(nome,nomeFile.length()+1);
  Serial.println(nome);
  if (file.exists(nome))
  {  
    res=file.openFile(nome, FILEMODE_TEXT_READ);
    if (res==NO_ERROR)
    {
      int maxHour = -1;
      result=0;
      while ((result!=EOF) and (result!=FILE_IS_EMPTY))
      {
        result=file.readLn(textBuffer, 27);
        if (result!=FILE_IS_EMPTY)
        {
          if (result==BUFFER_OVERFLOW) {
            Serial.print(textBuffer);
          }
          else{
            Serial.println(textBuffer);
            
            char cOra[] = {textBuffer[0], textBuffer[1]};
            Serial.print(textBuffer[0]);
            Serial.println(textBuffer[1]);
            Serial.println(cOra);
            int ora = atoi(cOra);
            Serial.print("ora: ");
            Serial.println(ora);
            if(maxHour<ora){
              maxHour=ora;
     
              char cTemp[] = {textBuffer[10], textBuffer[11], textBuffer[12], textBuffer[13]};
              float tTemp = atof(cTemp);
              
            
              char cHum[] = {textBuffer[16], textBuffer[17]};
              int tHum = atoi(cHum);
         
              char cPress[] = {textBuffer[20], textBuffer[21], textBuffer[22], textBuffer[23], textBuffer[24], textBuffer[25]};
              float tPress = atof(cPress);
              storico.saveCurrent(tTemp, tHum, tPress);
            }
            
          }
        }
        else
          Serial.println("** ERROR: File is empty...");
      }
      Serial.println();
      file.closeFile();
      int diffHour = t.hour - maxHour;
      if(diffHour)
        for(int i=0;i<diffHour;i++)
          storico.saveCurrent(0, 0, 0);
      else {
          currPress=storico.getPress();
          currTemp=storico.getTemp();
          currHum=storico.getHum();
      }
    }
    else
    {
      switch(res)
      {
        case ERROR_ANOTHER_FILE_OPEN:
          Serial.println("** ERROR: Another file is already open...");
          break;
        default:
          Serial.print("** ERROR: ");
          Serial.println(res,HEX);
          break;
      }
    }
  }
  else
    Serial.println("** ERROR: file does not exist...");
}

void storeData () {
  Serial.println("store data");
  storico.saveCurrent(currTemp, currHum, currPress);
  saveToSd();
}

void recuperaDatiInterni() {
  Serial.println("aggiorno dati interni");
  delay(dht.getMinimumSamplingPeriod());
  currInHum = dht.getHumidity();
  currInTemp = dht.getTemperature();
}

void checkDataOra() {
   printDataOra(false); 
}

void recuperaDati() {
  Serial.println("aggiorno dati esterni");
  char* bufferWifi;
  // Should be a message for us now   
  uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
  if (nrf24.recv(buf, &len)) {
    bufferWifi=(char*)buf;
    currPress=sealevel(getPressione(bufferWifi),altitudine);
    currTemp=getTemp(bufferWifi);
    currHum= getHum(bufferWifi);
//    if((currStrTemp!= "--.-") && (currStrHum != "--") && (currPress!=0)) {
//      currTemp=tempWifi;
//      Serial.print(currTemp);
//      currHum=humWifi;
//    }
//    else {
//      currTemp=0;
//      currStrTemp= "--.-";
//      currHum=0;
//      currStrHum = "--";
//    }
    nrf24.setModeIdle();   //Moallità risparmio energetico wifi
  }
  if(!lcdActive);
    t = rtc.getTime();
  printSerialTime(t);
  printSerialTime(oldDataSaved);
  if(((oldDataSaved.hour==23) && (t.hour==0))){
      storeData();
      oldDataSaved=t;
  }
  else if ((oldDataSaved.hour==t.hour-1)) 
    {
      storeData();
      oldDataSaved=t;
    }
}
float getPressione(char* buff) 
{
  if(buff[0]=='p' && buff[1]>='0' && buff[1]<='9') 
  {
    char pr[7];
    for(uint8_t i=0;i<7;i++) {
      pr[i]=buff[i+1];
    }
    pr[6]='\0';
    return atof(pr);
  }
  else 
    return 0.0;
}
float getTemp(char* buff) 
{
  if(buff[14]=='t' && buff[15]>='0' && buff[15]<='9') 
  {
    for(uint8_t i=15;i<19;i++)
      currStrTemp[i-15]=buff[i];
    currStrTemp[4]='\0';
    return atof(currStrTemp);
  }
  else {
    currStrTemp= "--.-";
    return 0.0;
  }
}
float getHum(char* buff) 
{
  if(buff[8]=='u'&& buff[9]>='0' && buff[9]<='9') 
  {
    for(uint8_t i=9;i<11;i++)
      currStrHum[i-9]=buff[i];
    currStrHum[2]='\0';
    return atof(currStrHum);
  }
  else {
    currStrHum = "--";
    return 0.0;
  }
}

double sealevel(double P, double A)
// Given a pressure P (mb) taken at a specific altitude (meters),
// return the equivalent pressure (mb) at sea level.
// This produces pressure readings that can be used for weather measurements.
{
	return(P/pow(1-(A/44330.0),5.255));
}

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
           myGLCD.drawRect(5, 37, 105, 177);
           
        }
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
      else if (((y>=132) && (y<=162)) && ((x>=280) && (x<=310))) //clicco su settings
      {
        myGLCD.setColor(255, 165, 0);
        myGLCD.drawRect(280, 132, 310, 162);
        setPressione();
      }
      else if(y>=220) {
         myGLCD.setColor(255, 165, 0);
         myGLCD.drawRect(0,221, 319, 239);
         setClock();
      }
      break;
    case 3: // schermata situazione esterna
      myGLCD.drawRect(15, 37, 111,219);
      if (((y>=37) && (y<=219)) && ((x>=15) && (x<=111)))        //clicco sul primo rettangolo
      {
        myGLCD.setColor(255, 165, 0);
        myGLCD.drawRect(15, 37, 111,219);
        grafico();
      }
      else if (((y>=37) && (y<=219)) && ((x>=112) && (x<=207)))  //clicco sul secondo rettangolo 
      {
        myGLCD.setColor(255, 165, 0);
        myGLCD.drawRect(112, 37, 207, 219);
        printMain();
      }
      else if (((y>=37) && (y<=219)) && ((x>=208) && (x<=303)))  //clicco sul terzo rettangolo 
      {
        myGLCD.setColor(255, 165, 0);
        myGLCD.drawRect(208, 37, 303, 219);
        grafico();
      }
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

int readAltitude() {
  int alt = EEPROM.read(0)*100;
  alt += EEPROM.read(1);
  return alt;
}

void backlightOn() 
{
  digitalWrite(backlightPin,LOW); 
}
void backlightOff() 
{
  digitalWrite(backlightPin,HIGH);
}

void printSerialTime(Time x){
  Serial.print('\n');
  Serial.print(x.hour);
  Serial.print(x.min);
  Serial.print(x.sec);
  Serial.print(":");
}

int nextInt(float num) {
  if(num>0)
    return (int)(num+0.5);
  else
    return (int)(num-0.5);
}

void calcolaOrariSole() {
  if(!lcdActive);
    t = rtc.getTime();
  // The date on which you want to calculate astronomical events
  uint8_t day = t.date;
  uint8_t month = t.mon;
  int millennio = ((int)(t.year/100))*100;
  uint16_t year = t.year-millennio;
  
  // This functions are used to set your geographical coordinates of your location
  mySun.setPosition(myLatitude, myLongitude);
  
  // This is the function that allows you to calculate sunrise and sunset
  boolean check = mySun.computeSR(timeArray, twilight_minutes, day, month, year);
  //return true if all the data entered are correct
  
  if( !check == true )
    // Some parameter is incorrect and the function can not perform the calculation
    Serial.println("Something wrong in function computeSR...please check your input parameters");
  timeArray[SUNSET_H]++;
  timeArray[SUNRISE_H]++;
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
  Serial.println("isDaytime");
  Time sunset;
  Time sunrise;
  sunset.hour = timeArray[SUNSET_H];
  sunset.min = timeArray[SUNSET_M];
  sunset.sec = 0;
  sunrise.hour = timeArray[SUNRISE_H];
  sunrise.min = timeArray[SUNSET_M];
  sunrise.sec = 0;
  
  if( (getTime(t) < getTime(sunset)) && (getTime(t) > getTime(sunrise)) )
      return true;
  else
      return false;  
      
  //Serial.println(now());
  //Serial.println(getTime(sunset));
  //Serial.println(getTime(sunrise));
}

unsigned long getTime(const Time& _time) {
    unsigned long secondi; 
    secondi = ((unsigned long)_time.hour) * 3600;
    secondi += ((unsigned long)_time.min) * 60;
    secondi += _time.sec;
    return secondi;
}
