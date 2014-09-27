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



float calcolaPrevisione(int ora) {
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

// void saveToSd() {
 // if(sdAviable) {
   // res=file.openFile("BIGFILE.TXT", FILEMODE_TEXT_WRITE);
    // if (res==NO_ERROR)
    // {
      // String stringa = rtc.getTimeStr();
      // stringa += "t:";
      // stringa += currStrTemp;
      // stringa += "h:";
      // stringa += currStrHum;
      // stringa += "p:";
      // stringa += currPress;
      // char * buf;
      // stringa.toCharArray(buf,stringa.length());
      // file.writeLn(buf);
      // file.closeFile();
    // }
    // else
    // {
      // switch(res)
      // {
        // case ERROR_ANOTHER_FILE_OPEN:
          // Serial.println("** ERROR: Another file is already open...");
          // break;
        // default:
          // Serial.print("** ERROR: ");
          // Serial.println(res, HEX);
          // break;
      // }
    // }
  //Serial.println("***** All done... *****");
  // }
// }

void storeData () {
  Serial.println("store data");
  storico.saveCurrent(currTemp, currHum, currPress);
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
    float tempWifi=getTemp(bufferWifi);
    float humWifi= getHum(bufferWifi);
    if((currStrTemp!= "--.-") && (currStrHum != "--") && (currPress!=0)) {
      currTemp=tempWifi;
      currHum=humWifi;
    }
    else {
      currTemp=0;
      currStrTemp= "--.-";
      currHum=0;
      currStrHum = "--";
    }
    
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
    for(int i=0;i<7;i++) {
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
    for(int i=15;i<19;i++)
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
    for(int i=9;i<11;i++)
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
      //      else if (((y>=184) && (y<=214)) && ((x>=15) && (x<=45)))        //clicco su indietro
//      {
//        myGLCD.setColor(255, 165, 0);
//        myGLCD.drawRect(15,184, 45, 214);
//        printMain();
//      }
//      else if (((y>=184) && (y<=214)) && ((x>=60) && (x<=184))) //clicco su settings
//      {
//        myGLCD.setColor(255, 165, 0);
//        myGLCD.drawRect(60,184, 90, 214);
//        //printSettings();
//        setClock();
//      }
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

uint8_t* calcolaOrariSole() {
  if(!lcdActive);
    t = rtc.getTime();
  // The date on which you want to calculate astronomical events
  uint8_t day = t.date;
  uint8_t month = t.mon;
  uint16_t year = t.year;
  
  // The array where will be saved variables of sunrise and sunset
  // with the following form:
  // timeArray[ Rise_hours, Rise_minutes, Set_hours, Set_minutes ]
  // if you want you can use specially created index:
  // SUNRISE_H SUNRISE_M SUNSET_H SUNSET_M
  uint8_t* timeArray;
  
  // This functions are used to set your geographical coordinates of your location
  mySun.setPosition(myLatitude, myLongitude);
  
  // This is the function that allows you to calculate sunrise and sunset
  boolean check = mySun.computeSR(timeArray, twilight_minutes, day, month, year);
  //return true if all the data entered are correct

  if( check == true )
  {
    return timeArray;
  }
  else
    // Some parameter is incorrect and the function can not perform the calculation
    Serial.println("Something wrong in function computeSR...please check your input parameteer");
}

//char *verboseError(byte err)
//{
//	switch (err)
//	{
//	case ERROR_MBR_READ_ERROR:
//		return "Error reading MBR";
//		break;
//	case ERROR_MBR_SIGNATURE:
//		return "MBR Signature error";
//		break;
//	case ERROR_MBR_INVALID_FS:
//		return "Unsupported filesystem";
//		break;
//	case ERROR_BOOTSEC_READ_ERROR:
//		return "Error reading Boot Sector";
//		break;
//	case ERROR_BOOTSEC_SIGNATURE:
//		return "Boot Sector Signature error";
//		break;
//	default:
//		return "Unknown error";
//		break;
//	}
//}
