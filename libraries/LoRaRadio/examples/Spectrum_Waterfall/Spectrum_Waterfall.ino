#include "LoRaRadio.h"

#define START_FREQ      8600 //780.0 MHz
#define END_FREQ        8700 //950.0 MHz
#define STEP_FREQ       1 //.5 MHz
#define SAMPLES_PER_SCAN 101 //
#define SCANS       20

int16_t buffer[SAMPLES_PER_SCAN * SCANS][2];

void setup( void )
{
    Serial.begin(19200);
    
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
    uint16_t num_points = LoRaRadio.readWaterfallRssi(buffer, 
                                                    SAMPLES_PER_SCAN, 
                                                    START_FREQ, 
                                                    END_FREQ, 
                                                    STEP_FREQ,
                                                    SCANS);
    
    uint32_t end = armv6m_systick_millis( );

    if(num_points != 0)
    {
        Serial.print("Spectrum reading done, number of points per scan is ");
        Serial.println(num_points);
    }
    else
    {
        Serial.println("Spectrum reading not done!");
        while(1);
    }

    Serial.print("Elapsed time: ");
    Serial.print(end - start);
    Serial.println(" ms");
    
    //Print out parameters for for python procesing
    Serial.println("START_FREQ, END_FREQ, STEP_FREQ, SCANS, NUM_POINTS");
    Serial.print(START_FREQ);
    Serial.print(", ");
    Serial.print(END_FREQ);
    Serial.print(", ");
    Serial.print(STEP_FREQ);
    Serial.print(", ");
    Serial.print(SCANS);
    Serial.print(", ");
    Serial.println(num_points);

    LoRaRadio.printWaterfallRssi(buffer, num_points, SCANS);
    
    while(1);
}
