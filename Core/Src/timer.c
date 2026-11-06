#include "input_reading.h"
#include "led_display.h"
#include "traffic_light.h"

extern int countdown1;
extern int countdown2;

static int counter_for_1s = 0;
static int counter_for_blink = 0;
int blink_flag = 0; // Khai báo extern bên traffic_light.c nếu cần dùng

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM2)
	    {
	        button_reading(); // Đọc nút nhấn mỗi 10ms

	        // Đếm 1 giây cho Mode 1 (100 lần * 10ms = 1000ms)
	        counter_for_1s++;
	        if (counter_for_1s >= 10) {
	            counter_for_1s = 0;
	            traffic_light_mode1();
	        }

	        // Đếm 0.5 giây cho hiệu ứng nhấp nháy (50 lần * 10ms = 500ms)
	        counter_for_blink++;
	        if (counter_for_blink >= 5) {
	            counter_for_blink = 0;
	            blink_flag = 1 - blink_flag; // Đảo trạng thái 0 <-> 1 liên tục mỗi 0.5s
	        }

	        // Quét LED 7 đoạn
	        static int led_index = 0;
	        update7SEG(led_index);
	        led_index = (led_index + 1) % 4;
	    }
}
