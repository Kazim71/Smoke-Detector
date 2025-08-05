#include <ESP8266WiFi.h>      // uncomment this line if you are using esp8266 and comment the line above
#include <Firebase_ESP_Client.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "Pixel 7"
#define WIFI_PASSWORD "mnbvmnbv"

// Insert Firebase project API Key
#define API_KEY "AIzaSyAW2noYM1MCJ1pnJnZ6K9_6VW_rrSSee3k"       //API key or SECRET key

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://console.firebase.google.com/u/0/project/fire-base-alarm-b1cfa/database/fire-base-alarm-b1cfa-default-rtdb/data/~2F"   //url.firebasedatabase.app/

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
bool signupOK = false; 


int buzzer = D2;
int smokeA0 = A0;

// Your threshold value. You might need to change it.
int sensorThres = 645;

void setup() {
 pinMode(buzzer, OUTPUT);
 pinMode(smokeA0, INPUT);
 Serial.begin(9600);

WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
 if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback;  //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  Firebase.RTDB.setInt(&fbdo, "fire", 0); 
}




void loop() {
 int analogSensor = analogRead(smokeA0);

 Serial.print("Pin A0: ");
 Serial.println(analogSensor);

 if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 10 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
  }


 // Checks if it has reached the threshold value
 if (analogSensor > sensorThres)
 {
   tone(buzzer, 1000, 200);
   Firebase.RTDB.setInt(&fbdo, "fire", 1);
 }
 else
 {
   noTone(buzzer);
   Firebase.RTDB.setInt(&fbdo, "fire", 0);
 }
 delay(100);
}