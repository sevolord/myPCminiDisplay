// основано на проекте https://github.com/AlexGyver/PCdisplay
// монитор железа https://github.com/DeltaDesignRus/LibreHardwareMonitor
// упрощено и выведено на дисплейчик, который был 
// используется дисплей 128 на 32 с библиотекой https://github.com/adafruit/Adafruit_SSD1306
// -------------------- БИБЛИОТЕКИ ---------------------
#include <TimerOne.h>           // библиотека таймера
#include <string.h>             // библиотека расширенной работы со строками
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//#include <Fonts/FreeSerif9pt7b.h>
// -------------------- БИБЛИОТЕКИ ---------------------
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

char inData[82];       // массив входных значений (СИМВОЛЫ)
int PCdata[20];        // массив численных значений показаний с компьютера
byte index = 0;
String string_convert;
boolean lightState, reDraw_flag = 1, updateDisplay_flag, updateTemp_flag, timeOut_flag = 1;
unsigned long timeout/*, uptime_timer*/;


void setup()
{
  Serial.begin(9600);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for (;;);
  }
  //display.setFont(&FreeSerif9pt7b);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Hello");
  display.println("there");
  display.display();
  //delay(2000);
}

void loop()
{
  parsing();                          // парсим строки с компьютера
  updateDisplay();                    // обновить показания на дисплее
}

void parsing() {
  while (Serial.available() > 0) {
    char aChar = Serial.read();
    if (aChar != 'E') {
      inData[index] = aChar;
      index++;
      inData[index] = '\0';
    } else {
      char *p = inData;
      char *str;
      index = 0;
      String value = "";
      while ((str = strtok_r(p, ";", &p)) != NULL) {
        string_convert = str;
        PCdata[index] = string_convert.toInt();
        index++;
      }
      index = 0;
      updateDisplay_flag = 1;
      updateTemp_flag = 1;
    }
    if (!timeOut_flag) {                                // если связь была потеряна, но восстановилась
      //if (!ERROR_BACKLIGHT) lcd.backlight();            // включить подсветку при появлении сигнала, если разрешено
      //if (ERROR_UPTIME) uptime_timer = millis();        // сбросить uptime, если разрешено
    }
    timeout = millis();
    timeOut_flag = 1;
  }
}
void updateDisplay() {
  if (updateDisplay_flag) {
    if (reDraw_flag) {
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      //верхняя строка
      display.setCursor(0, 0);
      display.print("CPU");
      display.setCursor(70, 0);
      display.print("GPU");
      //вторая строка
      display.setCursor(0, 10);
      display.println(PCdata[0]); //CPU temp
      display.setCursor(15, 10);
      display.print("C");
      
      display.setCursor(70, 10);
      display.println(PCdata[1]); //GPU temp
      display.setCursor(85, 10);
      display.print("C");
      //третья строка
      display.setCursor(0, 20);
      display.println(PCdata[4]); //CPU load
      display.setCursor(20, 20);
      display.print("%");

      display.setCursor(70, 20);
      display.println(PCdata[5]); //GPU load
      display.setCursor(90, 20);
      display.print("%");

      display.display();
    }
    updateDisplay_flag = 0;
  }
}
