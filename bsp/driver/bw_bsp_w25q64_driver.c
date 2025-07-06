#include "bw_bsp_w25q64_driver.h"
#include "nrf_drv_spi.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "nrf_delay.h"

#define W25Q64_DEVICE_ID 0xEF16

static const nrf_drv_spi_t w25q64_spi_instance = NRF_DRV_SPI_INSTANCE(2);

static volatile bool spi_xfer_done = false;

static bsp_w25q64_driver_t *pw25q64_driver = NULL;

static bsp_w25q64_driver_init_stat_t w25q64_init_stat = BSP_w25q64_DRIVER_STAT_DEINITED;

#define W25Q64_SCK_PIN     NRF_GPIO_PIN_MAP(0, 5)  ///< W25Q64 serial clock (SCK) pin.
#define W25Q64_MOSI_PIN    NRF_GPIO_PIN_MAP(0, 26)  ///< W25Q64 serial data in (DI) pin.
#define W25Q64_MISO_PIN    NRF_GPIO_PIN_MAP(0, 6)  ///< W25Q64 serial data out (DO) pin.
#define W25Q64_CS_PIN      NRF_GPIO_PIN_MAP(1, 13)  ///< W25Q64 chip select (CS) pin.

#define  W25Q64_WRITE_ENABLE_CMD 		0x06
#define  W25Q64_WRITE_DISABLE_CMD		0x04
#define  W25Q64_READ_SR_CMD				0x05
#define  W25Q64_WRITE_SR_CMD			0x01
#define  W25Q64_READ_DATA				0x03
#define  W25Q64_FASTREAD_DATA			0x0b
#define  W25Q64_WRITE_PAGE				0x02
#define  W25Q64_ERASE_PAGE      		0x81
#define  W25Q64_ERASE_SECTOR       		0x20
#define	 W25Q64_ERASE_BLOCK				0xd8
#define	 W25Q64_ERASE_CHIP				0xc7
#define  W25Q64_POWER_DOWN				0xb9
#define  W25Q64_RELEASE_POWER_DOWN      0xab
#define  W25Q64_READ_DEVICE_ID      	0x90
#define  W25Q64_READ_JEDEC_ID      		0x9f

static ret_code_t w25q64_spi_send(const uint8_t *p_tx_buffer, uint8_t tx_buffer_length)
{
    ret_code_t err_code;
    spi_xfer_done = false;
    err_code = nrf_drv_spi_transfer(&w25q64_spi_instance, p_tx_buffer, tx_buffer_length, NULL, 0);
    if(err_code != NRF_SUCCESS)
    {
        NRF_LOG_ERROR("w25q64_spi_transfer error: %d", err_code);
        return err_code;
    }
    while(!spi_xfer_done)
    {
        sd_app_evt_wait();
    }
    return err_code;
}

static ret_code_t w25q64_spi_receive(uint8_t *p_rx_buffer, uint8_t rx_buffer_length)
{
    ret_code_t err_code;
    spi_xfer_done = false;
    err_code = nrf_drv_spi_transfer(&w25q64_spi_instance, NULL, 0, p_rx_buffer, rx_buffer_length);
    if(err_code != NRF_SUCCESS)
    {
        NRF_LOG_ERROR("w25q64_spi_transfer error: %d", err_code);
        return err_code;
    }
    while(!spi_xfer_done)
    {
        sd_app_evt_wait();
    }
    return err_code;
}

static uint8_t w25q64_read_status(void)
{
    uint8_t cmd = W25Q64_READ_SR_CMD;
    uint8_t status = 2;
    nrf_gpio_pin_clear(W25Q64_CS_PIN);
    w25q64_spi_send(&cmd, 1);
    w25q64_spi_receive(&status, 1);
    nrf_gpio_pin_set(W25Q64_CS_PIN);
    return status;
}

