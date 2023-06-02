#include <Wire.h>
#include <Adafruit_INA219.h>
#include <LiquidCrystal.h>
#include <ArduinoBLE.h>
BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214");
BLEByteCharacteristic switchCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
Adafruit_INA219 ina219 = Adafruit_INA219();

const int ledPin = LED_BUILTIN;
void setup(void) {
  Serial.begin(9600);
  while (!Serial){
    delay(1);
  }

  pinMode(ledPin, OUTPUT);

  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");
 
    while (1);
  }

  BLE.setLocalName("LED");
  BLE.setAdvertisedService(ledService);

  ledService.addCharacteristic(switchCharacteristic);
  BLE.addService(ledService);
  switchCharacteristic.writeValue(0);
  BLE.advertise();
  Serial.println("BLE LED Peripheral");
  
  lcd.begin(20, 4);
  lcd.setCursor(0, 0);
  lcd.print("Vatimetro");
  lcd.setCursor(0, 1);
  lcd.print("digital DC.");
  delay(2000);
  lcd.clear();
  uint32_t CurrentFrequency;

  if (! ina219.begin()){
    lcd.setCursor(0, 0);
    lcd.print("Failed to find");
    lcd.setCursor(0, 1);
    lcd.print("INA 219 chip.");
    while (1) {delay(10);}
  }
  lcd.clear();
  
  ina219.setCalibration_32V_2A();
  lcd.setCursor(0, 0);
  lcd.print("Measuring Voltage");
  lcd.setCursor(0, 1);
  lcd.print("and current with");
  lcd.setCursor(0, 2);
  lcd.print("INA219...");
  delay(2000);
  lcd.clear();  
}

void loop(void) {
  float shuntvoltage, busvoltage, current_mA, loadvoltage, power_mW = 0;
  lcd.clear();
  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getPower_mW();
  int potencia = power_mW/1000;
  loadvoltage = busvoltage + (shuntvoltage/1000);
  lcd.setCursor(0, 0);
  lcd.print("Voltage: "); lcd.print(loadvoltage); lcd.print("V");
  lcd.setCursor(0, 1);
  lcd.print("Current: "); lcd.print(current_mA); lcd.print("mA");
  lcd.setCursor(0, 2);
  lcd.print("Power: "); lcd.print(power_mW); lcd.print("mW");
  Serial.write(potencia);
  delay(1000);

  BLEDevice central = BLE.central();
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());
  
 while (central.connected()) {
        if (switchCharacteristic.written()) {
          if (switchCharacteristic.value()) {   
            Serial.println("LED on");
            digitalWrite(ledPin, LOW); // changed from HIGH to LOW       
          } else {                              
            Serial.println(F("LED off"));
            digitalWrite(ledPin, HIGH); // changed from LOW to HIGH     
          }
        }
      }
 
    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }




  
}
