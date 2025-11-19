#include "fsm_command_parser.h"
#include "main.h"

extern uint8_t buffer[MAX_BUFFER_SIZE];
extern uint8_t index_buffer;
extern uint8_t buffer_flag;

// Biến FSM
CommandParserState cmd_parser_state = CMD_WAITING_START;
CommandType command_flag = CMD_NONE;


void command_parser_fsm(void) {
    uint8_t data = buffer[index_buffer - 1]; // Lấy ký tự mới nhất trong buffer

    if (data == 8 || data == 127) { // 8: Backspace, 127: Delete
            // Ký tự backspace đã chiếm index_buffer - 1.
            // Cần xóa ký tự backspace VÀ ký tự trước đó.

            if (index_buffer > 2) {
                // Nếu có ít nhất 2 ký tự trong buffer (1 ký tự lệnh + 1 backspace)
                // Giảm index_buffer 2 lần:
                // 1. Xóa ký tự backspace (data)
                // 2. Xóa ký tự lệnh đứng trước nó
                index_buffer -= 2;
            } else{
                index_buffer = 0;
                // Quay về trạng thái chờ
                cmd_parser_state = CMD_WAITING_START;
            }
            return;
    }

    switch (cmd_parser_state) {
        case CMD_WAITING_START:
            // Chờ ký tự bắt đầu lệnh '!'
            if (data == '!') {
                cmd_parser_state = CMD_RECEIVING;
            }
            // Nếu không phải '!', có thể là rác, giữ nguyên trạng thái
            break;

        case CMD_RECEIVING:
            // Chờ ký tự kết thúc lệnh '#'
            if (data == '#') {

                // Kiểm tra lệnh !RST# (Độ dài 5)
                if (index_buffer >= 5) {
                    if (buffer[index_buffer - 5] == '!' &&
                        buffer[index_buffer - 4] == 'R' &&
                        buffer[index_buffer - 3] == 'S' &&
                        buffer[index_buffer - 2] == 'T' &&
                        buffer[index_buffer - 1] == '#') {
                        command_flag = CMD_RST;
                    }
                }

                // Kiểm tra lệnh !OK# (Độ dài 4)
                if (command_flag == CMD_NONE && index_buffer >= 4) {
                    if (buffer[index_buffer - 4] == '!' &&
                        buffer[index_buffer - 3] == 'O' &&
                        buffer[index_buffer - 2] == 'K' &&
                        buffer[index_buffer - 1] == '#') {
                        command_flag = CMD_OK;
                    }
                }

                // Nếu không phải !RST# hoặc !OK#, sẽ là CMD_UNKNOWN nếu muốn xử lý

                // Đã xử lý lệnh xong, reset buffer và trở lại trạng thái chờ
                index_buffer = 0; // Reset index_buffer
                cmd_parser_state = CMD_WAITING_START;
            }
            // Nếu data là '!' trong khi đang nhận, đây là bắt đầu lệnh mới -> reset buffer và bắt đầu lại
            else if (data == '!') {
                 index_buffer = 1; // Chỉ giữ lại ký tự '!' hiện tại
                 // Tiếp tục ở trạng thái CMD_RECEIVING
            }

            break;

        default:
            cmd_parser_state = CMD_WAITING_START;
            break;
    }
}
