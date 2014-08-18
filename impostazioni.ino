void setPressione()
{
  int t_alt;
  int x, y;
  int res = 0;
  boolean ct=false;
//  boolean cd=false;
  
  waitForTouchRelease();    
  
  //myGLCD.clrScr();
  myGLCD.setFont(BigFont);
  
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
  myGLCD.print("Altitudine:", 10, 40);
  //myGLCD.print(":", 154, 40);
  //myGLCD.print(":", 202, 40);
  //myGLCD.print("Date:", 10, 140);
  //myGLCD.print(".", 154, 140);
  //myGLCD.print(".", 202, 140);
  
  // Draw Buttons
  drawUpButton(172, 10);
  drawUpButton(220, 10);
  //drawUpButton(218, 10);
  drawDownButton(172, 61);
  drawDownButton(220, 61);
  //drawDownButton(218, 61);
//  drawUpButton(122, 110);
//  drawUpButton(170, 110);
//  drawUpButton(234, 110);
//  drawDownButton(122, 161);
//  drawDownButton(170, 161);
//  drawDownButton(234, 161);

  // Print current altitude
  myGLCD.setColor(0, 0, 0);
  t_alt = (int)altitudine;
  
  if (t_alt>=1000)
  {
    //myGLCD.printNumI(0, 170, 40);
    myGLCD.printNumI(t_alt, 188, 40);
  }
  else if (t_alt>=100)
  {
    //myGLCD.printNumI(0, 170, 40);
    myGLCD.printNumI(t_alt, 204, 40);
  }
  else
  {
    myGLCD.printNumI(t_alt, 220, 40);
  }
  while (res==0)
  {
    if (myTouch.dataAvailable())
    {
      myTouch.read();
      x=myTouch.getX();
      y=myTouch.getY();
      if ((y>=10) && (y<=35)) // Buttons: Time UP
      {
        if ((x>=172) && (x<=204))
        {
          buttonWait(172, 10);
          if(t_alt+100<=4000)
            t_alt+=100;
          if (t_alt>=1000)
          {
            //myGLCD.printNumI(0, 170, 40);
            myGLCD.printNumI(t_alt, 188, 40);
          }
          else if (t_alt>=100)
          {
            //myGLCD.printNumI(0, 170, 40);
            myGLCD.printNumI(t_alt, 204, 40);
          }
          else
          {
            myGLCD.printNumI(t_alt, 220, 40);
          }
          if (ct==false)
          {
            ct=true;
            myGLCD.setColor(64, 64, 128);
            myGLCD.setFont(SmallFont);
            myGLCD.print("Changed!", 10, 10);
            myGLCD.setColor(0, 0, 0);
            myGLCD.setFont(BigFont);
          }
        }
        else if ((x>=220) && (x<=252))
        {
          buttonWait(220, 10);
          if(t_alt+10<=4000)
            t_alt+=10;
          if (t_alt>=1000)
          {
            //myGLCD.printNumI(0, 170, 40);
            myGLCD.printNumI(t_alt, 188, 40);
          }
          else if (t_alt>=100)
          {
            //myGLCD.printNumI(0, 170, 40);
            myGLCD.printNumI(t_alt, 204, 40);
          }
          else
          {
            myGLCD.printNumI(t_alt, 220, 40);
          }
          if (ct==false)
          {
            ct=true;
            myGLCD.setColor(64, 64, 128);
            myGLCD.setFont(SmallFont);
            myGLCD.print("Changed!", 10, 10);
            myGLCD.setColor(0, 0, 0);
            myGLCD.setFont(BigFont);
          }
        }
//        else if ((x>=218) && (x<=250))
//        {
//          buttonWait(218, 10);
//          t_temp.sec+=1;
//          if (t_temp.sec==60)
//            t_temp.sec=0;
//          if (t_temp.sec<10)
//          {
//            myGLCD.printNumI(0, 218, 40);
//            myGLCD.printNumI(t_temp.sec, 234, 40);
//          }
//          else
//          {
//            myGLCD.printNumI(t_temp.sec, 218, 40);
//          }
//          if (ct==false)
//          {
//            ct=true;
//            myGLCD.setColor(64, 64, 128);
//            myGLCD.setFont(SmallFont);
//            myGLCD.print("Changed!", 10, 10);
//            myGLCD.setColor(0, 0, 0);
//            myGLCD.setFont(BigFont);
//          }
//        }
      }
      else if ((y>=61) && (y<=86)) // Buttons: Time DOWN
      {
        if ((x>=172) && (x<=204))
        {
          buttonWait(172, 61);
          if(t_alt>=100)
            t_alt-=100;
          if (t_alt>=1000)
          {
            //myGLCD.printNumI(0, 170, 40);
            myGLCD.printNumI(t_alt, 188, 40);
          }
          else if (t_alt>=100)
          {
            myGLCD.print(" ", 188, 40);
            //myGLCD.printNumI(0, 170, 40);
            myGLCD.printNumI(t_alt, 204, 40);
          }
          else
          {
            myGLCD.print("  ", 188, 40);
            myGLCD.printNumI(t_alt, 220, 40);
          }
          if (ct==false)
          {
            ct=true;
            myGLCD.setColor(64, 64, 128);
            myGLCD.setFont(SmallFont);
            myGLCD.print("Changed!", 10, 10);
            myGLCD.setColor(0, 0, 0);
            myGLCD.setFont(BigFont);
          }
        }
        else if ((x>=220) && (x<=252))
        {
          buttonWait(220, 61);
          if(t_alt>=10)
            t_alt-=10;
         if (t_alt>=1000)
          {
            //myGLCD.printNumI(0, 170, 40);
            myGLCD.printNumI(t_alt, 188, 40);
          }
          else if (t_alt>=100)
          {
            myGLCD.print(" ", 188, 40);
            //myGLCD.printNumI(0, 170, 40);
            myGLCD.printNumI(t_alt, 204, 40);
          }
          else
          {
            myGLCD.print("  ", 188, 40);
            myGLCD.printNumI(t_alt, 220, 40);
          }
          if (ct==false)
          {
            ct=true;
            myGLCD.setColor(64, 64, 128);
            myGLCD.setFont(SmallFont);
            myGLCD.print("Changed!", 10, 10);
            myGLCD.setColor(0, 0, 0);
            myGLCD.setFont(BigFont);
          }
        }
//        else if ((x>=218) && (x<=250))
//        {
//          buttonWait(218, 61);
//          t_temp.sec-=1;
//          if (t_temp.sec==255)
//            t_temp.sec=59;
//          if (t_temp.sec<10)
//          {
//            myGLCD.printNumI(0, 218, 40);
//            myGLCD.printNumI(t_temp.sec, 234, 40);
//          }
//          else
//          {
//            myGLCD.printNumI(t_temp.sec, 218, 40);
//          }
//          if (ct==false)
//          {
//            ct=true;
//            myGLCD.setColor(64, 64, 128);
//            myGLCD.setFont(SmallFont);
//            myGLCD.print("Changed!", 10, 10);
//            myGLCD.setColor(0, 0, 0);
//            myGLCD.setFont(BigFont);
//          }
//        }
//      }
//      if ((y>=110) && (y<=135)) // Buttons: Date UP
//      {
//        if ((x>=122) && (x<=154))
//        {
//          buttonWait(122, 110);
//          t_temp.date+=1;
//          t_temp.date=validateDate(t_temp.date, t_temp.mon, t_temp.year);
//          if (t_temp.date<10)
//          {
//            myGLCD.printNumI(0, 122, 140);
//            myGLCD.printNumI(t_temp.date, 138, 140);
//          }
//          else
//          {
//            myGLCD.printNumI(t_temp.date, 122, 140);
//          }
//          if (cd==false)
//          {
//            cd=true;
//            myGLCD.setColor(64, 64, 128);
//            myGLCD.setFont(SmallFont);
//            myGLCD.print("Changed!", 10, 110);
//            myGLCD.setColor(0, 0, 0);
//            myGLCD.setFont(BigFont);
//          }
//        }
//        else if ((x>=170) && (x<=202))
//        {
//          buttonWait(170, 110);
//          t_temp.mon+=1;
//          if (t_temp.mon==13)
//            t_temp.mon=1;
//          if (t_temp.mon<10)
//          {
//            myGLCD.printNumI(0, 170, 140);
//            myGLCD.printNumI(t_temp.mon, 186, 140);
//          }
//          else
//          {
//            myGLCD.printNumI(t_temp.mon, 170, 140);
//          }
//          t_temp.date=validateDateForMonth(t_temp.date, t_temp.mon, t_temp.year);
//          if (cd==false)
//          {
//            cd=true;
//            myGLCD.setColor(64, 64, 128);
//            myGLCD.setFont(SmallFont);
//            myGLCD.print("Changed!", 10, 110);
//            myGLCD.setColor(0, 0, 0);
//            myGLCD.setFont(BigFont);
//          }
//        }
//        else if ((x>=218) && (x<=250))
//        {
//          buttonWait(234, 110);
//          t_temp.year+=1;
//          if (t_temp.year==2100)
//            t_temp.year=2000;
//          myGLCD.printNumI(t_temp.year, 218, 140);
//          t_temp.date=validateDateForMonth(t_temp.date, t_temp.mon, t_temp.year);
//          if (cd==false)
//          {
//            cd=true;
//            myGLCD.setColor(64, 64, 128);
//            myGLCD.setFont(SmallFont);
//            myGLCD.print("Changed!", 10, 110);
//            myGLCD.setColor(0, 0, 0);
//            myGLCD.setFont(BigFont);
//          }
//        }
//        t_temp.dow=calcDOW(t_temp.date, t_temp.mon, t_temp.year);
//        showDOW(t_temp.dow);
//      }
//      else if ((y>=161) && (y<=186)) // Buttons: Date DOWN
//      {
//        if ((x>=122) && (x<=154))
//        {
//          buttonWait(122, 161);
//          t_temp.date-=1;
//          t_temp.date=validateDate(t_temp.date, t_temp.mon, t_temp.year);
//          if (t_temp.date<10)
//          {
//            myGLCD.printNumI(0, 122, 140);
//            myGLCD.printNumI(t_temp.date, 138, 140);
//          }
//          else
//          {
//            myGLCD.printNumI(t_temp.date, 122, 140);
//          }
//          if (cd==false)
//          {
//            cd=true;
//            myGLCD.setColor(64, 64, 128);
//            myGLCD.setFont(SmallFont);
//            myGLCD.print("Changed!", 10, 110);
//            myGLCD.setColor(0, 0, 0);
//            myGLCD.setFont(BigFont);
//          }
//        }
//        else if ((x>=170) && (x<=202))
//        {
//          buttonWait(170, 161);
//          t_temp.mon-=1;
//          if (t_temp.mon==0)
//            t_temp.mon=12;
//          if (t_temp.mon<10)
//          {
//            myGLCD.printNumI(0, 170, 140);
//            myGLCD.printNumI(t_temp.mon, 186, 140);
//          }
//          else
//          {
//            myGLCD.printNumI(t_temp.mon, 170, 140);
//          }
//          t_temp.date=validateDateForMonth(t_temp.date, t_temp.mon, t_temp.year);
//          if (cd==false)
//          {
//            cd=true;
//            myGLCD.setColor(64, 64, 128);
//            myGLCD.setFont(SmallFont);
//            myGLCD.print("Changed!", 10, 110);
//            myGLCD.setColor(0, 0, 0);
//            myGLCD.setFont(BigFont);
//          }
//        }
//        else if ((x>=218) && (x<=250))
//        {
//          buttonWait(234, 161);
//          t_temp.year-=1;
//          if (t_temp.year==1999)
//            t_temp.year=2099;
//          myGLCD.printNumI(t_temp.year, 218, 140);
//          t_temp.date=validateDateForMonth(t_temp.date, t_temp.mon, t_temp.year);
//          if (cd==false)
//          {
//            cd=true;
//            myGLCD.setColor(64, 64, 128);
//            myGLCD.setFont(SmallFont);
//            myGLCD.print("Changed!", 10, 110);
//            myGLCD.setColor(0, 0, 0);
//            myGLCD.setFont(BigFont);
//          }
//        }
//        t_temp.dow=calcDOW(t_temp.date, t_temp.mon, t_temp.year);
//        showDOW(t_temp.dow);
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
  
  waitForTouchRelease();

  if (res==1)
  {
    if (ct)
      saveAltitude(t_alt);
//    if (cd)
//    {
//      rtc.setDate(t_temp.date, t_temp.mon, t_temp.year);
//      rtc.setDOW(t_temp.dow);
//    }
  }
  
  printMain();
}
