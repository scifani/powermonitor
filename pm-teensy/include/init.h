#include <Arduino.h>
#include <SPI.h>
#include <NativeEthernet.h>

int serial_init(uint64_t baud) {

    Serial.begin(baud);
    delay(50);

    return 0;
}

int eth_init(IPAddress ip_addr, uint8_t* mac, uint32_t timeout=5000) {

  Ethernet.begin(mac, ip_addr);
  
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    return 1;    
  }

  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
    return 1;
  }

  return 0;
}

int led_init(bool led_on=false) {

    uint8_t status = led_on ? LOW : HIGH;

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, status);

    return 0;
}