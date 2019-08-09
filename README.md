# Spectrum analyzer with Murata's Lora module

## Intro
Possile implementation of spectrum analyzer with Murata's LoRa module.

## Board
STMicro's Discovery Board [B-L072Z-LRWAN1](https://www.st.com/en/evaluation-tools/b-l072z-lrwan1.html)

## The description of application  
The goal of this project is to see if Murata's LoRa module could be used as a spectrum analyzer.

[ArduinoCore](https://github.com/GrumpyOldPizza/ArduinoCore-stm32l0) for STM32l0 that was made by [GrumpyOldPizza](https://github.com/GrumpyOldPizza) is used.

The starting point of this project is to implement the readings of RSSI values from RSSI register from SX1276 chip.

Going through Semtech's documentation for SX1276 chip and the source code provided by GrumpyOldPizza it seems that implementing readings of RSSI values should be possible. A function called SX1276ReadRssi is already implemented in sx1276.c file and should be accessible as a method of Radio object which is defined and initialized in the specific radio board implementation.

### Radio.Rssi() approach
New LoraRadio method called readRssi() which was created was only calling Radio.Rssi() method and returning its output. Example of Spectrum_Scan.ino sketch and implementation of LoraRadio.readRssi()can be seen below.

```
#include "LoRaRadio.h"

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
    Serial.println(LoRaRadio.readRssi());
    delay(100);
}
```
```
int16_t LoRaRadioClass::readRssi()
{
    int16_t rssi = Radio.Rssi();
    return rssi;
}
```

The only number that was recived on serial monitor was -157 which is a RSSI offset value, which means that RSSI register was returning 0 all the time. This was happening even when another nearby Lora Module board was transmiting packets on same frequency.  

### Radio.IsChannelFree() approach
In the file LoRaRadio.cpp method LoraRadio.sense() was found which uses Radio.IsChannelFree() method to check RSSI value at certain frequency and compares it to threshold value to determine if channel is free. 

The LoraRadio.sense() worked as expected, it was returning 1 when another Lora Module was off and combination of 1 and 0 when Lora Module was transmiting. 


Radio.IsChannelFree() method was modified so that it does not make any comparisons with threshold value but that it only outputs RSSI value that is returned by SX1276ReadRssi() function. Example of LoraRadio.readRssi() method which calls Radio.IsChannelFree() can be seen below.

```
int16_t LoRaRadioClass::readRssi()
{
    
    IRQn_Type irq;
    bool isChannelFree;

    if (!_initialized) {
        return 0;
    }

    irq = (IRQn_Type)((__get_IPSR() & 0x1ff) - 16);

    if (irq != Reset_IRQn) {
        return 0;
    }

    if (!LoRaRadioCall(__Sense)) {
        return 0;
    }
    

    int16_t rssi = Radio.IsChannelFree(MODEM_LORA, _frequency, -65, 10);

    _busy = 0;
    return rssi;
}
```
```
int16_t SX1276IsChannelFree( RadioModems_t modem, uint32_t freq, int16_t rssiThresh, uint32_t maxCarrierSenseTime )
{
    bool status = true;
    int16_t rssi = 0;
    uint32_t carrierSenseTime = 0;

    SX1276SetStby( );

    SX1276SetModem( modem );

    SX1276SetChannel( freq );

    SX1276SetOpMode( RF_OPMODE_RECEIVER );

    SX1276Delay( 1 );

    carrierSenseTime = armv6m_systick_millis( );
    /*
    // Perform carrier sense for maxCarrierSenseTime
    while( (uint32_t)( armv6m_systick_millis( ) - carrierSenseTime ) < maxCarrierSenseTime )
    {
        rssi = SX1276ReadRssi( );

        if( rssi > rssiThresh )
        {
            status = false;
            break;
        }
    }
    */

    rssi = SX1276ReadRssi( );
    SX1276SetStby( );

    return rssi;
}
```
In this case Radio.IsChannelFree() is still only returning 1 or 0 not matter that body and return type of function SX1276IsChannelFree was changed. Hardcoding RSSI variable to some arbitary numerical value produced same result.

### Conclusion
I was lead to believe that cmwx1zzabz-board.c defines and initializes Radio_s structure which holds pointers to all functions connected with SX1276 chip, most notably SX1276IsChannelFree. Modifying that function had no effect on return value of that function. Using SX1276ReadRssi function directly returned only the minimal possible value, in both of the cases if another Lora Module was transmiting or not. 

Displaying readings of RSSI value of current carrier frequency was not achieved.
