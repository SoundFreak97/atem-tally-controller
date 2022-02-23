#include <WiFi.h>
#include <SPI.h>
#include <SD.h>


// Define chip select pin and file names
#define CHIPSELECT 5
#define SETTING_FILE "/setting.cfg"

File myFile;

// Download these from here
// https://github.com/kasperskaarhoj/SKAARHOJ-Open-Engineering/tree/master/ArduinoLibs
#include <SkaarhojPgmspace.h>
#include <ATEMbase.h>
#include <ATEMstd.h>

// You can customize the red/green/grey if you want
// http://www.barth-dev.de/online/rgb565-color-picker/
#define GRAY  0x0841 //   8   8  8
#define BLACK 0x0000 //   8   8  8
#define GREEN 0x0400 //   0 128  0
#define RED   0xF800 // 255   0  0



/////////////////////////////////////////////////////////////
// You probably don't need to change things below this line
#define LED_PIN 10

ATEMstd AtemSwitcher;

int cameraNumber = 1;

int previewTallyPrevious = 1;
int programTallyPrevious = 1;

String hostname = "tally-" + WiFi.macAddress();

// Some variables to store main settings
int len, wid, hei;
char welcomeMsg[64]; 
const char* ssid;
const char* psk;
const char* ip;

String line1, line2, line3, line4, line5, line6, line7;

void setup() {
  Serial.begin(115200);

  if (!SD.begin(CHIPSELECT)) { //make sure sd card was found
    while (true);
  }
 
  hostname.replace(":", "");

  int recNum = 0; // We have read 0 records so far

  myFile = SD.open(SETTING_FILE);

  while (myFile.available())
  {
    String list = myFile.readStringUntil('\r\n');
    //Serial.println(list);
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

  Serial.println(ssid);
  Serial.println(psk);
  Serial.println(ip);

  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());

  WiFi.setHostname(hostname.c_str());

  WiFi.begin(ssid, psk);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("Connecting to WiFi: ");
    Serial.println(ssid);
  }
  Serial.println("Connected to the WiFi network");

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  AtemSwitcher.begin(switcherIp);
  AtemSwitcher.serialOutput(0x80);
  AtemSwitcher.connect();

}

void loop() {

  // Check for packets, respond to them etc. Keeping the connection alive!
  AtemSwitcher.runLoop();

  int programTally = AtemSwitcher.getProgramTally(cameraNumber);
  int previewTally = AtemSwitcher.getPreviewTally(cameraNumber);

  if ((programTallyPrevious != programTally) || (previewTallyPrevious != previewTally)) { // changed?
    if (programTally && !previewTally) { // only program
      drawLabel(RED, BLACK, LOW);
    } else if (programTally && previewTally) { // program AND preview
      drawLabel(RED, GREEN, LOW);
    } else if (previewTally && !programTally) { // only preview
      drawLabel(GREEN, BLACK, HIGH);
    } else if (!previewTally || !programTally) { // neither
      drawLabel(BLACK, GRAY, HIGH);
    }
  }

  programTallyPrevious = programTally;
  previewTallyPrevious = previewTally;
}

void drawLabel(unsigned long int screenColor, unsigned long int labelColor, bool ledValue) {
  digitalWrite(LED_PIN, ledValue);
}
