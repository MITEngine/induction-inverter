#include "app.h"
#include "math.h"
#include "main.h"
#include "string.h"

#define PI 3.14159265359
#define THIRD (2*PI/3)

extern TIM_HandleTypeDef htim1;//3 phase output
extern TIM_HandleTypeDef htim2;//counter

extern UART_HandleTypeDef huart2;

static float angle = 0;
static float half_amplitude = 15;

static uint8_t rx_data[5];

//uint8_t state = 0;
//
//float freq = 1000000-1;
//volatile float diff = 25;

static void app_set_angle(float angle) {
	const float a = half_amplitude*sin(angle)+half_amplitude;
	const float b = half_amplitude*sin(angle+THIRD)+half_amplitude;
	const float c = half_amplitude*sin(angle+2*THIRD)+half_amplitude;

	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, (uint16_t)a);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, (uint16_t)b);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, (uint16_t)c);
}

void App_Init(void) {
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);

	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);

	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);

	HAL_TIM_Base_Start_IT(&htim2);

	HAL_UART_Receive_IT(&huart2, rx_data, 5);
}

void App_Update(void) {
	app_set_angle(angle);
	angle+=0.01;
	if (angle > 2*PI) {
		angle -= 2*PI;
	}
	HAL_Delay(1);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
	if (htim == &htim2) {
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (rx_data[0]) {
		memcpy(&half_amplitude, rx_data+1, 4);
	}else{
		memcpy(&angle, rx_data+1, 4);
	}
	app_set_angle(angle);

//	if (rx_data[0]) {
//		memcpy(&freq, rx_data+1, 4);
//		__HAL_TIM_SET_AUTORELOAD(&htim2, (uint32_t)freq);
//	}else{
//		memcpy(&diff, rx_data+1, 4);
//	}

	HAL_UART_Receive_IT(&huart2, rx_data, 5);
}
