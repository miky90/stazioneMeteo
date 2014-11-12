void drawUpButton(int x, int y)
{
  myGLCD.setColor(64, 64, 128);
  myGLCD.fillRoundRect(x, y, x+32, y+25);
  myGLCD.setColor(128, 128, 255);
  myGLCD.drawRoundRect(x, y, x+32, y+25);
  myGLCD.setColor(128, 128, 255);
  for (int i=0; i<15; i++)
    myGLCD.drawLine(x+6+(i/1.5), y+20-i, x+27-(i/1.5), y+20-i);
}

void drawDownButton(int x, int y)
{
  myGLCD.setColor(64, 64, 128);
  myGLCD.fillRoundRect(x, y, x+32, y+25);
  myGLCD.setColor(128, 128, 255);
  myGLCD.drawRoundRect(x, y, x+32, y+25);
  myGLCD.setColor(128, 128, 255);
  for (int i=0; i<15; i++)
    myGLCD.drawLine(x+6+(i/1.5), y+5+i, x+27-(i/1.5), y+5+i);
}

void showDOW(byte dow)
{
  char* str[] = {"SUN","MON","TUE","WED","THU","FRI","SAT"};
  
  myGLCD.setColor(128, 128, 255);
  myGLCD.setBackColor(255, 255, 255);
  myGLCD.print(str[dow-1], 26, 167);
}

byte validateDate(byte d, byte m, word y)
{
  byte mArr[12] = {31,0,31,30,31,30,31,31,30,31,30,31};
  byte od;
  
  if (m==2)
  {
    if ((y % 4)==0)
    {
      if (d==30)
        od=1;
      else if (d==0)
        od=29;
      else
        od=d;
    }
    else
    {
      if (d==29)
        od=1;
      else if (d==0)
        od=28;
      else
        od=d;
    }
  }
  else
  {
    if (d==0)
      od=mArr[m-1];
    else if (d==(mArr[m-1]+1))
      od=1;
    else
      od=d;
  }
  
  return od;
}

byte validateDateForMonth(byte d, byte m, word y)
{
  byte mArr[12] = {31,0,31,30,31,30,31,31,30,31,30,31};
  byte od;
  boolean dc=false;
  
  if (m==2)
  {
    if ((y % 4)==0)
    {
      if (d>29)
      {
        d=29;
        dc=true;
      }
    }
    else
    {  
      if (d>28)
      {
        d=28;
        dc=true;
      }
    }
  }
  else
  {
    if (d>mArr[m-1])
    {
      d=mArr[m-1];
      dc=true;
    }
  }
  
  if (dc)
    myGLCD.printNumI(d, 122, 140, 2, '0');  
  return d;
}

