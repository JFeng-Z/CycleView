#include "hal_u8g2.h"
#include "u8g2.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "nrf_spi.h"
#include "nrf_drv_spi.h"
#include "nrfx_spim.h"
#include "app_error.h"
#include "stdbool.h"
#include "nrf_log.h"

#define delay_ms(x) nrf_delay_ms(x)
#define delay_us(x) nrf_delay_us(x)

#define SEND_BUFFER_DISPLAY_MS(u8g2, ms)\
  do {\
    u8g2_SendBuffer(u8g2); \
    delay_ms(ms);\
  }while(0);

#define OLED_SPI_INSTANCE  0
#define OLED_SCLK_PIN NRF_GPIO_PIN_MAP(0, 13)
#define OLED_MOSI_PIN NRF_GPIO_PIN_MAP(0, 20)
#define OLED_CS_PIN NRF_GPIO_PIN_MAP(0, 22)
#define OLED_DC_PIN NRF_GPIO_PIN_MAP(1, 0)
#define OLED_RST_PIN NRF_GPIO_PIN_MAP(0, 24)

// ====== SPIM3 配置 ======
#define SPIM3_INSTANCE_ID  3  // SPIM3实例ID (Nordic官方定义)
#define SPI_FREQUENCY      NRF_SPIM_FREQ_32M  // 32MHz高速模式

// 传输完成标志
static volatile bool spi_xfer_done = false;

// SPIM3实例句柄
static nrfx_spim_t spim3 = NRFX_SPIM_INSTANCE(SPIM3_INSTANCE_ID);

// ====== 事件处理函数 ======
void spim3_event_handler(nrfx_spim_evt_t const * p_event,
                         void * p_context)
{
    switch (p_event->type)
    {
        case NRFX_SPIM_EVENT_DONE:
            spi_xfer_done = true;  // 标记传输完成
            break;
            
        default:
            break;
    }
}

// ====== 数据传输函数 ======
void spim3_write(const uint8_t * p_data, size_t length)
{
    spi_xfer_done = false;
    
    // 构建传输描述符 (忽略接收数据)
    nrfx_spim_xfer_desc_t xfer_desc = NRFX_SPIM_XFER_TX(p_data, length);
    
    // 启动非阻塞传输
    nrfx_err_t err = nrfx_spim_xfer(&spim3, &xfer_desc, 0);
    
    if (err != NRFX_SUCCESS) {
        // 处理传输错误
        return;
    }
    
    // 等待传输完成 (超时保护)
    uint32_t timeout = 1000000; // 防止死锁
    while (!spi_xfer_done && (--timeout > 0));
    
    if (!spi_xfer_done) {
        // 超时处理: 中止传输
        nrfx_spim_abort(&spim3);
    }
}

static void oled_spi_init(void)
{
    nrfx_err_t err;
    
    nrfx_spim_config_t config = NRFX_SPIM_DEFAULT_CONFIG;
    
    // 硬件引脚配置
    config.sck_pin        = OLED_SCLK_PIN;
    config.mosi_pin       = OLED_MOSI_PIN;
    config.miso_pin       = NRFX_SPIM_PIN_NOT_USED; // 仅发送模式
    config.ss_pin         = NRFX_SPIM_PIN_NOT_USED;
    config.ss_active_high = false; // CS低电平有效
    
    // 高速模式配置
    config.frequency      = SPI_FREQUENCY;
    config.mode           = NRF_SPIM_MODE_0; // CPOL=0, CPHA=0
    config.bit_order      = NRF_SPIM_BIT_ORDER_MSB_FIRST;
    config.irq_priority   = NRFX_SPIM_DEFAULT_CONFIG_IRQ_PRIORITY;
    
    // 启用DMA传输 (提升性能)
    config.use_hw_ss      = false; // 硬件控制CS
    
    // 初始化SPIM3
    err = nrfx_spim_init(&spim3, &config, spim3_event_handler, NULL);
    
    if (err != NRFX_SUCCESS)
    {
        NRF_LOG_INFO("SPIM3 Init Failed!");
    }
	nrf_gpio_cfg_output(OLED_DC_PIN);
    nrf_gpio_cfg_output(OLED_RST_PIN);	
    //High
    nrf_gpio_pin_set(OLED_RST_PIN);
    nrf_delay_ms(10);
    
    //Low
    nrf_gpio_pin_clear(OLED_RST_PIN);
    nrf_delay_ms(10);
    
    //High
    nrf_gpio_pin_set(OLED_RST_PIN);
    nrf_delay_ms(10);
}
static void oled_spi_WriteBuff(const uint8_t *buff, uint8_t len)
{
    spim3_write(buff, len);
}

