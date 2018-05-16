//i2c Slave(Uno)
#include <Wire.h>
#include <EEPROM.h>
#include <SPFD5408_Adafruit_GFX.h>
#include <SPFD5408_Adafruit_TFTLCD.h>
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4
#define WHITE   0x0000
#define YELLOW  0x001F
#define BLUE    0xF800
#define GREEN   0x07E0
#define RED     0x07FF
#define PINK    0xF81F
#define CYAN    0xFFE0
#define BLACK   0xFFFF
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

const int previous_but = 22;
const int decrement_but = 24;
const int increment_but = 26;
const int next_but = 28;
// Variablese
static int main_page_print = 1;
static int switch_no = 2;
int but_read = 0;
int setup_set_time = 1;
static int setup_switch_day = 1;
static int page_no = 2;
int i = 0;  // No of Available Zones
static int zone_traverse_no = 1 ;
int cursor_x = 0;
static int on_sec  = 0 ;
static int on_min  = 0 ;
static int on_hour  = 0 ;
static int off_sec  = 0 ;
static int off_min  = 0 ;
static int off_hour  = 0 ;
static int zone_select[6] = {};
static int days_select  = 1;
static int view_zone_select = 1;
static int zone_1 = 0; static int zone_2 = 0; static int zone_3 = 0; static int zone_4 = 0; static int zone_5 = 0; static int zone_6 = 0;
static int pattern_days = 0; static int pattern_alternate = 0; static int pattern_for = 0;
static int switch_pattern = 1;
static int sunday = 0; static int monday = 0; static int tuesday = 0; static int wednesday = 0; static int thursday = 0; static int friday = 0; static int saturday = 0;



void setup()
{
  Serial.begin(9600);
  Wire.begin(5);
  Wire.onRequest(commonregisters);
  tft.reset();
  tft.begin(0x9341);
  tft.setRotation(1);
  tft.fillScreen(WHITE);
  pinMode(previous_but, 22);
  pinMode(decrement_but, 24);
  pinMode(increment_but, 26);
  pinMode(next_but, 28);
  EEPROM.write(0, 1), EEPROM.write(1, 1); EEPROM.write(2, 1); EEPROM.write(3, 1); EEPROM.write(4, 1); EEPROM.write(5, 1);
  int setup_print = 0;
  roll();
  delay(2000);
  do
  {
    if (setup_print == 0)
    {
      clear_lcd();
      setup_page();
      setup_print = 1;
    }
    button_read();
    setup_switch_function();
  } while (setup_set_time == 1);
  Serial.println("I am Out");
}

