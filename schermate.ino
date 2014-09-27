// FUNZIONI *****************************************************************//
void printError(int id){
  switch(id) {
    case 0 : 
      errorConnection=true;
      myGLCD.setFont(SmallFont);
      myGLCD.setColor(0, 0, 0); 
      myGLCD.setBackColor(255, 165, 0); //Arancione
      myGLCD.print("Conn. Failed", 230, 3);
      break;
    default: 
      break;
  }
}

void flashPrevision(){
  if(schermata==2) {
    if(flashBar) {
      if(mSecFlash==0) {
        drawPressBar(1,calcolaPrevisione(1));
        mSecFlash+=1;
      }
      else if(mSecFlash<100)
        mSecFlash+=1;
      else {
        flashBar=false;
        mSecFlash=0;
      }
    }
    else {
      if(mSecFlash==0) {
        drawEmptyBar(1);
        mSecFlash+=1;
      }
      else if(mSecFlash<50)
        mSecFlash+=1;
      else {
        flashBar=true;
        mSecFlash=0;
      }
    }
  } 
}

void printSituazioneEsterna() {
  detachInterrupt(5);
  //numero schermata
  schermata=3;
  
  //variabili
  float pressione[4]; //0= -1h, 1= ora, 2= +1h, 3= +3h
  float umidita[2]; //0= -1h, 1= ora
  float gradi[2];  //0= -1h, 1= ora
  //recupero dati sensori
  pressione[0]=storico.getPress(-1); pressione[1]=currPress;
  pressione[2]=calcolaPrevisione(1); pressione[3]=calcolaPrevisione(2);
  umidita[0]=storico.getHum(-1); umidita[1]=currHum;
  gradi[0]=storico.getTemp(-1); gradi[1]=currTemp;
 
  //LAYOUT
  myGLCD.setFont(BigFont);
  //Titolo
//  myGLCD.setColor(255, 165, 0); //Arancione
//  myGLCD.fillRect(0, 0, 319, 20);
//  myGLCD.setColor(255, 255, 255); //Bianco
//  myGLCD.setBackColor(255, 165, 0); //Arancione
//  myGLCD.print("METEO", CENTER, 1);
  
  if(errorConnection)
    printError(0);
  
  //2 riga
  myGLCD.setColor(64, 64, 64); //Grigiet sotto
  myGLCD.fillRect(0, 218, 319, 239);
  myGLCD.setColor(255, 255, 255); //Bianco
  myGLCD.setBackColor(64, 64, 64); //Grigiet sotto
  myGLCD.print("  -1h    ora   +1h  ", CENTER, 21);

  //'sfondo'
  myGLCD.setColor(255,255,255); 
  myGLCD.fillRect(0, 38, 319, 219);
   
  //ultima riga data
  printDataOra(true);
  
  //DATI 
  //8 riga - pressione
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(0, 0, 0); //Bianco
  myGLCD.setBackColor(255,255,255);
  if(pressione[0]!=0)
    myGLCD.printNumF(pressione[0], 1 , 28, 141);
  else
    myGLCD.print("----.-", 28, 141);
  myGLCD.print("hPa", 76, 141);
  
  //previson +1
  if(pressione[2]==0)
    myGLCD.print("----.-hPa    ", 220, 141);
  else  {
    myGLCD.printNumF(pressione[2], 1, 220, 141);
    myGLCD.print("hPa    ", 268, 141);
  }
    
  //9riga - umidità
  myGLCD.setFont(BigFont);
  myGLCD.setColor(0, 0, 0);
  if(umidita[0]!=0)
    myGLCD.printNumI(umidita[0],48,161);
  else
    myGLCD.print("--",48,161);
  myGLCD.print("%", 80, 161);
  
  // previsione +2h
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(64,64,64);
  myGLCD.print("  +2h ", 208, 161); 
  myGLCD.setFont(SmallFont);
  myGLCD.setBackColor(255,255,255);
  myGLCD.setColor(0, 0, 0);
  if(pressione[3]==0)
    myGLCD.print("----.-hPa", 220, 183);
  else {
    myGLCD.printNumF(pressione[3],1, 220, 183);
    myGLCD.print("hPa", 268, 183);
  }
 
  //10riga - gradi
  myGLCD.setFont(BigFont);
  myGLCD.setColor(0, 0, 0); //Bianco
  myGLCD.setBackColor(255, 255, 255);
  if(gradi[0]!=0){
    //Serial.println(gradi[0]);
    myGLCD.printNumF(gradi[0],1,32 , 181); 
  }
  else
    myGLCD.print("--.-",32 , 181); 
  myGLCD.print("'", 96, 181);
  //FINE DATI
 
  //primo recttangolo
  myGLCD.setColor(170,170,170);
  myGLCD.drawRect(15, 37, 111,219);
  
  //secondo rettangolo
  myGLCD.setColor(210, 210, 210); //grigio
  myGLCD.drawRect(112, 37, 207, 219);
  
  //terzo rettangolo
  myGLCD.setColor(170, 170, 170); //Piu grigio
  myGLCD.drawRect(208, 37, 303, 219);
  //FINE LAYOUT
  
  //immagini meteo
  printImageMeteo(0,pressione[0]);
  //printImageMeteo(1,round(pressione[1]));
  printMeteoAttuale();
  printImageMeteo(2,pressione[2]);
  
  attachInterrupt(5, buttonPressed, RISING);
  //FINE DATI
}

