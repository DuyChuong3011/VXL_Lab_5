#ifndef __FSM_COMMAND_PARSER_H
#define __FSM_COMMAND_PARSER_H

#include "main.h"
#include <string.h>

#define MAX_BUFFER_SIZE 30
// Định nghĩa trạng thái của FSM Command Parser
typedef enum {
    CMD_WAITING_START,
    CMD_RECEIVING
} CommandParserState;

// Định nghĩa các lệnh đã nhận
typedef enum {
    CMD_NONE = 0,
    CMD_RST,
    CMD_OK,
    CMD_UNKNOWN
} CommandType;

// Biến toàn cục để lưu trạng thái parser và kết quả
extern CommandParserState cmd_parser_state;
extern CommandType command_flag; // Lưu lệnh đã nhận được (CMD_RST, CMD_OK)

// Khai báo hàm FSM
void command_parser_fsm(void);

#endif /* __FSM_COMMAND_PARSER_H */
