#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "Timer0A.h"
#include "TempShow.h"
#include "LCD.h"
#define TEMP_SENSOR_CHANNEL 0  // ADC channel connected to temperature sensor (e.g., LM35, LM45)
#define ADC_MAX_VALUE 4095
#define VOLTAGE_REF 3.3

float temperatureCelsius;
uint8_t temperatureCelsius_Update=0; // tempertaureCelsius status variable
void TempRead(void) {
        // Read the ADC value
        uint32_t adcValue = ADC_Read();

        // Convert the ADC value to temperature in Celsius
        temperatureCelsius = ConvertToCelsius(adcValue);
        temperatureCelsius_Update=1; //temperature Celsius has been updated
        // Add delay between readings (e.g., 1 second)
        Timer0A_Wait1ms(100);
        return;
}

void ADC_Init(void) {
    // Enable clock for ADC0 module
    SYSCTL_RCGCADC_R |= 0x1;

    // Enable clock for port E
    SYSCTL_RCGCGPIO_R |= (1 << 4); // Clock for port E

    // Configure PE5 as analog input (ADC0 channel 8)
    GPIO_PORTE_AFSEL_R |= (1 << 5); // Enable alternate function on PE5
    GPIO_PORTE_DEN_R &= ~(1 << 5); // Disable digital function on PE5
    GPIO_PORTE_AMSEL_R |= (1 << 5); // Enable analog function on PE5

    // Configure ADC0
    ADC0_PC_R = 0x1; // Configure for 125k samples/second
    ADC0_SSPRI_R = 0x0123; // Configure the ADC priority
    ADC0_ACTSS_R &= ~0x8; // Disable SS3 during configuration
    ADC0_EMUX_R &= ~0xF000; // Software trigger conversion
    ADC0_SSMUX3_R = 8; // Select channel 8 (AIN8)
    ADC0_SSCTL3_R = 0x6; // Take one sample, set flag at end
    ADC0_ACTSS_R |= 0x8; // Enable sample sequencer 3
}

uint32_t ADC_Read(void) {
    // Start the conversion
    ADC0_PSSI_R = 0x8;

    // Wait for conversion to complete
    while ((ADC0_RIS_R & 0x8) == 0) {
        // Busy wait
    }

    // Get the ADC result
    uint32_t adcValue = ADC0_SSFIFO3_R;

    // Clear the completion flag
    ADC0_ISC_R = 0x8;

    return adcValue;
}

float ConvertToCelsius(uint32_t adcValue) {
    // Convert ADC value to voltage
    float voltage = (float)adcValue / ADC_MAX_VALUE * VOLTAGE_REF;

    // Convert voltage to temperature
    // For LM35, the output voltage is directly proportional to temperature in Celsius
    // For LM45, the output is 10mV/degree Celsius
    //float temperatureCelsius = voltage * 100; // For LM35
    float temperature = voltage / 0.01; // For LM45

    return temperature;
}

void DisplayTemperature(float temperature) {
    // Clear the LCD screen
    LCD_Clear();

    // Display "Temperature: XX.Y C" on the LCD
    LCD_OutString("Temp.:");

    // Display temperature to one decimal place
    int integerPart = (int)temperature;
    int fractionalPart = (int)((temperature - integerPart) * 10);

    LCD_OutUDec(integerPart);
    LCD_data('.');
    LCD_OutUDec(fractionalPart);
    LCD_data('C');
}




