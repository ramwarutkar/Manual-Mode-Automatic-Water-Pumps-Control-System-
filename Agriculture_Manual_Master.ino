#include <Wire.h>
#include <EEPROM.h>
#include <DS3231.h>
DS3231  rtc(SDA, SCL);
Time  t;
int Hor;
int Min;
int Sec;
int Date;
int Month;
int Year;
int Day;

static int previous;
static int next;

boolean setting_rtc_time = true;
boolean alternate = false;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  rtc.begin();
  setting_time();
}

void loop() {

  time_value();
  next = Date;
  Serial.print("Next - ");
  Serial.println(next);
  alternate_true_flase_zone_1();
  time_display();
  commonregisters();

}

void settings_exe()
{

  if (EEPROM.read(0) == 0)
  {
    if (EEPROM.read(84) == 1)   // For Normal Days
    {

    }
    else if (EEPROM.read(85) == 1) // For Alternate
    {
      if (alternate == true)       // To know when is alternate days
      {
        if (EEPROM.read(8) == Hor && EEPROM.read(7) == Min && EEPROM.read(6) == Sec) // ON Time
        {
          Serial.print("PUMP ZONE 1 ON");
        }
        else if (EEPROM.read(11) == Hor && EEPROM.read(10) == Min && EEPROM.read(9) == Sec)  // Off Time
        {
          Serial.print("PUMP ZONE 1 OFF");
        }
      }
    }
    else if (EEPROM.read(86) == 1)  // For Number of Days
    {

    }
  }

  if (EEPROM.read(1) == 0)
  {
    if (EEPROM.read(87) == 1)      // For Normal Days
    {

    }
    else if (EEPROM.read(88) == 1)  // For Alternate Days
    {
      if (EEPROM.read(21) == Hor && EEPROM.read(20) == Min && EEPROM.read(19) == Sec)
      {
        Serial.println("PUMP ZONE 2 ON");
      }
      else if (EEPROM.read(24) == Hor && EEPROM.read(23) == Min && EEPROM.read(22))
      {
        Serial.println("PUMP ZONE 2 OFF");
      }
    }
    else if (EEPROM.read(89) == 1)   // For Number of Days
    {

    }
  }

  if (EEPROM.read(2) == 0)
  {

  }
  if (EEPROM.read(3) == 0)
  {

  }
  if (EEPROM.read(4) == 0)
  {

  }
  if (EEPROM.read(5) == 0)
  {

  }
}






void time_value()
{
  t = rtc.getTime();
  Hor = t.hour;
  Min = t.min;
  Sec = t.sec;
  Date = t.date;
  Month = t.mon;
  Year = t.year;
  Day = t.dow;
}

void time_display()
{
  Serial.print(rtc.getDOWStr());
  Serial.print(" ");
  Serial.print(rtc.getDateStr());
  Serial.print(" -- ");
  Serial.println(rtc.getTimeStr());
}

void alternate_true_flase_zone_1()
{
  if (previous != next)
  {
    alternate = true;
    previous = next;
    Serial.print("Alternate True - ");
    Serial.println(alternate);
  }
  else
  {
    alternate = false;
    Serial.print("Alternate False - ");
    Serial.println(alternate);
  }
}

void setting_time()
{
  static int set_time = 1;
  time_value();
  if (set_time = 1)
  {
    rtc.setTime(EEPROM.read(103), EEPROM.read(104), EEPROM.read(105));
    rtc.setDate(EEPROM.read(108), EEPROM.read(107), 2000 + EEPROM.read(106));
    switch (EEPROM.read(102))
    {
      case 1:
        {
          rtc.setDOW(MONDAY);
          break;
        }
      case 2:
        {
          rtc.setDOW(TUESDAY);
          break;
        }
      case 3:
        {
          rtc.setDOW(WEDNESDAY);
          break;
        }
      case 4:
        {
          rtc.setDOW(THURSDAY);
          break;
        }
      case 5:
        {
          rtc.setDOW(FRIDAY);
          break;
        }
      case 6:
        {
          rtc.setDOW(SATURDAY);
          break;
        }
      case 7:
        {
          rtc.setDOW(SUNDAY);
          break;
        }
      default: break;
    }
    set_time = 0;
  }
  Serial.println("");
  Serial.print("Date/Time Set to -- ");
  Serial.print(rtc.getDOWStr());
  Serial.print(" ");
  Serial.print(rtc.getDateStr());
  Serial.print(" -- ");
  Serial.println(rtc.getTimeStr());
  previous = Date;
  Serial.println("Previous Value - ");
  Serial.println(previous);
}

void commonregisters()
{
  static int register_address = 0;
  static int start_bit = 0;
  Wire.requestFrom(5, 1);
  while (Wire.available())
  {
    int c = Wire.read();
    if (start_bit == 0)
    {
      if (c == 255)
      {
        start_bit = 1;
        Serial.print("Start Bit Received  - ");
        Serial.println(start_bit);
      }
    }
    else if (start_bit == 1)
    {
      EEPROM.write(register_address, c);
      Serial.print("Register Address - ");
      Serial.println(register_address);
      Serial.print("EEPROM Value - ");
      Serial.println(EEPROM.read(register_address));
      register_address++;
      if (register_address == 109)
      {
        register_address = 0;
        start_bit = 0;
      }
    }
  }
}
