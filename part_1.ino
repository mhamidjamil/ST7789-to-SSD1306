#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
// temperature sensor define
#define ONE_WIRE_BUS 16
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature tempSensor(&oneWire);
float tempCelsius;    // temperature in Celsius
float tempFahrenheit; // temperature in Fahrenheit

const int motorSpeedL = 14; // A0 Motor PWM for L
const int motorSpeedR = 15; // A1 Motor PWM for L
const int relay = 3;        // Relay
// int display = 0;
int process = 0;

int upperswitch = 6;  // Enter your upper switch pin here A3
int lowerswitch = 17; // Enter your lower switch pin here A4

int cold_button = 7; // Enter your cold process button here A5
int hot_button = 4;  // Enter your hot process button here

int old_temp;
// Debounce Cold_button(cold_button, 20);
// Debounce Hot_button(hot_button, 20);
bool hot_ready = 0;
bool cold = 0;
bool hot = 0;
bool lowerlimit = 0;

// ST7789 display module connections
//#define display_CS 10 // define chip select pin
//#define display_DC 9  // define data/command pin
//#define display_RST 8 // define reset pin, or set to -1 and connect to Arduino RESET pin

unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
const long interval = 5000;
int counter = 0;

// Adafruit_ST7789 display = Adafruit_ST7789(display_CS, display_DC, display_RST);

void setup()
{
    Serial.begin(9600);
    tempSensor.begin();
    pinMode(motorSpeedL, OUTPUT);
    pinMode(motorSpeedR, OUTPUT);
    pinMode(relay, OUTPUT);
    digitalWrite(relay, HIGH);

    pinMode(lowerswitch, INPUT);
    pinMode(upperswitch, INPUT);
    pinMode(hot_button, INPUT);
    pinMode(cold_button, INPUT);

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    { // Address 0x3D for 128x64
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ;
    }

    // Welcome Screen
    delay(2000);
    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 10);
    // Display static text
    display.println("Welcome to Soap make");
    display.display();

    Serial.println("Welcome to Soap make");
    delay(2000);
    if (digitalRead(upperswitch) == LOW)
    {
        Serial.println("Upper limit not met. Moving up now");
        // Limit not met messeges
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        // Display static text
        display.println("Upper limit not met");
        display.display();
        while (digitalRead(upperswitch) == LOW)
        {
            display.setCursor(0, 10);
            display.print("Moving up");
            display.display();
            move_up(); // make this turn right if motor moves the other way
        }
        Serial.println("Head moved to upper limit. Ready to proceed");
        display.setCursor(0, 20);
        display.display();
        display.print("Upper limit met");
        Serial.println("Stopping motor");
        motor_stop();
    }
    // Standby for command
    display.clearDisplay();
    display.setTextSize(3);
    display.setTextColor(WHITE);
    display.setCursor(30, 20);
    // Display static text
    display.println("Ready");
    display.display();

    delay(2000);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("Ready");
    display.setCursor(10, 10);
    display.print("WELCOME");
    display.print("TO");
    display.print("SOAP MAKE");
    display.setTextSize(2);
    display.setCursor(20, 25);
    display.print("Enter Mode");
    display.display();

    Serial.println("Welcome to Soap make");
    Serial.println("Exiting Setup now and entering loop. Standy by for commands..!");
    Serial.println("Waiting for mode");
}

