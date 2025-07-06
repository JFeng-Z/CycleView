#include "bw_bsp_gps_driver.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "app_uart.h"
#include "nrf_drv_uart.h"
#if defined (UART_PRESENT)
#include "nrf_uart.h"
#endif
#if defined (UARTE_PRESENT)
#include "nrf_uarte.h"
#endif

#define GPS_UART_RXD_PIN   NRF_GPIO_PIN_MAP(0, 17)
#define GPS_UART_TXD_PIN   NRF_GPIO_PIN_MAP(0, 15)
#define GPS_ON_OFF_PIN  NRF_GPIO_PIN_MAP(1, 11)
#define GPS_1PPS_PIN   NRF_GPIO_PIN_MAP(1, 2)

#define UART_TX_BUF_SIZE 1024                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 1024                         /**< UART RX buffer size. */

static bsp_gps_driver_t *pfgps_driver;

static void uart_error_handle(app_uart_evt_t * p_event)
{
    switch (p_event->evt_type)
    {
    case APP_UART_DATA_READY:
        uint8_t gps_rx_byte = 0;
        if(app_uart_get(&gps_rx_byte) == NRF_SUCCESS && pfgps_driver->pfcallback != NULL)
        {
            pfgps_driver->pfcallback(&gps_rx_byte);
        }
        break;
    case APP_UART_COMMUNICATION_ERROR:
        break;
    case APP_UART_FIFO_ERROR:
        break;
    case APP_UART_TX_EMPTY:
        break;
    default:
        break;
    }
}

static int8_t bsp_gps_uart_init(void)
{
    uint32_t err_code;
    nrf_gpio_cfg_output(GPS_ON_OFF_PIN);
    nrf_gpio_pin_set(GPS_ON_OFF_PIN);

    nrf_gpio_cfg_input(GPS_UART_RXD_PIN, NRF_GPIO_PIN_PULLUP);

    const app_uart_comm_params_t comm_params =
      {
        .tx_pin_no = GPS_UART_TXD_PIN,
        .rx_pin_no = GPS_UART_RXD_PIN,
        .flow_control = APP_UART_FLOW_CONTROL_DISABLED,
        .use_parity = false,
#if defined (UART_PRESENT)
        .baud_rate = NRF_UART_BAUDRATE_9600
#else
        .baud_rate = NRF_UARTE_BAUDRATE_115200
#endif
      };

    APP_UART_FIFO_INIT(&comm_params,
                         UART_RX_BUF_SIZE,
                         UART_TX_BUF_SIZE,
                         uart_error_handle,
                         APP_IRQ_PRIORITY_LOWEST,
                         err_code);

    if(err_code != NRF_SUCCESS)
    {
        return -1;
    }
    return 0;
}

static int8_t bsp_gps_uart_deinit(void)
{
    uint32_t err_code = app_uart_close();

    nrf_gpio_cfg_default(GPS_UART_RXD_PIN);
    nrf_gpio_cfg_default(GPS_UART_TXD_PIN);
    nrf_gpio_cfg_default(GPS_ON_OFF_PIN);
    nrf_gpio_cfg_default(GPS_1PPS_PIN);

    if(err_code != NRF_SUCCESS)
    {
         return -1;
    }

    return 0;
}

static int8_t bsp_gps_uart_read(void *pdata)
{
    if(pdata == NULL)return -1;
    // *((uint8_t *)pdata) = gps_rx_byte;
    return 0;
}

static int8_t bsp_gps_uart_write(uint8_t data)
{
    uint32_t err_code = app_uart_put(data);
    if(err_code != NRF_SUCCESS)
    {
        return -1;
    }
    return 0;
}

int8_t bsp_gps_driver_instance(int8_t (*pcallback)(void *pdata),
                               bsp_gps_driver_t *pgps_driver,
                               bsp_gps_driver_interface_t *pfgpsdriver_interface)
{
    if(pgps_driver == NULL || pfgpsdriver_interface == NULL || pcallback == NULL)
    {
        return -1;
    }

    pfgpsdriver_interface->init = bsp_gps_uart_init;
    pfgpsdriver_interface->deinit = bsp_gps_uart_deinit;
    pfgpsdriver_interface->read = bsp_gps_uart_read;
    pfgpsdriver_interface->write = bsp_gps_uart_write;

    pgps_driver->init_stat = BSP_GPS_DRIVER_STAT_DEINITED;
    pgps_driver->pfcallback = pcallback;
    pgps_driver->pfinterface = pfgpsdriver_interface;
    
    pfgps_driver = pgps_driver;
    return 0;
}

int8_t bsp_gps_driver_init(void)
{
    if(pfgps_driver == NULL)
    {
        return -1;
    }
    int8_t ret = pfgps_driver->pfinterface->init();
    if(ret != 0)
    {
        return -1;
    }
    pfgps_driver->init_stat = BSP_GPS_DRIVER_STAT_INITED;
    return 0;
}

int8_t bsp_gps_driver_deinit(void)
{
    if(pfgps_driver == NULL)
    {
        return -1;
    }
    int8_t ret = pfgps_driver->pfinterface->deinit();
    if(ret != 0)
    {
        return -1;
    }
    pfgps_driver->init_stat = BSP_GPS_DRIVER_STAT_DEINITED;
    return 0;
}

int8_t bsp_gps_driver_read(void *pdata)
{
    if(pfgps_driver == NULL)
    {
        return -1;
    }
    int8_t ret = pfgps_driver->pfinterface->read(pdata);
    if(ret != 0)
    {
        return -1;
    }
    return 0;
}

int8_t bsp_gps_driver_write(uint8_t data)
{
    if(pfgps_driver == NULL)
    {
        return -1;
    }
    int8_t ret = pfgps_driver->pfinterface->write(data);
    if(ret != 0)
    {
        return -1;
    }
    return 0;
}
