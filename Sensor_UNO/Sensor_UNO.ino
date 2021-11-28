#include <SoftwareSerial.h>

#define BAUD_RATE 115200 //  Change BAUD_RATE here
#define TRIGGER_PIN 10
#define ECHO_PIN 9
#define SOFTWARE_RX 2
#define SOFTWARE_TX 3
#define RELAY_PIN 7
#define DISTANCE_THRESHOLD 7       //  Change DISTANCE_THRESHOLD here
#define BUZZER_MINIMUM_THRESHOLD 5 //  Change BUZZER_MINIMUM_THRESHOLD here
#define TANK_HEIGHT 11
#define BUZZER 6

SoftwareSerial softwareSerial(SOFTWARE_RX, SOFTWARE_TX);

void setup()
{
  pinMode(TRIGGER_PIN, OUTPUT);    // Sets the TRIGGER_PIN as an Output
  pinMode(ECHO_PIN, INPUT);        // Sets the ECHO_PIN as an Input
  pinMode(RELAY_PIN, OUTPUT);      // Set RELAY_PIN as an OUTPUT
  pinMode(BUZZER, OUTPUT);         // Set BUZZER as an OUTPUT
  digitalWrite(RELAY_PIN, HIGH);   // Set RELAY_PIN to HIGH (Turn OFF relay)
  digitalWrite(BUZZER, LOW);       // Set BUZZER to LOW (Turn OFF buzzer)
  Serial.begin(BAUD_RATE);         // Init Hardware Serial
  softwareSerial.begin(BAUD_RATE); // Init Software Serial
} // void setup ends

void loop()
{
  String tx_string = "";
  int waterLevel = 0;

  waterLevel = Read_Sensor();                // Get Distace
  if (waterLevel < BUZZER_MINIMUM_THRESHOLD) //  Check if distance is greater than DISTANCE_THRESHOLD
  {
    digitalWrite(RELAY_PIN, LOW); //  Set RELAY_PIN to LOW (Turn ON relay)
    digitalWrite(BUZZER, HIGH);   // Set BUZZER to HIGH (Turn ON buzzer)
  }
  else if (waterLevel > BUZZER_MINIMUM_THRESHOLD && waterLevel < DISTANCE_THRESHOLD)
  {
    digitalWrite(RELAY_PIN, LOW); //  Set RELAY_PIN to LOW (Turn ON relay)
    digitalWrite(BUZZER, LOW);    // Set BUZZER to LOW (Turn OFF buzzer)
  }
  else
  {
    digitalWrite(RELAY_PIN, HIGH); // Set RELAY_PIN to HIGH (Turn OFF RELAY)
  }
  tx_string = "$" + tx_string + String(waterLevel) + "#"; //  Append start and exit characters
  if (waterLevel != 0)
  {
    Serial.print("tx data: ");
    Serial.println(tx_string);
    softwareSerial.print(tx_string); // Write to software serial
  }
  delay(250);
} // void loop ends

int Read_Sensor(void)
{
  /*
  * Function: Read_Sensor
  * -------------------------
  *   Returns the distance calculated by the Ultrasonic Sensor.
  *   
  *   parms: void
  *   
  *   returns:  distance
  */
  unsigned int distance = 0;
  long duration = 0;
  digitalWrite(TRIGGER_PIN, LOW); // Sets the TRIGGER_PIN on LOW state
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH); // Sets the TRIGGER_PIN on HIGH state for 10 micro seconds
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);     // Sets the TRIGGER_PIN on LOW state
  duration = pulseIn(ECHO_PIN, HIGH); // Reads the ECHO_PIN, returns the sound wave travel time in microseconds
  distance = duration * 0.034 / 2;    // Calculate distance
  distance = TANK_HEIGHT - distance;
  return distance;
}
