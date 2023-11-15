#include <NewPing.h>
#define TRIGGER_PIN_1  12 
#define ECHO_PIN_1     11 
#define TRIGGER_PIN_2  6 
#define ECHO_PIN_2     7 
#define TRIGGER_PIN_3  4 
#define ECHO_PIN_3      3 
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sensor1(TRIGGER_PIN_1, ECHO_PIN_1, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
NewPing sensor2(TRIGGER_PIN_1, ECHO_PIN_2, MAX_DISTANCE);
NewPing sensor3(TRIGGER_PIN_1, ECHO_PIN_3, MAX_DISTANCE);

void setup() {
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
}


void loop() {
  delay(5000);                      // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  unsigned int uS1 = sensor1.ping(); // Send ping, get ping time in microseconds (uS).
  Serial.print("Ping 1: ");
  Serial.print(sensor1.convert_cm(uS1)); // Convert ping time to distance and print result (0 = outside set distance range, no ping echo)
  Serial.println("cm");
  unsigned int uS2 = sensor2.ping(); // Send ping, get ping time in microseconds (uS).
  Serial.print("Ping 2: ");
  Serial.print(sensor2.convert_cm(uS2)); // Convert ping time to distance and print result (0 = outside set distance range, no ping echo)
  Serial.println("cm");
  unsigned int uS3 = sensor3.ping(); // Send ping, get ping time in microseconds (uS).
  Serial.print("Ping 3: ");
  Serial.print(sensor3.convert_cm(uS3)); // Convert ping time to distance and print result (0 = outside set distance range, no ping echo)
  Serial.println("cm");
}