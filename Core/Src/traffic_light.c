#include "traffic_light.h"
#include "input_reading.h"
#include "led_display.h"

// --- Biến trạng thái và thời gian cấu hình ---
int status_traffic = MODE_1;

int red_time = 5;
int green_time = 3;
int yellow_time = 2;

// --- Trạng thái FSM cho Mode 1 ---
TrafficState state_traffic = INIT; // Lưu ý: Kiểu TrafficState nên được định nghĩa trong traffic_light.h để các file khác có thể dùng nếu cần

// --- Biến đếm ngược hiển thị lên LED 7 đoạn ---
int countdown1 = 0;
int countdown2 = 0;

// --- Biến nội bộ (chỉ dùng trong file này) ---
static int timer_counter = 0;

// --- Biến ngoại vi từ timer.c ---
extern int blink_flag;

void fsm_for_traffic_lights(void) {
    switch (state_traffic) {
        case INIT:
            state_traffic = AUTO_GREEN_RED;
            timer_counter = green_time;
            countdown1 = green_time;
            countdown2 = red_time;
            break;

        case AUTO_GREEN_RED:
            // --- Bắc-Nam: XANH (PA2), Đông-Tây: ĐỎ (PA3) ---
            // Bắc-Nam: Tắt Đỏ (PA0 -> SET), Tắt Vàng (PA1 -> SET), Bật Xanh (PA2 -> RESET)
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);   // Đỏ BN OFF
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);   // Vàng BN OFF
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET); // Xanh BN ON

            // Đông-Tây: Bật Đỏ (PA3 -> RESET), Tắt Vàng (PA4 -> SET), Tắt Xanh (PA5 -> SET)
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET); // Đỏ ĐT ON
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);   // Vàng ĐT OFF
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);   // Xanh ĐT OFF

            countdown1 = timer_counter;
            countdown2 = timer_counter + yellow_time; // Đỏ ĐT = Xanh hiện tại + Vàng

            if (timer_counter <= 0) {
                state_traffic = AUTO_YELLOW_RED;
                timer_counter = yellow_time;
            }
            break;

        case AUTO_YELLOW_RED:
            // --- Bắc-Nam: VÀNG (PA1), Đông-Tây: ĐỎ (PA3) ---
            // Bắc-Nam: Tắt Đỏ (PA0 -> SET), Bật Vàng (PA1 -> RESET), Tắt Xanh (PA2 -> SET)
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);   // Đỏ BN OFF
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET); // Vàng BN ON
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);   // Xanh BN OFF

            // Đông-Tây vẫn giữ Đỏ (PA3 -> RESET)
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET); // Đỏ ĐT ON
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);   // Vàng ĐT OFF
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);   // Xanh ĐT OFF

            countdown1 = timer_counter;
            countdown2 = timer_counter; // Đỏ ĐT đếm theo thời gian vàng còn lại của BN

            if (timer_counter <= 0) {
                state_traffic = AUTO_RED_GREEN;
                timer_counter = green_time;
            }
            break;

        case AUTO_RED_GREEN:
            // --- Bắc-Nam: ĐỎ (PA0), Đông-Tây: XANH (PA5) ---
            // Bắc-Nam: Bật Đỏ (PA0 -> RESET), Tắt Vàng (PA1 -> SET), Tắt Xanh (PA2 -> SET)
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET); // Đỏ BN ON
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);   // Vàng BN OFF
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);   // Xanh BN OFF

            // Đông-Tây: Tắt Đỏ (PA3 -> SET), Tắt Vàng (PA4 -> SET), Bật Xanh (PA5 -> RESET)
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);   // Đỏ ĐT OFF
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);   // Vàng ĐT OFF
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET); // Xanh ĐT ON

            countdown1 = timer_counter + yellow_time; // Đỏ BN = Xanh ĐT + Vàng
            countdown2 = timer_counter;

            if (timer_counter <= 0) {
                state_traffic = AUTO_RED_YELLOW;
                timer_counter = yellow_time;
            }
            break;

        case AUTO_RED_YELLOW:
            // --- Bắc-Nam: ĐỎ (PA0), Đông-Tây: VÀNG (PA4) ---
            // Bắc-Nam vẫn giữ Đỏ (PA0 -> RESET)
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET); // Đỏ BN ON
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);   // Vàng BN OFF
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);   // Xanh BN OFF

            // Đông-Tây: Tắt Đỏ (PA3 -> SET), Bật Vàng (PA4 -> RESET), Tắt Xanh (PA5 -> SET)
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);   // Đỏ ĐT OFF
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); // Vàng ĐT ON
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);   // Xanh ĐT OFF

            countdown1 = timer_counter; // Đỏ BN đếm theo thời gian vàng còn lại của ĐT
            countdown2 = timer_counter;

            if (timer_counter <= 0) {
                state_traffic = AUTO_GREEN_RED;
                timer_counter = green_time;
            }
            break;

        default:
            state_traffic = INIT;
            break;
    }
}

