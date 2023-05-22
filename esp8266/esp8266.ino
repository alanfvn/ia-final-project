#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

//   MOSI <---> D7
//   MISO <---> D6
//    SCK <---> D5
//     SS <---> D4

#define PN532_SCK  14
#define PN532_MOSI 13
#define PN532_SS   2
#define PN532_MISO 12

Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);

const char* ssid = "INT_CASA";
const char* password = "Internet170casa_321";

//#define redPin 1
#define greenPin 16
#define bluePin 4

void setup(void) {
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  Serial.begin(115200);

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");

  }

  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);

  nfc.setPassiveActivationRetries(0xFF);
}

void loop(void) {

  boolean success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };	// Buffer to store the returned UID
  uint8_t uidLength;				// Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);

  if (success) {
    enviarPost();
    for(int i = 0; i < uidLength; i++){
      Serial.printf("%02X-", uid[i]);
    }
    pintarLed(255, 128);
    Serial.println();
    delay(1000);
    apagarLed();
  }
  else
  {
    Serial.println("Timed out waiting for a card");
  }
}

char* getTag(uint8_t uidLen, uint8_t uid[]){
  char* result = {};
  return result;
}

void enviarPost() {

  HTTPClient http;
  WiFiClient client;

  http.begin(client,"http://192.168.0.100:8000/card/");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String data = "parametro1=valor1&parametro2=valor2";
  int httpCode = http.POST(data);

  if (httpCode > 0) {
    String payload = http.getString();
    Serial.println(payload);
  } else {
    Serial.printf("Error al realizar la solicitud HTTP. Código de error: %d\n", httpCode);
  }

  http.end();
}


void pintarLed( int g, int b){
  analogWrite(greenPin,g);
  analogWrite(bluePin,b);
}

void apagarLed(){
  analogWrite(greenPin,0);
  analogWrite(bluePin,0);
}