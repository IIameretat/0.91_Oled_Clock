/* This code works with DS1307 RTC with OLED display
 - It displays the time and date in a 12h format then Year
 - displaying of am(Sun)/pm(Moon) and blinking ":"
 - displaying day of week upside the display
 - displaying second right side of the display
 - Refer to https://discord.gg/h8ZQkpgWBv for more details
*/

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include <Fonts/FreeSans9pt7b.h>

#include "APM.h"
#include "Clock.h"
#define buzzer 2

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)

const char* dayName[7] = {
 "S", "M", "T", "W", "T", "F", "S"};

const char* monthName[12] = {
 "Jan", "Feb", "Mar", "Apr", "May", "Jun",  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

bool a,points;
int  x = 0;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //Declaring the display name (display)
tmElements_t tm;

bool getTime(const char *str)
{
  int Hour, Min, Sec;

  if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3) return false;
  tm.Hour = Hour;
  tm.Minute = Min;
  tm.Second = Sec;
  return true;
}

bool getDate(const char *str)
{
  char Month[12];
  int Day, Year;
  uint8_t monthIndex;

  if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3) return false;
  for (monthIndex = 0; monthIndex < 12; monthIndex++) {
    if (strcmp(Month, monthName[monthIndex]) == 0) break;
  }
  if (monthIndex >= 12) return false;
  tm.Day = Day;
  tm.Month = monthIndex + 1;
  tm.Year = CalendarYrToTm(Year);
  return true;
}

void setup() {
  Wire.begin();
  Serial.begin(115200);
  delay(1000);
    
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //Start the OLED display
  display.clearDisplay();
  display.display();

  bool parse=false;
  bool config=false;

  // get the date and time the compiler was run
  if (getDate(__DATE__) && getTime(__TIME__)) {
    parse = true;
    // and configure the RTC with this info
    if (RTC.write(tm)) {
      config = true;
    }
  }

  int w = ((tm.Day + (2.6 * (tm.Month)) - (0.2)) - (2*19) + 22 + (22/4) + (20/4));
  int mod = w/7;
  int dy = (w - (mod * 7));

  Serial.print (" Day: ");
  Serial.println (dy);  
  Serial.print (" Mod: ");
  Serial.println (mod);  
  Serial.print (" W: ");
  Serial.println (w);
  Serial.print ("\n\n");
  
  Serial.print (" Time: ");
  Serial.print (tm.Hour);
  Serial.print (":");
  Serial.print (tm.Minute);
  Serial.print (":");
  Serial.println (tm.Second);
  
  Serial.print (" Date: ");
  Serial.print (tm.Day);
  Serial.print (", ");
  Serial.print (tm.Month);
  Serial.print (", 2022");
}

void loop() {
  display.setFont();
  RTC.read(tm);
  x = tm.Second%2;                      //Dividing the seconds value by 2 and store the rest value, it's either 0 or 1
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(0,8);
  //ZeroH();                  
  ampm(tm.Hour);
  SecArm();
      
  display.setTextSize(3);
  display.print(tm.Minute);
  display.setTextSize(1);
  display.setCursor(55,0);

  if(a==true)
  //display.print(" AM");
  display.drawBitmap(99, 0, image_Sun, 20, 9, 1);
  if(a==false)
  //display.print(" PM");
  display.drawBitmap(99, 0, image_Moon, 20, 9, 1);
  
  int  w = ((tm.Day + (2.6 * (tm.Month)) - (0.2)) - (2*19) + 22 + (22/4) + (20/4));
  int  mod = w/7;
  int dy = (w - (mod * 7));

  for (int i = 0; i<=7; i++) {
    //display.drawPixel((i*12)+1, 0, WHITE);        // Day of Week Section
  }
  
  for (int j = 0; j <= 3; j++) {                    // Sec Section
  display.drawPixel(127, (j*15)+1, WHITE);
  }
  
  display.drawLine(127, 32-(tm.Second/2), 127, 32-(tm.Second/2)-4, WHITE);         // Second Arm
  
  display.setTextSize(1);
  display.drawLine((dy*10)+2, 0, (dy*10)+10, 0, WHITE);
  
  //display.setCursor((dy*11)+4,0);
  //display.print(dayName[dy]);
  //display.print(dy);

  display.drawLine(91, 1, 91, 31, WHITE);
  display.setCursor(96,14);
  display.print(tm.Day);  
  display.setCursor(96,23);
  display.print(monthName[tm.Month-1]);
  
  /*display.setCursor(99,26);
  display.setTextSize(1);
  display.print(tmYearToCalendar(tm.Year));*/
  dimdisplay();
  ClockRing();
  display.display();
  display.clearDisplay();
}

