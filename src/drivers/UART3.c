#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stm32g070xx.h>

#include "time/time.h"


static bool txCompleted = true;
static bool new_data = false;
static uint16_t receivedBytes = 0;
static uint16_t rx_buffer_size = 0;

void USART3_RX_Complete_Callback()
{

}
void USART3_NewDataFlagReset()
{
    new_data=false;
}

bool USART3_NewDataFlag(void){
    return new_data;
}
uint16_t USART3_GetReceivedBytes(void){
    uint16_t ret = receivedBytes;
    return ret;
}

bool USART3_Check_Tx_end(void){
    return txCompleted;
}

void USART3_Transmit_DMA(uint8_t* tx_buffer, uint16_t len){

    //DMA1_Channel2->CCR&=~(DMA_CCR2_EN);
    txCompleted = false;

    DMA1_Channel2->CNDTR = len;
    DMA1_Channel2->CMAR = (uint32_t)tx_buffer;
   // DMA1_Channel2->CCR |= DMA_CCR2_EN;
}

static timeUs_t time = 0;

void USART3_Receive_DMA(uint8_t* rx_buffer, uint16_t buffer_size){
    rx_buffer_size = buffer_size;
   // DMA1_Channel3->CCR&=~(DMA_CCR3_EN);
    time = micros();
    while(DMA1_Channel3->CCR)//&DMA_CCR3_EN)    //wait for it
            if(micros()-time>1000)
        {
           // DMA1_Channel3->CCR&= ~(DMA_CCR3_EN);
            return;
        }
    DMA1_Channel3->CMAR = (uint32_t)rx_buffer;
    DMA1_Channel3->CNDTR = buffer_size; 
    //DMA1_Channel3->CCR |= DMA_CCR3_EN; 
}
void USART3_StopReceiving(void){
  //  DMA1_Channel3->CCR&= ~(DMA_CCR3_EN); 
}

void USART3_IRQHandler(void)
{
  if (USART3->ISR & USART_ISR_IDLE)
    {
        //USART3->DR;                             //If not read usart willl crush                  
        //DMA1_Channel3->CCR &= ~DMA_CCR3_EN;       /* Disable DMA on stream 5 - trigers dma TC */
    }  
}

void DMA1_Channel3_IRQHandler(void)
{
    if(DMA1->ISR & DMA_ISR_TCIF3){            //if interupt is TC
        DMA1->IFCR = DMA_IFCR_CTCIF3;         //clear tc flag
        new_data = true;
        receivedBytes = rx_buffer_size - DMA1_Channel3->CNDTR;    //we expected USART3_RX_BUFFER_SIZE NDTR keeps how many bytes left to transfe
        USART3_RX_Complete_Callback();                        
    }
}

void DMA1_Channel2_IRQHandler(void)
{
    if(DMA1->ISR & DMA_ISR_TCIF2){ //if interupt is TC
        txCompleted = true;
        DMA1->IFCR = DMA_IFCR_CTCIF2;     //clear tc flag
    }
}