void loop()   // LOOOOOOOOOOOOOOOOOOOOOOOOPPPPPPPPPPPPPPPPPPPP
{
  static int setup_sec, setup_min, setup_hour, setup_date, setup_month;
  static int setup_year = 2018;
  static int setup_switch = 1;
  if (main_page_print == 1)
  {
    clear_lcd();
    tft.setCursor(5, 10);
    tft.setTextColor(BLACK);
    tft.setTextSize(2);
    tft.print("SELECT OPTION -");
    tft.setCursor(50, 90);
    tft.setTextColor(BLACK);
    tft.setTextSize(2);
    tft.print("ZONE ACTIVITY");
    tft.setCursor(50, 140);
    tft.setTextColor(BLACK);
    tft.setTextSize(2);
    tft.print("VIEW ZONE ACTIVITY");
    main_page_print = 0;
  }
  button_read();
  if (but_read == -2) // Previous
  {
    switch_no = switch_no - 2;
    clear_lcd();
    Serial.println(switch_no);
  }
  else if (but_read == -1 ) // Decrement
  {
    switch_no  = 2;
    Serial.println(switch_no);
  }
  else if (but_read == +1) // Increment
  {
    switch_no = 3;
    Serial.println(switch_no);
  }
  else if (but_read == +2) // Next
  {
    switch_no = switch_no + 2;
    clear_lcd();
    Serial.println(switch_no);
  }

  switch (switch_no)
  {
    case 2:    // Arrow Zone Activity
      {
        tft.fillRect(0, 140, 22, 20, WHITE);
        tft.setCursor(0, 90);
        tft.setTextColor(RED);
        tft.setTextSize(2);
        tft.print(">>");
        break;
      }
    case 3:   // Arrow View Zone Activity
      {
        tft.fillRect(0, 90, 22, 20, WHITE);
        tft.setCursor(0, 140);
        tft.setTextColor(RED);
        tft.setTextSize(2);
        tft.print(">>");
        break;
      }
    case 4:    // Zone Activity  -> Select Zone
      {
        i = 0;
        cursor_x = 0;
        zone_display();
        zone_traverse_no = 1 ;
        do
        {
          zone_traverse();
        } while (switch_no == 4);
        break;
      }
    case 5:     // View Zone Activity  -> Select Zone
      {
        view_zone_print();
        do
        {
          view_select_zone();
        } while (switch_no == 5);
        break;
      }
    case 6:  // Zone Activity  -> Select Zone  -> ON/OFF Time
      {
        on_off_time_print();
        do
        {
          on_off_set();
        } while (switch_no == 6);
        break;
      }
    case 7:  // View Zone Activity  -> Select Zone  -> View Selected Zone
      {
        do
        {
          view_details();
        } while (switch_no == 7);
        break;
      }
    case 8:  // Zone Activity  -> Select Zone  -> ON/OFF Time -> Days Pattern
      {
        switch_pattern = 1;
        days_pattern_print();
        do
        {
          pattern_select();
        } while (switch_no == 8);
        break;
      }
    default: break;
  }
}
// **************************************  Config Device  ****************************
void roll()
{
  tft.setCursor(100, 75);
  tft.setTextColor(BLACK);
  tft.setTextSize(4);
  tft.print("SAMOM");
  tft.setCursor(17, 115);
  tft.setTextColor(BLACK);
  tft.setTextSize(4);
  tft.print("TECHNOLOGIES");
}
void setup_page()
{
  tft.setCursor(0, 5);
  tft.setTextColor(RED);
  tft.setTextSize(3);
  tft.print("CONFIG DEVICE:-");
  tft.setCursor(0, 70);
  tft.setTextColor(RED);
  tft.setTextSize(3);
  tft.print("TIME -");
  tft.setCursor(120, 70);
  tft.setTextColor(RED);
  tft.setTextSize(3);
  tft.print("00/00/00");
  tft.setCursor(0, 120);
  tft.setTextColor(RED);
  tft.setTextSize(3);
  tft.print("DATE -");
  tft.setCursor(120, 120);
  tft.setTextColor(RED);
  tft.setTextSize(3);
  tft.print("1 /1 /2018");
  tft.setCursor(0, 170);
  tft.setTextColor(RED);
  tft.setTextSize(3);
  tft.print("DAY  -");
  tft.setCursor(120, 170);
  tft.setTextColor(RED);
  tft.setTextSize(3);
  tft.print("MONDAY");
  tft.setCursor(140, 220);
  tft.setTextColor(RED);
  tft.setTextSize(2);
  tft.print("NEXT");
  tft.drawRect(130, 217, 60, 20, BLACK);

}
void setup_switch_function()
{
  static int setup_sec, setup_min, setup_hour, setup_date, setup_month;
  static int setup_year = 2018;
  static int setup_switch = 1;
  if (but_read == -2 && setup_switch >= 2)
  {
    setup_switch--;
    Serial.println(setup_switch);
  }
  else if (but_read == +2 && setup_switch <= 8)
  {
    setup_switch++;
    Serial.println(setup_switch);
  }
  switch (setup_switch)
  {
    case 1:
      {
        tft.fillRect(184, 93, 20, 10, WHITE);
        tft.setCursor(130, 80);
        tft.setTextColor(RED);
        tft.setTextSize(3);
        tft.print("_");
        if (but_read == -1 && setup_hour >= 1)
        {
          setup_hour--;
          tft.fillRect(120, 70, 39, 25, WHITE);
          tft.setCursor(122, 70);
          tft.setTextColor(RED);
          tft.setTextSize(3);
          tft.print(setup_hour);
          Serial.println("Setup Hour -");
          Serial.println(setup_hour);
        }
        else if (but_read == +1 && setup_hour <= 22)
        {
          setup_hour++;
          tft.fillRect(120, 70, 39, 25, WHITE);
          tft.setCursor(122, 70);
          tft.setTextColor(RED);
          tft.setTextSize(3);
          tft.print(setup_hour);
          Serial.println("Setup Hour -");
          Serial.println(setup_hour);
        }
        break;
      }
    case 2:
      {
        tft.fillRect(130, 93, 20, 10, WHITE);
        tft.fillRect(238, 93, 20, 10, WHITE);
        tft.setCursor(184, 80);
        tft.setTextColor(RED);
        tft.setTextSize(3);
        tft.print("_");

        if (but_read == -1 && setup_min >= 1)
        {
          setup_min--;
          tft.fillRect(170, 65, 39, 27, WHITE);
          tft.setCursor(175, 70);
          tft.setTextColor(RED);
          tft.setTextSize(3);
          tft.print(setup_min);
          Serial.println("Setup Min -");
          Serial.println(setup_min);
        }
        else if (but_read == +1 && setup_min <= 58)
        {
          setup_min++;
          tft.fillRect(170, 65, 39, 27, WHITE);
          tft.setCursor(175, 70);
          tft.setTextColor(RED);
          tft.setTextSize(3);
          tft.print(setup_min);
          Serial.println("Setup Min -");
          Serial.println(setup_min);
        }
        break;
      }
    case 3:
      {
        tft.fillRect(184, 93, 20, 10, WHITE);
        tft.fillRect(122, 143, 20, 10, WHITE);
        tft.setCursor(238, 80);
        tft.setTextColor(RED);
        tft.setTextSize(3);
        tft.print("_");
        if (but_read == -1 && setup_sec >= 1)
        {
          setup_sec--;
          tft.fillRect(225, 65, 39, 27, WHITE);
          tft.setCursor(228, 70);
          tft.setTextColor(RED);
          tft.setTextSize(3);
          tft.print(setup_sec);
          Serial.println("Setup Sec -");
          Serial.println(setup_sec);
        }
        else if (but_read == +1 && setup_sec <= 58)
        {
          setup_sec++;
          tft.fillRect(225, 65, 39, 27, WHITE);
          tft.setCursor(228, 70);
          tft.setTextColor(RED);
          tft.setTextSize(3);
          tft.print(setup_sec);
          Serial.println("Setup Sec -");
          Serial.println(setup_sec);
        }
        break;
      }
    case 4:
      {
        tft.fillRect(238, 93, 20, 10, WHITE);
        tft.fillRect(176, 143, 20, 10, WHITE);
        tft.setCursor(123, 130);
        tft.setTextColor(RED);
        tft.setTextSize(3);
        tft.print("_");
        if (but_read == -1 && setup_date >= 2)
        {
          setup_date--;
          tft.fillRect(110, 120, 43, 25, WHITE);
          tft.setCursor(120, 120);
          tft.setTextColor(RED);
          tft.setTextSize(3);
          tft.print(setup_date);
          Serial.println("Setup Date -");
          Serial.println(setup_date);
        }
        else if (but_read == +1 && setup_date <= 30)
        {
          setup_date++;
          tft.fillRect(110, 120, 43, 25, WHITE);
          tft.setCursor(120, 120);
          tft.setTextColor(RED);
          tft.setTextSize(3);
          tft.print(setup_date);
          Serial.println("Setup Date -");
          Serial.println(setup_date);
        }
        break;
      }
    case 5:
      {
        tft.fillRect(122, 143, 20, 10, WHITE);
        tft.fillRect(230, 143, 80, 10, WHITE);
        tft.setCursor(179, 130);
        tft.setTextColor(RED);
        tft.setTextSize(3);
        tft.print("_");
        if (but_read == -1 && setup_month >= 2)
        {
          setup_month--;
          tft.fillRect(172, 120, 38, 25, WHITE);
          tft.setCursor(173, 120);
          tft.setTextColor(RED);
          tft.setTextSize(3);
          tft.print(setup_month);
          Serial.println("Setup Month -");
          Serial.println(setup_month);
        }
        else if (but_read == +1 && setup_month <= 11)
        {
          setup_month++;
          tft.fillRect(172, 120, 38, 25, WHITE);
          tft.setCursor(173, 120);
          tft.setTextColor(RED);
          tft.setTextSize(3);
          tft.print(setup_month);
          Serial.println("Setup Month -");
          Serial.println(setup_month);
        }
        break;
      }
    case 6:
      {
        tft.fillRect(176, 143, 20, 10, WHITE);
        tft.fillRect(122, 196, 120, 10, WHITE);
        tft.setCursor(230, 130);
        tft.setTextColor(RED);
        tft.setTextSize(3);
        tft.print("____");
        if (but_read == -1 && setup_year >= 2019)
        {
          setup_year--;
          tft.fillRect(227, 120, 70, 25, WHITE);
          tft.setCursor(227, 120);
          tft.setTextColor(RED);
          tft.setTextSize(3);
          tft.print(setup_year);
          Serial.println("Setup Year -");
          Serial.println(setup_year);
        }
        else if (but_read == +1 && setup_year <= 2050)
        {
          setup_year++;
          tft.fillRect(227, 120, 70, 25, WHITE);
          tft.setCursor(227, 120);
          tft.setTextColor(RED);
          tft.setTextSize(3);
          tft.print(setup_year);
          Serial.println("Setup Year -");
          Serial.println(setup_year);
        }
        break;
      }
    case 7:
      {
        tft.fillRect(130, 217, 60, 20, WHITE);
        tft.setCursor(140, 220);
        tft.setTextColor(RED);
        tft.setTextSize(2);
        tft.print("NEXT");
        tft.drawRect(130, 217, 60, 20, BLACK);

        tft.fillRect(230, 143, 80, 10, WHITE);
        tft.setCursor(122, 180);
        tft.setTextColor(RED);
        tft.setTextSize(3);
        tft.print("______");
        if (but_read == -1 && setup_switch_day >= 2)
        {
          setup_switch_day--;
          Serial.println("setup_switch_day");
          Serial.println(setup_switch_day);
        }
        else if (but_read == +1 && setup_switch_day <= 6)
        {
          setup_switch_day++;
          Serial.println("setup_switch_day");
          Serial.println(setup_switch_day);
        }
        switch (setup_switch_day)
        {
          case 1:
            {
              tft.fillRect(120, 170, 170, 23, WHITE);
              tft.setCursor(120, 170);
              tft.setTextColor(RED);
              tft.setTextSize(3);
              tft.print("MONDAY");
              break;
            }
          case 2:
            {
              tft.fillRect(120, 170, 170, 23, WHITE);
              tft.setCursor(120, 170);
              tft.setTextColor(RED);
              tft.setTextSize(3);
              tft.print("TUESDAY");
              break;
            }
          case 3:
            {
              tft.fillRect(120, 170, 170, 23, WHITE);
              tft.setCursor(120, 170);
              tft.setTextColor(RED);
              tft.setTextSize(3);
              tft.print("WEDNESDAY");
              break;
            }
          case 4:
            {
              tft.fillRect(120, 170, 170, 23, WHITE);
              tft.setCursor(120, 170);
              tft.setTextColor(RED);
              tft.setTextSize(3);
              tft.print("THURSDAY");
              break;
            }
          case 5:
            {
              tft.fillRect(120, 170, 170, 23, WHITE);
              tft.setCursor(120, 170);
              tft.setTextColor(RED);
              tft.setTextSize(3);
              tft.print("FRIDAY");
              break;
            }
          case 6:
            {
              tft.fillRect(120, 170, 170, 23, WHITE);
              tft.setCursor(120, 170);
              tft.setTextColor(RED);
              tft.setTextSize(3);
              tft.print("SATURDAY");
              break;
            }
          case 7:
            {
              tft.fillRect(120, 170, 170, 23, WHITE);
              tft.setCursor(120, 170);
              tft.setTextColor(RED);
              tft.setTextSize(3);
              tft.print("SUNDAY");
              break;
            }
          default: break;
        }
        break;
      }
    case 8:
      {
        tft.fillRect(122, 196, 120, 10, WHITE);
        tft.fillRect(130, 217, 60, 20, RED);
        tft.setCursor(140, 220);
        tft.setTextColor(BLACK);
        tft.setTextSize(2);
        tft.print("NEXT");
        break;
      }
    case 9:
      {
        clear_lcd();
        switch (setup_switch_day)
        {
          case 1:
            {
              //rtc.setDOW(MONDAY);
              EEPROM.write(102, 1);
              break;
            }
          case 2:
            {
              //rtc.setDOW(TUESDAY);
              EEPROM.write(102, 2);
              break;
            }
          case 3:
            {
              //rtc.setDOW(WEDNESDAY);
              EEPROM.write(102, 3);
              break;
            }
          case 4:
            {
              //rtc.setDOW(THURSDAY);
              EEPROM.write(102, 4);
              break;
            }
          case 5:
            {
              //rtc.setDOW(FRIDAY);
              EEPROM.write(102, 5);
              break;
            }
          case 6:
            {
              //rtc.setDOW(SATURDAY);
              EEPROM.write(102, 6);
              break;
            }
          case 7:
            {
              //rtc.setDOW(SUNDAY);
              EEPROM.write(102, 7);
              break;
            }
          default: break;
        }
        //rtc.setTime(setup_hour, setup_min, setup_sec);
        //rtc.setDate(setup_date, setup_month, setup_year);
        EEPROM.write(103, setup_hour);
        EEPROM.write(104, setup_min);
        EEPROM.write(105, setup_sec);
        EEPROM.write(106, setup_year - 2000);
        EEPROM.write(107, setup_month);
        EEPROM.write(108, setup_date);
        tft.setCursor(10, 10);
        tft.setTextColor(BLACK);
        tft.setTextSize(2);
        tft.print("SAVING.......");
        setup_set_time = 0;
        delay(1000);
        break;
      }
    default: break;
  }
}
// **************************************  Config Device  ****************************