void ZeroH(){
  if(tm.Hour <= 21 && tm.Hour >= 13) {
    display.print("0");
  }
  else if(tm.Hour <= 9 && tm.Hour >= 1){
    display.print("0");
  }
}

void ZeroM(){
  if(tm.Minute >= 0 && tm.Minute <= 9) {
    display.print("0");
    //display.print(tm.Hour);
  }
}
 
void ampm(int Hour){               //ampm function takes the hour value 0-23
if(Hour==0){                       //if it's 0 it becomes 12
  Hour=12;
  a=true;                          //true if it's am, false otherwise
  display.print(Hour);
}
else if(Hour>0 && Hour<12){
  a=true;                          //normal value
  display.print(Hour);
}
else if(Hour==12){
  a=false;                         //changing to pm
  display.print(Hour);
}
else if(Hour>12){
  Hour=Hour-12;                    //substract 12 to bring it back to 1,2,3,...
  a=false;
  display.print(Hour);
 }
}

void dimdisplay(){ 
  if(tm.Hour >= 21) {
    display.dim(true);
  }
  else if(tm.Hour <= 5) {
    display.dim(true);
  }
  else {
    display.dim(false);
  }
}

void SecArm () {
  if (tm.Hour == 0 or tm.Hour > 9) {
     if(x==0) {                       //if x=0 we don't display the ":"
      display.print(" ");
      display.fillCircle(43, 15, 2, WHITE);
    }
    
     if(x==1) {                       //if x=1 we display it, so for every 1s we display or not the ":"
      display.print(" ");
      display.fillCircle(43, 21, 2, WHITE);
     }
    }
    else {
      if(x==0) {                       //if x=0 we don't display the ":"
      display.print(" ");
      display.fillCircle(38, 15, 2, WHITE);
    }
    
      if(x==1) {                       //if x=1 we display it, so for every 1s we display or not the ":"
      display.print(" ");
      display.fillCircle(38, 21, 2, WHITE);
     }
    }
}

void Buzzer () {
  if (tm.Minute == 0 && tm.Second <10) {
    digitalWrite(buzzer, HIGH);
    delay(200);
    digitalWrite(buzzer, LOW);
    delay(400);
  }
}
  
