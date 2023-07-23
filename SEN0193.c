#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>

#define ADC_CHANNEL 0 // ADC channel connected to the soil moisture sensor
#define SPI_CHANNEL 0 // SPI channel used for communication (0 for default SPI channel on Raspberry Pi)

int read_soil_moisture() {
    int value = 0;
    unsigned char data[3];

    // Read data from the ADC
    data[0] = 0b00000001; // Start bit, single-ended, channel 0
    data[1] = 0b10000000; // Most significant bit for channel selection
    data[2] = 0;

    wiringPiSPIDataRW(SPI_CHANNEL, data, 3);

    // Convert the received data to a 10-bit integer value
    value = ((data[1] & 0x03) << 8) + data[2];

    return value;
}

int main() {
    if (wiringPiSetup() == -1) {
        printf("wiringPiSetup failed\n");
        return 1;
    }

    int soil_moisture;

    // Initialize the SPI communication with the ADC
    wiringPiSPISetup(SPI_CHANNEL, 1000000); // 1 MHz SPI clock speed

    while (1) {
        // Read soil moisture value from the sensor
        soil_moisture = read_soil_moisture();

        // Print the soil moisture value
        printf("Soil Moisture: %d\n", soil_moisture);

        // Add delay before taking the next reading
        delay(1000); // 1 second delay
    }

    return 0;
}
