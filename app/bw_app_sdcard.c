#include "bw_app_sdcard.h"
#include "sdcard.h"
#include "fileDriver.h"
#include "ui_conf.h"

extern uint8_t SD_Card_state;

void BW_SDCard_Init(void)
{
    if(SD_Card_state == 0)
    SDcard_Init();
}

void BW_SDCard_DeInit(void)
{
    if(SD_Card_state == 1)
    {
        SDcard_GetInfo(); // 卸载前更新内存信息
        SDcard_DeInit();
    }
}

void SDCard_state_control(ui_t *ui)
{
    switch (*(uint8_t *)ui->nowItem->element->data->ptr)
    {
    case 0:
        SDcard_DeInit();
        break;
    case 1:
        SDcard_Init();
        break;
    default:
        break;
    }
}

bool file_delete(ui_t *ui) 
{
    BW_SDCard_Init();
    Dynamic_DeleteItem(ui->nowItem);
    BW_SDCard_DeInit();
    return true;
}

void delete_recursive(const TCHAR* path) 
{
    DIR dir;
    FILINFO fno;
    if (f_opendir(&dir, path) == FR_OK) {
        while (f_readdir(&dir, &fno) == FR_OK) {
            if (fno.fname[0] == 0) break; // 终止条件
            if (strcmp(fno.fname, ".") == 0 || strcmp(fno.fname, "..") == 0) continue;
            TCHAR subpath[_MAX_LFN];
            sprintf(subpath, "%s/%s", path, fno.fname);
            if (fno.fattrib & AM_DIR) {
                delete_recursive(subpath); // 递归删除子目录
            }
            f_unlink(subpath); // 删除文件或空目录
        }
        f_closedir(&dir);
    }
}

void sdcard_format(ui_t *ui)
{
    FRESULT ff_result;
    BYTE work[_MAX_SS];
    ff_result = f_mount(NULL, MEMORY_DEVICE_NO, 0);
    ff_result = f_mkfs(MEMORY_DEVICE_NO, FM_FAT, 0, work, sizeof(work));
    if (ff_result)
    {
        NRF_LOG_ERROR("Format failed. %d", ff_result);
        return;
    }
    else 
    {
        NRF_LOG_INFO("Format Success, System Reset");
        NVIC_SystemReset();
    }
}