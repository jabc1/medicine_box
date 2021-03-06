/*
 *  Author: Kaka Xie
 *  brief: lc12s wireless module task process
 */

#include "lc12s_wireless_task.h"
#include "delay.h"
#include "lc12s_wireless.h"

OS_STK LC12S_UART_COM_SEND_TASK_STK[LC12S_UART_COM_SEND_TASK_STK_SIZE];
OS_STK LC12S_UART_COM_RCV_TASK_STK[LC12S_UART_COM_RCV_TASK_STK_SIZE];

OS_EVENT *wireless_com_data_come_sem;


uint32_t my_id = 0x12345678;
uint32_t rcv_id = 0;

uint8_t cal_check_sum(uint8_t *data, uint8_t len)
{
    uint8_t i = 0;
    uint8_t sum = 0;
    for(i = 0; i < len; i++)
    {
        sum += data[i];
    }
    return sum;
}


extern void lc12s_send_com_test(void);
static void send_heart_beat(uint32_t cnt)
{
    uint8_t send_buf[13] = {0};
    send_buf[0] = FRAME_HEADER;
    send_buf[1] = 13;
    send_buf[2] = FRAME_HEART_BEAT;
    send_buf[3] = my_id >> 24;
    send_buf[4] = (my_id >> 16) & 0xff;
    send_buf[5] = (my_id >> 8) & 0xff;
    send_buf[6] = my_id & 0xff;
    send_buf[7] = (uint8_t)(cnt >> 24);
    send_buf[8] = (uint8_t)((cnt >> 16) & 0xff);
    send_buf[9] = (uint8_t)((cnt >> 8) & 0xff);
    send_buf[10] = (uint8_t)(cnt  & 0xff);
    send_buf[11] = cal_check_sum(send_buf, 11);
    send_buf[12] = FRAME_FOOTER;
    lc12s_com_send(send_buf, 13);
}


static void send_test(void)
{
    lc12s_com_send("123456789", 9);
}

void lc12s_send_task(void *pdata)
{
    uint32_t cnt = 0;
    while(1)
    {
//        send_heart_beat(cnt++);
//        send_test();
        delay_ms(1000);
    }
}


uint8_t check_frame_sum(uint8_t *data, uint8_t data_len)
{
    uint8_t sum = 0;
    uint8_t i = 0;
    for( i = 0; i < data_len - 1; i++)
    {
        sum += data[i];
    }
    return (sum == data[data_len - 1]);

}

uint32_t heart_beat_cnt = 0;
uint32_t heart_beat_cnt_2 = 0;
static int frame_proc(uint8_t *frame, uint16_t len)
{
    uint8_t type = frame[0];
    if(len > LC12S_RCV_SIZE - 4)
    {
        return -2;  //parameter error
    }

    switch(type)
    {
        case FRAME_HEART_BEAT:
            heart_beat_cnt_2++;
            rcv_id = frame[4];
            rcv_id |= frame[3] << 8;
            rcv_id |= frame[2] << 16;
            rcv_id |= frame[1] << 24;

            heart_beat_cnt = frame[8];
            heart_beat_cnt |= frame[7] << 8;
            heart_beat_cnt |= frame[6] << 16;
            heart_beat_cnt |= frame[5] << 24;
            printf("frame heart beat %d", heart_beat_cnt);
            break;
        default :
            break;
    }
    return -1;
}

com_rcv_opt_t wireless_rcv_com_opt = {0};

void lc12s_rcv_task(void *pdata)
{
    uint8_t err = 0;
    while(1)
    {
        fifo_data_struct data_tmp;

        OSSemPend(wireless_com_data_come_sem, 0, &err);
        OSSemSet(wireless_com_data_come_sem, 0, &err);

        while(is_fifo_empty(wireless_uart_fifo) == FALSE)
        {
            get_data_from_fifo(wireless_uart_fifo, &data_tmp);
            wireless_rcv_com_opt.rcv_buf[wireless_rcv_com_opt.rcv_cnt] = data_tmp;
            if(wireless_rcv_com_opt.start_flag == TRUE)
            {
                if(wireless_rcv_com_opt.rcv_cnt == 1)
                {
                    wireless_rcv_com_opt.data_len = data_tmp;
                }
                if(wireless_rcv_com_opt.rcv_cnt == wireless_rcv_com_opt.data_len - 1)
                {
                    if(wireless_rcv_com_opt.rcv_buf[wireless_rcv_com_opt.rcv_cnt] == FRAME_FOOTER)
                    {
//                        uint8_t ctype = wireless_rcv_com_opt.rcv_buf[2];
                        wireless_rcv_com_opt.end_flag = TRUE;
                        wireless_rcv_com_opt.start_flag = FALSE;
                        wireless_rcv_com_opt.rcv_cnt = 0;
                        if(check_frame_sum(wireless_rcv_com_opt.rcv_buf, wireless_rcv_com_opt.data_len - 1))
                        {
                            frame_proc(&wireless_rcv_com_opt.rcv_buf[2], wireless_rcv_com_opt.data_len - 4);
                        }

                        //printf("o\n");
                        break;
                    }
                    else
                    {
                        wireless_rcv_com_opt.end_flag = FALSE;
                        wireless_rcv_com_opt.start_flag = FALSE;
                        wireless_rcv_com_opt.rcv_cnt = 0;
                    }
                }
            }
            else
            {
                if(data_tmp == FRAME_HEADER)
                {
                    wireless_rcv_com_opt.start_flag = TRUE;
                    wireless_rcv_com_opt.end_flag = FALSE;
                }
                wireless_rcv_com_opt.rcv_cnt = 0;
            }

            if(wireless_rcv_com_opt.rcv_cnt++ >= sizeof(wireless_rcv_com_opt.rcv_buf) - 1)
            {
                wireless_rcv_com_opt.start_flag = FALSE;
                wireless_rcv_com_opt.end_flag = FALSE;
                wireless_rcv_com_opt.rcv_cnt = 0;
            }
        }

//        delay_ms(1000);
    }
}

