// http://librarymanager/All#M5StickC https://github.com/m5stack/M5StickC
#include <WiFi.h>

// Download these from here
// https://github.com/kasperskaarhoj/SKAARHOJ-Open-Engineering/tree/master/ArduinoLibs
#include <SkaarhojPgmspace.h>
#include <ATEMbase.h>
#include <ATEMstd.h>

// Define the IP address of your ATEM switcher
IPAddress switcherIp(10, 11, 200, 1);

// Put your wifi SSID and password here
const char* ssid = "";
const char* password = "";

// Set this to 1 if you want the orientation to update automatically
#define AUTOUPDATE_ORIENTATION 0

// You can customize the red/green/grey if you want
// http://www.barth-dev.de/online/rgb565-color-picker/
#define GRAY  0x0841 //   8   8  8
#define GREEN 0x0400 //   0 128  0
#define RED   0xF800 // 255   0  0



/////////////////////////////////////////////////////////////
// You probably don't need to change things below this line
#define LED_PIN 10

ATEMstd AtemSwitcher;

int cameraNumber = 1;

int previewTallyPrevious = 1;
int programTallyPrevious = 1;
int cameraNumberPrevious = cameraNumber;

void setup() {
  Serial.begin(9600);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  AtemSwitcher.begin(switcherIp);
  AtemSwitcher.serialOutput(0x80);
  AtemSwitcher.connect();

  // GPIO
  pinMode(26, INPUT); 
  pinMode(36, INPUT);
  pinMode( 0, INPUT);
  pinMode(32, INPUT);
  pinMode(33, INPUT);
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
  cameraNumberPrevious = cameraNumber;
  orientationPrevious  = orientation;
}

void drawLabel(unsigned long int screenColor, unsigned long int labelColor, bool ledValue) {
  digitalWrite(LED_PIN, ledValue);
}


