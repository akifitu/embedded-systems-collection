#include "bms_port_app.h"

#if defined(STM32F446xx)
#include "main.h"
#include "adc.h"
#include "can.h"
#include "gpio.h"
#endif

int main(void) {
#if defined(STM32F446xx)
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_ADC1_Init();
    MX_CAN1_Init();
#endif

    bms_port_app_init();
    bms_port_app_run_forever();

    return 0;
}
