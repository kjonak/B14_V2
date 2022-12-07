#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stm32g070xx.h>

#include "time/time.h"

static bool isReceiving = false;
static bool txCompleted = true;
static bool new_data = false;
static uint16_t receivedBytes = 0;
static uint16_t rx_buffer_size = 0;

void USART1_RX_Complete_Callback();

void USART1_NewDataFlagReset()
{
    new_data=false;
}

bool USART1_NewDataFlag(void){
    return new_data;
}
uint16_t USART1_GetReceivedBytes(void){
    uint16_t ret = receivedBytes;
    return ret;
}

bool USART1_Check_Tx_end(void){
    return txCompleted;
}

void USART1_Transmit_DMA(uint8_t* tx_buffer, uint16_t len){

    DMA1_Channel2->CCR &=~(DMA_CCR_EN);
    DMA1_Channel2->CNDTR = len;
    DMA1_Channel2->CMAR = (uint32_t)tx_buffer;
    DMA1_Channel2->CPAR = (uint32_t)&(USART1->TDR);
    txCompleted = false;
    DMA1_Channel2->CCR |=DMA_CCR_EN;
}

static timeUs_t time = 0;

void USART1_Receive_DMA(uint8_t* rx_buffer, uint16_t buffer_size){
    rx_buffer_size = buffer_size;
    DMA1_Channel1->CCR&=~(DMA_CCR_EN);
    time = micros();
    while(DMA1_Channel1->CCR&DMA_CCR_EN)    //wait for it
        if(micros()-time>1000)
            {
            DMA1_Channel1->CCR&= ~(DMA_CCR_EN);
                return;
            }
    DMA1_Channel1->CMAR = (uint32_t)rx_buffer;
    DMA1_Channel1->CNDTR = buffer_size; 
    DMA1_Channel1->CPAR = (uint32_t)&(USART1->RDR);
    DMA1_Channel1->CCR |= DMA_CCR_EN; 
    isReceiving = true;
}
bool USART1_IsReceiving(void)
{
    return isReceiving;
}
void USART1_StopReceiving(void){
    DMA1_Channel1->CCR&= ~(DMA_CCR_EN); 
}

void USART1_IRQHandler(void)
{
  if (USART1->ISR & USART_ISR_IDLE)
    {
        //USART1->DR;                             //If not read usart willl crush                  
        DMA1_Channel1->CCR &= ~DMA_CCR_EN;       /* Disable DMA on stream 5 - trigers dma TC */
        USART1->ICR |= USART_ICR_IDLECF;
        new_data = true;
        receivedBytes = rx_buffer_size - DMA1_Channel1->CNDTR;    //we expected USART1_RX_BUFFER_SIZE NDTR keeps how many bytes left to transfe
        isReceiving = false;
        USART1_RX_Complete_Callback();
    }  
}


void DMA1_Channel1_IRQHandler(void)
{
    if(DMA1->ISR & DMA_ISR_TCIF1){            //if interupt is TC
        DMA1->IFCR = DMA_IFCR_CTCIF1;         //clear tc flag
        new_data = true;
        receivedBytes = rx_buffer_size - DMA1_Channel1->CNDTR;    //we expected USART1_RX_BUFFER_SIZE NDTR keeps how many bytes left to transfe
        isReceiving = false;
        USART1_RX_Complete_Callback();                        
    }
}

//DMA channel 2 = tx channel

void DMA1_Channel2_3_IRQHandler(void)
{
    if(DMA1->ISR & DMA_ISR_TCIF2){ //if interupt is TC
        txCompleted = true;
        DMA1->IFCR = DMA_IFCR_CTCIF2;     //clear tc flag
    }
}