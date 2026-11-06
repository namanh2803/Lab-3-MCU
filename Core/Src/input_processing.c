#include "main.h"
#include "input_reading.h"
#include "input_processing.h"

// Định nghĩa các trạng thái của nút/FSM cơ bản
enum ButtonState {BUTTON_RELEASED, BUTTON_PRESSED, BUTTON_PRESSED_MORE_THAN_1_SECOND};
enum ButtonState buttonState = BUTTON_RELEASED;


