#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    void (*TransmitDMA)(uint8_t* tx_Buffer, uint16_t len);
    void (*ReceiveDMA)(uint8_t* rx_buffer, uint16_t buffer_len);
    void (*StopReceiving) ();
    bool (*NewDataFlag)();
    void (*NewDataFlagReset)();
    bool (*CheckTxEnd)();
    bool (*IsReceiving)();
    uint16_t (*GetReceivedBytes)();
    void (*RXCompleteCallback)();
}USART_t;

typedef enum
{
    UART1,

} UART_e;

USART_t* USART_GetUSART(UART_e usart);