#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "config.h"
#include "time/time.h"
#include "drivers/USART.h"
#include "Comunication/com_protocol.h"
#include "ControlTask/CTRL_Task.h"
#include "ControlTask/odometry.h"

#define COMM_HANDLER_BUFFER_LEN 100

static uint8_t rx_buffer[COMM_HANDLER_BUFFER_LEN];
static uint8_t tx_buffer[COMM_HANDLER_BUFFER_LEN];

static timeUs_t last_time = 0;
static COMPROTO_msg_info_t msg;
static COMPROTO_msg_t msg_from_okon;

static USART_t uart;

enum
{
    NEW_MSG,
    CHUNK
} msg_status;

static bool new_msg = false;
static uint16_t received_bytes = 0;
static uint16_t msg_len = 0;

void HandlePIDRequest();


void uart_rxEnd_irq();

timeUs_t COMHANDLER_TimeSinceLastUpdate()
{
    return micros() - last_time;
}

void rx_ir()
{
    uart.ReceiveDMA(rx_buffer, COMM_HANDLER_BUFFER_LEN);
}
void COMHANDLER_Init()
{
    USART_t *temp = USART_GetUSART(UART1);
    temp->RXCompleteCallback = uart_rxEnd_irq;
    uart = *temp;
    uart.ReceiveDMA(rx_buffer, COMM_HANDLER_BUFFER_LEN);
    msg_from_okon.tx_buffer = tx_buffer;
}

bool COMHANDLER_CheckFun(timeUs_t currentTime, timeUs_t deltaTime)
{
    last_time = currentTime;
    if(!uart.IsReceiving())
        uart.ReceiveDMA(rx_buffer, COMM_HANDLER_BUFFER_LEN);

    return new_msg;
}

void uart_rxEnd_irq()
{
    if (msg_status == NEW_MSG)
    {
        int len = COMPROTO_ParseHeader(rx_buffer);
        if ((len == -1) || (len > COMM_HANDLER_BUFFER_LEN))
        {
            uart.ReceiveDMA(rx_buffer, COMM_HANDLER_BUFFER_LEN);
            return;
        }
        msg_len = 6 + len;
        if ((msg_len) == uart.GetReceivedBytes())
        {
            received_bytes = msg_len;
            new_msg = true;
        }
        else
        {
            msg_status = CHUNK;
            uint16_t count = uart.GetReceivedBytes();
            received_bytes = count;
            uart.ReceiveDMA(rx_buffer + count, msg_len - count);
        }
    }
    else if (msg_status == CHUNK)
    {
        received_bytes += uart.GetReceivedBytes();
        if ((received_bytes) < msg_len)
        {
            uart.ReceiveDMA(rx_buffer + received_bytes, msg_len - received_bytes);
        }
        else
        {
            msg_status = NEW_MSG;
            new_msg = true;
        }
    }
}

void COMHANDLER_Task(timeUs_t t)
{
    new_msg = false;

    COMPROTO_ParseMsg(rx_buffer, received_bytes, &msg);
    if (!msg.valid)
    {
        uart.ReceiveDMA(rx_buffer, COMM_HANDLER_BUFFER_LEN);
        return;
    }

    switch (msg.msg_type)
    {
    case TO_B14_MSG_REQUEST_PID:
        HandlePIDRequest();
        break;

    case TO_B14_MSG_NEW_PID:
        CTRL_UpdatePID(msg.data,msg.len);

        msg_from_okon.data_len = 0;
        msg_from_okon.type = MSG_TO_USER_NEW_PID_CONFIRM;
        msg_from_okon.tx_buffer = tx_buffer;


        COMPROTO_CreateMsg(&msg_from_okon);
        uart.TransmitDMA(msg_from_okon.tx_buffer, msg_from_okon.buffer_len);
        break;

    case TO_B14_MSG_HEART_BEAT:
        CTRL_NewHB();
        break;
    case TO_B14_MSG_STOP_MOTORS:
        CTRL_StopMotors();

        break;
    case TO_B14_MSG_ENABLE_MOTORS:
        CTRL_EnableMotors();
        break;

    case TO_B14_MSG_SET_PWM:
        CTRL_NewInput(msg.data, msg.len);
        break;
    case TO_B14_RESET_ODOMETRY:
        ODO_ResetDist();

    default:

        break;
    }

    uart.ReceiveDMA(rx_buffer, COMM_HANDLER_BUFFER_LEN);
}

void COMHANDLER_SendResponse(uint8_t *data, uint8_t len)
{
    uart.TransmitDMA(data, len);
}

void HandlePIDRequest()
{
    msg_from_okon.user_data = CTRL_SerializePID(&msg_from_okon.data_len);
    msg_from_okon.type = MSG_TO_USER_PID;
    COMPROTO_CreateMsg(&msg_from_okon);
  //  free(msg_from_okon.user_data);

    uart.TransmitDMA(msg_from_okon.tx_buffer, msg_from_okon.buffer_len);
}



void COMHANDLER_SendHeartBeat()
{
    msg_from_okon.data_len = 0;
    msg_from_okon.type = MSG_TO_USER_HEART_BEAT;
    COMPROTO_CreateMsg(&msg_from_okon);
    uart.TransmitDMA(msg_from_okon.tx_buffer, msg_from_okon.buffer_len);
}