void printMeteoAttuale() {
  //ora
  myGLCD.setFont(SmallFont);
  myGLCD.setBackColor(255,255,255);
  myGLCD.setColor(0, 0, 0);
  if(currPress==0)
    myGLCD.print("----.-hPa",128,141);
  else {
    myGLCD.printNumF(currPress,1,128,141);
    myGLCD.print("hPa",176,141);
  }
  myGLCD.setFont(BigFont);
  myGLCD.setColor(0, 0, 0); //Bianco
  myGLCD.setBackColor(255, 255, 255);
  myGLCD.print(String(currStrHum)+"%",CENTER,161);
  myGLCD.print(String(currStrTemp)+"'",CENTER,181);
  printImageMeteo(1,currPress);
}
//metodo print interfaccia principale
void printMain() {
  schermata=1;
  //LAYOUT
  myGLCD.setFont(BigFont);
  //Titolo
  myGLCD.setColor(255, 165, 0); //Arancione
  myGLCD.fillRect(0, 0, 319, 20);
  myGLCD.setColor(255, 255, 255); //Bianco
  myGLCD.setBackColor(255, 165, 0); //Arancione
  myGLCD.print("METEO", CENTER, 1);
  
  //2 riga
  myGLCD.setFont(BigFont);
  myGLCD.setColor(64, 64, 64); //Grigiet sotto
  myGLCD.fillRect(0, 218, 319, 239);
  myGLCD.setColor(255, 255, 255); //Bianco
  myGLCD.setBackColor(64, 64, 64); //Grigiet sotto
  myGLCD.print(" Situazione Attuale ", CENTER, 21);

  //'sfondo'
  myGLCD.setColor(255,255,255); 
  myGLCD.fillRect(0, 38, 319, 219);
  
  //ultima riga data
  printDataOra(true);
  
  //primo recttangolo situazione interna
  myGLCD.setColor(170,170,170);
  myGLCD.drawRect(5, 37, 105,177);
  
  //secondo rettangolo sistuazione esterna
  myGLCD.setColor(210, 210, 210); //grigio
  myGLCD.drawRect(106, 37, 206, 177);
  
  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(255,255,255);
  myGLCD.setColor(0,0,0);
  
  //IN
  myGLCD.print("IN",38,52);
  char tmpTemp[2];
  dtostrf(currInTemp,2,1,tmpTemp);
  //myGLCD.setFont(SevenSegNumFont);
  //myGLCD.printNum(tmpTemp,14,87);
  myGLCD.print(String(tmpTemp)+"'",20,87);
  char tmpHum[3];
  dtostrf(currInHum,2,0,tmpHum);
  myGLCD.setFont(BigFont);
  //myGLCD.print(String(tmpHum)+"%",33,147);
  myGLCD.print(String(tmpHum)+"%",33,117);
  
  //OUT
  myGLCD.print("OUT",131,52);
  //myGLCD.setFont(SevenSegNumFont);
  //myGLCD.print(currStrTemp,116,87);
  myGLCD.print(String(currStrTemp)+"'",122,87);
  myGLCD.setFont(BigFont);
  //myGLCD.print(String(currStrHum)+"%",138,147);
  myGLCD.print(String(currStrHum)+"%",138,117);
  
  //icona indietro
  myGLCD.drawBitmap(15,184,30,30,arrow);
  //icona settings
  myGLCD.drawBitmap(60,184,30,30,settings);
  
  //pressione
  myGLCD.setFont(BigFont);
  if(currPress==0)
    myGLCD.print("----.-hPa",100,192);
  else {
    myGLCD.printNumF(currPress,1,100,192);
    myGLCD.print("hPa",196,192);
  }
  
  //immagine meteo
  printImageMeteo(2,currPress);
}