// ************************************ Zone Select Functions ********************************
void zone_display()
{
  // No of Zones = 6 || EEPROM (0 - 5) || EEPROM 1 = AVAILABLE, EEPROM 0 = NOT AVAILABLE
  if (EEPROM.read(0) != 1 && EEPROM.read(1) != 1 && EEPROM.read(2) != 1 && EEPROM.read(3) != 1 && EEPROM.read(4) != 1 && EEPROM.read(5) != 1)
  {
    tft.fillScreen(WHITE);
    tft.setCursor(50, 80);
    tft.setTextColor(BLACK);
    tft.setTextSize(2);
    tft.print("No Zone Available");
    delay(2000);
    clear_lcd();
    main_page_print = 1;
    switch_no = 2;
  }
  else
  {
    tft.setCursor(0, 0);
    tft.setTextColor(RED);
    tft.setTextSize(3);
    tft.print("Select Zone - ");
    tft.setCursor(10, 130);
    tft.setTextColor(BLACK);
    tft.setTextSize(3);
    tft.print("-");
    i = zone_available(i);
    Serial.print("i -");
    Serial.println(i);
    Serial.println("Zone Select Array");
    Serial.print("Element 0 -");
    Serial.println(zone_select[0]);
    Serial.print("Element 1 -");
    Serial.println(zone_select[1]);
    Serial.print("Element 2 -");
    Serial.println(zone_select[2]);
    Serial.print("Element 3 -");
    Serial.println(zone_select[3]);
    Serial.print("Element 4 -");
    Serial.println(zone_select[4]);
    Serial.print("Element 5 -");
    Serial.println(zone_select[5]);
    Serial.print("Element 6 -");
    Serial.println(zone_select[6]);
  }
}
int zone_available(int i)  // To Display Available Zones Sequentially
{
  int cursor_x = 0;
  int cursor_y = 100;

  if (EEPROM.read(0) == 1)
  {
    Serial.println(EEPROM.read(0));
    tft.setCursor(cursor_x = cursor_x + 10, cursor_y = cursor_y);
    tft.setTextColor(BLACK);
    tft.setTextSize(3);
    tft.print("Z1");
    i = i + 1;
    zone_select[i] = 1;
  }
  if (EEPROM.read(1) == 1)
  {
    Serial.println(EEPROM.read(1));
    tft.setCursor(cursor_x = cursor_x + 50, cursor_y = cursor_y);
    tft.setTextColor(BLACK);
    tft.setTextSize(3);
    tft.print("Z2");
    i = i + 1;
    zone_select[i] = 2;
  }
  if (EEPROM.read(2) == 1)
  {
    Serial.println(EEPROM.read(2));
    tft.setCursor(cursor_x = cursor_x + 50, cursor_y = cursor_y);
    tft.setTextColor(BLACK);
    tft.setTextSize(3);
    tft.print("Z3");
    i = i + 1;
    zone_select[i] = 3;
  }
  if (EEPROM.read(3) == 1)
  {
    Serial.println(EEPROM.read(3));
    tft.setCursor(cursor_x = cursor_x + 50, cursor_y = cursor_y);
    tft.setTextColor(BLACK);
    tft.setTextSize(3);
    tft.print("Z4");
    i = i + 1;
    zone_select[i] = 4;
  }
  if (EEPROM.read(4) == 1)
  {
    Serial.println(EEPROM.read(4));
    tft.setCursor(cursor_x = cursor_x + 50, cursor_y = cursor_y);
    tft.setTextColor(BLACK);
    tft.setTextSize(3);
    tft.print("Z5");
    i = i + 1;
    zone_select[i] = 5;
  }
  if (EEPROM.read(5) == 1)
  {
    Serial.println(EEPROM.read(5));
    tft.setCursor(cursor_x = cursor_x + 50, cursor_y = cursor_y);
    tft.setTextColor(BLACK);
    tft.setTextSize(3);
    tft.print("Z6");
    i = i + 1;
    zone_select[i] = 6;
  }
  delay(100);
  return i;
}
void zone_traverse()
{
  button_read();
  static int zone_rect_select = 0;
  if (but_read == -2 ) // Previous
  {
    if (zone_traverse_no == 1)
    {
      switch_no = switch_no - 2;
      main_page_print = 1;
      clear_lcd();
      Serial.println("Switch No -");
      Serial.println(switch_no);
    }
    else
    {
      zone_traverse_no--;
      cursor_x = cursor_x - 52;
      zone_rect_select = cursor_x;
      tft.setCursor(cursor_x, 130);
      tft.setTextColor(BLACK);
      tft.setTextSize(3);
      tft.print("-");
      tft.fillRect(cursor_x + 45, 132, 30 , 20, WHITE);
      delay(500);
      Serial.println("Zone Traverse Number -");
      Serial.println(zone_traverse_no);
      Serial.println("Curosor_x - ");
      Serial.println(cursor_x);

    }
  }
  else if (but_read == +2) // Next
  {
    if (zone_traverse_no == i)
    {
      switch_no = switch_no + 2;
      clear_lcd();
      Serial.println("Switch No -");
      Serial.println(switch_no);
    }
    else
    {
      zone_traverse_no++;
      cursor_x = cursor_x + 55;
      zone_rect_select = cursor_x;
      tft.setCursor(cursor_x, 130);
      tft.setTextColor(BLACK);
      tft.setTextSize(3);
      tft.print("-");
      tft.fillRect(cursor_x - 55, 132, 30 , 20, WHITE);
      delay(500);
      Serial.println("Zone Traverse Number -");
      Serial.println(zone_traverse_no);
      Serial.println("Curosor_x - ");
      Serial.println(cursor_x);
    }
  }

  switch (zone_select[zone_traverse_no])
  {
    case 1:
      {
        if (but_read == +1)
        {
          Serial.println("Zone 1 Selected");
          tft.setCursor(zone_rect_select - 3, 70);
          tft.setTextColor(BLACK);
          tft.setTextSize(3);
          tft.print("-");
          zone_1 = 1;
        }
        else if (but_read == -1)
        {
          Serial.println("Zone 1 Deselected");
          tft.fillRect(zone_rect_select - 3, 72, 30 , 20, BLACK);
          zone_1 = 0;
        }
        break;
      }
    case 2:
      {
        if (but_read == +1)
        {
          Serial.println("Zone 2 Selected");
          tft.setCursor(zone_rect_select - 3, 70);
          tft.setTextColor(BLACK);
          tft.setTextSize(3);
          tft.print("-");
          zone_2 = 1;
        }
        else if (but_read == -1)
        {
          Serial.println("Zone 2 Deselected");
          tft.fillRect(zone_rect_select - 3, 72, 30 , 20, BLACK);
          zone_2 = 0;
        }
        break;
      }
    case 3:
      {
        if (but_read == +1)
        {
          Serial.println("Zone 3 Selected");
          tft.setCursor(zone_rect_select - 3, 70);
          tft.setTextColor(BLACK);
          tft.setTextSize(3);
          tft.print("-");
          zone_3 = 1;
        }
        else if (but_read == -1)
        {
          Serial.println("Zone 3 Deselected");
          tft.fillRect(zone_rect_select - 3, 72, 30 , 20, BLACK);
          zone_3 = 0;
        }
        break;
      }
    case 4:
      {
        if (but_read == +1)
        {
          Serial.println("Zone 4 Selected");
          tft.setCursor(zone_rect_select - 3, 70);
          tft.setTextColor(BLACK);
          tft.setTextSize(3);
          tft.print("-");
          zone_4 = 1;
        }
        else if (but_read == -1)
        {
          Serial.println("Zone 4 Deselected");
          tft.fillRect(zone_rect_select - 3, 72, 30 , 20, BLACK);
          zone_4 = 0;
        }
        break;
      }
    case 5:
      {
        if (but_read == +1)
        {
          Serial.println("Zone 5 Selected");
          tft.setCursor(zone_rect_select - 3, 70);
          tft.setTextColor(BLACK);
          tft.setTextSize(3);
          tft.print("-");
          zone_5 = 1;
        }
        else if (but_read == -1)
        {
          Serial.println("Zone 5 Deselected");
          tft.fillRect(zone_rect_select - 3, 72, 30 , 20, BLACK);
          zone_5 = 0;
        }
        break;
      }
    case 6:
      {
        if (but_read == +1)
        {
          Serial.println("Zone 6 Selected");
          tft.setCursor(zone_rect_select - 3, 70);
          tft.setTextColor(BLACK);
          tft.setTextSize(3);
          tft.print("-");
          zone_6 = 1;
        }
        else if (but_read == -1)
        {
          Serial.println("Zone 6 Deselected");
          tft.fillRect(zone_rect_select - 3, 72, 30 , 20, BLACK);
          zone_6 = 0;
        }
        break;
      }
    default: break;
  }
}

