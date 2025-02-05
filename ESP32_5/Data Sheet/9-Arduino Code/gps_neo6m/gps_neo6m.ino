#include <TinyGPS++.h>
#include <HardwareSerial.h>

#define GPS_TX 3
#define GPS_RX 4

// The TinyGPS++ object
TinyGPSPlus gps;

// Define the serial port for GPS communication (UART2)
HardwareSerial gpsSerial(2);

void setup() {
  Serial.begin(115200); // Start serial communication for debugging
  gpsSerial.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX); // Initialize UART

  Serial.println("NEO-6M GPS module initialized.");
}

void loop() {
  // This sketch displays information every time a new sentence is correctly encoded.
  unsigned long start = millis();

  while (millis() - start < 1000) {
    while (gpsSerial.available() > 0) {
      gps.encode(gpsSerial.read());
    }
    if (gps.location.isUpdated()) {
      Serial.print("LAT: ");
      Serial.println(gps.location.lat(), 6);
      Serial.print("LONG: "); 
      Serial.println(gps.location.lng(), 6);
      Serial.print("SPEED (km/h) = "); 
      Serial.println(gps.speed.kmph()); 
      Serial.print("ALT (min)= "); 
      Serial.println(gps.altitude.meters());
      Serial.print("HDOP = "); 
      Serial.println(gps.hdop.value() / 100.0); 
      Serial.print("Satellites = "); 
      Serial.println(gps.satellites.value()); 
      Serial.print("Time in UTC: ");
      Serial.println(String(gps.date.year()) + "/" + String(gps.date.month()) + "/" + String(gps.date.day()) + "," + String(gps.time.hour()) + ":" + String(gps.time.minute()) + ":" + String(gps.time.second()));
      Serial.println("");
    }
  }
  delay(1000); // Wait for 1 second before reading again
}