void fsm_for_input_processing(void) {
    // 1. Nút 1: Chuyển Mode (1 -> 2 -> 3 -> 4 -> 1)
    if (is_button_pressed(0) == 1) {
        // ĐOẠN LOGIC BẠN VỪA ĐỀ XUẤT:
        // Trước khi nhảy từ Mode 4 về lại Mode 1, tự động tính toán lại thời gian Đỏ
        if (status_traffic == MODE_4) {
            red_time = green_time + yellow_time;
        }

        status_traffic++;
        if (status_traffic > MODE_4) {
            status_traffic = MODE_1;
        }
    }

    // 2. Nút 2: Tăng giá trị thời gian tùy theo Mode đang đứng
    if (is_button_pressed(1) == 1) {
        if (status_traffic == MODE_2) {
            red_time++;
            if (red_time > 99) red_time = 1;
        }
        else if (status_traffic == MODE_3) {
            yellow_time++;
            if (yellow_time > 99) yellow_time = 1;
        }
        else if (status_traffic == MODE_4) {
            green_time++;
            if (green_time > 99) green_time = 1;
        }
    }

    // 3. Nút 3: Nút lưu/xác nhận riêng cho từng mode (nếu cần)
    if (is_button_pressed(2) == 1) {
        // Tại đây bạn có thể để trống hoặc dùng để chớp nháy LED báo hiệu đã nhận lệnh lưu của từng mode
    }
}

void traffic_light_mode1(void) {
    if (status_traffic == MODE_1) {
        if (timer_counter > 0) {
            timer_counter--;
        }
    }
}

void traffic_light_mode2(void) {
    // 1. Tắt toàn bộ đèn Vàng và Xanh của cả 2 hướng
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET); // Vàng BN OFF
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET); // Xanh BN OFF
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); // Vàng ĐT OFF
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET); // Xanh ĐT OFF

    // 2. Nhấp nháy đèn Đỏ (PA0 và PA3) theo blink_flag (Active-Low: RESET = Sáng, SET = Tắt)
    if (blink_flag == 1) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET); // Đỏ BN Sáng
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET); // Đỏ ĐT Sáng
    } else {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);   // Đỏ BN Tắt
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);   // Đỏ ĐT Tắt
    }
}

// Hàm xử lý Mode 3: Chỉnh thời gian Đèn Vàng
void traffic_light_mode3(void) {
    // Tắt toàn bộ đèn Đỏ và Xanh (SET = Tắt)
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET); // Đỏ BN OFF
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET); // Đỏ ĐT OFF
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET); // Xanh BN OFF
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET); // Xanh ĐT OFF

    // Nhấp nháy đèn Vàng (PA1 và PA4) theo blink_flag
    if (blink_flag == 1) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET); // Vàng BN Sáng
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); // Vàng ĐT Sáng
    } else {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);   // Vàng BN Tắt
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);   // Vàng ĐT Tắt
    }
}

// Hàm xử lý Mode 4: Chỉnh thời gian Đèn Xanh
void traffic_light_mode4(void) {
    // Tắt toàn bộ đèn Đỏ và Vàng (SET = Tắt)
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET); // Đỏ BN OFF
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET); // Đỏ ĐT OFF
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET); // Vàng BN OFF
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); // Vàng ĐT OFF

    // Nhấp nháy đèn Xanh (PA2 và PA5) theo blink_flag
    if (blink_flag == 1) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET); // Xanh BN Sáng
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET); // Xanh ĐT Sáng
    } else {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);   // Xanh BN Tắt
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);   // Xanh ĐT Tắt
    }
}