static void w25q64_write_enable(void)
{
    uint8_t cmd = W25Q64_WRITE_ENABLE_CMD;
    nrf_gpio_pin_clear(W25Q64_CS_PIN);
    w25q64_spi_send(&cmd, 1);
    nrf_gpio_pin_set(W25Q64_CS_PIN);
    uint8_t stat = w25q64_read_status();
}

static void w25q64_write_disable(void)
{
    uint8_t cmd = W25Q64_WRITE_DISABLE_CMD;
    nrf_gpio_pin_clear(W25Q64_CS_PIN);
    w25q64_spi_send(&cmd, 1);
    nrf_gpio_pin_set(W25Q64_CS_PIN);
}

/**
 * 检测是否忙
 * @return 忙返回1，空闲返回0
 */
static uint8_t w25q64_check_busy(void)
{
    uint8_t status = w25q64_read_status();
    return (status & 0x01);
}

static void w25q64_wait_busy(void)
{
    while(w25q64_check_busy());
}

static void w25q64_write_status(uint8_t status)
{
    uint8_t cmd = W25Q64_WRITE_SR_CMD;
    w25q64_write_enable();
    nrf_gpio_pin_clear(W25Q64_CS_PIN);
    w25q64_spi_send(&cmd, 1);
    w25q64_spi_send(&status, 1);
    w25q64_spi_send(&status, 1);
    nrf_gpio_pin_set(W25Q64_CS_PIN);
    w25q64_wait_busy();
}