// ************************************ Zone Select Functions ********************************
// ******************************** ON/OFF Time **********************************
void on_off_time_print()
{
  tft.setCursor(0, 0);
  tft.setTextColor(BLACK);
  tft.setTextSize(3);
  tft.print("Set ON OFF Time - ");
  tft.setCursor(0, 70);
  tft.setTextColor(BLACK);
  tft.setTextSize(3);
  tft.print("ON -");
  tft.setCursor(120, 70);
  tft.setTextColor(BLACK);
  tft.setTextSize(3);
  tft.print("00/00/00");
  tft.setCursor(0, 120);
  tft.setTextColor(BLACK);
  tft.setTextSize(3);
  tft.print("OFF -");
  tft.setCursor(120, 120);
  tft.setTextColor(BLACK);
  tft.setTextSize(3);
  tft.print("00/00/00");
}
void on_off_set()
{
  static int switch_on_off = 1;
  switch (switch_on_off)
  {
    case 1:
      {
        tft.fillRect(184, 93, 20, 10, WHITE);
        tft.setCursor(130, 80);
        tft.setTextColor(BLACK);
        tft.setTextSize(3);
        tft.print("_");
        if (but_read == -1 && on_hour >= 1)
        {
          on_hour--;
          tft.fillRect(120, 70, 39, 25, WHITE);
          tft.setCursor(122, 70);
          tft.setTextColor(BLACK);
          tft.setTextSize(3);
          tft.print(on_hour);
          Serial.println("On Hour -");
          Serial.println(on_hour);
        }
        else if (but_read == +1 && on_hour <= 22)
        {
          on_hour++;
          tft.fillRect(120, 70, 39, 25, WHITE);
          tft.setCursor(122, 70);
          tft.setTextColor(BLACK);
          tft.setTextSize(3);
          tft.print(on_hour);
          Serial.println("On Hour -");
          Serial.println(on_hour);
        }
        break;
      }
    case 2:
      {
        tft.fillRect(130, 93, 20, 10, WHITE);
        tft.fillRect(238, 93, 20, 10, WHITE);
        tft.setCursor(184, 80);
        tft.setTextColor(BLACK);
        tft.setTextSize(3);
        tft.print("_");

        if (but_read == -1 && on_min >= 1)
        {
          on_min--;
          tft.fillRect(170, 65, 39, 27, WHITE);
          tft.setCursor(175, 70);
          tft.setTextColor(BLACK);
          tft.setTextSize(3);
          tft.print(on_min);
          Serial.println("On Min -");
          Serial.println(on_min);
        }
        else if (but_read == +1 && on_min <= 58)
        {
          on_min++;
          tft.fillRect(170, 65, 39, 27, WHITE);
          tft.setCursor(175, 70);
          tft.setTextColor(BLACK);
          tft.setTextSize(3);
          tft.print(on_min);
          Serial.println("On Min -");
          Serial.println(on_min);
        }
        break;
      }
    case 3:
      {
        tft.fillRect(184, 93, 20, 10, WHITE);
        tft.fillRect(122, 143, 20, 10, WHITE);
        tft.setCursor(238, 80);
        tft.setTextColor(BLACK);
        tft.setTextSize(3);
        tft.print("_");
        if (but_read == -1 && on_sec >= 1)
        {
          on_sec--;
          tft.fillRect(225, 65, 39, 27, WHITE);
          tft.setCursor(228, 70);
          tft.setTextColor(BLACK);
          tft.setTextSize(3);
          tft.print(on_sec);
          Serial.println("On Sec -");
          Serial.println(on_sec);
        }
        else if (but_read == +1 && on_sec <= 58)
        {
          on_sec++;
          tft.fillRect(225, 65, 39, 27, WHITE);
          tft.setCursor(228, 70);
          tft.setTextColor(BLACK);
          tft.setTextSize(3);
          tft.print(on_sec);
          Serial.println("On Sec -");
          Serial.println(on_sec);
        }
        break;
      }
    case 4:
      {
        tft.fillRect(238, 93, 20, 10, WHITE);
        tft.fillRect(176, 143, 20, 10, WHITE);
        tft.setCursor(123, 130);
        tft.setTextColor(BLACK);
        tft.setTextSize(3);
        tft.print("_");
        if (but_read == -1 && off_hour >= 1)
        {
          off_hour--;
          tft.fillRect(110, 120, 43, 25, WHITE);
          tft.setCursor(120, 120);
          tft.setTextColor(BLACK);
          tft.setTextSize(3);
          tft.print(off_hour);
          Serial.println("Off Hour -");
          Serial.println(off_hour);
        }
        else if (but_read == +1 && off_hour <= 30)
        {
          off_hour++;
          tft.fillRect(110, 120, 43, 25, WHITE);
          tft.setCursor(120, 120);
          tft.setTextColor(BLACK);
          tft.setTextSize(3);
          tft.print(off_hour);
          Serial.println("Off Hour -");
          Serial.println(off_hour);
        }
        break;
      }
    case 5:
      {
        tft.fillRect(122, 143, 20, 10, WHITE);
        tft.fillRect(230, 143, 80, 10, WHITE);
        tft.setCursor(179, 130);
        tft.setTextColor(BLACK);
        tft.setTextSize(3);
        tft.print("_");
        if (but_read == -1 && off_min >= 1)
        {
          off_min--;
          tft.fillRect(172, 120, 38, 25, WHITE);
          tft.setCursor(173, 120);
          tft.setTextColor(BLACK);
          tft.setTextSize(3);
          tft.print(off_min);
          Serial.println("Off Min -");
          Serial.println(off_min);
        }
        else if (but_read == +1 && off_min <= 58)
        {
          off_min++;
          tft.fillRect(172, 120, 38, 25, WHITE);
          tft.setCursor(173, 120);
          tft.setTextColor(BLACK);
          tft.setTextSize(3);
          tft.print(off_min);
          Serial.println("Off Min-");
          Serial.println(off_min);
        }
        break;
      }
    case 6:
      {
        tft.fillRect(176, 143, 20, 10, WHITE);
        tft.fillRect(122, 196, 120, 10, WHITE);
        tft.setCursor(230, 130);
        tft.setTextColor(BLACK);
        tft.setTextSize(3);
        tft.print("_");
        if (but_read == -1 && off_sec >= 1)
        {
          off_sec--;
          tft.fillRect(227, 120, 70, 25, WHITE);
          tft.setCursor(227, 120);
          tft.setTextColor(BLACK);
          tft.setTextSize(3);
          tft.print(off_sec);
          Serial.println("Off Sec -");
          Serial.println(off_sec);
        }
        else if (but_read == +1 && off_sec <= 58)
        {
          off_sec++;
          tft.fillRect(227, 120, 70, 25, WHITE);
          tft.setCursor(227, 120);
          tft.setTextColor(BLACK);
          tft.setTextSize(3);
          tft.print(off_sec);
          Serial.println("Off Sec -");
          Serial.println(off_sec);
        }
        break;
      }
    default: break;
  }
  button_read();
  if (but_read == -2) // Previous
  {
    if (switch_on_off == 1)
    {
      switch_no = switch_no - 2;
      clear_lcd();
      Serial.println(switch_no);
    }
    else
    {
      switch_on_off--;
      Serial.println("Switch On/Off-");
      Serial.println(switch_on_off);
    }
  }
  else if (but_read == +2) // Next
  {
    if (switch_on_off == 6)
    {
      switch_no = switch_no + 2;
      clear_lcd();
      Serial.print("Switch No/ On-Off - ");
      Serial.println(switch_no);
    }
    else
    {
      switch_on_off++;
      Serial.println("Switch On/Off-");
      Serial.println(switch_on_off);
    }
  }
}

