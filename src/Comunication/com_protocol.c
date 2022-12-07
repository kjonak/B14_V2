#include <stdint.h>
#include <string.h>
#include "com_protocol.h"

#define MSG_HEADER_1 0x69
#define MSG_HEADER_2 0x68

uint16_t calculateChecksum(uint8_t *data_p, uint16_t length)
{
    uint8_t x;
    uint16_t crc = 0xFFFF;

    while (length--)
    {
        x = crc >> 8 ^ *data_p++;
        x ^= x >> 4;
        crc = (crc << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x << 5)) ^ ((uint16_t)x);
    }
    return crc;
}

void COMPROTO_CreateMsg(COMPROTO_msg_t *msg)
{
    msg->buffer_len = 6 + msg->data_len;

    msg->tx_buffer[0] = MSG_HEADER_1;
    msg->tx_buffer[1] = MSG_HEADER_2;
    msg->tx_buffer[2] = msg->data_len;
    msg->tx_buffer[3] = msg->type;
    memcpy(msg->tx_buffer + 4, msg->user_data, msg->data_len);
    uint16_t checksum = calculateChecksum(msg->tx_buffer, msg->buffer_len - 2);
    msg->tx_buffer[msg->buffer_len - 2] = checksum >> 8;
    msg->tx_buffer[msg->buffer_len - 1] = checksum;
}

int COMPROTO_ParseHeader(uint8_t *data)
{
    if ((data[0] != MSG_HEADER_1) && (data[1] != MSG_HEADER_2))
    {
        return -1;
    }
    return (data[2]);
}

void COMPROTO_ParseMsg(uint8_t *user_input, uint16_t len, COMPROTO_msg_info_t *msg_struct)
{
    if ((user_input[0] != MSG_HEADER_1) && (user_input[1] != MSG_HEADER_2))
    {
        msg_struct->valid = false;
        return;
    }

    uint16_t dataLen = (user_input[2]);
    uint16_t checksum = (user_input[len - 2] << 8) | user_input[len - 1];
    uint16_t calc_crc = calculateChecksum(user_input, len - 2);
    if (checksum != calc_crc)
    {
        msg_struct->valid = false;
        return;
    }
    msg_struct->len = dataLen;

    uint8_t Packet_type = user_input[3];
    uint8_t *data = user_input + 4;

    msg_struct->valid = true;
    msg_struct->data = data;
    msg_struct->msg_type = Packet_type;
    msg_struct->len = dataLen;

    return;
}