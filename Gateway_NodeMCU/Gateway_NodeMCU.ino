#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include "ThingSpeak.h"
#include "DHT.h"

#define BAUD_RATE 115200 //  Change BAUD_RATE here
#define UPLOAD_STATUS_LED D0
#define DHT_PIN D1
#define DHT_TYPE DHT11        // DHT sensor type
#define SOFTWARE_RX 0         // D3 pin on NodeMCU
#define SOFTWARE_TX 14        // D5 pin on NodeMCU
#define MAX_SERIAL_LENGTH 6   // Maximum serial data length
#define DISTANCE_THRESHOLD 80 //  Change DISTANCE_THRESHOLD here

unsigned long previousMillis = 0;             // Store last updated
const long interval = 30000;                  // Delay interval (milliseconds)
const char *ssid = "Startrek";                // SSID of the network
const char *password = "Face fade12#";        // Password for the network
unsigned long channelNumber = 1586338;        // Thingspeak Channel Number
const char *writeAPIKey = "27F0Q9RGU6DMTQ0B"; // Thingspeak API key

SoftwareSerial softwareSerial(SOFTWARE_RX, SOFTWARE_TX);
WiFiClient client;
DHT dht(DHT_PIN, DHT_TYPE);

void setup()
{
  pinMode(UPLOAD_STATUS_LED, OUTPUT); // Set RELAY_PIN as an OUTPUT

  Serial.begin(BAUD_RATE);         // Init Hardware Serial
  softwareSerial.begin(BAUD_RATE); // Init Hardware Serial
  WiFi_init();                     // Init WiFi
  dht.begin();                     // Init DHT
  ThingSpeak.begin(client);        // Initialize ThingSpeak
} // void setup ends

void loop()
{
  if (WiFi.status() != WL_CONNECTED) // Connect or reconnect to WiFi
  {
    Serial.print("Attempting to connect");
    WiFi_init(); // Init WiFi
  }
  digitalWrite(UPLOAD_STATUS_LED, LOW); //  Set UPLOAD_STATUS_LED to LOW (Turn OFF LED)
  String incomingString = "";
  while (softwareSerial.available() > 0) // Check if incoming serial data exits
  {
    incomingString = softwareSerial.readStringUntil('#'); // Read serial data until "#"
    if (incomingString.startsWith("$"))                   // Check if serial data starts with "$"
    {
      String ultrasonicSensor = "";
      ultrasonicSensor = ultrasonicSensor + incomingString; // Append incoming data to variable
      ultrasonicSensor.remove(0, 1);                        // Remove "$" from the variable
      if ((millis() - previousMillis) >= interval)          // Calculate the intravel
      {
        float humidity = dht.readHumidity();       // Read Humidity
        float temperature = dht.readTemperature(); // Read Temperature
        bool motorStatus = false;
        if (isnan(humidity) || isnan(temperature)) // Check for data
        {
          Serial.println(F("Failed to read from DHT sensor!"));
          return; // Return if data doesn't exit
        }
        digitalWrite(UPLOAD_STATUS_LED, HIGH); //  Set UPLOAD_STATUS_LED to HIGH (Turn ON LED)
        if (ultrasonicSensor.length() != 0)
        {
          if (ultrasonicSensor.toInt() > DISTANCE_THRESHOLD)
          {
            motorStatus = true;
          }
          else
          {
            motorStatus = false;
          }
          Upload_Data(ultrasonicSensor, temperature, humidity, motorStatus);
          digitalWrite(UPLOAD_STATUS_LED, LOW); //  Set UPLOAD_STATUS_LED to LOW (Turn OFF LED)
        }
        previousMillis = millis(); // Assgin current millis
      }
    }
    if (incomingString.length() > MAX_SERIAL_LENGTH) // Check for length of the serial data
    {
      Serial.println("Buffer Overflow");
      break; // Break if length exceeds
    }
  } // while loop ends (softwareSerial)
} // void loop ends

void Upload_Data(String ultrasonicSensor, float temperature, float humidity, bool motorStatus)
{
  /*
  * Function: Upload_Data
  * -------------------------
  *   Uploads data to Thingspeak dashboard.
  *   
  *   ultrasonicSensor: (String) Water Level
  *   temperature: (float) Temperature Value
  *   humidity: (float) Humidity Value
  *   motorStatus: (boolean) Motor Status
  *   
  *   returns:  None
  */
  ThingSpeak.setField(1, ultrasonicSensor);                   // Set Field 1 for Water Level
  ThingSpeak.setField(2, motorStatus);                        // Set Field 2 for Motor Status
  ThingSpeak.setField(3, temperature);                        // Set Field 3 for Temperature
  ThingSpeak.setField(4, humidity);                           // Set Field 4 for Humidity
  int x = ThingSpeak.writeFields(channelNumber, writeAPIKey); // Upload the Data to thingspeak and get status code

  if (x == 200) // Check if upload is success
  {
    Serial.println("Channel update successful.");
  }
  else
  {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
}

void WiFi_init(void)
{
  /*
  * Function: WiFi_init
  * -------------------------
  *   Connects the device to a network.
  *   
  *   parms: void
  *   
  *   returns:  None
  */
  WiFi.begin(ssid, password); // Connect to your WiFi Network
  Serial.println("");

  while (WiFi.status() != WL_CONNECTED) // Wait for connection
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println("WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); // IP address assigned to your Device
}
