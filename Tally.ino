#include <WiFi.h>
#include <SPI.h>
#include <SD.h>
#include <FastLED.h>

#define NUM_LEDS 7

#define DATA_PIN 17

// Define chip select pin and file names
#define CHIPSELECT 5
#define SETTING_FILE "/setting.cfg"

File myFile;

// Download these from here
// https://github.com/kasperskaarhoj/SKAARHOJ-Open-Engineering/tree/master/ArduinoLibs
#include <SkaarhojPgmspace.h>
#include <ATEMbase.h>
#include <ATEMstd.h>


/////////////////////////////////////////////////////////////
// You probably don't need to change things below this line

// This is an array of leds.  One item for each led in your strip.
CRGB leds[NUM_LEDS];

ATEMstd AtemSwitcher;

int cameraNumber = 1;

int previewTallyPrevious = 1;
int programTallyPrevious = 1;

String hostname = "tally-" + WiFi.macAddress();

const char* ssid;
const char* psk;
const char* ip;

String line1, line2, line3, line4, line5, line6, line7;

void setup() {
  Serial.begin(115200);

  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS); 

  if (!SD.begin(CHIPSELECT)) { //make sure sd card was found
    fill_solid( leds, NUM_LEDS, CRGB(150,50,170));
    FastLED.show();
    delay(1000);
    while (true);
  }
 
  hostname.replace(":", "");

  int recNum = 0; // We have read 0 records so far

  myFile = SD.open(SETTING_FILE);

  while (myFile.available())
  {
    String list = myFile.readStringUntil('\r\n');
    recNum++; // Count the record

    if(recNum == 1)
    {
       line1 = list;
    }
    if(recNum == 2)
    {
       line2 = list;
    }
    if(recNum == 3)
    {
       line3 = list;
    }
    if(recNum == 4)
    {
       line4 = list;
    }
    if(recNum == 5)
    {
       line5 = list;
    }
    if(recNum == 6)
    {
       line6 = list;
    }
    if(recNum == 7)
    {
       line7 = list;
    }
  }

  ssid = line1.c_str();
  psk = line2.c_str();
  cameraNumber = line3.toInt();

  // Define the IP address of your ATEM switcher
  IPAddress switcherIp(line4.toInt(), line5.toInt(), line6.toInt(), line7.toInt());

  Serial.println();
  Serial.println();
  Serial.println("---");
  Serial.println("Trying to Connect to WiFi with:");
  Serial.println();

  Serial.print("#### Tally Light MAC Address: ");
  Serial.println(WiFi.macAddress());

  Serial.print("#### Tally Light Host Name:   ");
  Serial.println(hostname);

  Serial.println(); 
  Serial.print("#### Tally Light WiFi SSID:   ");
  Serial.println(ssid);
  Serial.print("#### Tally Light WiFi PSK:    ");
  Serial.println(psk);
  Serial.println();
  Serial.print("#### Tally Light ATEM IP:     ");
  Serial.print(line4);
  Serial.print(".");
  Serial.print(line5);
  Serial.print(".");
  Serial.print(line6);
  Serial.print(".");
  Serial.println(line7);
  Serial.print("#### Tally Light Camera ID:   ");
  Serial.println(cameraNumber);
  Serial.println();
  Serial.println("---");
  Serial.println();

  Serial.println(); 
  

  WiFi.setHostname(hostname.c_str());

  WiFi.begin(ssid, psk);
  while (WiFi.status() != WL_CONNECTED) {
    fill_solid( leds, NUM_LEDS, CRGB(0,0,170));
    FastLED.show();
    delay(500);
    fill_solid( leds, NUM_LEDS, CRGB(0,0,0));
    FastLED.show();
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  
  Serial.print("\r\nConnected IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  leds[6] = CRGB(0,0,50);     
  FastLED.show(); 

  setColorRing(255, 0, 0);
  delay(1000);
  setColorRing(255, 255, 0);
  delay(1000);
  setColorRing(0, 255, 0);
  delay(1000);
  setColorRing(0, 0, 0);
  
  AtemSwitcher.begin(switcherIp);
  AtemSwitcher.serialOutput(0x80);
  AtemSwitcher.connect();

  leds[6] = CRGB(0,50,0);     
  FastLED.show(); 

}

void loop() {

  // Check for packets, respond to them etc. Keeping the connection alive!
  AtemSwitcher.runLoop();

  int programTally = AtemSwitcher.getProgramTally(cameraNumber);
  int previewTally = AtemSwitcher.getPreviewTally(cameraNumber);

  if ((programTallyPrevious != programTally) || (previewTallyPrevious != previewTally)) { // changed?
    if (programTally && !previewTally) { // only program
      setColorRing(255, 0, 0);
    } else if (programTally && previewTally) { // program AND preview
      setColorRing(255, 255, 0);
    } else if (previewTally && !programTally) { // only preview
      setColorRing(0, 255, 0);
    } else if (!previewTally || !programTally) { // neither
      setColorRing(0, 0, 0);
    }
  }

  programTallyPrevious = programTally;
  previewTallyPrevious = previewTally;
}

void setColorRing(int r, int g, int b) {
  leds[0] = CRGB(r,g,b);  
  leds[1] = CRGB(r,g,b);
  leds[2] = CRGB(r,g,b);
  leds[3] = CRGB(r,g,b);
  leds[4] = CRGB(r,g,b);
  leds[5] = CRGB(r,g,b);   
  FastLED.show(); 
}
