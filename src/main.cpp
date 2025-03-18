#include <TinyWireM.h>
#include <Tiny_BME280.h>
#define Wire TinyWireM

#define TX_PIN PB3
#define BIT_DELAY 417  // 2400 baud at 8MHz

Tiny_BME280 bme;

void serial_init() {
    pinMode(TX_PIN, OUTPUT);
    digitalWrite(TX_PIN, HIGH);
}

void serial_write(char c) {
    digitalWrite(TX_PIN, LOW);
    delayMicroseconds(BIT_DELAY);

    for (uint8_t i = 0; i < 8; i++) {
        digitalWrite(TX_PIN, (c & (1 << i)) ? HIGH : LOW);
        delayMicroseconds(BIT_DELAY);
    }

    digitalWrite(TX_PIN, HIGH);
    delayMicroseconds(BIT_DELAY);
}

void serial_print(const char *str) {
    while (*str) serial_write(*str++);
}

void serial_print_int(int value, int width) {
    char buf[8];
    itoa(value, buf, 10);
    int len = strlen(buf);
    for (int i = len; i < width; i++) serial_write('0');
    serial_print(buf);
}

bool validate_reading(int new_value, int prev_value) {
    if (prev_value == 0) return true;
    int diff = abs(new_value - prev_value);
    return (diff <= (prev_value / 5)); // within 20%
}

int avg(int vals[], int count) {
    int sum = 0;
    for (int i = 0; i < count; i++) sum += vals[i];
    return sum / count;
}

void setup() {
    Wire.begin();
    serial_init();

    while (!bme.begin()) {
        serial_print("X");
        delay(1000);
    }
}

void loop() {
    static int temps[3] = {0}, hums[3] = {0}, press[3] = {0};
    static int idx = 0;
    static unsigned long last_read = 0;

    if (millis() - last_read >= 2000) {
        int t = bme.readTemperature() * 100;
        int h = bme.readHumidity() * 100;
        int p = bme.readPressure() / 100;

        if (validate_reading(t, temps[(idx+2)%3])) temps[idx] = t;
        if (validate_reading(h, hums[(idx+2)%3])) hums[idx] = h;
        if (validate_reading(p, press[(idx+2)%3])) press[idx] = p;

        idx = (idx + 1) % 3;
        last_read = millis();
    }

    int t_avg = avg(temps, 3);
    int h_avg = avg(hums, 3);
    int p_avg = avg(press, 3);

    serial_print_int(t_avg, 4);
    serial_print_int(h_avg, 4);
    serial_print_int(p_avg, 4);
    serial_print("\r\n");
}
