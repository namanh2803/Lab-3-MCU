#ifndef INC_TRAFFIC_LIGHT_H_
#define INC_TRAFFIC_LIGHT_H_

// Định nghĩa các Mode hoạt động
#define MODE_1 1
#define MODE_2 2
#define MODE_3 3
#define MODE_4 4

// Khai báo kiểu dữ liệu trạng thái
typedef enum {
    INIT,
    AUTO_GREEN_RED,
    AUTO_YELLOW_RED,
    AUTO_RED_GREEN,
    AUTO_RED_YELLOW
} TrafficState;

// Khai báo extern các biến toàn cục để file khác sử dụng được
extern int status_traffic;
extern int red_time;
extern int green_time;
extern int yellow_time;
extern int countdown1;
extern int countdown2;

// Khai báo nguyên mẫu các hàm
void fsm_for_traffic_lights(void);
void fsm_for_input_processing(void);
void traffic_light_mode1(void);
void traffic_light_mode2(void);
void traffic_light_mode3(void);
void traffic_light_mode4(void);

#endif /* INC_TRAFFIC_LIGHT_H_ */