void ClockRing () {
  int frame_delay = 24;
  double px = 2.8;
  int py = 1;
  if (tm.Minute == 0 && tm.Second <10) {
    display.clearDisplay();
    display.drawBitmap((1*px)-22, py, epd_bitmap_gif_0, 30, 30, 1);
    display.display();
    delay(frame_delay);
    display.clearDisplay();
    display.drawBitmap((2*px)-18, py, epd_bitmap_gif_1, 30, 30, 1);
    display.display();
    delay(frame_delay);
    display.clearDisplay();
    display.drawBitmap((3*px)-16, py, epd_bitmap_gif_2, 30, 30, 1);
    display.display();
    delay(frame_delay);
    display.clearDisplay();
    display.drawBitmap((4*px)-14, py, epd_bitmap_gif_3, 30, 30, 1);
    display.display();
    delay(frame_delay);
    display.clearDisplay();
    display.drawBitmap((5*px)-12, py, epd_bitmap_gif_4, 30, 30, 1);
    display.display();
    delay(frame_delay);
    display.clearDisplay();
    display.drawBitmap((6*px)-10, py, epd_bitmap_gif_5, 30, 30, 1);
    display.display();
    delay(frame_delay);
    display.clearDisplay();
    display.drawBitmap((7*px)-8, py, epd_bitmap_gif_6, 30, 30, 1);
    display.display();
    delay(frame_delay);
    display.clearDisplay();
    display.drawBitmap((8*px)-6, py, epd_bitmap_gif_7, 30, 30, 1);
    display.display();
    delay(frame_delay);
    display.clearDisplay();
    display.drawBitmap((9*px)-4, py, epd_bitmap_gif_8, 30, 30, 1);
    display.display();
    delay(frame_delay);
    display.clearDisplay();
    display.drawBitmap((10*px)-2, py, epd_bitmap_gif_9, 30, 30, 1);
    display.display();
    delay(frame_delay);
    display.clearDisplay();
    display.drawBitmap((11*px)+2, py, epd_bitmap_gif_10, 30, 30, 1);
    display.display();
    delay(frame_delay);
    display.clearDisplay();
    display.drawBitmap((12*px)+4, py, epd_bitmap_gif_11, 30, 30, 1);
    display.display();
    delay(frame_delay);
    display.clearDisplay();
    display.drawBitmap((13*px)+6, py, epd_bitmap_gif_12, 30, 30, 1);
    display.display();
    delay(frame_delay);
    display.clearDisplay();
    display.drawBitmap((14*px)+8, py, epd_bitmap_gif_13, 30, 30, 1);
    display.display();
    delay(frame_delay);
    display.clearDisplay();
    display.drawBitmap((15*px)+10, py, epd_bitmap_gif_14, 30, 30, 1);
    display.display();
    delay(frame_delay);
    display.clearDisplay();
    display.drawBitmap((16*px)+12, py, epd_bitmap_gif_15, 30, 30, 1);
    display.display();
    delay(frame_delay);
    display.clearDisplay();
    display.drawBitmap((17*px)+14, py, epd_bitmap_gif_16, 30, 30, 1);
    display.display();
    delay(frame_delay);
    display.clearDisplay();
    display.drawBitmap((18*px)+16, py, epd_bitmap_gif_17, 30, 30, 1);
    display.display();
    delay(frame_delay);
    display.clearDisplay();
    display.drawBitmap((19*px)+18, py, epd_bitmap_gif_18, 30, 30, 1);
    display.display();
    delay(frame_delay);
    display.clearDisplay();
    display.drawBitmap((20*px)+20, py, epd_bitmap_gif_19, 30, 30, 1);
    display.display();
    delay(frame_delay);
    display.clearDisplay();
    display.drawBitmap((21*px)+22, py, epd_bitmap_gif_20, 30, 30, 1);
    display.display();
    delay(frame_delay);
    display.clearDisplay();
    display.drawBitmap((22*px)+24, py, epd_bitmap_gif_21, 30, 30, 1);
    display.display();
    delay(frame_delay);
    display.clearDisplay();
    display.drawBitmap((23*px)+26, py, epd_bitmap_gif_22, 30, 30, 1);
    display.display();
    delay(frame_delay);
    display.clearDisplay();
    display.drawBitmap((24*px)+28, py, epd_bitmap_gif_23, 30, 30, 1);
    display.display();
    delay(frame_delay);
    display.clearDisplay();
    display.drawBitmap((25*px)+30, py, epd_bitmap_gif_24, 30, 30, 1);
    display.display();
    delay(frame_delay);
    display.clearDisplay();
    display.drawBitmap((26*px)+32, py, epd_bitmap_gif_25, 30, 30, 1);
    display.display();
    delay(frame_delay);
    display.clearDisplay();
    display.drawBitmap((27*px)+34, py, epd_bitmap_gif_26, 30, 30, 1);
    display.display();
    delay(frame_delay);
    display.clearDisplay();
    display.drawBitmap((28*px)+36, py, epd_bitmap_gif_27, 30, 30, 1);
    display.display();
    delay(frame_delay);
  }
}
