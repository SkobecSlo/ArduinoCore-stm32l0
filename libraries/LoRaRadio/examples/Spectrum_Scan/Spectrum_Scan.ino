#include "LoRaRadio.h"

uint32_t start_freq = 100000000; //100 MHz
uint32_t end_freq = 1000000000; //1 GHz
uint32_t step_freq = 1000000; //1MHz
int16_t buffer[900];
void setup( void )
{
    Serial.begin(115200);
    
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
uint16_t i = 0;
    for(uint32_t freq = start_freq; freq<end_freq; freq += step_freq)
    {
        buffer[i] = LoRaRadio.readRssi(freq);
        i++;
    }
    for(uint16_t f = 0; f < 900; f++)
    {
        Serial.println(buffer[f]);
    }

    delay(10000);
}
