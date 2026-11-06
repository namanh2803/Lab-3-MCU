#include "main.h"

#define NO_OF_BUTTONS               3

// Timer interrupt duration là 10ms, do đó để đếm đủ 1 giây (1000ms) ta cần 100 lần ngắt
#define DURATION_FOR_AUTO_INCREASING 100
#define BUTTON_IS_PRESSED           GPIO_PIN_RESET
#define BUTTON_IS_RELEASED          GPIO_PIN_SET

// Lưu trữ kết quả cuối cùng sau khi chống dội
static GPIO_PinState buttonBuffer[NO_OF_BUTTONS] = {BUTTON_IS_RELEASED, BUTTON_IS_RELEASED, BUTTON_IS_RELEASED};

// Hai bộ đệm dùng để chống dội phím (debounce)
static GPIO_PinState debounceButtonBuffer1[NO_OF_BUTTONS] = {BUTTON_IS_RELEASED, BUTTON_IS_RELEASED, BUTTON_IS_RELEASED};
static GPIO_PinState debounceButtonBuffer2[NO_OF_BUTTONS] = {BUTTON_IS_RELEASED, BUTTON_IS_RELEASED, BUTTON_IS_RELEASED};

// Cờ báo hiệu một cú nhấn bình thường (đã được xử lý một lần duy nhất, tránh lặp)
static uint8_t button_flag[NO_OF_BUTTONS] = {0, 0, 0};

// Cờ báo hiệu nút được giữ quá 1 giây
static uint8_t flagForButtonPress1s[NO_OF_BUTTONS] = {0, 0, 0};

// Bộ đếm thời gian khi giữ nút
static uint16_t counterForButtonPress1s[NO_OF_BUTTONS] = {0, 0, 0};

// Hàm đọc trạng thái nút nhấn (được gọi định kỳ mỗi 10ms trong ngắt Timer)
void button_reading(void) {
    for (int i = 0; i < NO_OF_BUTTONS; i++) {
        debounceButtonBuffer2[i] = debounceButtonBuffer1[i];

        // Đọc trạng thái chân tương ứng cho từng nút
        if (i == 0) {
            debounceButtonBuffer1[i] = HAL_GPIO_ReadPin(BUTTON_1_GPIO_Port, BUTTON_1_Pin);
        } else if (i == 1) {
            debounceButtonBuffer1[i] = HAL_GPIO_ReadPin(BUTTON_2_GPIO_Port, BUTTON_2_Pin);
        } else if (i == 2) {
            debounceButtonBuffer1[i] = HAL_GPIO_ReadPin(BUTTON_3_GPIO_Port, BUTTON_3_Pin);
        }

        if (debounceButtonBuffer1[i] == debounceButtonBuffer2[i]) {
            // Kiểm tra xem trạng thái có thực sự thay đổi so với bộ đệm cũ không (Sườn xuống)
            if (buttonBuffer[i] != debounceButtonBuffer1[i]) {
                buttonBuffer[i] = debounceButtonBuffer1[i];
                if (buttonBuffer[i] == BUTTON_IS_PRESSED) {
                    // PHÁT HIỆN CÚ NHẤN XUỐNG: Bật cờ ghi nhận 1 lần duy nhất
                    button_flag[i] = 1;

                    // Reset bộ đếm 1s khi vừa bắt đầu bấm
                    counterForButtonPress1s[i] = 0;
                    flagForButtonPress1s[i] = 0;
                }
            }

            // Xử lý trường hợp giữ nút (dùng cho tính năng auto-increment sau 1 giây)
            if (buttonBuffer[i] == BUTTON_IS_PRESSED) {
                if (counterForButtonPress1s[i] < DURATION_FOR_AUTO_INCREASING) {
                    counterForButtonPress1s[i]++;
                } else {
                    // Sau 1 giây giữ nút, bật cờ 1s và cho phép lặp liên tục khi giữ
                    flagForButtonPress1s[i] = 1;
                    counterForButtonPress1s[i] = 80; // Cho phép lặp nhanh hơn mỗi 200ms sau khi giữ đủ 1s
                    button_flag[i] = 1; // Cho phép tăng liên tục khi giữ phím lâu
                }
            } else {
                // Nếu nhả nút, reset lại bộ đếm và cờ giữ 1s
                counterForButtonPress1s[i] = 0;
                flagForButtonPress1s[i] = 0;
            }
        }
    }
}

// Kiểm tra xem nút đã được nhấn hay chưa (chỉ trả về 1 lần duy nhất cho mỗi cú click)
unsigned char is_button_pressed(uint8_t index) {
    if (index >= NO_OF_BUTTONS) return 0;
    if (button_flag[index] == 1) {
        button_flag[index] = 0; // Xóa cờ ngay lập tức sau khi đọc để tránh lặp
        return 1;
    }
    return 0;
}

// Kiểm tra xem nút có bị giữ quá 1 giây hay không
unsigned char is_button_pressed_1s(unsigned char index) {
    if (index >= NO_OF_BUTTONS) return 0;
    return (flagForButtonPress1s[index] == 1);
}
