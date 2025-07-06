 #include "bw_bsp_battery_handle.h"
 #include <stdbool.h>
 #include <stdint.h>
 #include <stdio.h>
 #include <string.h>
 #include "nrf.h"
 #include "nrf_drv_saadc.h"
 #include "nrf_drv_ppi.h"
 #include "nrf_drv_timer.h"
 #include "boards.h"
 #include "app_error.h"
 #include "nrf_delay.h"
 #include "app_util_platform.h"
 #include "nrf_pwr_mgmt.h"
 
 #include "nrf_log.h"
 #include "nrf_log_ctrl.h"
 #include "nrf_log_default_backends.h"

 #include "app_scheduler.h"
 
 #define SAMPLES_IN_BUFFER 5
 volatile uint8_t state = 1;
 
 static const nrf_drv_timer_t m_timer = NRF_DRV_TIMER_INSTANCE(1);
 static nrf_saadc_value_t     m_buffer_pool[2][SAMPLES_IN_BUFFER];
 static nrf_ppi_channel_t     m_ppi_channel;
 static uint32_t              m_adc_evt_counter;
 
 uint8_t battery_level;
 
// 三元锂电池SOC-OCV表（基于NCM材料，25℃工况）
typedef struct {
    float soc;   // SOC值（0.0~1.0）
    float ocv;   // OCV值（单位：V）
} SOC_OCV_Table;

SOC_OCV_Table ncm_soc_ocv_table[] = {
    {0.00, 2.75},  // 0% SOC [1,4](@ref)
    {0.05, 3.20},  
    {0.10, 3.40},  // 10% [5,7](@ref)
    {0.20, 3.60},  
    {0.30, 3.65},  // 30% (电压平台起始)
    {0.40, 3.67},  
    {0.50, 3.68},  // 50% (电压平台核心) [4,7](@ref)
    {0.60, 3.69},  
    {0.70, 3.70},  // 70% [5](@ref)
    {0.80, 3.75},  
    {0.90, 3.90},  // 90%
    {1.00, 4.15}   // 100% [1](@ref)
};

// 表格尺寸计算
const int TABLE_SIZE = sizeof(ncm_soc_ocv_table) / sizeof(ncm_soc_ocv_table[0]);

// 温度补偿模型（每℃补偿0.3mV）
static float temp_compensation(float ocv, float temp) {
    return ocv + (25.0f - temp) * 0.0003f; // 温度基准25℃
}

static float get_soc_by_ocv(float ocv) {
    // 边界检查
    if(ocv <= ncm_soc_ocv_table[0].ocv) return 0.0;
    if(ocv >= ncm_soc_ocv_table[TABLE_SIZE-1].ocv) return 1.0;

    // 二分法查找区间
    int low=0, high=TABLE_SIZE-1;
    while(low <= high){
        int mid = (low+high)/2;
        if(ncm_soc_ocv_table[mid].ocv < ocv) 
            low = mid + 1;
        else 
            high = mid - 1;
    }
    
    // 线性插值
    float delta_ocv = ncm_soc_ocv_table[low].ocv - ncm_soc_ocv_table[high].ocv;
    float delta_soc = ncm_soc_ocv_table[low].soc - ncm_soc_ocv_table[high].soc;
    return ncm_soc_ocv_table[high].soc + 
           (ocv - ncm_soc_ocv_table[high].ocv) * (delta_soc/delta_ocv);
}

#define AGING_WINDOW 5
static float aging_factors[AGING_WINDOW] = {0};
static void update_ocv_table(float aging_factor) {
    // 滑动窗口更新系数
    memmove(&aging_factors[0], &aging_factors[1], (AGING_WINDOW-1)*sizeof(float));
    aging_factors[AGING_WINDOW-1] = aging_factor;
    
    // 应用平均补偿
    float avg = 0;
    for(int i=0; i<AGING_WINDOW; i++) avg += aging_factors[i];
    avg /= AGING_WINDOW;
    
    for(int i=0; i<TABLE_SIZE; i++){
        ncm_soc_ocv_table[i].ocv *= avg; 
    }
}