// ******************************** ON/OFF Time **********************************

//  *********************************  Pattern Days ******************************
void days_pattern_print()
{
  tft.setCursor(0, 0);
  tft.setTextColor(BLACK);
  tft.setTextSize(3);
  tft.print("Days Pattern - ");
  tft.setCursor(40, 50);
  tft.setTextColor(BLACK);
  tft.setTextSize(2);
  tft.print("By Days ");
  tft.setCursor(40, 90);
  tft.setTextColor(RED);
  tft.setTextSize(2);
  tft.print("S M T W Th Fr Sa");
  tft.setCursor(40, 130);
  tft.setTextColor(BLACK);
  tft.setTextSize(2);
  tft.print("Alternate Days");
  tft.setCursor(40, 180);
  tft.setTextColor(BLACK);
  tft.setTextSize(2);
  tft.print("For No. of Days ");
  tft.setCursor(0, 50);
  tft.setTextColor(BLACK);
  tft.setTextSize(2);
  tft.print(">>");
}
void pattern_select()
{
  switch (switch_pattern)
  {
    case 1:                             //  S M T W Th Fr Sa
      {
        tft.fillRect(0, 120, 30, 30, WHITE);
        tft.setCursor(0, 50);
        tft.setTextColor(BLACK);
        tft.setTextSize(2);
        tft.print(">>");
        Serial.print("Switch Pattern - Days - ");
        Serial.println(switch_pattern);
        break;
      }
    case 2:                             // Alternate
      {
        tft.fillRect(0, 40, 30, 30, WHITE);
        tft.fillRect(0, 170, 30, 30, WHITE);
        tft.setCursor(0, 130);
        tft.setTextColor(BLACK);
        tft.setTextSize(2);
        tft.print(">>");
        Serial.print("Switch Pattern - Alternate - ");
        Serial.println(switch_pattern);
        break;
      }
    case 3:                        // For no. of Days
      {
        tft.fillRect(0, 120, 30, 30, WHITE);
        tft.setCursor(0, 180);
        tft.setTextColor(BLACK);
        tft.setTextSize(2);
        tft.print(">>");
        Serial.print("Switch Pattern - For No. - ");
        Serial.println(switch_pattern);
        break;
      }
    case 4:                 // Select Days
      {
        pattern_days_select();
      }
    case 5:
      {
        saving_data();
        break;
      }
    case 6:
      {
        saving_data();
        break;
      }
    default: break;
  }
  button_read();
  if (but_read == -2) // Previous
  {
    if (switch_pattern == 4  && days_select == 1)
    {
      switch_no = switch_no - 2;
      clear_lcd();
      Serial.println(switch_no);
    }
  }
  else if (but_read == +2)  // Next
  {
    if (switch_pattern == 1)    // Dayss
    {
      switch_pattern = 4;
      Serial.print("switch_pattern - ");
      Serial.println(switch_pattern);
    }
    else if (switch_pattern == 2)  // Alternate Days
    {
      switch_pattern = 5;
      pattern_alternate = 1;
      Serial.print("switch_pattern -");
      Serial.println(switch_pattern);
    }
    else if (switch_pattern == 3)   // For No. of Days
    {
      switch_pattern = 6;
      pattern_for = 1;
      Serial.print("switch_pattern -");
      Serial.println(switch_pattern);
    }
  }
  else if (switch_pattern != 4)
  {
    if (but_read == -1 && switch_pattern >= 1)  // Decrement
    {
      switch_pattern--;
    }
    else if (but_read == +1 && switch_pattern <= 2)  // Increment
    {
      switch_pattern++;
    }
  }
}
void pattern_days_select()
{
  button_read();
  if (but_read == -2 && days_select >= 1)  // Decrement
  {
    days_select--;
    delay(100);
  }
  else if (but_read == +2 && days_select <= 7)  // Increment
  {
    days_select++;
    delay(100);
  }
  switch (days_select)
  {
    case 1:
      {
        tft.fillRect(64, 114, 12, 5, WHITE);
        tft.setCursor(40, 110);
        tft.setTextColor(RED);
        tft.setTextSize(2);
        tft.print("-");
        Serial.print("days_select -");
        Serial.println(days_select);
        button_read();
        if (but_read == -1)
        {
          tft.setCursor(40, 70);
          tft.setTextColor(WHITE);
          tft.setTextSize(2);
          tft.print("_");
          sunday = 0;
        }
        else if (but_read == +1)
        {
          tft.setCursor(40, 70);
          tft.setTextColor(RED);
          tft.setTextSize(2);
          tft.print("_");
          sunday = 1;
        }
        break;
      }
    case 2:
      {
        tft.fillRect(40, 114, 12, 5, WHITE);
        tft.fillRect(87, 114, 12, 5, WHITE);
        tft.setCursor(64, 110);
        tft.setTextColor(RED);
        tft.setTextSize(2);
        tft.print("-");
        button_read();
        if (but_read == -1)
        {
          tft.setCursor(64, 70);
          tft.setTextColor(WHITE);
          tft.setTextSize(2);
          tft.print("_");
          monday = 0;
        }
        else if (but_read == +1)
        {
          tft.setCursor(64, 70);
          tft.setTextColor(RED);
          tft.setTextSize(2);
          tft.print("_");
          monday = 1;
        }
        Serial.print("days_select -");
        Serial.println(days_select);
        break;
      }
    case 3:
      {
        tft.fillRect(64, 114, 12, 5, WHITE);
        tft.fillRect(110, 114, 12, 5, WHITE);
        tft.setCursor(87, 110);
        tft.setTextColor(RED);
        tft.setTextSize(2);
        tft.print("-");
        button_read();
        if (but_read == -1)
        {
          tft.setCursor(87, 70);
          tft.setTextColor(WHITE);
          tft.setTextSize(2);
          tft.print("_");
          tuesday = 0;
        }
        else if (but_read == +1)
        {
          tft.setCursor(87, 70);
          tft.setTextColor(RED);
          tft.setTextSize(2);
          tft.print("_");
          tuesday = 1;
        }
        Serial.print("days_select -");
        Serial.println(days_select);
        break;
      }
    case 4:
      {
        tft.fillRect(87, 114, 12, 5, WHITE);
        tft.fillRect(140, 114, 12, 5, WHITE);
        tft.setCursor(110, 110);
        tft.setTextColor(RED);
        tft.setTextSize(2);
        tft.print("-");
        if (but_read == -1)
        {
          tft.setCursor(110, 70);
          tft.setTextColor(WHITE);
          tft.setTextSize(2);
          tft.print("_");
          wednesday = 0;
        }
        else if (but_read == +1)
        {
          tft.setCursor(110, 70);
          tft.setTextColor(RED);
          tft.setTextSize(2);
          tft.print("_");
          wednesday = 1;
        }
        Serial.print("days_select -");
        Serial.println(days_select);
        break;
      }
    case 5:
      {
        tft.fillRect(110, 114, 12, 5, WHITE);
        tft.fillRect(178, 114, 12, 5, WHITE);
        tft.setCursor(140, 110);
        tft.setTextColor(RED);
        tft.setTextSize(2);
        tft.print("-");
        if (but_read == -1)
        {
          tft.setCursor(140, 70);
          tft.setTextColor(WHITE);
          tft.setTextSize(2);
          tft.print("_");
          thursday = 0;
        }
        else if (but_read == +1)
        {
          tft.setCursor(140, 70);
          tft.setTextColor(RED);
          tft.setTextSize(2);
          tft.print("_");
          thursday = 1;
        }
        Serial.print("days_select -");
        Serial.println(days_select);
        Serial.print("days_select -");
        Serial.println(days_select);
        break;
      }
    case 6:
      {
        tft.fillRect(140, 114, 12, 5, WHITE);
        tft.fillRect(212, 114, 12, 5, WHITE);
        tft.setCursor(178, 110);
        tft.setTextColor(RED);
        tft.setTextSize(2);
        tft.print("-");
        if (but_read == -1)
        {
          tft.setCursor(178, 70);
          tft.setTextColor(WHITE);
          tft.setTextSize(2);
          tft.print("_");
          friday = 0;
        }
        else if (but_read == +1)
        {
          tft.setCursor(178, 70);
          tft.setTextColor(RED);
          tft.setTextSize(2);
          tft.print("_");
          friday = 1;
        }
        Serial.print("days_select -");
        Serial.println(days_select);
        break;
      }
    case 7:
      {
        tft.fillRect(178, 114, 12, 5, WHITE);
        tft.setCursor(212, 110);
        tft.setTextColor(RED);
        tft.setTextSize(2);
        tft.print("-");
        if (but_read == -1)
        {
          tft.setCursor(212, 70);
          tft.setTextColor(RED);
          tft.setTextSize(2);
          tft.print("_");
          saturday = 0;
        }
        else if (but_read == +1)
        {
          tft.setCursor(212, 70);
          tft.setTextColor(WHITE);
          tft.setTextSize(2);
          tft.print("_");
          saturday = 1;
        }
        Serial.print("days_select -");
        Serial.println(days_select);
        break;
      }
    case 8:
      {
        days_select = 1;
        Serial.println("I am in");
        saving_data();
        break;
      }
    default: break;
  }
}
// ************************************************************************************
// **********************************  Saving Funtion  ********************************
void saving_data()
{
  clear_lcd();
  tft.setCursor(70, 110);
  tft.setTextColor(RED);
  tft.setTextSize(2);
  tft.print("Saving ......");
  if (zone_1 == 1)           // For Zone 1  (On time EEPROM - 6,7,8) || (Off time EEPROM - 9,10,11) ||(S M T W Th Fr Sa - 12,13,14,15,16,17,18) || (Zone_1 EEPROM - 0)
  { //  (Pattern - EEPROM - 84 = pattern_days || EEPROM - 85 = pattern_alternate || EEPROM - 86 = pattern_for)
    EEPROM.write(0, 0);
    if (pattern_days == 1)
    {
      EEPROM.write(84, 1);
      EEPROM.write(6, on_sec); EEPROM.write(7, on_min); EEPROM.write(8, on_hour);    // On Time EEPROM Save
      EEPROM.write(9, off_sec); EEPROM.write(10, off_min); EEPROM.write(11, off_hour); // Off Time EEPROM Save
      EEPROM.write(12, sunday); EEPROM.write(12, monday); EEPROM.write(12, tuesday); EEPROM.write(12, wednesday); EEPROM.write(12, thursday); EEPROM.write(12, friday); EEPROM.write(12, saturday);
    }
    else if  (pattern_alternate == 1)
    {
      EEPROM.write(85, 1);
      EEPROM.write(6, on_sec); EEPROM.write(7, on_min); EEPROM.write(8, on_hour);    // On Time EEPROM Save
      EEPROM.write(9, off_sec); EEPROM.write(10, off_min); EEPROM.write(11, off_hour); // Off Time EEPROM Save
    }
    else if (pattern_for == 1)
    {
      EEPROM.write(86, 1);
      EEPROM.write(6, on_sec); EEPROM.write(7, on_min); EEPROM.write(8, on_hour);    // On Time EEPROM Save
      EEPROM.write(9, off_sec); EEPROM.write(10, off_min); EEPROM.write(11, off_hour); // Off Time EEPROM Save
    }
  }
  if (zone_2 == 1)        // For Zone 2 (On time EEPROM - 19,20,21) || (Off time EEPROM - 22,23,24) ||(S M T W Th Fr Sa - 25,26,27,28,29,30,31)
  { //  (Pattern - EEPROM - 87 = pattern_days || EEPROM - 88 = pattern_alternate || EEPROM - 89 = pattern_for)
    EEPROM.write(1, 0);
    if (pattern_days == 1)
    {
      EEPROM.write(87, 1);
      EEPROM.write(19, on_sec); EEPROM.write(20, on_min); EEPROM.write(21, on_hour);    // On Time EEPROM Save
      EEPROM.write(22, off_sec); EEPROM.write(23, off_min); EEPROM.write(24, off_hour); // Off Time EEPROM Save
      EEPROM.write(25, sunday); EEPROM.write(26, monday); EEPROM.write(27, tuesday); EEPROM.write(28, wednesday); EEPROM.write(29, thursday); EEPROM.write(30, friday); EEPROM.write(31, saturday);
    }
    else if  (pattern_alternate == 1)
    {
      EEPROM.write(88, 1);
      EEPROM.write(19, on_sec); EEPROM.write(20, on_min); EEPROM.write(21, on_hour);    // On Time EEPROM Save
      EEPROM.write(22, off_sec); EEPROM.write(23, off_min); EEPROM.write(24, off_hour); // Off Time EEPROM Save
    }
    else if (pattern_for == 1)
    {
      EEPROM.write(89, 1);
      EEPROM.write(19, on_sec); EEPROM.write(20, on_min); EEPROM.write(21, on_hour);    // On Time EEPROM Save
      EEPROM.write(22, off_sec); EEPROM.write(23, off_min); EEPROM.write(24, off_hour); // Off Time EEPROM Save
    }
  }
  if (zone_3 == 1)      // For Zone 3 (On time EEPROM - 32,33,34) || (Off time EEPROM - 35,36,37) ||(S M T W Th Fr Sa - 38,39,40,41,42,43,44)
  { //  (Pattern - EEPROM - 90 = pattern_days || EEPROM - 91 = pattern_alternate || EEPROM - 92 = pattern_for)
    EEPROM.write(2, 0);
    if (pattern_days = 1)
    {
      EEPROM.write(90, 1);
      EEPROM.write(32, on_sec); EEPROM.write(33, on_min); EEPROM.write(34, on_hour);    // On Time EEPROM Save
      EEPROM.write(35, off_sec); EEPROM.write(36, off_min); EEPROM.write(37, off_hour); // Off Time EEPROM Save
      EEPROM.write(38, sunday); EEPROM.write(39, monday); EEPROM.write(40, tuesday); EEPROM.write(41, wednesday); EEPROM.write(42, thursday); EEPROM.write(43, friday); EEPROM.write(44, saturday);
    }
    else if  (pattern_alternate == 1)
    {
      EEPROM.write(91, 1);
      EEPROM.write(32, on_sec); EEPROM.write(33, on_min); EEPROM.write(34, on_hour);    // On Time EEPROM Save
      EEPROM.write(35, off_sec); EEPROM.write(36, off_min); EEPROM.write(37, off_hour); // Off Time EEPROM Save
    }
    else if (pattern_for == 1)
    {
      EEPROM.write(92, 1);
      EEPROM.write(32, on_sec); EEPROM.write(33, on_min); EEPROM.write(34, on_hour);    // On Time EEPROM Save
      EEPROM.write(35, off_sec); EEPROM.write(36, off_min); EEPROM.write(37, off_hour); // Off Time EEPROM Save

    }
  }
  if (zone_4 == 1)        // For Zone 4 (On time EEPROM - 45,46,47) || (Off time EEPROM - 48,49,50) ||(S M T W Th Fr Sa - 51,52,53,54,55,56,57)
  { //  (Pattern - EEPROM - 93 = pattern_days || EEPROM - 94 = pattern_alternate || EEPROM - 95 = pattern_for)
    EEPROM.write(3, 0);
    if (pattern_days = 1)
    {
      EEPROM.write(93, 1);
      EEPROM.write(45, on_sec); EEPROM.write(46, on_min); EEPROM.write(47, on_hour);    // On Time EEPROM Save
      EEPROM.write(48, off_sec); EEPROM.write(49, off_min); EEPROM.write(50, off_hour); // Off Time EEPROM Save
      EEPROM.write(51, sunday); EEPROM.write(52, monday); EEPROM.write(53, tuesday); EEPROM.write(54, wednesday); EEPROM.write(55, thursday); EEPROM.write(56, friday); EEPROM.write(57, saturday);
    }
    else if  (pattern_alternate == 1)
    {
      EEPROM.write(94, 1);
      EEPROM.write(45, on_sec); EEPROM.write(46, on_min); EEPROM.write(47, on_hour);    // On Time EEPROM Save
      EEPROM.write(48, off_sec); EEPROM.write(49, off_min); EEPROM.write(50, off_hour); // Off Time EEPROM Save
    }
    else if (pattern_for == 1)
    {
      EEPROM.write(95, 1);
      EEPROM.write(45, on_sec); EEPROM.write(46, on_min); EEPROM.write(47, on_hour);    // On Time EEPROM Save
      EEPROM.write(48, off_sec); EEPROM.write(49, off_min); EEPROM.write(50, off_hour); // Off Time EEPROM Save
    }
  }
  if (zone_5 == 1)        // For Zone 5 (On time EEPROM - 58,59,60) || (Off time EEPROM - 61,62,63) ||(S M T W Th Fr Sa - 64,65,66,67,68,69,70)
  { //  (Pattern - EEPROM - 96 = pattern_days || EEPROM - 97 = pattern_alternate || EEPROM - 98 = pattern_for)
    EEPROM.write(4, 0);
    if (pattern_days = 1)
    {
      EEPROM.write(96, 1);
      EEPROM.write(58, on_sec); EEPROM.write(59, on_min); EEPROM.write(60, on_hour);    // On Time EEPROM Save
      EEPROM.write(61, off_sec); EEPROM.write(62, off_min); EEPROM.write(63, off_hour); // Off Time EEPROM Save
      EEPROM.write(64, sunday); EEPROM.write(65, monday); EEPROM.write(66, tuesday); EEPROM.write(67, wednesday); EEPROM.write(68, thursday); EEPROM.write(69, friday); EEPROM.write(70, saturday);
    }
    else if  (pattern_alternate == 1)
    {
      EEPROM.write(97, 1);
      EEPROM.write(58, on_sec); EEPROM.write(59, on_min); EEPROM.write(60, on_hour);    // On Time EEPROM Save
      EEPROM.write(61, off_sec); EEPROM.write(62, off_min); EEPROM.write(63, off_hour); // Off Time EEPROM Save
    }
    else if (pattern_for == 1)
    {
      EEPROM.write(98, 1);
      EEPROM.write(58, on_sec); EEPROM.write(59, on_min); EEPROM.write(60, on_hour);    // On Time EEPROM Save
      EEPROM.write(61, off_sec); EEPROM.write(62, off_min); EEPROM.write(63, off_hour); // Off Time EEPROM Save
    }
  }
  if (zone_6 == 1)          // For Zone 6  (On time EEPROM - 71,72,73) || (Off time EEPROM - 74,75,76) ||(S M T W Th Fr Sa - 77,78,79,80,81,82,83)
  { //  (Pattern - EEPROM - 99 = pattern_days || EEPROM - 100 = pattern_alternate || EEPROM - 101 = pattern_for)
    EEPROM.write(5, 0);
    if (pattern_days = 1)
    {
      EEPROM.write(99, 1);
      EEPROM.write(71, on_sec); EEPROM.write(72, on_min); EEPROM.write(73, on_hour);    // On Time EEPROM Save
      EEPROM.write(74, off_sec); EEPROM.write(75, off_min); EEPROM.write(76, off_hour); // Off Time EEPROM Save
      EEPROM.write(77, sunday); EEPROM.write(78, monday); EEPROM.write(79, tuesday); EEPROM.write(80, wednesday); EEPROM.write(81, thursday); EEPROM.write(82, friday); EEPROM.write(83, saturday);
    }
    else if  (pattern_alternate == 1)
    {
      EEPROM.write(100, 1);
      EEPROM.write(71, on_sec); EEPROM.write(72, on_min); EEPROM.write(73, on_hour);    // On Time EEPROM Save
      EEPROM.write(74, off_sec); EEPROM.write(75, off_min); EEPROM.write(76, off_hour); // Off Time EEPROM Save
    }
    else if (pattern_for == 1)
    {
      EEPROM.write(101, 1);
      EEPROM.write(71, on_sec); EEPROM.write(72, on_min); EEPROM.write(73, on_hour);    // On Time EEPROM Save
      EEPROM.write(74, off_sec); EEPROM.write(75, off_min); EEPROM.write(76, off_hour); // Off Time EEPROM Save
    }
  }
  for (int i = 0 ; i <= 108; i++)
  {
    Serial.print("EEPROM VALUE - ");
    Serial.print(i);
    Serial.print(" - ");
    Serial.println(EEPROM.read(i));
  }
  clear_everything();
  switch_no = 2;
  main_page_print = 1;
  clear_lcd();
  Serial.print("Switch No. - Saving - ");
  Serial.println(switch_no);
}
//  ************************************************************************************