void loop()
{
    // Serial.print("Cold=");
    // Serial.print(digitalRead(cold_button));
    // Serial.print("     ");
    // Serial.print("Hot=");
    // Serial.print(digitalRead(hot_button));
    // Serial.println();

    while (process == 0)
    {
        if (digitalRead(cold_button) == HIGH) // lock
        {
            process = 1;
            Serial.println("Cold process selected");
            Serial.print("Process variable=");
            Serial.print(process);
            Serial.println();
        }
        if (digitalRead(hot_button) == HIGH) // lock
        {
            process = 2;
            Serial.println("Hot process selected");
            Serial.print("Process variable=");
            Serial.print(process);
            Serial.println();
        }
    }

    switch (process)
    {
    case 1:
        // Cold process confirm
        // Cold process selected
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println("Ready");
        display.setCursor(10, 10);
        display.print("Cold Process");
        display.setTextSize(2);
        display.setCursor(20, 10);
        display.print("Press again  to confirm");
        display.display();

        Serial.println("Are you sure to proceed Cold process?");
        Serial.println("Press again to confirm");
        Serial.println("Waiting for confirmation.....!");

        Serial.print("Cold=");
        Serial.print(digitalRead(cold_button));
        Serial.print("     ");
        Serial.print("Hot=");
        Serial.print(digitalRead(hot_button));
        Serial.println();

        while (digitalRead(cold_button) == LOW || process != 10)
        {
            if (digitalRead(cold_button) == HIGH)
            {
                process = 10;
                Serial.println("Cold process Confirmed");
                Serial.print("Process variable=");
                Serial.print(process);
                Serial.println();
                Serial.println("Starting Cold process");
            }
        }
        start_cold_process();
        break;

    case 2:
        // hot process confirm
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println("Ready");
        display.setCursor(10, 10);
        display.print("Hot Process");
        display.setTextSize(2);
        display.setCursor(20, 10);
        display.print("Press again  to confirm");
        display.display();

        Serial.print("Cold=");
        Serial.print(digitalRead(cold_button));
        Serial.print("     ");
        Serial.print("Hot=");
        Serial.print(digitalRead(hot_button));
        Serial.println();

        while (digitalRead(hot_button) == LOW || process != 20)
        {
            if (digitalRead(hot_button) == HIGH)
            {
                process = 20;
                Serial.println("Hot process Confirmed");
                Serial.print("Process variable=");
                Serial.print(process);
                Serial.println();
                Serial.println("Starting Hot process");
            }
        }
        start_hot_process();
        break;
    }
}

void sense_val()
{
    tempSensor.requestTemperatures();            // send the command to get temperatures
    tempCelsius = tempSensor.getTempCByIndex(0); // read temperature in Celsius
    tempFahrenheit = tempCelsius * 9 / 5 + 32;   // convert Celsius to Fahrenheit

    char string[10];                    // Create a character array of 10 characters
    dtostrf(tempCelsius, 3, 0, string); // (<variable>,<amount of digits we are going to use>,<amount of decimal digits>,<string name>)
    // display.setCursor(21, 125);             // Set position (x,y)
    // display.setTextColor(ST77XX_WHITE);     // Set color of text. First is the color of text and after is color of background
    // display.setTextSize(4);                 // Set text size. Goes from 0 (the smallest) to 20 (very big)
    // display.println(string);                // Print a text or value
}

void move_down()
{
    digitalWrite(motorSpeedL, HIGH);
    digitalWrite(motorSpeedR, LOW);
}

void move_up()
{
    digitalWrite(motorSpeedL, LOW);
    digitalWrite(motorSpeedR, HIGH);
}

void start_cold_process()
{
    // Cold process action
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("Starting Cold process");
    display.display();
    if (process == 10)
    {
        // Display  process started
        display.setCursor(40, 25);
        display.setTextSize(2);
        display.print("Moving Down");
        display.display();
        Serial.println("Moving down");
        while (digitalRead(lowerswitch) == LOW)
        {
            move_down(); // make this turn left if motor moves the other way
            delay(1);
        }
        Serial.println("Lower limit met");
    }

    if (digitalRead(lowerswitch) == HIGH && process == 10)
    {
        process = 0;
        lowerlimit = 1;
        Serial.println("Setting control variables");
    }
    Serial.print("Process=");
    Serial.print(process);
    Serial.print("  ");
    Serial.print("lowerlimit=");
    Serial.print(lowerlimit);
    Serial.println();
    if (process == 0 && lowerlimit == 1)
    {
        display.setTextSize(2);
        display.setCursor(50, 25);
        display.print("Moving Up");
        display.display();
        Serial.println("Moving up");
        while (digitalRead(upperswitch) == LOW)
        {
            move_up(); // make this turn right if motor moves the other way
            delay(1);
        }
        Serial.println("Upper limit met");
    }

    if (digitalRead(upperswitch) == HIGH && lowerlimit == 1)
    {
        // display process completed
        while (lowerlimit == 1)
        {
            motor_stop();
            display.setTextSize(2);
            display.setCursor(60, 25);
            display.print("Completed!");
            display.display();
            Serial.println("Cold process completed");
            Serial.print("Process variable=");
            Serial.print(process);
            Serial.println();
            delay(3000);

            display.clearDisplay();
            display.setTextSize(1);
            display.setTextColor(WHITE);
            display.setCursor(0, 0);
            display.println("Ready");
            display.setCursor(10, 10);
            display.print("WELCOME");
            display.print("TO");
            display.print("SOAP MAKE");
            display.setTextSize(2);
            display.setCursor(20, 25);
            display.print("Enter Mode");
            display.display();

            Serial.println("Welcome to Soap make");
            Serial.println("Waiting for mode");
            process = 0;
            lowerlimit = 0;
            display = 1;
        }
    }
}

