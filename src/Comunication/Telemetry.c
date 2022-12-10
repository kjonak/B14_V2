#include "Variables.h"
#include "Telemetry.h"
#include "drivers/USART.h"
#include "com_protocol.h"


static USART_t* uart;
static uint8_t tx_buffer[200];
static COMPROTO_msg_t msg;
void TELEMETRY_task(timeUs_t currentTime)
{
    if(!uart->CheckTxEnd())
        return;
    msg.type = MSG_TO_USER_TELEMETRY;
    msg.tx_buffer = tx_buffer;
    msg.user_data = (uint8_t*)VAR_GetFloatPtr(VAR_VELOCITY);
    msg.data_len = 7*sizeof(float);
    COMPROTO_CreateMsg(&msg);
    uart->TransmitDMA(msg.tx_buffer, msg.buffer_len);
}
void TELEMETRY_init()
{
    uart = USART_GetUSART(UART1);
    
}