/*****************************************************************************************************************
***************************    Author  : Ehab Magdy Abdullah                      ********************************
***************************    Linkedin: https://www.linkedin.com/in/ehabmagdyy/  ********************************
***************************    Youtube : https://www.youtube.com/@EhabMagdyy      ********************************
******************************************************************************************************************/

#ifdef ESP32
#include <WiFi.h>
#elif ESP8266
#include <ESP8266WiFi.h>
#endif
#include <PubSubClient.h>
#include "DHT.h"

// Digital pin connected to the DHT sensor
#define DHT_PIN       4

// I'm using DHT22, if you use DHT11 you can change DHT22 to DHT11 
#define DHT_TYPE      DHT22

DHT dht(DHT_PIN, DHT_TYPE);

#define ssid        "Your_Wifi_SSID"       /* Enter Your Wifi SSID */
#define password    "Your_Wifi_Password"   /* Enter Your Wifi Password */
#define mqtt_server "test.mosquitto.org"   /* Mosquitto Server URL */

WiFiClient espClient;
PubSubClient client(espClient);

float humidity;
float temperature;

void setup_wifi()
{ 
    delay(10);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.print(ssid);
    WiFi.begin(ssid, password);

    while(WiFi.status() != WL_CONNECTED) 
    { 
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void reconnect() 
{ 
    while(!client.connected()) 
    {
        Serial.println("Attempting MQTT connection...");

        if(client.connect("ESPClient")) 
        {
            Serial.println("Connected");
            client.subscribe("Ehab/DHT/Humidity");
            client.subscribe("Ehab/DHT/Temp");
        } 
        else 
        {
            Serial.print("Failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void setup()
{
    Serial.begin(115200);
    setup_wifi(); 
    client.setServer(mqtt_server, 1883);
    dht.begin();
}

void loop()
{
    if(!client.connected()) { reconnect(); }

    // Reading humidity & temperature values
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    // Check if readings are valid
    if (isnan(humidity) || isnan(temperature)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }

    char h[5];
    char t[5];

    snprintf(h, sizeof(h), "%.1f", humidity);
    snprintf(t, sizeof(t), "%.1f", temperature);

    // Publish humidity & temperature to Node-Red
    client.publish("Ehab/DHT/Humidity", h, false);
    client.publish("Ehab/DHT/Temp", t, false);


    // printing humidity & temperature on the serial monitor
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.print("% - Temperature: ");
    Serial.print(temperature);
    Serial.println("°C");

    delay(2000);
}
