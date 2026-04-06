#include "bms_port_hal.h"

#include "bms_port_config.h"

#if defined(STM32F446xx)
#include "adc.h"
#include "gpio.h"

extern uint16_t g_bms_adc_dma_buffer[7];
#endif

static float scale_voltage(uint16_t raw_count) {
    float sensed_voltage =
        ((float)raw_count / BMS_PORT_ADC_MAX_COUNT) * BMS_PORT_ADC_REFERENCE_V;
    return sensed_voltage * BMS_PORT_CELL_DIVIDER_SCALE;
}

static float scale_current(uint16_t raw_count) {
    float sensor_voltage =
        ((float)raw_count / BMS_PORT_ADC_MAX_COUNT) * BMS_PORT_ADC_REFERENCE_V;
    return (sensor_voltage - 1.65f) * BMS_PORT_CURRENT_SCALE_A_PER_V +
           BMS_PORT_CURRENT_ZERO_A;
}

static float scale_temperature(uint16_t raw_count) {
    float sensor_voltage =
        ((float)raw_count / BMS_PORT_ADC_MAX_COUNT) * BMS_PORT_ADC_REFERENCE_V;
    return (sensor_voltage - 0.76f) / 0.0025f + 25.0f;
}

void bms_port_hal_init(void) {
#if defined(STM32F446xx)
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)g_bms_adc_dma_buffer, 7u);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
#endif
}

bool bms_port_hal_read_sample(bms_sample_t *sample) {
    if (sample == 0) {
        return false;
    }

    sample->dt_seconds = BMS_PORT_SAMPLE_PERIOD_MS / 1000.0f;

#if defined(STM32F446xx)
    sample->cell_voltage_v[0] = scale_voltage(g_bms_adc_dma_buffer[0]);
    sample->cell_voltage_v[1] = scale_voltage(g_bms_adc_dma_buffer[1]);
    sample->cell_voltage_v[2] = scale_voltage(g_bms_adc_dma_buffer[2]);
    sample->cell_voltage_v[3] = scale_voltage(g_bms_adc_dma_buffer[3]);
    sample->pack_current_a = scale_current(g_bms_adc_dma_buffer[4]);
    sample->temperatures_c[0] = scale_temperature(g_bms_adc_dma_buffer[5]);
    sample->temperatures_c[1] = scale_temperature(g_bms_adc_dma_buffer[6]);
#else
    sample->cell_voltage_v[0] = 4.00f;
    sample->cell_voltage_v[1] = 4.02f;
    sample->cell_voltage_v[2] = 4.01f;
    sample->cell_voltage_v[3] = 3.99f;
    sample->pack_current_a = 0.0f;
    sample->temperatures_c[0] = 25.0f;
    sample->temperatures_c[1] = 25.0f;
#endif

    return true;
}

void bms_port_hal_apply_output(const bms_output_t *output) {
    if (output == 0) {
        return;
    }

#if defined(STM32F446xx)
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8,
                      output->charge_enabled ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9,
                      output->discharge_enabled ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10,
                      output->balancing_enabled[0] ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11,
                      output->balancing_enabled[1] ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12,
                      output->balancing_enabled[2] ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4,
                      output->balancing_enabled[3] ? GPIO_PIN_SET : GPIO_PIN_RESET);
#else
    (void)output;
#endif
}

uint32_t bms_port_hal_millis(void) {
#if defined(STM32F446xx)
    return HAL_GetTick();
#else
    static uint32_t simulated_tick = 0;
    simulated_tick += BMS_PORT_SAMPLE_PERIOD_MS;
    return simulated_tick;
#endif
}

void bms_port_hal_log(const char *message) {
    (void)message;
#if defined(STM32F446xx)
    /* Route this to UART, SWO, or RTT during board bring-up. */
#endif
}
