
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define ANCHO_PANTALLA 128 // Ancho de la pantalla OLED
#define ALTO_PANTALLA 64   // Alto de la pantalla OLED

#define OLED_RESET -1           // Pin reset incluido en algunos modelos de pantallas (-1 si no disponemos de pulsador).
#define DIRECCION_PANTALLA 0x3C // Dirección de comunicacion: 0x3D para 128x64, 0x3C para 128x32

Adafruit_SSD1306 display(ANCHO_PANTALLA, ALTO_PANTALLA, &Wire, OLED_RESET);

#define LOGO_WIDTH 84
#define LOGO_HEIGHT 52

void setup()
{
  Serial.begin(9600);

  display.begin(SSD1306_SWITCHCAPVCC, DIRECCION_PANTALLA);
  // Serial.println(F("Fallo en la asignacion de SSD1306"));

  //  display.clearDisplay();
  //  display.setTextSize(2);
  //  display.setTextColor(WHITE);
  //  display.setCursor(10,10);
  //  display.println("TESTING");
  // display.display();
}

void loop()
{

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10, 10);
  display.println("TESTING5");
  display.display();
}