void start_hot_process()
{
    // Hot processs action
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("Starting Hot process");
    display.display();

    Serial.println("Hot process work!!!!");
    Serial.print("Temperature=");
    Serial.print(tempCelsius);
    Serial.println();
    while (tempCelsius <= 70 && digitalRead(cold_button) == LOW) // debug only
    {
        if (hot_ready == 0)
        {
            Serial.println("Relay On");
            digitalWrite(relay, LOW);
            sense_val(); // display heating progress
            display_temp();
            if (tempCelsius >= 70 || digitalRead(upperswitch) == HIGH) // debug only
            {
                while (counter == 0)
                {
                    currentMillis = previousMillis = millis();
                    counter = 1;
                    Serial.println("Time set");
                }

                while (currentMillis - previousMillis <= interval)
                {
                    currentMillis = millis();
                    temp_maintain();
                    if (currentMillis - previousMillis >= interval)
                    {
                        hot_ready = 1;
                        Serial.print("hot_ready==");
                        Serial.print(hot_ready);
                        Serial.println();
                        Serial.print("process==");
                        Serial.print(process);
                        Serial.println();
                        Serial.println("Time inverval passed");
                    }
                }
            }
        }
    }

    if (process == 20 && hot_ready == 1)
    {
        display.setTextSize(2);
        display.setCursor(10, 20);
        display.print("Moving Down");
        display.display();

        Serial.println("Moving down");
        while (digitalRead(lowerswitch) == LOW)
        {
            display_temp();
            move_down(); // make this turn left if motor moves the other way
        }
    }

    if (digitalRead(lowerswitch) == HIGH && process == 20)
    {
        Serial.println("Moving up");
        process = 0;
        lowerlimit = 1;
    }

    if (process == 0 && lowerlimit == 1)
    {
        while (digitalRead(upperswitch) == LOW)
        {
            // Display moving up
            display_temp();
            display.setTextSize(2);
            display.setCursor(10, 130);
            display.print("Moving Up");
            move_up(); // make this turn right if motor moves the other way
        }
    }

    if (digitalRead(upperswitch) == HIGH && lowerlimit == 1)
    {
        // display process completed and wait
        digitalWrite(motorSpeedL, LOW);
        digitalWrite(motorSpeedR, LOW);
        process = 0;
        lowerlimit = 0;
        hot_ready = 0;
        counter = 0;
        digitalWrite(relay, HIGH);
        display.setTextSize(2);
        display.setCursor(10, 25);
        display.print("Completed!");
        display.display();
        Serial.println("Hot process completed");
        Serial.print("Process variable=");
        Serial.print(process);
        Serial.println();
        delay(3000);

        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println("Ready");
        display.setTextSize(2);
        display.setCursor(20, 25);
        display.print("Enter Mode");
        display.display();
         Serial.println("Waiting for mode");
    }
}

void motor_stop()
{
    digitalWrite(motorSpeedL, LOW);
    digitalWrite(motorSpeedR, LOW);
}

void temp_maintain()
{
    Serial.println("Temperature maintaining");
    if (tempCelsius < 65)
    {
        digitalWrite(relay, LOW); // Relay on
    }
    if (tempCelsius > 70)
    {
        digitalWrite(relay, HIGH); // Relay off
    }
}

void display_temp()
{
    if (old_temp != tempCelsius)
    {
        display.fillRoundRect(10, 200, 240, 50, 8, 0);
        display.setCursor(10, 25);
        display.setTextSize(3);
        display.println(tempCelsius); // Print a text or value
        display.setCursor(12, 28);
        display.setTextSize(3); // Set text size. Goes from 0 (the smallest) to 20 (very big)
        display.println("C");
        Serial.println(tempCelsius); // Print a text or value
        old_temp = tempCelsius;
    }
}
