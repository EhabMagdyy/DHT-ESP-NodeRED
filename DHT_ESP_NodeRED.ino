/*****************************************************************************************************************************
**********************************    Author  : Ehab Magdy Abdullah                      *************************************
**********************************    Linkedin: https://www.linkedin.com/in/ehabmagdyy/  *************************************
**********************************    Youtube : https://www.youtube.com/@EhabMagdyy      *************************************
******************************************************************************************************************************/
#include <WiFi.h>  
#include <PubSubClient.h>
#include "DHT.h"

/* Digital pin connected to the DHT sensor */
#define DHT_PIN       4

/* I'm using DHT22, if you use DHT11 you can change DHT22 to DHT11 */
#define DHT_TYPE      DHT22

DHT dht(DHT_PIN, DHT_TYPE);

const char* ssid = "2001";                        /* Your Wifi SSID */
const char* password = "19821968";                /* Your Wifi Password */
const char* mqtt_server = "test.mosquitto.org";   /* Mosquitto Server URL */

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

    Serial.println("");
    Serial.println("WiFi connected");
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

    humidity = dht.readHumidity();
    temperature = dht.readTemperature();

    char h[3] = {0};
    char t[3] = {0};

    h[0] = (uint8_t)humidity / 10 + '0';
    h[1] += (uint8_t)humidity % 10 + '0';
    t[0] += (uint8_t)temperature / 10 + '0';
    t[1] += (uint8_t)temperature % 10 + '0';

    /* Sending Data to Node-Red */
    client.publish("Ehab/DHT/Humidity", h, false);
    client.publish("Ehab/DHT/Temp", t, false); 

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.print("% - Temperature: ");
    Serial.print(temperature);
    Serial.println("°C");

    delay(2000);
}