void setClock()
{
  schermata = 5;
  tmElements_t t_temp;
  int x, y;
  int res = 0;
  boolean ct=false;
  boolean cd=false;
  
  waitForTouchRelease();    
  
  //myGLCD.clrScr();
  myGLCD.setFont(franklingothic_normal);
  
  myGLCD.setColor(255, 255, 255); //Bianco
  myGLCD.fillRect(0, 0, 319, 239);
  
  // Draw Save button
  myGLCD.setColor(64, 64, 128);
  myGLCD.fillRoundRect(165, 200, 319, 239);
  myGLCD.setColor(128, 128, 255);
  myGLCD.drawRoundRect(165, 200, 319, 239);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(64, 64, 128);
  myGLCD.print("Save", 210, 212);
  myGLCD.setBackColor(255, 255, 255);
  // Draw Cancel button
  myGLCD.setColor(64, 64, 128);
  myGLCD.fillRoundRect(0, 200, 154, 239);
  myGLCD.setColor(128, 128, 255);
  myGLCD.drawRoundRect(0, 200, 154, 239);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(64, 64, 128);
  myGLCD.print("Cancel", 29, 212);
  myGLCD.setBackColor(255, 255, 255);
  
  // Draw frames
  myGLCD.setColor(0, 0, 255);
  myGLCD.drawRoundRect(0, 0, 319, 96);
  myGLCD.drawRoundRect(0, 100, 319, 196);
  myGLCD.print("Time:", 10, 40);
  myGLCD.print(":", 154, 40);
  myGLCD.print(":", 202, 40);
  myGLCD.print("Date:", 10, 140);
  myGLCD.print(".", 154, 140);
  myGLCD.print(".", 202, 140);
  
  // Draw Buttons
  drawUpButton(122, 10);
  drawUpButton(170, 10);
  drawUpButton(218, 10);
  drawDownButton(122, 61);
  drawDownButton(170, 61);
  drawDownButton(218, 61);
  drawUpButton(122, 110);
  drawUpButton(170, 110);
  drawUpButton(234, 110);
  drawDownButton(122, 161);
  drawDownButton(170, 161);
  drawDownButton(234, 161);
  
  // Print current time and date
  myGLCD.setColor(0, 0, 0);
  if (day()==0)
  {
    t_temp.Day=1;
    t_temp.Month=1;
    t_temp.Year=2010-1970;
    t_temp.Wday=5;
    t_temp.Hour=0;
    t_temp.Minute=0;
    t_temp.Second=0;
    ct=true;
    cd=true;
  }
  else {
    t_temp.Day=day();
    t_temp.Month=month();
    t_temp.Year=year()-1970;
    t_temp.Wday=weekday();
    t_temp.Hour=hour();
    t_temp.Minute=minute();
    t_temp.Second=second();
  }
  myGLCD.printNumI(t_temp.Hour, 122, 40,2,'0');
  myGLCD.printNumI(t_temp.Minute, 170, 40,2,'0');
  myGLCD.printNumI(t_temp.Second, 218, 40,2,'0');
  myGLCD.printNumI(t_temp.Day, 122, 140,2,'0');
  myGLCD.printNumI(t_temp.Month, 170, 140,2,'0');
  myGLCD.printNumI(t_temp.Year+1970, 218, 140,4);
  showDOW(t_temp.Wday);
  
  while (res==0 & schermata == 5)
  {
    if (myTouch.dataAvailable())
    {
      myTouch.read();
      x=myTouch.getX();
      y=myTouch.getY();
      if ((y>=10) && (y<=35)) // Buttons: Time UP
      {
        if ((x>=122) && (x<=154))
        {
          buttonWait(122, 10);
          t_temp.Hour+=1;
          if (t_temp.Hour==24)
            t_temp.Hour=0;
          myGLCD.printNumI(t_temp.Hour, 122, 40, 2, '0');
          if (ct==false)
          {
            ct=true;
            myGLCD.setColor(64, 64, 128);
            myGLCD.setFont(SmallFont);
            myGLCD.print("Changed!", 10, 10);
            myGLCD.setColor(0, 0, 0);
            myGLCD.setFont(franklingothic_normal);
          }
        }
        else if ((x>=170) && (x<=202))
        {
          buttonWait(170, 10);
          t_temp.Minute+=1;
          if (t_temp.Minute==60)
            t_temp.Minute=0;
          myGLCD.printNumI(t_temp.Minute, 170, 40, 2, '0');
          if (ct==false)
          {
            ct=true;
            myGLCD.setColor(64, 64, 128);
            myGLCD.setFont(SmallFont);
            myGLCD.print("Changed!", 10, 10);
            myGLCD.setColor(0, 0, 0);
            myGLCD.setFont(franklingothic_normal);
          }
        }
        else if ((x>=218) && (x<=250))
        {
          buttonWait(218, 10);
          t_temp.Second+=1;
          if (t_temp.Second==60)
            t_temp.Second=0;
          myGLCD.printNumI(t_temp.Second, 218, 40, 2, '0');
          if (ct==false)
          {
            ct=true;
            myGLCD.setColor(64, 64, 128);
            myGLCD.setFont(SmallFont);
            myGLCD.print("Changed!", 10, 10);
            myGLCD.setColor(0, 0, 0);
            myGLCD.setFont(franklingothic_normal);
          }
        }
      }
      else if ((y>=61) && (y<=86)) // Buttons: Time DOWN
      {
        if ((x>=122) && (x<=154))
        {
          buttonWait(122, 61);
          t_temp.Hour-=1;
          if (t_temp.Hour==255)
            t_temp.Hour=23;
          myGLCD.printNumI(t_temp.Hour, 122, 40, 2, '0');
          if (ct==false)
          {
            ct=true;
            myGLCD.setColor(64, 64, 128);
            myGLCD.setFont(SmallFont);
            myGLCD.print("Changed!", 10, 10);
            myGLCD.setColor(0, 0, 0);
            myGLCD.setFont(franklingothic_normal);
          }
        }
        else if ((x>=170) && (x<=202))
        {
          buttonWait(170, 61);
          t_temp.Minute-=1;
          if (t_temp.Minute==255)
            t_temp.Minute=59;
          myGLCD.printNumI(t_temp.Minute, 170, 40, 2, '0');
          if (ct==false)
          {
            ct=true;
            myGLCD.setColor(64, 64, 128);
            myGLCD.setFont(SmallFont);
            myGLCD.print("Changed!", 10, 10);
            myGLCD.setColor(0, 0, 0);
            myGLCD.setFont(franklingothic_normal);
          }
        }
        else if ((x>=218) && (x<=250))
        {
          buttonWait(218, 61);
          t_temp.Second-=1;
          if (t_temp.Second==255)
            t_temp.Second=59;
          myGLCD.printNumI(t_temp.Second, 218, 40, 2, '0');
          if (ct==false)
          {
            ct=true;
            myGLCD.setColor(64, 64, 128);
            myGLCD.setFont(SmallFont);
            myGLCD.print("Changed!", 10, 10);
            myGLCD.setColor(0, 0, 0);
            myGLCD.setFont(franklingothic_normal);
          }
        }
      }
      if ((y>=110) && (y<=135)) // Buttons: Date UP
      {
        if ((x>=122) && (x<=154))
        {
          buttonWait(122, 110);
          t_temp.Day+=1;
          t_temp.Day=validateDate(t_temp.Day, t_temp.Month, t_temp.Year+1970);
          myGLCD.printNumI(t_temp.Day, 122, 140, 2, '0');
          if (cd==false)
          {
            cd=true;
            myGLCD.setColor(64, 64, 128);
            myGLCD.setFont(SmallFont);
            myGLCD.print("Changed!", 10, 110);
            myGLCD.setColor(0, 0, 0);
            myGLCD.setFont(franklingothic_normal);
          }
        }
        else if ((x>=170) && (x<=202))
        {
          buttonWait(170, 110);
          t_temp.Month+=1;
          if (t_temp.Month==13)
            t_temp.Month=1;
          myGLCD.printNumI(t_temp.Month, 170, 140, 2, '0');
          t_temp.Day=validateDateForMonth(t_temp.Day, t_temp.Month, t_temp.Year+1970);
          if (cd==false)
          {
            cd=true;
            myGLCD.setColor(64, 64, 128);
            myGLCD.setFont(SmallFont);
            myGLCD.print("Changed!", 10, 110);
            myGLCD.setColor(0, 0, 0);
            myGLCD.setFont(franklingothic_normal);
          }
        }
        else if ((x>=218) && (x<=250))
        {
          buttonWait(234, 110);
          t_temp.Year+=1;
          if (t_temp.Year==130)
            t_temp.Year=30;
          myGLCD.printNumI(t_temp.Year+1970, 218, 140);
          t_temp.Day=validateDateForMonth(t_temp.Day, t_temp.Month, t_temp.Year+1970);
          if (cd==false)
          {
            cd=true;
            myGLCD.setColor(64, 64, 128);
            myGLCD.setFont(SmallFont);
            myGLCD.print("Changed!", 10, 110);
            myGLCD.setColor(0, 0, 0);
            myGLCD.setFont(franklingothic_normal);
          }
        }
        t_temp.Wday=calcDOW(t_temp.Day, t_temp.Month, t_temp.Year+1970);
        showDOW(t_temp.Wday);
      }
      else if ((y>=161) && (y<=186)) // Buttons: Date DOWN
      {
        if ((x>=122) && (x<=154))
        {
          buttonWait(122, 161);
          t_temp.Day-=1;
          t_temp.Day=validateDate(t_temp.Day, t_temp.Month, t_temp.Year+1970);
          myGLCD.printNumI(t_temp.Day, 122, 140, 2, '0');
          if (cd==false)
          {
            cd=true;
            myGLCD.setColor(64, 64, 128);
            myGLCD.setFont(SmallFont);
            myGLCD.print("Changed!", 10, 110);
            myGLCD.setColor(0, 0, 0);
            myGLCD.setFont(franklingothic_normal);
          }
        }
        else if ((x>=170) && (x<=202))
        {
          buttonWait(170, 161);
          t_temp.Month-=1;
          if (t_temp.Month==0)
            t_temp.Month=12;
          myGLCD.printNumI(t_temp.Month, 170, 140, 2, '0');
          t_temp.Day=validateDateForMonth(t_temp.Day, t_temp.Month, t_temp.Year+1970);
          if (cd==false)
          {
            cd=true;
            myGLCD.setColor(64, 64, 128);
            myGLCD.setFont(SmallFont);
            myGLCD.print("Changed!", 10, 110);
            myGLCD.setColor(0, 0, 0);
            myGLCD.setFont(franklingothic_normal);
          }
        }
        else if ((x>=218) && (x<=250))
        {
          buttonWait(234, 161);
          t_temp.Year-=1;
          if (t_temp.Year==29)
            t_temp.Year=129;
          myGLCD.printNumI(t_temp.Year+1970, 218, 140);
          t_temp.Day=validateDateForMonth(t_temp.Day, t_temp.Month, t_temp.Year+1970);
          if (cd==false)
          {
            cd=true;
            myGLCD.setColor(64, 64, 128);
            myGLCD.setFont(SmallFont);
            myGLCD.print("Changed!", 10, 110);
            myGLCD.setColor(0, 0, 0);
            myGLCD.setFont(franklingothic_normal);
          }
        }
        t_temp.Wday=calcDOW(t_temp.Day, t_temp.Month, t_temp.Year+1970);
        showDOW(t_temp.Wday);
      }
      else if ((y>=200) && (y<=239)) // Buttons: CANCEL / SAVE
      {
        if ((x>=165) && (x<=319))
        {
          res=1;
          myGLCD.setColor (255, 0, 0);
          myGLCD.drawRoundRect(165, 200, 319, 239);
        }
        else if ((x>=0) && (x<=154))
        {
          res=2;
          myGLCD.setColor (255, 0, 0);
          myGLCD.drawRoundRect(0, 200, 154, 239);
        }
      }
    }
  }
  if(schermata == 5) {
    waitForTouchRelease();
  
    if (res==1)
    {
      if (ct || cd) {
        time_t tmp_time = makeTime(t_temp);
        setTime(tmp_time);
        RTC.set(tmp_time);
      }
    }
    
    printMain();
  }
}

void waitForTouchRelease()
{
  // Wait for release
  while (myTouch.dataAvailable()==true)
    myTouch.read();
}

void waitDelayTouch() {
  int ms = 0; 
  while (myTouch.dataAvailable()==true && ms<150) {
    ms++;
    delay(10);
  }
}

void buttonWait(int x, int y)
{
  myGLCD.setColor(255, 0, 0);
  myGLCD.drawRoundRect(x, y, x+32, y+25);
  waitDelayTouch();
  myGLCD.setColor(0, 0, 0);
  myGLCD.drawRoundRect(x, y, x+32, y+25);
}

byte calcDOW(byte d, byte m, int y)
{
  int dow;
  byte mArr[12] = {6,2,2,5,0,3,5,1,4,6,2,4};
  
  dow = (y % 100);
  dow = dow*1.25;
  dow += d;
  dow += mArr[m-1];
  if (((y % 4)==0) && (m<3))
    dow -= 1;
  while (dow>7)
    dow -= 7;
    
  return dow;
}
