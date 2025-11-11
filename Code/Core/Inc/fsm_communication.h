#ifndef __FSM_COMMUNICATION_H
#define __FSM_COMMUNICATION_H

#include "main.h"
#include <stdio.h>

// Định nghĩa trạng thái của FSM Communication
typedef enum {
    COMM_IDLE,
    COMM_WAITING_RST_ACK,
    COMM_SENDING_ADC,
    COMM_WAITING_OK
} CommFSMState;

// Biến toàn cục FSM
extern CommFSMState comm_fsm_state;

// Khai báo hàm FSM
void uart_communiation_fsm(void);

#endif /* __FSM_COMMUNICATION_H */