uint8_t u8x8_byte_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {

    uint8_t *p = (uint8_t*)arg_ptr;
    switch (msg)
    {
        /*通过SPI发送arg_int个字节数据*/
        case U8X8_MSG_BYTE_SEND: 
            oled_spi_WriteBuff(p, arg_int);
            break;
 
        /*设置DC引脚，DC引脚控制发送的是数据还是命令*/
        case U8X8_MSG_BYTE_SET_DC: 
            // nrf_gpio_cfg_output(OLED_DC_PIN);
            if(arg_int)nrf_gpio_pin_set(OLED_DC_PIN);
            else nrf_gpio_pin_clear(OLED_DC_PIN);
            // nrf_gpio_cfg_default(OLED_CS_PIN);
            break;
        
        /*初始化函数*/
        case U8X8_MSG_BYTE_INIT: 
            oled_spi_init();
            break;
       
        /* 下面功能无需定义 */
        
        /*开始传输前会进行的操作，如果使用软件片选可以在这里进行控制*/
        case U8X8_MSG_BYTE_START_TRANSFER: 
            nrf_gpio_cfg_output(OLED_CS_PIN);
            nrf_gpio_pin_clear(OLED_CS_PIN);
            break;
 
        /*传输后进行的操作，如果使用软件片选可以在这里进行控制*/
        case U8X8_MSG_BYTE_END_TRANSFER: 
            nrf_gpio_pin_set(OLED_CS_PIN);
            nrf_gpio_cfg_default(OLED_CS_PIN);
            break;
        default:
            return 0;
    }
    return 1;
}
uint8_t u8x8_gpio_and_delay_hw(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    switch (msg) {
        case U8X8_MSG_DELAY_100NANO: // delay arg_int * 100 nano seconds
            break;
        case U8X8_MSG_DELAY_10MICRO: // delay arg_int * 10 micro seconds
            break;
        case U8X8_MSG_DELAY_MILLI: // delay arg_int * 1 milli second
            delay_ms(1);
            break;
        case U8X8_MSG_DELAY_I2C: // arg_int is the I2C speed in 100KHz, e.g. 4 = 400 KHz
            break;                    // arg_int=1: delay by 5us, arg_int = 4: delay by 1.25us
        case U8X8_MSG_GPIO_I2C_CLOCK: // arg_int=0: Output low at I2C clock pin
            break;                    // arg_int=1: Input dir with pullup high for I2C clock pin
        case U8X8_MSG_GPIO_I2C_DATA:  // arg_int=0: Output low at I2C data pin
            break;                    // arg_int=1: Input dir with pullup high for I2C data pin
        case U8X8_MSG_GPIO_MENU_SELECT:
            u8x8_SetGPIOResult(u8x8, /* get menu select pin state */ 0);
            break;
        case U8X8_MSG_GPIO_MENU_NEXT:
            u8x8_SetGPIOResult(u8x8, /* get menu next pin state */ 0);
            break;
        case U8X8_MSG_GPIO_MENU_PREV:
            u8x8_SetGPIOResult(u8x8, /* get menu prev pin state */ 0);
            break;
        case U8X8_MSG_GPIO_MENU_HOME:
            u8x8_SetGPIOResult(u8x8, /* get menu home pin state */ 0);
            break;
        default:
            u8x8_SetGPIOResult(u8x8, 1); // default return value
            break;
    }
    return 1;
}

uint8_t *u8g2_m_16_8_f(uint8_t *page_cnt)
{
  #ifdef U8G2_USE_DYNAMIC_ALLOC
  *page_cnt = 8;
  return 0;
  #else
  static uint8_t buf[1024];
  *page_cnt = 8;
  return buf;
  #endif
}

void u8g2_Setup_sh1106_128x64_noname_f(u8g2_t *u8g2, const u8g2_cb_t *rotation, u8x8_msg_cb byte_cb, u8x8_msg_cb gpio_and_delay_cb)
{
  uint8_t tile_buf_height;
  uint8_t *buf;
  u8g2_SetupDisplay(u8g2, u8x8_d_sh1106_128x64_noname, u8x8_cad_001, byte_cb, gpio_and_delay_cb);
  buf = u8g2_m_16_8_f(&tile_buf_height);
  u8g2_SetupBuffer(u8g2, buf, tile_buf_height, u8g2_ll_hvline_vertical_top_lsb, rotation);
}