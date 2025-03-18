#include <TinyWireM.h>  // Include TinyWireM library for I2C communication
#include <Tiny_BME280.h>  // Include Tiny_BME280 library for BME280 sensor
#define Wire TinyWireM  // Define Wire as TinyWireM for compatibility

#define TX_PIN PB3  // Define TX_PIN as PB3 for serial communication
#define BIT_DELAY 417  // Define bit delay for 2400 baud rate at 8MHz

Tiny_BME280 bme;  // Create an instance of Tiny_BME280

// Initialize serial communication
void serial_init() {
    pinMode(TX_PIN, OUTPUT);  // Set TX_PIN as output
    digitalWrite(TX_PIN, HIGH);  // Set TX_PIN high (idle state)
}

// Write a character to the serial port
void serial_write(char c) {
    digitalWrite(TX_PIN, LOW);  // Start bit
    delayMicroseconds(BIT_DELAY);

    // Send each bit of the character
    for (uint8_t i = 0; i < 8; i++) {
        digitalWrite(TX_PIN, (c & (1 << i)) ? HIGH : LOW);
        delayMicroseconds(BIT_DELAY);
    }

    digitalWrite(TX_PIN, HIGH);  // Stop bit
    delayMicroseconds(BIT_DELAY);
}

// Print a string to the serial port
void serial_print(const char *str) {
    while (*str) serial_write(*str++);  // Write each character of the string
}

// Print an integer to the serial port with leading zeros
void serial_print_int(int value, int width) {
    char buf[8];
    itoa(value, buf, 10);  // Convert integer to string
    int len = strlen(buf);
    for (int i = len; i < width; i++) serial_write('0');  // Add leading zeros
    serial_print(buf);  // Print the integer string
}

// Validate sensor reading by comparing with previous value
bool validate_reading(int new_value, int prev_value) {
    // Define valid ranges for temperature, humidity, and pressure
    const int MIN_TEMP = -4000;  // Minimum temperature in hundredths of degrees Celsius (-40.00°C)
    const int MAX_TEMP = 8500;   // Maximum temperature in hundredths of degrees Celsius (85.00°C)
    const int MIN_HUM = 0;       // Minimum humidity in hundredths of percent (0.00%)
    const int MAX_HUM = 10000;   // Maximum humidity in hundredths of percent (100.00%)
    const int MIN_PRESS = 300;   // Minimum pressure in hPa (300 hPa)
    const int MAX_PRESS = 1100;  // Maximum pressure in hPa (1100 hPa)

    // Check if the new value is within the valid range
    if ((new_value < MIN_TEMP || new_value > MAX_TEMP) &&
        (new_value < MIN_HUM || new_value > MAX_HUM) &&
        (new_value < MIN_PRESS || new_value > MAX_PRESS)) {
        return false;  // Return false if the new value is out of range
    }

    if (prev_value == 0) return true;  // If no previous value, accept new value
    int diff = abs(new_value - prev_value);  // Calculate difference
    if (diff <= (prev_value / 5)) {  // Accept if within 20% of previous value
        return true;
    } else {
        // Adjust the new value slightly towards the previous value
        if (new_value > prev_value) {
            new_value = prev_value + (prev_value / 100);  // Increase by 1%
        } else {
            new_value = prev_value - (prev_value / 100);  // Decrease by 1%
        }
        return true;
    }
}

// Calculate average of an array of integers
int avg(int vals[], int count) {
    int sum = 0;
    for (int i = 0; i < count; i++) sum += vals[i];  // Sum all values
    return sum / count;  // Return average
}

// Setup function, runs once at startup
void setup() {
    Wire.begin();  // Initialize I2C communication
    serial_init();  // Initialize serial communication

    // Try to initialize BME280 sensor, retry if failed
    while (!bme.begin()) {
        serial_print("X");  // Print 'X' if initialization fails
        delay(1000);  // Wait for 1 second before retrying
    }
}

// Main loop function, runs repeatedly
void loop() {
    static int temps[3] = {0}, hums[3] = {0}, press[3] = {0};  // Arrays to store sensor readings
    static int idx = 0;  // Index for circular buffer
    static unsigned long last_read = 0;  // Timestamp of last reading

    // Read sensor data every 2 seconds
    if (millis() - last_read >= 2000) {
        int t = bme.readTemperature() * 100;  // Read temperature and scale
        int h = bme.readHumidity() * 100;  // Read humidity and scale
        int p = bme.readPressure() / 100;  // Read pressure and scale

        // Validate and store readings in circular buffer
        if (validate_reading(t, temps[(idx+2)%3])) temps[idx] = t;
        if (validate_reading(h, hums[(idx+2)%3])) hums[idx] = h;
        if (validate_reading(p, press[(idx+2)%3])) press[idx] = p;

        idx = (idx + 1) % 3;  // Update index for circular buffer
        last_read = millis();  // Update timestamp of last reading
    }

    // Calculate averages of the readings
    int t_avg = avg(temps, 3);
    int h_avg = avg(hums, 3);
    int p_avg = avg(press, 3);

    // Print averages to serial port
    serial_print_int(t_avg, 4);
    serial_print_int(h_avg, 4);
    serial_print_int(p_avg, 4);
    serial_print("\r\n");  // Print newline
}
