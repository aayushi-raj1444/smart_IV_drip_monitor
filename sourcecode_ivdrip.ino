#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const char* ssid = "victory";         
const char* password = "smartivdrip";  

#define BOT_TOKEN "8828225757:AAFayjeweeoxhOeD7UI1juLr1qqk" 
#define CHAT_ID   "82510450"                                     

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_MOSI   13   
#define OLED_CLK    14   
#define OLED_DC      2   
#define OLED_RESET   4   
#define OLED_CS     15   

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

#define ULTRASONIC_TRIG  5    
#define ULTRASONIC_ECHO 18    
#define BUZZER_PIN      23   
#define IR_SENSOR_PIN   16   

#define TOTAL_VOLUME_ML 500.0   
#define EMPTY_DISTANCE_CM 25.0  
#define ALERT_VOLUME_PCT  10.0
#define DROP_FACTOR       20.0  
#define BLOCKAGE_TIMEOUT  5000  

unsigned long lastTimeBotRan = 0;
const unsigned long botInterval = 2000; 

volatile unsigned long lastDropTime = 0;
volatile unsigned long dropIntervalMs = 0;
bool irLastState = HIGH;

bool lowVolumeAlertSent = false;
bool blockageAlertSent = false;

void IRAM_ATTR countDrop() {
  unsigned long now = millis();
  dropIntervalMs = now - lastDropTime;
  lastDropTime = now;
}

void setup() {
  Serial.begin(115200);

  pinMode(ULTRASONIC_TRIG, OUTPUT);
  pinMode(ULTRASONIC_ECHO, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(IR_SENSOR_PIN, INPUT_PULLUP);
  digitalWrite(BUZZER_PIN, LOW);

  attachInterrupt(digitalPinToInterrupt(IR_SENSOR_PIN), countDrop, FALLING);

  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed!"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("Smart IV Drip IoT"));
  display.println(F("Connecting to Wi-Fi..."));
  display.display();

  WiFi.begin(ssid, password);
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi connected!");
  display.setCursor(0, 30);
  display.println(F("Wi-Fi Connected!"));
  display.display();
  delay(2000);
}

void loop() {

  digitalWrite(ULTRASONIC_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG, LOW);

  long duration = pulseIn(ULTRASONIC_ECHO, HIGH, 30000); 
  float distanceCm = duration * 0.034 / 2.0;

  if (distanceCm <= 2.0 || distanceCm == 0) distanceCm = 2.0;
  if (distanceCm > EMPTY_DISTANCE_CM) distanceCm = EMPTY_DISTANCE_CM;

  float volumePercentage = (1.0 - (distanceCm / EMPTY_DISTANCE_CM)) * 100.0;
  if (volumePercentage < 0) volumePercentage = 0;
  float remainingMl = (volumePercentage / 100.0) * TOTAL_VOLUME_ML;

  unsigned long timeSinceLastDrop = millis() - lastDropTime;
  float dropRateMlHr = 0.0;
  bool isBlocked = false;

  if (timeSinceLastDrop > BLOCKAGE_TIMEOUT && lastDropTime > 0) {
    isBlocked = true;
    dropRateMlHr = 0.0;
  } else if (dropIntervalMs > 0) {
    dropRateMlHr = 3600000.0 / (dropIntervalMs * DROP_FACTOR);
  }

  int timeRemainingHours = 0;
  int timeRemainingMins = 0;
  if (dropRateMlHr > 0) {
    float totalTimeHrs = remainingMl / dropRateMlHr;
    timeRemainingHours = (int)totalTimeHrs;
    timeRemainingMins = (int)((totalTimeHrs - timeRemainingHours) * 60);
  }

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print(F("Vol: ")); 
  display.print(remainingMl, 1); 
  display.print(F(" mL"));
  
  display.setCursor(0, 12);
  display.print(F("Level: ")); 
  display.print(volumePercentage, 1); 
  display.print(F(" %"));

  display.setCursor(0, 24);
  display.print(F("Time: ")); 
  if (isBlocked || dropRateMlHr == 0) {
    display.print(F("--h --m")); 
  } else {
    display.print(timeRemainingHours);
    display.print(F("h "));
    display.print(timeRemainingMins);
    display.print(F("m"));
  }

  display.drawLine(0, 36, 128, 36, SSD1306_WHITE);

  String timeStr = (isBlocked || dropRateMlHr == 0) ? "--h --m" : (String(timeRemainingHours) + "h " + String(timeRemainingMins) + "m");

  display.setCursor(0, 44);
  if (volumePercentage <= ALERT_VOLUME_PCT) {
    display.println(F("ALERT: LOW VOLUME!"));
    digitalWrite(BUZZER_PIN, HIGH);

    if (!lowVolumeAlertSent) {
      String msg = "⚠️ *SMART IV ALERT*\nLow fluid volume detected!\nRemaining: " + String(remainingMl, 1) + " mL\nTime Left: " + timeStr;
      bot.sendMessage(CHAT_ID, msg, "Markdown");
      lowVolumeAlertSent = true;
    }
  } 
  else if (isBlocked) {
    display.println(F("ALERT: BLOCKAGE!"));
    digitalWrite(BUZZER_PIN, HIGH);

    if (!blockageAlertSent) {
      String msg = "🚨 *SMART IV BLOCKAGE ALERT*\nNo flow detected from IR sensor for over 5 seconds! Please check the IV line immediately.";
      bot.sendMessage(CHAT_ID, msg, "Markdown");
      blockageAlertSent = true;
    }
  } 
  else {
    display.println(F("Status: NORMAL"));
    digitalWrite(BUZZER_PIN, LOW);
    lowVolumeAlertSent = false;
    blockageAlertSent = false;
  }

  display.display();

  if (millis() - lastTimeBotRan > botInterval) {
    int numNewMessages = bot.getUpdates(bot.last_message_received);
    while (numNewMessages > 0) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received);
    }
    lastTimeBotRan = millis();
  }

  delay(500);
}

void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    if (text == "/status") {

      String statusTime = "--h --m";
      if (dropIntervalMs > 0 && (millis() - lastDropTime <= BLOCKAGE_TIMEOUT)) {
         float currentRate = 3600000.0 / (dropIntervalMs * DROP_FACTOR);
         float currentVol = (1.0 - (pulseIn(ULTRASONIC_ECHO, HIGH, 30000) * 0.034 / 2.0 / EMPTY_DISTANCE_CM)) * TOTAL_VOLUME_ML;
         if (currentVol < 0) currentVol = 0;
         float hrs = currentVol / currentRate;
         statusTime = String((int)hrs) + "h " + String((int)((hrs - (int)hrs) * 60)) + "m";
      }

      String reply = "📊 *IV Monitor Status*\nSystem State: Active\nRemaining Volume: " + String(((1.0 - (pulseIn(ULTRASONIC_ECHO, HIGH, 30000) * 0.034 / 2.0 / EMPTY_DISTANCE_CM)) * TOTAL_VOLUME_ML), 1) + " mL\nEstimated Time Left: " + statusTime;
      bot.sendMessage(chat_id, reply, "Markdown");
    }
  }
}
