#include "led_display.h"

int led_buffer[4] = {1, 2, 3, 4};

// Bảng mã bật tắt trực tiếp từng thanh đoạn cho LED Anode chung (0: Sáng, 1: Tắt)
// Thứ tự bit tương ứng với: [SEG6, SEG5, SEG4, SEG3, SEG2, SEG1, SEG0]
// Tùy thuộc vào việc SEG0..SEG6 tương ứng với các thanh nào (a,b,c,d,e,f,g)
const uint8_t led7_table[10] = {
    0x40, // Giả lập hiển thị số 0 (Chỉ cần chỉnh lại mảng này nếu bị sai hình dạng)
    0x79, // Số 1
    0x24, // Số 2
    0x30, // Số 3
    0x19, // Số 4
    0x12, // Số 5
    0x02, // Số 6
    0x78, // Số 7
    0x00, // Số 8 (Sáng tất cả các thanh -> mức 0)
    0x10  // Số 9
};

void display7SEG(int num) {
    if (num < 0 || num > 9) return;

    uint8_t code = led7_table[num];

    // Gán trực tiếp từng bit ra các chân PB9 -> PB15 (SEG0 -> SEG6)
    // Vì là Anode chung nên bit 0 là Sáng (RESET), bit 1 là Tắt (SET)
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9,  (code & (1 << 0)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, (code & (1 << 1)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, (code & (1 << 2)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, (code & (1 << 3)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, (code & (1 << 4)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, (code & (1 << 5)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, (code & (1 << 6)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void update7SEG(int index) {
    // 1. Tắt tất cả các Digit trước (Khóa transistor PNP bằng mức HIGH)
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6, GPIO_PIN_SET);

    // 2. Bật LED tương ứng và xuất dữ liệu
    switch (index) {
        case 0:
            display7SEG(led_buffer[0]);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET); // Mở Q1 (PB3)
            break;
        case 1:
            display7SEG(led_buffer[1]);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET); // Mở Q2 (PB4)
            break;
        case 2:
            display7SEG(led_buffer[2]);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET); // Mở Q3 (PB5)
            break;
        case 3:
            display7SEG(led_buffer[3]);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET); // Mở Q4 (PB6)
            break;
        default:
            break;
    }
}