//  *****************************  View Function  **************************************
void view_zone_print()
{

  tft.setCursor(0, 0);
  tft.setTextColor(BLACK);
  tft.setTextSize(3);
  tft.print("Select Zone - ");
  tft.setCursor(10, 100);
  tft.setTextColor(BLACK);
  tft.setTextSize(3);
  tft.print("Z1 Z2 Z3 Z4 Z5 Z6");
}
void view_select_zone()
{
  switch (view_zone_select)
  {
    case 1:
      {
        tft.fillRect(74, 130, 30, 20, WHITE);
        tft.setCursor(14, 120);
        tft.setTextColor(BLACK);
        tft.setTextSize(3);
        tft.print("_");
        break;
      }
    case 2:
      {
        tft.fillRect(14, 130, 30, 20, WHITE);
        tft.fillRect(130, 130, 30, 20, WHITE);
        tft.setCursor(74, 120);
        tft.setTextColor(BLACK);
        tft.setTextSize(3);
        tft.print("_");
        break;
      }
    case 3:
      {
        tft.fillRect(74, 130, 30, 20, WHITE);
        tft.fillRect(180, 130, 30, 20, WHITE);
        tft.setCursor(130, 120);
        tft.setTextColor(BLACK);
        tft.setTextSize(3);
        tft.print("_");
        break;
      }
    case 4:
      {
        tft.fillRect(130, 130, 30, 20, WHITE);
        tft.fillRect(235, 130, 30, 20, WHITE);
        tft.setCursor(180, 120);
        tft.setTextColor(BLACK);
        tft.setTextSize(3);
        tft.print("_");
        break;
      }
    case 5:
      {
        tft.fillRect(180, 130, 30, 20, WHITE);
        tft.fillRect(290, 130, 30, 20, WHITE);
        tft.setCursor(235, 120);
        tft.setTextColor(BLACK);
        tft.setTextSize(3);
        tft.print("_");
        break;
      }
    case 6:
      {
        tft.fillRect(235, 130, 30, 20, WHITE);
        tft.setCursor(290, 120);
        tft.setTextColor(BLACK);
        tft.setTextSize(3);
        tft.print("_");
        break;
      }
    default: break;
  }

  button_read();
  if (but_read == -2) // Previous
  {
    switch_no = switch_no - 2;
    main_page_print = 1;
    clear_lcd();
    Serial.println(switch_no);
  }
  else if (but_read == +2) //Next
  {
    switch_no = switch_no + 2;
    clear_lcd();
    Serial.println(switch_no);
  }
  else if (but_read == -1 && view_zone_select >= 1)
  {
    view_zone_select--;
    Serial.print("View Zone Select");
    Serial.println(view_zone_select);
  }
  else if (but_read == +1 && view_zone_select <= 5)
  {
    view_zone_select++;
    Serial.print("View Zone Select");
    Serial.println(view_zone_select);
  }
}