static void w25q64_general_read(uint32_t addr, uint8_t *pdata, uint32_t size)
{
    uint8_t cmd[4] = {W25Q64_READ_DATA, (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, addr & 0xFF};
    w25q64_wait_busy();
    nrf_gpio_pin_clear(W25Q64_CS_PIN);
    w25q64_spi_send(&cmd[0], 1);
    w25q64_spi_send(&cmd[1], 3);
    w25q64_spi_receive(pdata, size);
    nrf_gpio_pin_set(W25Q64_CS_PIN);
    w25q64_wait_busy();
}

static void w25q64_fast_read(uint32_t addr, uint8_t *pdata, uint32_t size)
{
    uint8_t cmd[5] = {W25Q64_FASTREAD_DATA, 0xFF, (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, addr & 0xFF};
    w25q64_wait_busy();
    nrf_gpio_pin_clear(W25Q64_CS_PIN);
    w25q64_spi_send(&cmd[0], 1);
    w25q64_spi_send(&cmd[1], 1);
    w25q64_spi_send(&cmd[2], 3);
    w25q64_spi_receive(pdata, size);
    nrf_gpio_pin_set(W25Q64_CS_PIN);
}

static int8_t w25q64_read(uint32_t addr, uint8_t *pdata, uint32_t size)
{
    w25q64_general_read(addr, pdata, size);
    if(pdata  == NULL)return -1;
    return 0;
}

static void w25q64_sector_erase(uint32_t addr)
{
    uint8_t cmd[4] = {W25Q64_ERASE_SECTOR, (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, addr & 0xFF};
    addr *= 4096;
    w25q64_wait_busy();
    w25q64_write_enable();
    nrf_gpio_pin_clear(W25Q64_CS_PIN);
    w25q64_spi_send(&cmd[0], 1);
    w25q64_spi_send(&cmd[1], 3);
    nrf_gpio_pin_set(W25Q64_CS_PIN);
}

static void w25q64_block_erase(uint32_t addr)
{
    uint8_t cmd[4] = {W25Q64_ERASE_BLOCK, (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, addr & 0xFF};
    addr *= 65536;
    w25q64_wait_busy();
    w25q64_write_enable();
    nrf_gpio_pin_clear(W25Q64_CS_PIN);
    w25q64_spi_send(&cmd[0], 1);
    w25q64_spi_send(&cmd[1], 3);
    nrf_gpio_pin_set(W25Q64_CS_PIN);
}

static void w25q64_chip_erase(void)
{
    uint8_t cmd = W25Q64_ERASE_CHIP;
    w25q64_wait_busy();
    w25q64_write_enable();
    nrf_gpio_pin_clear(W25Q64_CS_PIN);
    w25q64_spi_send(&cmd, 1);
    nrf_gpio_pin_set(W25Q64_CS_PIN);
}

static void w25q64_page_write(uint32_t addr, uint8_t *pdata, uint32_t size)
{
    uint8_t cmd[4] = {W25Q64_WRITE_PAGE, (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, addr & 0xFF};
    w25q64_sector_erase(addr);
    w25q64_wait_busy();
    w25q64_write_enable();
    nrf_gpio_pin_clear(W25Q64_CS_PIN);
    w25q64_spi_send(&cmd[0], 1);
    w25q64_spi_send(&cmd[1], 3);
    w25q64_spi_send(pdata, size);
    nrf_gpio_pin_set(W25Q64_CS_PIN);
    w25q64_wait_busy();
}

static void w25q64_nocheck_write(uint32_t addr, uint8_t *pdata, uint32_t size)
{
    uint16_t page_re_bytes = 256 - (addr % 256);

    if(size < page_re_bytes)
    {
        w25q64_page_write(addr, pdata, size);
        return;
    }

    while(size > 0)
    {
        w25q64_page_write(addr, pdata, page_re_bytes);
        size -= page_re_bytes;
        pdata += page_re_bytes;
        addr += page_re_bytes;
        page_re_bytes = 256;
    }
}

static uint8_t w25q64_buffer[4096];

static void w25q64_check_write(uint32_t addr, uint8_t *pdata, uint32_t size)
{
    uint32_t sector_pos = 0;    //  扇区位置
    uint16_t sector_offset = 0; //  扇区偏移
    uint16_t sector_remain = 0; //  扇区剩余
    uint32_t i = 0;

    sector_pos = addr / 4096;
    sector_offset = addr % 4096;
    sector_remain = 4096 - sector_offset;

    if(size < sector_remain)
    {
        w25q64_fast_read(sector_pos * 4096, w25q64_buffer, 4096);
        for (i = 0; i < sector_remain; i++)
        {
            if(w25q64_buffer[sector_offset + i] !=  0xFF)break;
        }
        if(i == sector_remain)
        {
            w25q64_nocheck_write(addr, pdata, size);
            return ;
        }
        w25q64_sector_erase(sector_pos);
        w25q64_nocheck_write(sector_pos * 4096, pdata, size);
        return ;
    }
    while (size > 0)
    {
        w25q64_fast_read(sector_pos * 4096, w25q64_buffer, 4096);
        for (i = 0; i < sector_remain; i++)
        {
            if(w25q64_buffer[sector_offset + i] !=  0xFF)break;
        }
        if(i == sector_remain)
        {
            w25q64_nocheck_write(addr, pdata, size);
            return ;
        }
        w25q64_sector_erase(sector_pos);
        w25q64_nocheck_write(sector_pos * 4096, pdata, size);
        size -= sector_remain;
        pdata += sector_remain;
        addr += sector_remain;
        sector_pos++;
        sector_offset = 0;
        if(size < sector_remain)sector_remain = size;
        else sector_remain = 4096;
    }
}

static int8_t w25q64_write(uint32_t addr, uint8_t *pdata, uint32_t size)
{
    w25q64_check_write(addr, pdata, size);
    return 0;
}

static void w25q64_power_down(void)
{
    uint8_t cmd = W25Q64_POWER_DOWN;
    nrf_gpio_pin_clear(W25Q64_CS_PIN);
    w25q64_spi_send(&cmd, 1);
    nrf_gpio_pin_set(W25Q64_CS_PIN);
    nrf_delay_us(3);
}

static void w25q64_release(void)
{
    uint8_t cmd = W25Q64_RELEASE_POWER_DOWN;
    nrf_gpio_pin_clear(W25Q64_CS_PIN);
    w25q64_spi_send(&cmd, 1);
    nrf_gpio_pin_set(W25Q64_CS_PIN);
    nrf_delay_us(3);
}

static uint16_t w25q64_read_device_id(void)
{
    uint8_t tx_buf[4] = {W25Q64_READ_DEVICE_ID, 0x00, 0x00, 0x00};
    uint8_t rx_buf[2] = {0};
    uint16_t id = 0;
    nrf_gpio_pin_clear(W25Q64_CS_PIN);
    w25q64_spi_send(&tx_buf[0], 1);
    w25q64_spi_send(&tx_buf[1], 3);
    w25q64_spi_receive(rx_buf, 2);
    nrf_gpio_pin_set(W25Q64_CS_PIN);
    id |= (rx_buf[0] << 8);
    id |= rx_buf[1];
    return id;
}

static void w25q64_spi_event_handler(const nrf_drv_spi_evt_t *p_event, void *p_context)
{
    if(p_event->type == NRF_DRV_SPI_EVENT_DONE)
    spi_xfer_done = true;
}

static int8_t w25q64_init(void)
{
    ret_code_t err_code;
    nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    spi_config.sck_pin = W25Q64_SCK_PIN;
    spi_config.mosi_pin = W25Q64_MOSI_PIN;
    spi_config.miso_pin = W25Q64_MISO_PIN;
    spi_config.ss_pin = NRFX_SPI_PIN_NOT_USED;
    spi_config.mode = NRF_DRV_SPI_MODE_0;
    spi_config.frequency = NRF_DRV_SPI_FREQ_4M; 
    err_code = nrf_drv_spi_init(&w25q64_spi_instance, &spi_config, w25q64_spi_event_handler, NULL);
    nrf_gpio_cfg_output(W25Q64_CS_PIN);
    if(err_code != NRF_SUCCESS)return -1;
    w25q64_power_down();
    w25q64_release();
    uint16_t id = w25q64_read_device_id();
    if(id != W25Q64_DEVICE_ID)
    {
        NRF_LOG_INFO("W25Q64 device id error. id: %d.", id);
        return -1;
    }
    return 0;
}

static int8_t w25q64_deinit(void)
{
    w25q64_power_down();
    nrf_drv_spi_uninit(&w25q64_spi_instance);
    nrf_gpio_cfg_default(W25Q64_CS_PIN);
    nrf_gpio_cfg_default(W25Q64_SCK_PIN);
    nrf_gpio_cfg_default(W25Q64_MOSI_PIN);
    nrf_gpio_cfg_default(W25Q64_MISO_PIN);
    return 0;
}

int8_t bsp_w25q64_driver_init(void)
{
    if(w25q64_init() != 0)
    {
        return -1;
    }
    pw25q64_driver->init_stat = BSP_w25q64_DRIVER_STAT_INITED;
    return 0;
}

int8_t bsp_w25q64_driver_deinit(void)
{
    if(w25q64_deinit() != 0)
    {
        return -1;
    }
    pw25q64_driver->init_stat = BSP_w25q64_DRIVER_STAT_DEINITED;
    return 0;
}

int8_t bsp_w25q64_driver_read(uint32_t addr, uint8_t *pdata, uint32_t size)
{
    return w25q64_read(addr, pdata, size);
}

int8_t bsp_w25q64_driver_write(uint32_t addr, uint8_t *pdata, uint32_t size)
{
    return w25q64_write(addr, pdata, size);
}

int8_t bsp_w25q64_driver_instance(int8_t (*pcallback)(void *pdata),
                                  bsp_w25q64_driver_t *pw25q64_driver, 
                                  bsp_w25q64_interface_t *pw25q64driver_interface)
{
    if(pw25q64driver_interface == NULL || pw25q64_driver == NULL)
    {
        return -1;
    }
    pw25q64_driver->pcallback = pcallback;
    pw25q64_driver->init_stat = w25q64_init_stat;
    pw25q64driver_interface->pfdeinit = w25q64_init;
    pw25q64driver_interface->pfinit = w25q64_deinit;
    pw25q64driver_interface->pfread = w25q64_read;
    pw25q64driver_interface->pfwrite = w25q64_write;
    pw25q64_driver->pinterface = pw25q64driver_interface;
    return 0;
}