
#include <ESP8266WiFi.h>

int serial_init(uint64_t baud) {

    Serial.begin(baud);

    delay(50);

    return 0;
}

int wifi_init(const char* wifi_ssid, const char* wifi_pswd, int& wl_status, uint32_t timeout=5000) {

    WiFi.begin(wifi_ssid, wifi_pswd);

    delay(1500);

    uint32_t elapsed = 0;
    do {
        delay(100);
        elapsed += 100;
    } while (WiFi.status() != WL_CONNECTED && elapsed < timeout);

    wl_status = WiFi.status();

    return (wl_status == WL_CONNECTED) ? 0 : 1;
}

int led_init(bool led_on=false) {

    uint8_t status = led_on ? LOW : HIGH;

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, status);

    return 0;
}

void led_blink(uint8_t count, uint16_t pause, bool init_state=false) {

    uint8_t status = init_state ? HIGH : LOW;

    for(uint8_t i = 0; i < count; i++)
    {
        digitalWrite(LED_BUILTIN, status);
        delay(pause);
        status = (status == HIGH) ? LOW : HIGH;
        digitalWrite(LED_BUILTIN, status);
    }
}