void view_details()
{
  tft.setCursor(0, 0);
  tft.setTextColor(BLACK);
  tft.setTextSize(3);
  tft.print("Details - ");
  switch (view_zone_select)
  {
    case 1:
      {
        tft.setCursor(0, 40);
        tft.setTextColor(BLACK);
        tft.setTextSize(2);
        tft.print("Zone 1 -");
        break;
      }
    case 2:
      {
        tft.setCursor(0, 40);
        tft.setTextColor(BLACK);
        tft.setTextSize(2);
        tft.print("Zone 2 -");
        break;
      }
    case 3:
      {
        tft.setCursor(0, 40);
        tft.setTextColor(BLACK);
        tft.setTextSize(2);
        tft.print("Zone 3 -");
        break;
      }
    case 4:
      {
        tft.setCursor(0, 40);
        tft.setTextColor(BLACK);
        tft.setTextSize(2);
        tft.print("Zone 4 -");
        break;
      }
    case 5:
      {
        tft.setCursor(0, 40);
        tft.setTextColor(BLACK);
        tft.setTextSize(2);
        tft.print("Zone 5 -");
        break;
      }
    case 6:
      {
        tft.setCursor(0, 40);
        tft.setTextColor(BLACK);
        tft.setTextSize(2);
        tft.print("Zone 6 -");
        break;
      }
    default: break;
  }
  button_read();
  if (but_read == -2) // Previous
  {
    switch_no = switch_no - 2;
    clear_lcd();
    Serial.println(switch_no);
  }
  else if (but_read == +2) // Next
  {
    switch_no = switch_no + 2;
    clear_lcd();
    Serial.println(switch_no);
  }
}

