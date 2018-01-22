#include <RCSwitch.h>
#include <DHT.h>

RCSwitch tempSwitch = RCSwitch();

#define HEADER 0x2D
#define TEMP_STEP 0.0049438476f
#define TEMP_MIN -30.f
#define TEMP_MAX 50.f

#define SENSOR_ID 2
#define DHTPIN 2  
#define DHTTYPE DHT22 

DHT dht(DHTPIN, DHTTYPE);
int chk;
float hum;  
float temp;

unsigned int getTemperatureCode(float temperature) {
  float x = (temperature - TEMP_MIN)/TEMP_STEP;
  unsigned int code = (unsigned int)x;
  return code;
}

unsigned long createTemperatureMessage(byte id, float temperature) {
  unsigned long result = 0;

  // 6 bit header
  result |= HEADER;

  // 6 bit id
  result = result << 6;
  result |= (id & 0x3F);

  // 14 bits - temperature code
  const unsigned int tempCode = getTemperatureCode(temperature);
  result = result << 14;
  result |= tempCode & 0x3FFF;
  
  unsigned int checksum = tempCode;
  checksum ^= id;
  checksum ^= HEADER;
  checksum ^= (tempCode >> 8);
  
  // 6 bits checksum
  result = result << 6;
  result |= (checksum & 0x3F);

  return result;
}

void setup() {
  tempSwitch.enableTransmit(8);
  Serial.begin(9600);
  dht.begin();

  pinMode(13, OUTPUT);
}

void loop() {  
  hum = dht.readHumidity();
  temp= dht.readTemperature();
  
  unsigned long message = createTemperatureMessage(SENSOR_ID, temp);
  tempSwitch.send(message, 32);

  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);
  
  delay(5000);
}