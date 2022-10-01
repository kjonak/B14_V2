#include "drivers/GPIO.h"
#include "stm32f10x_spi.h"
#include "config.h"

void SetPinCS(bool value)
{
    if(value)
        GPIOSetPin(FLASH_CS_BUS, FLASH_CS_PIN);
    else
        GPIOResetPin(FLASH_CS_BUS, FLASH_CS_PIN);
}
void SetPinHOLD(bool value)
{
    if(value)
        GPIOSetPin(FLASH_HLD_BUS, FLASH_HLD_PIN);
    else
        GPIOResetPin(FLASH_HLD_BUS, FLASH_HLD_PIN);
}
void SetPinWP(bool value)
{
    if(value)
        GPIOSetPin(FLASH_WP_BUS, FLASH_WP_PIN);
    else
        GPIOResetPin(FLASH_WP_BUS, FLASH_WP_PIN);
}


void SPI_Transmit(uint8_t* data, uint16_t size)
{

    uint16_t i = 0;
    while(size){
        SPI_I2S_SendData(SPI1, data[i]);
        i++;
        size--;
        }
}

void SPI_Receive(uint8_t* data, uint16_t size)
{

    uint16_t i = 0;
    while(size)
    {
        data[i] = SPI_I2S_ReceiveData(SPI1);
        i++;
        size--;
    }
}