void saveToSd() {
 //if(t.minute%5) {
   //salvo su SD
 //}
}

void recuperaDatiInterni() {
  delay(dht.getMinimumSamplingPeriod());
  currInHum = dht.getHumidity();
  currInTemp = dht.getTemperature();
}

void checkDataOra() {
   printDataOra(false); 
}

void recuperaDati() {
  char* bufferWifi;
  // Should be a message for us now   
  uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
  if (nrf24.recv(buf, &len)) {
    bufferWifi=(char*)buf;
    currPress=sealevel(getPressione(bufferWifi),altitudine);
    getTemp(bufferWifi);
    getHum(bufferWifi);
    nrf24.setModeIdle();   //Moallità risparmio energetico wifi
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
void getTemp(char* buff) 
{
  if(buff[14]=='t' && buff[15]>='0' && buff[15]<='9') 
  {
    for(int i=15;i<19;i++)
      currStrTemp[i-15]=buff[i];
    currStrTemp[4]='\0';
  }
  else 
    currStrTemp= "--.-";
}
void getHum(char* buff) 
{
  if(buff[8]=='u'&& buff[9]>='0' && buff[9]<='9') 
  {
    for(int i=9;i<11;i++)
      currStrHum[i-9]=buff[i];
    currStrHum[2]='\0';
  }
  else 
    currStrHum = "--";
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
        //printSettings();
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

void backlightOn() 
{
  digitalWrite(backlightPin,LOW); 
}
void backlightOff() 
{
  digitalWrite(backlightPin,HIGH);
}
