#include "RTClib.h"
#include <SPI.h>       
#include <DMD.h>    
#include <TimerOne.h>  
#include "SystemFont5x7.h"
#include "TimesNewRoman12.h"
#include "TimesNewRoman12b.h"
#include <Wire.h>
#include "RTClib.h"
#include "Font_6x14.h"  //-> This font only contains numbers from 0-9

#define DISPLAYS_ACROSS 1 //-> Number of P10 panels used, side to side.
#define DISPLAYS_DOWN 1
DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);

RTC_DS1307 rtc;  //-> RTC Declaration

//--------------------------------------------------------------------------------------Variables for time and date
int _day, _month, _year, _hour24, _hour12, _minute, _second, _dtw;
int hr24;
String st;
char nameoftheday[7][12] = { "MINGGU", "SENN", "SELASA", "RABU", "KAMIS", "JUMAT", "SABTU" };
char month_name[12][12] = { "JANUARI", "FEBRUARI", "MARET", "APRIL", "MEI", "JUNI", "JULI", "AGUSTUS", "SEPTEMBER", "OKTOBER", "NOVEMBER", "DESEMBER" };
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------Variable for Millis
const long interval = 1000;  //-> Retrieve time and date data every 1 second
unsigned long previousMillis = 0;

const long interval_for_date = 75;  //-> For scroll speed
unsigned long previousMillis_for_date = 0;
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------Variable to display hour, minute and second
char hr_24[3];
String str_hr_24;
char mn[3];
String str_mn;
char sc[3];
String str_sc;
//--------------------------------------------------------------------------------------
bool ret = false;
char *Text = "";

String s,sdis;
char c[1];
int p=0;

void ScanDMD() { 
  dmd.scanDisplayBySPI();
}

void setup(void) {
  Timer1.initialize(1000);          
  Timer1.attachInterrupt(ScanDMD);   
  dmd.clearScreen(true);   
  Serial.begin(115200);

  Serial.println("Arduino RTC DS1307");
  delay(1000);

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1)
      ;
  }

  if (!rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    
  }
  
  Timer1.initialize(1000);
  Timer1.attachInterrupt(ScanDMD);
  dmd.clearScreen(true);

}

void loop() {
  //_______millis() to display time
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis; //-> save the last time

    GetDateTime(); //-> Retrieve time and date data from DS1307

    //=====================================================Showing hour in P10
    dmd.selectFont(Font_6x14);
    
    str_hr_24=String(_hour24);
    str_hr_24.toCharArray(hr_24,3);

    if (_hour24<10) {
      dmd.drawString(1, 0, "0", 1, GRAPHICS_NORMAL);
      dmd.drawString(8, 0, hr_24, 1, GRAPHICS_NORMAL);
    }
    else {
      dmd.drawString(1, 0, hr_24, 2, GRAPHICS_NORMAL);
    }
    //=====================================================

    //=====================================================Showing ":" in P10
    dmd.drawFilledBox(16,3,17,4, GRAPHICS_NORMAL);
    dmd.drawFilledBox(16,11,17,12, GRAPHICS_NORMAL);
    //=====================================================

    //=====================================================Showing minute and second in P10
    dmd.selectFont(SystemFont5x7);
    
    str_mn=String(_minute);
    str_mn.toCharArray(mn,3);

    if (_minute<10) {
      dmd.drawString(20, 0, "0", 1, GRAPHICS_NORMAL);
      dmd.drawString(26, 0, mn, 1, GRAPHICS_NORMAL);
    }
    else {
      dmd.drawString(20, 0, mn, 2, GRAPHICS_NORMAL);
    }

    str_sc=String(_second);
    str_sc.toCharArray(sc,3);

    if (_second<10) {
      dmd.drawString(20, 9, "0", 1, GRAPHICS_NORMAL);
      dmd.drawString(26, 9, sc, 1, GRAPHICS_NORMAL);
    }
    else {
      dmd.drawString(20, 9, sc, 2, GRAPHICS_NORMAL);
    }
    //=====================================================

    //=====================================================Call the scrolling_date() subroutine to display the date.
    if (_second==11) { //-> Display the date when seconds equal to 11
      scrolling_date();
      scrolling_Text();

    }
    //=====================================================
  }
  //_______
}
//--------------------------------------------------------------------------------------

//------------------------------------------------------------------------Subroutine to retrieve or update the time and date from DS1307
void GetDateTime() {
  DateTime now = rtc.now();
  _day=now.day();
  _month=now.month();
  _year=now.year();
  _hour24=now.hour();
  _minute=now.minute();
  _second=now.second();
  _dtw=now.dayOfTheWeek();

  hr24=_hour24;
  if (hr24>12) {
    _hour12=hr24-12;
  }
  else if (hr24==0) {
    _hour12=12;
  }
  else {
    _hour12=hr24;
  }

  if (hr24<12) {
    st="AM";
  }
  else {
    st="PM"; 
  }  
}
//------------------------------------------------------------------------