//  ***********************************************************************************



// ********************************  To Display Time  *********************************

// ********************************  To Display Time  *********************************
// ********************************  I2C   *******************************************
void commonregisters()
{
  static int start_bit = 0;
  static int address = 0 ;
  if (start_bit == 0)
  {
    Wire.write(255);
    start_bit = 1;
    Serial.print("Start Bit Start - ");
    Serial.println(start_bit);
  }
  else if (start_bit == 1)
  {
    Wire.write(EEPROM.read(address));
    address++;
    if (address == 109)
    {
      Serial.print("Address Previous- ");
      Serial.println(address);
      address = 0;
      start_bit = 0;
      Serial.print("Address - ");
      Serial.println(address);
      Serial.print("Start Bit Restart - ");
      Serial.println(start_bit);

    }
  }
}

// ************************************************************************************
//  **************************************  Time Display  *****************************

//  ************************************************************************************

// ********************************** Clear Everything  ********************************
void clear_everything()
{
  on_sec = 0;
  on_min = 0;
  on_hour = 0;
  off_sec = 0;
  off_min = 0;
  off_hour = 0;
  sunday = 0;
  monday = 0;
  tuesday = 0;
  wednesday = 0;
  thursday = 0;
  friday = 0;
  saturday = 0;
  zone_1 = 0;
  zone_2 = 0;
  zone_3 = 0;
  zone_4 = 0;
  zone_5 = 0;
  zone_6 = 0;
  pattern_days = 0;
  pattern_alternate = 0;
  pattern_for = 0;
}
//**************************************************************************************

//******************************* BUTTON READ / CLEAR LCD **************************************************
void button_read()
{
  but_read = digitalRead(previous_but) * -2 + digitalRead(decrement_but) * -1 + digitalRead(increment_but) * 1 + digitalRead(next_but) * 2;
  delay(100);
}
void clear_lcd()
{
  tft.fillScreen(WHITE);
}













