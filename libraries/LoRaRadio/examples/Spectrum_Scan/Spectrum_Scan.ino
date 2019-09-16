#include "LoRaRadio.h"

#define START_FREQ      8600 //780.0 MHz
#define END_FREQ        8700 //950.0 MHz
#define STEP_FREQ       1 //.5 MHz
#define MAX_NUM_SAMPLES 300 //

int16_t buffer[MAX_NUM_SAMPLES][2];

void setup( void )
{
    Serial.begin(9600);
    
    while (!Serial) { }

    LoRaRadio.begin(868000000);

    LoRaRadio.setFrequency(868000000);
    LoRaRadio.setTxPower(14);
    LoRaRadio.setBandwidth(LoRaRadio.BW_125);
    LoRaRadio.setSpreadingFactor(LoRaRadio.SF_7);
    LoRaRadio.setCodingRate(LoRaRadio.CR_4_5);
    LoRaRadio.setLnaBoost(true);
}

void loop( void )
{
    uint32_t start = armv6m_systick_millis( );
    uint16_t num_steps = LoRaRadio.readSpectrumRssi(buffer, 
                                                    MAX_NUM_SAMPLES, 
                                                    START_FREQ, 
                                                    END_FREQ, 
                                                    STEP_FREQ);
    
    uint32_t end = armv6m_systick_millis( );
    if(num_steps != 0)
    {
        Serial.println("Spectrum reading done");
    }
    else
    {
        Serial.println("Spectrum reading not done!");
        while(1);
    }

    Serial.print("Elapsed time: ");
    Serial.print(end - start);
    Serial.println(" ms");

    for(uint16_t i = 0; i < num_steps; i++)
    {
        Serial.print(buffer[i][0]);
        Serial.print(", ");
        Serial.println(buffer[i][1]);
            }

    while(1);
}
