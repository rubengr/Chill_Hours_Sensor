#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <EEPROM.h>

#define LED 16
const int oneWireBus = D3;  

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//Os pinos do I2C foram alterados na library para que o oled funcionasse


// define the number of bytes you want to access
#define EEPROM_SIZE 2

int horasfrio;
int seconds;     //10 em 10s variavel incrmenenta

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED, OUTPUT);     // Initialise the LED_BUILTIN 

  // initialize EEPROM with predefined size
  EEPROM.begin(EEPROM_SIZE);

  // read horas de frio from flash memory
  byte* p = (byte *) &horasfrio;
  *p = EEPROM.read(0);
  *(p+1) = EEPROM.read(1);

   // Start the DS18B20 sensor
  sensors.begin();
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println("SSD1306 allocation failed");
    for(;;);
  }

  display.clearDisplay();
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED, LOW);  
  //Serial.println("LED is off");
  delay(1000);                     
  digitalWrite(LED, HIGH);  
  // Serial.println("LED is on");
  sensors.requestTemperatures(); 
  float temperatureC = sensors.getTempCByIndex(0);
  Serial.print(temperatureC);
  Serial.println("ºC");

  //temperatureC = 9.9;
  //horasfrio = 0;
  //byte* p = (byte *) &horasfrio;
  //EEPROM.write(0, *p);
  //EEPROM.write(1, *(p+1));
  //EEPROM.commit();  
  if (temperatureC <= 10.0)
  {  //temperatura abaixo de 10ºc
    seconds++;    //soma
    if(seconds>=360){ //+ 1 hora de frio
        horasfrio++;
        seconds = 0;
        byte* p = (byte *) &horasfrio;
        EEPROM.write(0, *p);
        EEPROM.write(1, *(p+1));
        EEPROM.commit();
    }
  }else{
    seconds = 0;  //faz reset e começa de novo
  }
  Serial.print("Segundos: ");
  Serial.println(seconds);
  Serial.print("Horas de frio: ");
  Serial.println(horasfrio);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(40, 0);
  display.print("Peony");
  
  display.setCursor(0, 15);
  display.print("Temperature: ");
  display.print(temperatureC);
  display.println("C");

  display.setCursor(0, 25);
  display.print("Seconds <10C: ");
  display.print(seconds*10);
  display.println("s");


  display.setCursor(0, 45);
  display.print("Chill Hours: ");
  display.print(horasfrio);
  display.println("h");

  display.display();

  delay(9000);                     
}