static void timer_handler(nrf_timer_event_t event_type, void * p_context)
 {
 
 }
 
 
 static void saadc_sampling_event_init(void)
 {
     ret_code_t err_code;
 
     err_code = nrf_drv_ppi_init();
     if(err_code == NRF_ERROR_MODULE_ALREADY_INITIALIZED)return ;
     APP_ERROR_CHECK(err_code);
 
     nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
     timer_cfg.bit_width = NRF_TIMER_BIT_WIDTH_32;
     err_code = nrf_drv_timer_init(&m_timer, &timer_cfg, timer_handler);
     if(err_code == NRFX_ERROR_INVALID_STATE)return ;
     APP_ERROR_CHECK(err_code);
 
     /* setup m_timer for compare event every 1000ms */
     uint32_t ticks = nrf_drv_timer_ms_to_ticks(&m_timer, 1000);
     nrf_drv_timer_extended_compare(&m_timer,
                                    NRF_TIMER_CC_CHANNEL0,
                                    ticks,
                                    NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK,
                                    false);
     nrf_drv_timer_enable(&m_timer);
 
     uint32_t timer_compare_event_addr = nrf_drv_timer_compare_event_address_get(&m_timer,
                                                                                 NRF_TIMER_CC_CHANNEL0);
     uint32_t saadc_sample_task_addr   = nrf_drv_saadc_sample_task_get();
 
     /* setup ppi channel so that timer compare event is triggering sample task in SAADC */
     err_code = nrf_drv_ppi_channel_alloc(&m_ppi_channel);
     APP_ERROR_CHECK(err_code);
 
     err_code = nrf_drv_ppi_channel_assign(m_ppi_channel,
                                           timer_compare_event_addr,
                                           saadc_sample_task_addr);
     APP_ERROR_CHECK(err_code);
 }
 
 
 static void saadc_sampling_event_enable(void)
 {
     ret_code_t err_code = nrf_drv_ppi_channel_enable(m_ppi_channel);
     if(err_code == NRFX_ERROR_INVALID_STATE)return ;
     APP_ERROR_CHECK(err_code);
 }
 
 static float battery_level_get(nrf_saadc_value_t *adc_buffer)
 {
    uint8_t i, max_index, min_index;
    nrf_saadc_value_t adc_value, max, min;
    float voltage;
    max = adc_buffer[0];
    min = adc_buffer[0];
    for (i = 0; i < 5; i++)
    {
        if (adc_buffer[i] > max)
        {
            max = adc_buffer[i];
            max_index = i;
        }
        if (adc_buffer[i] < min)
        {
            min = adc_buffer[i];
            min_index = i;
        }
    }
    adc_value = adc_buffer[0] + adc_buffer[1] + adc_buffer[2] + adc_buffer[3] + adc_buffer[4] - adc_buffer[min_index] - adc_buffer[max_index];
    adc_value = adc_value / 3;
    voltage = (adc_value * 3.6f / 512);
    return voltage;
 }
 #include "ble_bas.h"
 extern ble_bas_t m_bas;
 static void saadc_callback(nrf_drv_saadc_evt_t const * p_event)
 {
     if (p_event->type == NRF_DRV_SAADC_EVT_DONE)
     {
         ret_code_t err_code;

         err_code = nrf_drv_saadc_buffer_convert(p_event->data.done.p_buffer, SAMPLES_IN_BUFFER);
         APP_ERROR_CHECK(err_code);
         battery_level = 100 * get_soc_by_ocv(temp_compensation(battery_level_get(p_event->data.done.p_buffer), 25.0f));
         ble_bas_battery_level_update(&m_bas, battery_level, BLE_CONN_HANDLE_ALL);
    }
 }
 
 
 static void saadc_init(void)
 {
     ret_code_t err_code;
     nrf_saadc_channel_config_t channel_config =
         NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN5);
 
     err_code = nrf_drv_saadc_init(NULL, saadc_callback);
     if(err_code == NRF_ERROR_INVALID_STATE)return ;
     APP_ERROR_CHECK(err_code);
 
     err_code = nrf_drv_saadc_channel_init(0, &channel_config);
     if(err_code == NRFX_ERROR_INVALID_STATE)return ;
     APP_ERROR_CHECK(err_code);
 
     err_code = nrf_drv_saadc_buffer_convert(m_buffer_pool[0], SAMPLES_IN_BUFFER);
     if(err_code == NRFX_ERROR_INVALID_STATE)return ;
     APP_ERROR_CHECK(err_code);
 
     err_code = nrf_drv_saadc_buffer_convert(m_buffer_pool[1], SAMPLES_IN_BUFFER);
     if(err_code == NRFX_ERROR_INVALID_STATE)return ;
     APP_ERROR_CHECK(err_code);
 
 }
 
 static void saddc_uninit(void)
 {
     nrfx_saadc_channel_uninit(0);
     nrf_drv_saadc_uninit();
     nrf_drv_ppi_channel_disable(m_ppi_channel);
     nrf_drv_ppi_uninit();
 }

 static uint16_t get_filtered_battery_adc(void)
 {

 }

 /**
  * @brief Function for main application entry.
  */
 void Battery_Init(void)
 {
     saadc_init();
     saadc_sampling_event_init();
     saadc_sampling_event_enable();
     NRF_LOG_INFO("SAADC started.");
 }

 void Battery_DeInit(void)
 {
     saddc_uninit();
     nrf_drv_timer_uninit(&m_timer);
     nrf_drv_ppi_channel_free(m_ppi_channel);
     NRF_LOG_INFO("SAADC stopped.");
 }
 
 /** @} */
 