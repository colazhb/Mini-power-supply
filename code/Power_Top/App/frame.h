#ifndef __FRAME_H
#define __FRAME_H

#include "stdint.h"


#define FRAME_BUF_LEN 128
#define FRAME_MAX_LEN 32
#define FRAME_HEAD 0x55

struct frame_buffer_obj
{
    uint8_t rx_buf[FRAME_BUF_LEN];
    uint8_t tx_buf[FRAME_BUF_LEN];
    uint8_t rx_len;
   uint8_t tx_len;

    //uint8_t rx_index;
    //uint8_t tx_index;
};
enum
{
    FRAME_STATE_HEAD = 0,
    FRAME_STATE_LEN,
    FRAME_STATE_CRC
};
struct frame_obj
{
    struct frame_buffer_obj frame_buffer;
    uint8_t frame_state;
    // 回调函数
    void (*frame_callback)(uint8_t *data, uint8_t len);


    // 发送数据回调
    void (*frame_send_data)(uint8_t data);
    
};

void frame_pack(struct frame_obj *frame, uint8_t *data, uint8_t len);
void frame_push_ch(struct frame_obj *frame, uint8_t ch);
void frame_loop(struct frame_obj *frame);

#endif