void aggiornaDati() {
  Serial.println("func. aggiorno dati");
  while (nrf24.available()) 
  {
    recuperaDati();
    recuperaDatiInterni();
    if(lcdActive & schermata==1) 
      printMeteoAttuale();
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
    currTemp=getTemp(bufferWifi);
    currHum=getHum(bufferWifi);
    nrf24.setModeIdle();   //Moallità risparmio energetico wifi
  }
  printSerialTime(t);
  printSerialTime(oldDataSaved);
  if(oldDataSaved.hour==23){
    if(t.hour==0|storico.getHum()==0)
    {
      storeData();
      oldDataSaved=t;
    }
  }
  else {
    if (oldDataSaved.hour==t.hour-1|storico.getHum()==0) 
    {
      storeData();
      oldDataSaved=t;
    }
  }
}
int getPressione(char* buff) 
{
  if(buff[0]=='p' && buff[1]>='0' && buff[1]<='9') 
  {
    char pr[5];
    for(int i=0;i<5;i++) {
      pr[i]=buff[i+1];
    }
    pr[4]='\0';
    return atoi(pr);
  }
  else 
    return 0;
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
        printSituazioneAttuale();
      }
      else if (((y>=37) && (y<=219)) && ((x>=208) && (x<=303)))  //clicco sul terzo rettangolo 
      {
        myGLCD.setColor(255, 165, 0);
        myGLCD.drawRect(208, 37, 303, 219);
        grafico();
      }
      break;
    case 2:  // caso 2 schermata grafico pressione
      if (((y>=45) && (y<=75)) && ((x>=280) && (x<=310)))        //clicco su indietro
      {
        myGLCD.setColor(255, 165, 0);
        myGLCD.drawRect(280, 45, 310, 75);
        printMain();
      }
      else if (((y>=132) && (y<=162)) && ((x>=280) && (x<=310))) //clicco su settings
      {
        myGLCD.setColor(255, 165, 0);
        myGLCD.drawRect(280, 132, 310, 162);
        setPressione();
      }
      break;
    case 3:
       if (((y>=184) && (y<=214)) && ((x>=15) && (x<=45)))        //clicco su indietro
      {
        myGLCD.setColor(255, 165, 0);
        myGLCD.drawRect(15,184, 45, 214);
        printMain();
      }
      else if (((y>=184) && (y<=214)) && ((x>=60) && (x<=184))) //clicco su settings
      {
        myGLCD.setColor(255, 165, 0);
        myGLCD.drawRect(60,184, 90, 214);
        //printSettings();
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


int readAltitude() {
  int alt = EEPROM.read(0);
  alt += EEPROM.read(1)*100;
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