void printImageMeteo(int colonna, float pressione) // '0' = -1h, '1' = ora, '2' = +1h  
{
  uint8_t* timeArray; 
  timeArray = calcolaOrariSole();
  // if you want you can use specially created index:
  // SUNRISE_H SUNRISE_M SUNSET_H SUNSET_M
  word res;
  myGLCD.setFont(BigFont);
  if(pressione!=0) 
  {
   int index=0;
    if(pressione>=1016)
    {
      if((t.hour<timeArray[SUNSET_H]) && (t.hour>timeArray[SUNRISE_H]))
        index=0;
      else
        index=3;
    }
    else if(pressione>=1009 & pressione<1016)
    {
      if((t.hour<timeArray[SUNSET_H]) && (t.hour>timeArray[SUNRISE_H]))
        index=1;
      else
        index=4;
    }
    else if(pressione<1009) {
      if(currTemp>1.0)
        index = 2;
      else
        index = 5;
    }
    if(colonna==0) //-1h
      res = myFiles.loadBitmap(18, 45, 90, 90, images[index]);
      //myGLCD.drawBitmap(18,45,90,90,immagine);
    else if(colonna==1) //ora
      res = myFiles.loadBitmap(115, 45, 90, 90, images[index]);
      //myGLCD.drawBitmap(115,45,90,90,immagine); 
    else if(colonna==2)  //+1h
      res = myFiles.loadBitmap(211, 45, 90, 90, images[index]);
      //myGLCD.drawBitmap(211,45,90,90,immagine);
    if (res!=0)
      {
        myGLCD.setFont(SmallFont);
        if (res==0x10)
        {
          myGLCD.print("File not found...", 0, 0);
          myGLCD.print(images[index], 0, 14);
        }
        else
        {
          myGLCD.print("ERROR: ", 0, 0);
          myGLCD.printNumI(res, 56, 0);
        }
      }
  }
  else
  {
    myGLCD.setFont(SmallFont);
    myGLCD.setColor(0, 0, 0);
    if(colonna==0) //-1h
      myGLCD.print("N.A.",60,87);
    else if(colonna==1) //ora
      myGLCD.print("N.A.",157,87); 
    else if(colonna==2)  //+1h
      myGLCD.print("N.A.",253,87);
  }
  
} 

