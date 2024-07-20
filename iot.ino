// LCD
#include <LiquidCrystal.h>
const int D8=13;
const int D1=5;
const int D2=4;
const int D3=0;
const int D4=2;
const int D6=12;
LiquidCrystal lcd(D8,D1,D2,D3,D4,D6);

// DHT SENSOR
#include <DHT.h>
# define DHTPIN 0
#define DHTTYPE 11
DHT dht(DHTPIN,DHTTYPE);

//TO FIREBASE
#include <ESP8266Firebase.h>
#include <ESP8266WiFi.h>

# define FIREBASE_HOST  "iot-database-964e1-default-rtdb.firebaseio.com"
// # define FIREBASE_AUTH  "eclC0Q5fzQu06OMpXZ01BQnzSslVzsDJtGq7AkbL"
# define WIFI_SSID "network name"
# define WIFI_PASSWORD "password"

Firebase firebase (FIREBASE_HOST);


// Machine learning
#include "weather.h"
Eloquent::ML::Port::LinearRegression LR;

void setup() {
  Serial.begin(2400);
  
  dht.begin();

  // connect to wifi
  Serial.println();
  Serial.println();
  Serial.print("connecting to: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID,WIFI_PASSWORD);
  
  while(WiFi.status() !=WL_CONNECTED){
    delay(200);
    Serial.print(".");
  }
  Serial.println("WiFi Connected");

  // lcd setup
  lcd.begin(16,2);

}


void loop(){
  delay(2000);
  float h = dht.readHumidity();
  float t = dht.readTemperature();


  if (isnan(h)||isnan(t)){
    Serial.println("Failed to read from DHT!!!");
    lcd.print("ERROR!!!");
    firebase.pushString("ERROR","Failed to read from sensor!!!");
    return;
  }

  // Showcase the readings 
  Serial.print("Humidity: ");
  Serial.println(h);
  firebase.pushFloat("conditions/Humidity ",h);


  Serial.print("Temperature: ");
  Serial.println(t);
  firebase.pushFloat("conditions/Temperature ", t);

  //LCD DISPLAY
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Temp: ");
  lcd.print(t);
  lcd.print(" C");

  lcd.setCursor(0,1);
  lcd.print("Hum: ");
  lcd.print(h);
  lcd.print(" %");

  // Predicting precipitaion
  float inputs[] ={t,h};
  float predicted = LR.predict(inputs);
  Serial.print("Precipitation Expected: ");
  Serial.println(predicted);

  // send to firebase
  firebase.setFloat("Precipitation Expected", predicted);
}

