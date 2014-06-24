// FUNZIONI *****************************************************************// 
//metodo print interfaccia principale
void printMain() {
  //numero schermata
  schermata=1;
  
  //variabili
  int pressione[4]; //0= -1h, 1= ora, 2= +1h, 3= +3h
  float umidita[2]; //0= -1h, 1= ora
  float gradi[2];  //0= -1h, 1= ora
  //recupero dati sensori
  pressione[0]=1017; pressione[1]=currPress; pressione[2]=1019; pressione[3]=1021;
  umidita[0]=63; umidita[1]=60;
  gradi[0]=21; gradi[1]=23.3;  
 
  //LAYOUT
  myGLCD.setFont(BigFont);
  //Titolo
  myGLCD.setColor(255, 165, 0); //Arancione
  myGLCD.fillRect(0, 0, 319, 20);
  myGLCD.setColor(255, 255, 255); //Bianco
  myGLCD.setBackColor(255, 165, 0); //Arancione
  myGLCD.print("METEO", CENTER, 1);
  
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
  myGLCD.print("     1017hPa                 1019hPa    ", CENTER, 141);
  
  //9riga - umidità
  myGLCD.setFont(BigFont);
  myGLCD.setColor(0, 0, 0);
  myGLCD.print("   78%", LEFT, 161);
  
  // previsione +3h
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(64,64,64);
  myGLCD.print("  +3h ", 208, 161); 
  myGLCD.setFont(SmallFont);
  myGLCD.setBackColor(255,255,255);
  myGLCD.setColor(0, 0, 0);
  myGLCD.print("1021hPa", 232, 183);
 
  //10riga - gradi
  myGLCD.setFont(BigFont);
  myGLCD.setColor(0, 0, 0); //Bianco
  myGLCD.setBackColor(255, 255, 255);
  myGLCD.print("   19'", LEFT, 181);
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
  printMeteoAttuale();
  //immagini meteo
  printImageMeteo(0,pressione[0]);
  printImageMeteo(1,pressione[1]);
  printImageMeteo(2,pressione[2]);
  
  //FINE DATI
}

void printMeteoAttuale() {
  //ora
  myGLCD.setFont(SmallFont);
  myGLCD.setBackColor(255,255,255);
  myGLCD.setColor(0, 0, 0);
  if(currPress==0)
    myGLCD.print("----hPa",CENTER,141);
  else
    myGLCD.print(String(currPress)+"hPa",CENTER,141);
  myGLCD.setFont(BigFont);
  myGLCD.setColor(0, 0, 0); //Bianco
  myGLCD.setBackColor(255, 255, 255);
  myGLCD.print(String(currStrHum)+"%",CENTER,161);
  myGLCD.print(String(currStrTemp)+"'",CENTER,181);
  printImageMeteo('0',currPress);
}
void printSituazioneAttuale() {
  //numero schermata
  schermata=3;
  //2 riga
  myGLCD.setColor(64, 64, 64); //Grigiet sotto
  myGLCD.fillRect(0, 218, 319, 239);
  myGLCD.setColor(255, 255, 255); //Bianco
  myGLCD.setBackColor(64, 64, 64); //Grigiet sotto
  myGLCD.print("Situazione Attuale", CENTER, 21);

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
    myGLCD.print("----hPa",110,192);
  else
    myGLCD.print(String(currPress)+"hPa",110,192);
  
  //immagine meteo
  printImageMeteo(2,currPress);
}

void printImageMeteo(int colonna, int pressione) // '0' = -1h, '1' = ora, '2' = +1h  
{
  myGLCD.setFont(BigFont);
  if(pressione!=0) 
  {
    prog_uint16_t* immagine;
    if(pressione>=1016)
      immagine=sereno;
    else if(pressione>=1009 & pressione<=1015)
      immagine=poconuv;
    else if(pressione<1009)
      immagine=pioggiadebole;
    if(colonna==0) //-1h
      myGLCD.drawBitmap(18,45,90,90,immagine);
    else if(colonna==1) //ora
      myGLCD.drawBitmap(115,45,90,90,immagine); 
    else if(colonna==2)  //+1h
      myGLCD.drawBitmap(211,45,90,90,immagine);
  }
  else
  {
    if(colonna==0) //-1h
      myGLCD.print("X",60,87);
    else if(colonna==1) //ora
      myGLCD.print("X",157,87); 
    else if(colonna==2)  //+1h
      myGLCD.print("X",253,87);
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
  myGLCD.drawRect(48, 24, 270, 192);
  
  //8 riga - pressione
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(255, 255, 255); //Bianco
  myGLCD.setBackColor(0,0,0);
//myGLCD.print("1234567890123456789012345678901234567890"
  myGLCD.print("      -24 -12  -6  -3  -1  ora +1      ", CENTER, 202);
  //righe griglia
  myGLCD.setColor(0, 0, 0);
  myGLCD.drawLine(80, 24, 80, 192);
  myGLCD.drawLine(112, 24, 112, 192);
  myGLCD.drawLine(144, 24, 144, 192);  
  myGLCD.drawLine(176, 24, 176, 192);
  myGLCD.drawLine(208, 24, 208, 192);
  myGLCD.drawLine(240, 24, 240, 192);
  
//rettangolini, altezza6 largh 29
  //-24
  myGLCD.setColor(50,50,50);
  myGLCD.fillRect(50, 152, 78, 158);//1001
  myGLCD.fillRect(50, 160, 78, 166);//1002
  myGLCD.fillRect(50, 168, 78, 174);//1003
  myGLCD.fillRect(50, 176, 78, 182);//1004
  myGLCD.fillRect(50, 184, 78, 190);//1005 
  //-12
  myGLCD.setColor(50,50,50);
  myGLCD.fillRect(82, 152, 110, 158);//1001
  myGLCD.fillRect(82, 160, 110, 166);//1002
  myGLCD.fillRect(82, 168, 110, 174);//1003
  myGLCD.fillRect(82, 176, 110, 182);//1004
  myGLCD.fillRect(82, 184, 110, 190);//1005
  //-6
  myGLCD.setColor(50,50,50);
  myGLCD.fillRect(114, 184, 142, 190);//1001
  //-3
  myGLCD.setColor(50,50,50);
  myGLCD.fillRect(146, 184, 174, 190);//1001
  //-1
  myGLCD.setColor(50,50,50);
  myGLCD.fillRect(178, 184, 206, 190);//1001
  //ora
  myGLCD.setColor(50,50,50);
  myGLCD.fillRect(210, 184, 238, 190);//1001
  //+1
  myGLCD.setColor(50,50,50);
  myGLCD.fillRect(242, 184, 268, 190);//1001
  //1020hpa grafico
  myGLCD.setColor(0, 0, 0); //Nero
  myGLCD.setBackColor(255,255,255);
  myGLCD.print("  1020", LEFT, 26);
  //1015hpa gr
  myGLCD.setColor(0, 0, 0); //Bianco
  myGLCD.print("  1015", LEFT, 62);
  myGLCD.drawLine(49, 66, 269, 66);
  //1010hpa grafico
  myGLCD.setColor(0, 0, 0); //Bianco
  myGLCD.print("  1010", LEFT, 104);
  myGLCD.drawLine(49, 108, 269, 108);
  //1005hpa gr
  myGLCD.setColor(0, 0, 0); //Bianco
  myGLCD.print("  1005", LEFT, 146);
  myGLCD.drawLine(49, 150, 269, 150);
  //1000hpa grafico
  myGLCD.setColor(0, 0, 0); //Bianco
  myGLCD.print("  1000", LEFT, 185);
 
  printPulsanti(); 
  printDataOra(true);
}
