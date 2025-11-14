#include "fsm_communication.h"
#include "fsm_command_parser.h"
#include "main.h"


// Khai báo các biến extern cần thiết
extern UART_HandleTypeDef huart2;
extern CommandType command_flag;
extern uint32_t ADC_value; // Giá trị ADC toàn cục, được cập nhật trong main.c
uint32_t ADC_old;

// Cấu hình Timeout (giả sử dùng HAL_GetTick() để đơn giản, nếu dùng Timer ngắt sẽ phức tạp hơn)
#define TIMEOUT_3S_MS 3000

// Biến FSM
CommFSMState comm_fsm_state = COMM_IDLE;
static uint32_t send_time_start = 0; // Biến lưu thời điểm bắt đầu gửi (dùng cho timeout)

void uart_communiation_fsm(void) {
    char str_tx[20]; // Buffer để chứa chuỗi gửi đi (e.g., !ADC=1234#)

    switch (comm_fsm_state) {
        case COMM_IDLE:
            // Chờ lệnh !RST#
            if (command_flag == CMD_RST) {
                // Chuẩn bị và gửi lệnh !ADC=XXXX# (Sử dụng giá trị ADC_value hiện tại)
                uint16_t len = sprintf(str_tx, "!ADC=%lu\r\n", ADC_value);
                HAL_UART_Transmit(&huart2, (uint8_t*)str_tx, len, 1000);
                ADC_old = ADC_value;
                // Reset cờ lệnh và chuyển trạng thái
                command_flag = CMD_NONE;
                comm_fsm_state = COMM_WAITING_OK;
                send_time_start = HAL_GetTick(); // Bắt đầu tính timeout
            }
            break;

        case COMM_WAITING_OK:
            // Chờ lệnh !OK#
            if (command_flag == CMD_OK) {
                // Nhận !OK#, giao tiếp thành công, trở về IDLE
                command_flag = CMD_NONE;
                comm_fsm_state = COMM_IDLE;
            }else if(command_flag == CMD_RST){
            	comm_fsm_state = COMM_IDLE;
            }else if ((HAL_GetTick() - send_time_start) >= TIMEOUT_3S_MS) {
                // Timeout 3 giây (3000ms)

                // Chuẩn bị và gửi lại gói tin ADC (dùng giá trị ADC_value cũ)
                uint16_t len = sprintf(str_tx, "!ADC=%lu\r\n", ADC_old);
                HAL_UART_Transmit(&huart2, (uint8_t*)str_tx, len, 1000);

                send_time_start = HAL_GetTick(); // Cập nhật thời điểm bắt đầu gửi lại
                // Tiếp tục ở trạng thái COMM_WAITING_OK
            }
            break;

        default:
            comm_fsm_state = COMM_IDLE;
            break;
    }
}
