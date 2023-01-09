#pragma once
#include <stdint.h>
#include <stdbool.h>
typedef enum
{
    TO_B14_MSG_REQUEST_PID = 0x23,
    TO_B14_MSG_NEW_PID = 0x24,
    TO_B14_MSG_STOP_MOTORS = 0x26,
    TO_B14_MSG_ENABLE_MOTORS = 0x27,
    TO_B14_MSG_SET_PWM = 0x01,
    TO_B14_RESET_ODOMETRY = 0x02
}FROM_USER_MSG_e;

typedef enum
{
        MSG_TO_USER_TELEMETRY = 0x04,
        MSG_TO_USER_NEW_PID_CONFIRM = 0x2,
        MSG_TO_USER_PID = 0x1,
}TO_USER_MSG_e;

typedef struct 
{
    TO_USER_MSG_e type;
    uint8_t data_len;
    uint8_t *user_data;
    uint8_t *tx_buffer;
    uint8_t buffer_len;
}COMPROTO_msg_t;

typedef struct
{
    FROM_USER_MSG_e msg_type;
    uint8_t* data;
    uint16_t len;
    bool valid;
}COMPROTO_msg_info_t;

void COMPROTO_CreateMsg(COMPROTO_msg_t* msg);
void COMPROTO_ParseMsg(uint8_t* user_input, uint16_t len, COMPROTO_msg_info_t* msg_struct);
int COMPROTO_ParseHeader(uint8_t* data);  