//------------------------------------------------------------------------Subroutine to display days, months and years
void scrolling_date() {
  dmd.clearScreen(true);
  delay(100);

  //=====================================================Holds date data to display
  String Date = String(nameoftheday[_dtw]) + ", " + String(_day) + "-" + String(month_name[_month - 1]) + "-" + String(_year);
  char dt[50];
  Date.toCharArray(dt, 50);
  int i = 32 + 10;
  int j = strlen(dt) + (strlen(dt) * 5);
  //=====================================================

  dmd.selectFont(SystemFont5x7);

  while (1) {
    //_____________________________________________________millis() to display time
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;  //-> save the last time

      //=====================================================Showing the clock in P10
      str_hr_24 = String(_hour24);
      str_hr_24.toCharArray(hr_24, 3);

      if (_hour24 < 10) {
        dmd.drawString(2, 0, "0", 1, GRAPHICS_NORMAL);
        dmd.drawString(8, 0, hr_24, 1, GRAPHICS_NORMAL);
      } else {
        dmd.drawString(2, 0, hr_24, 2, GRAPHICS_NORMAL);
      }
      //=====================================================

      //=====================================================Showing ":" in P10
      GetDateTime();  //-> Retrieve time and date data from DS1307
      if (_second % 2 == 0) {
        dmd.drawString(14, 0, ":", 2, GRAPHICS_OR);
      } else {
        dmd.drawString(14, 0, ":", 2, GRAPHICS_NOR);
      }
      //=====================================================

      //=====================================================Showing minutes in P10
      str_mn = String(_minute);
      str_mn.toCharArray(mn, 3);

      if (_minute < 10) {
        dmd.drawString(19, 0, "0", 1, GRAPHICS_NORMAL);
        dmd.drawString(25, 0, mn, 1, GRAPHICS_NORMAL);
      } else {
        dmd.drawString(19, 0, mn, 2, GRAPHICS_NORMAL);
      }
      //=====================================================
    }
    //_____________________________________________________

    //_____________________________________________________millis() for display and scrolling date
    unsigned long currentMillis_for_date = millis();
    if (currentMillis_for_date - previousMillis_for_date >= interval_for_date) {
      previousMillis_for_date = currentMillis_for_date;  //-> save the last time

      i--;
      dmd.drawString(i, 9, dt, strlen(dt), GRAPHICS_NORMAL);
      if (i <= ~j) {
        dmd.clearScreen(true);
        delay(100);
        return;
      }
    }
  }
}

void jam() {
  dmd.selectFont(Font_6x14);

  str_hr_24 = String(_hour24);
  str_hr_24.toCharArray(hr_24, 3);

  if (_hour24 < 10) {
    dmd.drawString(1, 0, "0", 1, GRAPHICS_NORMAL);
    dmd.drawString(8, 0, hr_24, 1, GRAPHICS_NORMAL);
  } else {
    dmd.drawString(1, 0, hr_24, 2, GRAPHICS_NORMAL);
  }
}
void menit() {
  dmd.selectFont(SystemFont5x7);

  str_mn = String(_minute);
  str_mn.toCharArray(mn, 3);

  if (_minute < 10) {
    dmd.drawString(20, 0, "0", 1, GRAPHICS_NORMAL);
    dmd.drawString(26, 0, mn, 1, GRAPHICS_NORMAL);
  } else {
    dmd.drawString(20, 0, mn, 2, GRAPHICS_NORMAL);
  }

  str_sc = String(_second);
  str_sc.toCharArray(sc, 3);

  if (_second < 10) {
    dmd.drawString(20, 9, "0", 1, GRAPHICS_NORMAL);
    dmd.drawString(26, 9, sc, 1, GRAPHICS_NORMAL);
  } else {
    dmd.drawString(20, 9, sc, 2, GRAPHICS_NORMAL);
  }
}

void scrolling_Text() {
  dmd.selectFont(System5x7);
  
  //----------------------------------------------
  s="XII";
  p=0;
  
  for (int i=8; i <= 25; i=i+6) {
    sdis = s.substring(p,p+1);
    sdis.toCharArray(c,2);
    for (int j=-7; j <= 0; j++) {
      dmd.drawString(i,j, c, 1, GRAPHICS_NORMAL);
      delay(100);
    }
    p++;
  }
  //----------------------------------------------
  
  delay(100);
  
  //----------------------------------------------
  s="MIPA3";
  p=0;
  
  for (int i=1; i <= 25; i=i+6) {
    sdis = s.substring(p,p+1);
    sdis.toCharArray(c,2);
    for (int j=17; j >= 9; j--) {
      dmd.drawString(i,j, c, 1, GRAPHICS_NORMAL);
      delay(50);
    }
    p++;
  }
  //----------------------------------------------
  
  delay(1000);
  
  //----------------------------------------------
  for (int i=0; i >= -7; i--) {
    dmd.drawString(8,i, "XII ", 25, GRAPHICS_NORMAL);
    delay(50);
  }
  //----------------------------------------------
  
  delay(100);
  
  //----------------------------------------------
  for (int i=9; i <= 17; i++) {
    dmd.drawString(1,i, "MIPA3", 5, GRAPHICS_NORMAL);
    delay(50);
    dmd.clearScreen(true);
  }
  //----------------------------------------------
    dmd.selectFont(TimesNewRoman12);
  Text = "KELOMPOK 2";
  dmd.drawMarquee(Text,strlen(Text),(32*DISPLAYS_ACROSS)-1,2);
  while(!ret){
    ret=dmd.stepMarquee(-1,0);
    delay(100);
  }
  ret=false;
  //----------------------------------------------

  delay(500);
  
  //----------------------------------------------
  dmd.selectFont(TimesNewRoman12b);
  Text = "SMAN 2 JONGGOL";
  dmd.drawMarquee(Text,strlen(Text),(32*DISPLAYS_ACROSS)-1,2);
  while(!ret){
    ret=dmd.stepMarquee(-1,0);
    delay(100);
  }
  ret=false;
  //----------------------------------------------
  
  delay(1000);
  dmd.clearScreen(true); 
}