void printDataOra(boolean ristampa) {
  t = rtc.getTime();
  int giorno, mese, anno, ore, minuti;
  minuti = t.min;
  if(ristampa|oldMin!=minuti) {
   //stampa data ora utlima riga
   oldMin=minuti;
  myGLCD.setFont(BigFont);
  myGLCD.setColor(255, 255, 255); //Bianco
  myGLCD.setBackColor(64, 64, 64); 
  
  giorno = t.date; mese = t.mon; anno = t.year; ore = t.hour;
  if(giorno<10) {
    myGLCD.printNumI(0,LEFT,221);
    myGLCD.printNumI(giorno, 16, 221);
  }
  else  
    myGLCD.printNumI(giorno, LEFT, 221);
  myGLCD.print("/",32,221);
  if(mese<10) {
    myGLCD.printNumI(0,48,221);
    myGLCD.printNumI(mese,64,221);
  }
  else
    myGLCD.printNumI(mese,48,221);
  myGLCD.print("/",80,221);
  
  myGLCD.printNumI(anno,96,221);
  if(ore<10) {
    myGLCD.printNumI(0,240,221);
    myGLCD.printNumI(ore,256,221);
  }
  else
    myGLCD.printNumI(ore,240,221);
  myGLCD.print(":",272,221);
  if(minuti<10) {
    myGLCD.printNumI(0,288,221);
    myGLCD.printNumI(minuti,304,221);
  }
  else
    myGLCD.printNumI(minuti,288,221); 
  }
}

void printPulsanti() {
  //icona indietro
  myGLCD.drawBitmap(280,45,30,30,arrow);
  //icona settings
  myGLCD.drawBitmap(280,132,30,30,settings);
}

void grafico () {
  schermata=2;
  flashBar=true;
  mSecFlash=0;
  
  //Titolo
  myGLCD.setFont(BigFont);
  myGLCD.setColor(255, 165, 0); //Arancione
  myGLCD.fillRect(0, 0, 319, 20);
  myGLCD.setColor(255, 255, 255); //Bianco
  myGLCD.setBackColor(255, 165, 0); //Arancione
  myGLCD.print("PRESSIONE", CENTER, 1);

  //'sfondo'
  myGLCD.setColor(255,255,255); 
  myGLCD.fillRect(0, 21, 319, 220);
  
  //rettangolo grafico
  myGLCD.setColor(0,0,0);
  myGLCD.drawRect(48, 24, 272, 192);
  
  //8 riga - pressione
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(255, 255, 255); //Bianco
  myGLCD.setBackColor(0,0,0);
//myGLCD.print("1234567890123456789012345678901234567890"
  myGLCD.print("      -24 -12  -6  -3  -1 ora  +1      ", CENTER, 202);
  //righe griglia
  myGLCD.setColor(0, 0, 0);
  myGLCD.drawLine(80, 24, 80, 192);
  myGLCD.drawLine(112, 24, 112, 192);
  myGLCD.drawLine(144, 24, 144, 192);  
  myGLCD.drawLine(176, 24, 176, 192);
  myGLCD.drawLine(208, 24, 208, 192);
  myGLCD.drawLine(240, 24, 240, 192);
  
  float pressioni[7];
  pressioni[0]=storico.getPress(-24);
  pressioni[1]=storico.getPress(-12);
  pressioni[2]=storico.getPress(-6);
  pressioni[3]=storico.getPress(-3);
  pressioni[4]=storico.getPress(-1);
  pressioni[5]=currPress;
  pressioni[6]=calcolaPrevisione(1);
  
  int massimo = max (pressioni[0],pressioni[1]);
  massimo = max (massimo,pressioni[2]);
  massimo = max (massimo,pressioni[3]);
  massimo = max (massimo,pressioni[4]);
  massimo = max (massimo,pressioni[5]);
  massimo = max (massimo,pressioni[6]);
  
  int minimo = min (pressioni[0],pressioni[1]);
  minimo = min (minimo,pressioni[2]);
  minimo = min (minimo,pressioni[3]);
  minimo = min (minimo,pressioni[4]);
  minimo = min (minimo,pressioni[5]);
  minimo = min (minimo,pressioni[6]);
  
  int  media = (massimo + minimo )/2;
  if(media > 1017) {
    if(minimo<1005)
      maxScala = 1020;
    else if(massimo <1025)
      maxScala = 1025;
    else
      maxScala = 1030;
  }
  else {
    if(massimo>1025)
		maxScala = 1030;
    else if(massimo>1020)
      maxScala = 1025;
    else
      maxScala = 1020;
  }
  
  /*if(massimo>1015)
    maxScala = 1025;
  else if(massimo<1015 & massimo>1005)
    maxScala = 1020;
  else if(massimo<1005)
    maxScala = 1015;
  */
  drawPressBar(-24,pressioni[0]);
  drawPressBar(-12,pressioni[1]);
  drawPressBar(-6,pressioni[2]);
  drawPressBar(-3,pressioni[3]);
  drawPressBar(-1,pressioni[4]);
  drawPressBar(0,pressioni[5]);
  drawPressBar(1,pressioni[6]);
  
  //1020hpa grafico
  myGLCD.setColor(0, 0, 0); //Nero
  myGLCD.setBackColor(255,255,255);
  myGLCD.printNumI(maxScala, 16, 26);
  //1015hpa gr
  myGLCD.setColor(0, 0, 0); //Bianco
  myGLCD.printNumI((maxScala-5), 16, 62);
  myGLCD.drawLine(49, 66, 271, 66);
  //1010hpa grafico
  myGLCD.setColor(0, 0, 0); //Bianco
  myGLCD.printNumI((maxScala-10), 16, 104);
  myGLCD.drawLine(49, 108, 271, 108);
  //1005hpa gr
  myGLCD.setColor(0, 0, 0); //Bianco
  myGLCD.printNumI((maxScala-15), 16, 146);
  myGLCD.drawLine(49, 150, 271, 150);
  //1000hpa grafico
  myGLCD.setColor(0, 0, 0); //Bianco
  myGLCD.printNumI((maxScala-20), 16, 185);
 
  printPulsanti(); 
  printDataOra(true);
}

