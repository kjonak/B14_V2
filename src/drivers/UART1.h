#ifndef USART1_H
#define USART1_H
#include <stdbool.h>
#include <stdint.h>

void USART1_Transmit_DMA(uint8_t *tx_buffer, uint16_t len);
void USART1_Receive_DMA(uint8_t *rx_buffer, uint16_t buffer_size); // once enabled it will serve forever untill stopreceiving will be called
void USART1_StopReceiving(void);
void USART1_NewDataFlagReset();
bool USART1_NewDataFlag(void);
bool USART1_Check_Tx_end(void); // its important to check it before new operation
bool USART1_IsReceiving();
uint16_t USART1_GetReceivedBytes(void); // returns number of bytes that sits in rx buffer

#endif