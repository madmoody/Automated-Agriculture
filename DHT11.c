#include <stdio.h>
#include <wiringPi.h>

#define DHT_PIN 4   // Replace '4' with the actual GPIO pin number to which the DHT22 sensor is connected

int read_dht_data(float *temperature, float *humidity) {
    int dht_data[5] = {0};
    int counter = 0;

    pinMode(DHT_PIN, OUTPUT);
    digitalWrite(DHT_PIN, LOW);
    delay(18);
    digitalWrite(DHT_PIN, HIGH);
    delayMicroseconds(40);
    pinMode(DHT_PIN, INPUT);

    while (digitalRead(DHT_PIN) == HIGH) {
        if (counter++ >= 1000) {
            return 0;  // Timeout
        }
        delayMicroseconds(1);
    }

    for (int i = 0; i < 40; i++) {
        counter = 0;
        while (digitalRead(DHT_PIN) == LOW) {
            if (counter++ >= 1000) {
                return 0;  // Timeout
            }
            delayMicroseconds(1);
        }

        delayMicroseconds(28);

        if (digitalRead(DHT_PIN) == HIGH) {
            dht_data[i / 8] |= (1 << (7 - (i % 8)));
        }
    }

    if ((uint8_t)(dht_data[0] + dht_data[1] + dht_data[2] + dht_data[3]) == dht_data[4]) {
        *humidity = (float)((dht_data[0] << 8) + dht_data[1]) / 10.0;
        *temperature = (float)(((dht_data[2] & 0x7F) << 8) + dht_data[3]) / 10.0;
        if (dht_data[2] & 0x80) {
            *temperature = -*temperature;
        }
        return 1;  // Successful read
    } else {
        return 0;  // Checksum error
    }
}

int main() {
    if (wiringPiSetup() == -1) {
        printf("wiringPiSetup failed\n");
        return 1;
    }

    float temperature, humidity;

    while (1) {
        if (read_dht_data(&temperature, &humidity)) {
            printf("Temperature: %.1f°C, Humidity: %.1f%%\n", temperature, humidity);
        } else {
            printf("Failed to read data from DHT22 sensor\n");
        }

        delay(2000); // Delay 2 seconds between readings
    }

    return 0;
}