void drawEmptyBar(int ora) {
  int pressione = maxScala;
  int x = 50;
  int y = 190;
  int colonna;
  switch (ora) {
    case -24: colonna = 0; break;
    case -12: colonna = 1; break;
    case -6: colonna = 2; break;
    case -3: colonna = 3; break;
    case -1: colonna = 4; break;
    case 0: colonna = 5; break;
    case 1: colonna = 6; break;
  }
  x = 50 + 32*colonna;
  int altezza = pressione - (maxScala - 20);
  myGLCD.setColor(255,255,255);
  int k=0;
  for(int i=0; i<altezza; i++) {
    if((i%5==0) & (i!=0))
      y-=2;
    myGLCD.fillRect(x, y-(8*i), x+28, y-(8*i)-6);//1001
  }
}

void drawPressBar(int ora, float tPressione) {
    int pressione = nextInt(tPressione);
    int x = 50;
    int y = 190;
    int colonna;
    switch (ora) {
      case -24: colonna = 0; break;
      case -12: colonna = 1; break;
      case -6: colonna = 2; break;
      case -3: colonna = 3; break;
      case -1: colonna = 4; break;
      case 0: colonna = 5; break;
      case 1: colonna = 6; break;
      //default: colonna = 10; break;
    }
    x = 50 + 32*colonna;
    if(pressione>1000) {
      int altezza = pressione - (maxScala - 20);
      if(colonna==6){
        if(tPressione<currPress)
          myGLCD.setColor(255,106,6);
        else
          myGLCD.setColor(64,177,76);
      }
      else
        myGLCD.setColor(150,150,150);
      int k=0;
      for(int i=0; i<altezza; i++) {
        if((i%5==0) & (i!=0))
          y-=2;
        myGLCD.fillRect(x, y-(8*i), x+28, y-(8*i)-6);//1001
      }
    }
}
