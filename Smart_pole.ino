#define Flame_Sensor1 32
#define Flame_Sensor2 33

#define LDR_Sensor1 34
#define LDR_Sensor2 35

#define Tilt_Sensor1 25
#define Tilt_Sensor2 26

#include <BlynkSimpleEsp32.h>
#include <OneWire.h>
#include <LiquidCrystal_I2C.h>

#define BLYNK_TEMPLATE_ID "YOUR_BLYNK_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "YOUR_BLYNK_TEMPLATE_NAME"
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_TOKEN"




LiquidCrystal_I2C lcd(0x27, 16, 2);  // set the LCD address to 0x3F for a 16 chars and 2 line display


char ssid[] = "YOUR_WIFI_NAME";
char pass[] = "YOUR_WIFI_PASSWORD";

//sender phone number with country code.
//not gsm module phone number
const String PHONE = "YOUR_PHONE_NUMBER";

//GSM Module RX pin to ESP32 2
//GSM Module TX pin to ESP32 4
#define rxPin 16
#define txPin 17
HardwareSerial sim800(1);

String smsStatus, senderNumber, receivedDate, msg;
boolean isReply = false;





void setup() {
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  lcd.begin();
  lcd.clear();
  lcd.backlight();  // Make sure backlight is on
  // initialize serial communication
  Serial.begin(9600);
  Serial.println("esp32 serial initialize");

  sim800.begin(9600, SERIAL_8N1, rxPin, txPin);
  Serial.println("SIM800C serial initialize");

  lcd.print("Fault detection");

  smsStatus = "";
  senderNumber = "";
  receivedDate = "";
  msg = "";


  sim800.println("AT+CMGF=1");  //SMS text mode
  delay(1000);
  sim800.println("AT+CMGD=1,4");  //delete all saved SMS
  delay(1000);


  // initialize the ESP32's pin as an input
  pinMode(Flame_Sensor1, INPUT);
  pinMode(Flame_Sensor2, INPUT);
  pinMode(LDR_Sensor1, INPUT);
  pinMode(LDR_Sensor2, INPUT);
  pinMode(Tilt_Sensor1, INPUT);
  pinMode(Tilt_Sensor2, INPUT);
}

void loop() {

  Blynk.run();


  // Light sensor
  int lightState1 = digitalRead(LDR_Sensor1);

  if (lightState1 == LOW) {
    Serial.println("It is on  LDR_Sensor1 ");
    Blynk.virtualWrite(V0, 0);
  }

  else {
    Serial.println("It is lights off LDR_Sensor1 ");
    Blynk.virtualWrite(V0, 1);
    // Display
    lcd.clear();          // Clear the LCD
    lcd.setCursor(0, 0);  // Set cursor to the first row
    lcd.print("Pole 1 Light off");
    delay(500);

    sendAlertSMS("Pole 1 Light off");
  }


  int lightState2 = digitalRead(LDR_Sensor2);

  if (lightState2 == LOW) {
    Serial.println("It is on LDR_Sensor2 ");
    Blynk.virtualWrite(V1, 0);
  } else {
    Serial.println("It is light LDR_Sensor2  ");
    Blynk.virtualWrite(V1, 1);
    // Display
    lcd.clear();          // Clear the LCD
    lcd.setCursor(0, 0);  // Set cursor to the first row
    lcd.print("Pole 2 Light off");
    delay(500);

    sendAlertSMS("Pole 2 Light off");
  }





  // Fire sensor
  int flame_state1 = digitalRead(Flame_Sensor1);
  int flame_state2 = digitalRead(Flame_Sensor2);
  if (flame_state1 == HIGH) {
    Serial.println("No flame dected => The fire is NOT detected");
    Blynk.virtualWrite(V2, 0);
  } else {
    Serial.println("Flame dected => The fire is detected Flame_Sensor1 ");
    Blynk.virtualWrite(V2, 1);

    // Display
    lcd.clear();          // Clear the LCD
    lcd.setCursor(0, 0);  // Set cursor to the first row
    lcd.print("Pole 1 on Fire");
    delay(500);

    sendAlertSMS("Pole 1 on Fire");
  }

  if (flame_state2 == HIGH) {
    Serial.println("No flame dected => The fire is NOT detected");
    Blynk.virtualWrite(V3, 0);
  } else {
    Serial.println("Flame dected => The fire is detected Flame_Sensor2 ");
    Blynk.virtualWrite(V3, 1);

    // Display
    lcd.clear();          // Clear the LCD
    lcd.setCursor(0, 0);  // Set cursor to the first row
    lcd.print("Pole 2 on Fire");
    delay(500);

    sendAlertSMS("Pole 2 on Fire");
  }




  int TiltState1 = digitalRead(Tilt_Sensor1);

  if (TiltState1 == HIGH) {
    Serial.println("It Tilt  Tilt_Sensor1  ");
    Blynk.virtualWrite(V4, 0);

  }

  else {
    Serial.println("It Tilt not  Tilt_Sensor1 ");
    Blynk.virtualWrite(V4, 1);
    lcd.clear();          // Clear the LCD
    lcd.setCursor(0, 0);  // Set cursor to the first row
    lcd.print("Pole 1 Tilted");
    delay(500);

    sendAlertSMS("Pole 1 Tilted");
  }


  int TiltState2 = digitalRead(Tilt_Sensor2);

  if (TiltState2 == HIGH) {
    Serial.println("It Tilt  Tilt_Sensor2 ");
    Blynk.virtualWrite(V5, 0);

  } else {
    Serial.println("It Tilt not  Tilt_Sensor2 ");
    Blynk.virtualWrite(V5, 1);
    lcd.clear();          // Clear the LCD
    lcd.setCursor(0, 0);  // Set cursor to the first row
    lcd.print("Pole 2 Tilted");
    delay(500);

    sendAlertSMS("Pole 2 Tilted");
  }

  delay(200);
}



void sendAlertSMS(String alertMsg) {
    sim800.print("AT+CMGF=1\r");
    delay(1000);
    sim800.print("AT+CMGS=\"" + PHONE + "\"\r");
    delay(1000);
    String alertMessage = String(alertMsg) +" \n";
    alertMessage += "Location: http://maps.google.com/maps?q=loc:" + String(12.96747682842095, 6) + "," + String(77.71336, 6);
    
    sim800.print(alertMessage);
    delay(100);
    sim800.write(0x1A);
    delay(1000);
    
    Serial.println("Alert SMS Sent Successfully.");
  }
