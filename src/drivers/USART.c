#include "UART1.h"
#include "USART.h"

USART_t USARTs[1] =
{
    [UART1] =
    {
        .CheckTxEnd = USART1_Check_Tx_end,
        .GetReceivedBytes = USART1_GetReceivedBytes,
        .IsReceiving = USART1_IsReceiving,
        .NewDataFlag = USART1_NewDataFlag,
        .NewDataFlagReset = USART1_NewDataFlagReset,
        .ReceiveDMA = USART1_Receive_DMA,
        .StopReceiving = USART1_StopReceiving,
        .TransmitDMA = USART1_Transmit_DMA
    }
};

void USART1_RX_Complete_Callback()
{
    if(USARTs[UART1].RXCompleteCallback)
    {
       USARTs[UART1].RXCompleteCallback(); 
    }
}


USART_t* USART_GetUSART(UART_e usart)
{
    if((usart>=0)&&(usart<3))
        return &USARTs[usart];
    else 
    return 0;
}