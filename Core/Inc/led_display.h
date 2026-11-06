#ifndef INC_LED_DISPLAY_H_
#define INC_LED_DISPLAY_H_

#include "main.h"

// Mảng chứa giá trị hiển thị cho 4 LED 7 đoạn
extern int led_buffer[4];

// Các hàm nguyên mẫu
void display7SEG(int num);
void update7SEG(int index);

#endif /* INC_LED_DISPLAY_H_ */
