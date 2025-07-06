#include "sdcard.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_gpio.h"
#include "ff.h"
#include "diskio_blkdev.h"
#include "nrf_block_dev_sdc.h"

#define SDC_SCK_PIN     NRF_GPIO_PIN_MAP(0, 5)  ///< SDC serial clock (SCK) pin.
#define SDC_MOSI_PIN    NRF_GPIO_PIN_MAP(0, 26)  ///< SDC serial data in (DI) pin.
#define SDC_MISO_PIN    NRF_GPIO_PIN_MAP(0, 6)  ///< SDC serial data out (DO) pin.
#define SDC_CS_PIN      NRF_GPIO_PIN_MAP(0, 8)  ///< SDC chip select (CS) pin.

sdcard_info_t sdcard_info = 
{
    .vendor = "JFeng",
    .product = "SDC",
    .revision = "1.00"
};

/**
 * @brief  SDC block device definition
 * */
NRF_BLOCK_DEV_SDC_DEFINE(
    m_block_dev_sdc,
    NRF_BLOCK_DEV_SDC_CONFIG(
            SDC_SECTOR_SIZE,
            APP_SDCARD_CONFIG(SDC_MOSI_PIN, SDC_MISO_PIN, SDC_SCK_PIN, SDC_CS_PIN)
     ),
     NFR_BLOCK_DEV_INFO_CONFIG("JFeng", "SDC", "1.00")
);

// Initialize FATFS disk I/O interface by providing the block device.
static diskio_blkdev_t drives[] =
{
        DISKIO_BLOCKDEV_CONFIG(NRF_BLOCKDEV_BASE_ADDR(m_block_dev_sdc, block_dev), NULL)
};

static FATFS fs;

uint8_t SD_Card_state = 0;

void SDcard_Init(void) 
{
    FRESULT ff_result;
    DSTATUS disk_state = STA_NOINIT;

    nrf_gpio_cfg_output(SDC_CS_PIN);
    nrf_gpio_pin_clear(SDC_CS_PIN);

    diskio_blockdev_register(drives, ARRAY_SIZE(drives));

    // NRF_LOG_INFO("Initializing disk 0 (SDC)...");
    for (uint32_t retries = 3; retries && disk_state; --retries)
    {
        disk_state = disk_initialize(0);
    }
    if (disk_state)
    {
        NRF_LOG_INFO("Disk initialization failed. %d", disk_state);
        return;
    }

    // 计算每兆字节中的块数
    uint32_t blocks_per_mb = (1024uL * 1024uL) / m_block_dev_sdc.block_dev.p_ops->geometry(&m_block_dev_sdc.block_dev)->blk_size;
    // 根据块总数和每兆字节的块数计算存储设备的容量（以MB为单位）
    uint32_t capacity = m_block_dev_sdc.block_dev.p_ops->geometry(&m_block_dev_sdc.block_dev)->blk_count / blocks_per_mb;
    // 日志输出存储设备的容量
    // NRF_LOG_INFO("Capacity: %d MB", capacity);

    // 尝试挂载文件系统卷
    // NRF_LOG_INFO("Mounting volume...");

    ff_result = f_mount(&fs, "0:", 1);
    if (ff_result)
    {
        SD_Card_state = 0;
        NRF_LOG_INFO("Mount failed.");
        return;
    }
    else 
    {
        SD_Card_state = 1;
        NRF_LOG_INFO("Fatfs Mounted Success.");
    }
}

void SDcard_DeInit(void) 
{
    FRESULT ff_result;

    ff_result = f_mount(NULL, "", 1);
    disk_uninitialize(0);
    if (ff_result)
    {
        SD_Card_state = 1;
        NRF_LOG_INFO("Unmount failed.");
        return;
    }
    else 
    {
        SD_Card_state = 0;
        NRF_LOG_INFO("Fatfs Unmounted Success.");
    }
}

void SDcard_Init_for_UI(void) 
{
    FRESULT ff_result;
    DSTATUS disk_state = STA_NOINIT;

    nrf_gpio_cfg_output(SDC_CS_PIN);
    nrf_gpio_pin_clear(SDC_CS_PIN);

    diskio_blockdev_register(drives, ARRAY_SIZE(drives));

    for (uint32_t retries = 3; retries && disk_state; --retries)
    {
        disk_state = disk_initialize(0);
    }
    if (disk_state)
    {
        NRF_LOG_INFO("Disk initialization failed. %d", disk_state);
        return;
    }

    // 计算每兆字节中的块数
    uint32_t blocks_per_mb = (1024uL * 1024uL) / m_block_dev_sdc.block_dev.p_ops->geometry(&m_block_dev_sdc.block_dev)->blk_size;
    // 根据块总数和每兆字节的块数计算存储设备的容量（以MB为单位）
    uint32_t capacity = m_block_dev_sdc.block_dev.p_ops->geometry(&m_block_dev_sdc.block_dev)->blk_count / blocks_per_mb;

    ff_result = f_mount(&fs, MEMORY_DEVICE_NO, 1);
    if (ff_result)
    {
        NRF_LOG_INFO("Mount failed.");
        return;
    }
    else 
    {
        NRF_LOG_INFO("Fatfs Mounted Success.");
    }
}

void SDcard_DeInit_for_UI(void) 
{
    FRESULT ff_result;

    ff_result = f_mount(NULL, "", 1);
    disk_uninitialize(0);
    if (ff_result)
    {
        NRF_LOG_INFO("Unmount failed.");
        return;
    }
    else 
    {
        NRF_LOG_INFO("Fatfs Unmounted Success.");
    }
}

uint8_t SDcard_State_Get(void)
{
    return SD_Card_state;
}

void SDcard_GetInfo(void) 
{
    FATFS *fs1 = &fs;
	uint8_t res;
    uint32_t fre_clust = 0, fre_sect = 0, tot_sect = 0;
    uint32_t total, free;
    //得到磁盘信息及空闲簇数量
    res = f_getfree(MEMORY_DEVICE_NO, (DWORD*)&fre_clust, &fs1);
    if(res == 0)
	{											   
	    tot_sect = (fs1->n_fatent-2)*fs1->csize;	//得到总扇区数
	    fre_sect = fre_clust*fs1->csize;			//得到空闲扇区数	   
#if _MAX_SS!=512				  				//扇区大小不是512字节,则转换为512字节
		tot_sect*=fs1->ssize/512;
		fre_sect*=fs1->ssize/512;
#endif	  
		total = (tot_sect >> 1) / 1024;   // 单位为MB
		free  = (fre_sect >> 1) / 1024;   // 单位为MB 
        sdcard_info.capacity = total; 
        sdcard_info.free_space = free; 
        sdcard_info.used_space = sdcard_info.capacity - sdcard_info.free_space;
        return ;
 	}
    NRF_LOG_INFO("f_getfree failed. %d", res);
}