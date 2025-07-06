#include "fileDriver.h"
#include "ui_logger.h"
#include "string.h"
#include "sdcard.h"

typedef enum
{
    UI_FILE_SYS_STAT_DEINITED = 0,
    UI_FILE_SYS_STAT_INITED    
} UI_FILE_SYS_STAT;

#if(UI_USE_FATFS == 1)

static ui_file_t file_driver = 
{
    .path_buffer = {0}
};

void fileDriverInit(void)
{
    file_driver.pfinit = SDcard_Init_for_UI;
    file_driver.pfdeinit = SDcard_DeInit_for_UI;
    file_driver.fatfs_stat_get = SDcard_State_Get; 
    file_driver.pfinit();
}

void fileDriverDeInit(void)
{
    file_driver.pfdeinit();
}

void reverse(char* str1,char * str2)
{
   
    char* left = str1;
    char* right = str2;
    while (left < right)
    {
        char tmp = 0;
        tmp = *left;
        *left = *right;
        *right = tmp;
        right--;
        left++;
    }
}

void inverted_str(char* arr)
{
    reverse(arr, arr + strlen(arr)-1);
    char* str1 = arr;
    char* str2 = arr;
    while (* str2 != '\0')
    {
        while (*str2 != '/' && *str2 != '\0')
        {
            str2++;
        }
        char* cur = str2 - 1;
        reverse(str1, cur);
        if (*str2 != '\0')
        {
            str2++;
        }
        str1 = str2;

    }

}

int filefolderPathSearch(ui_page_pt page, char *path)
{
    ui_page_pt t_page = page;
    memset(path, 0, _MAX_LFN);
    while(t_page != NULL) 
    {
        if(t_page != page)strcat(path, "/");
        strcat(path, t_page->gb2312_name);
        t_page = t_page->parent;
    }
    strcat(path, "/");
    strcat(path, MEMORY_DEVICE_NO);
    inverted_str(path);
    return 0;
}

/**
 * 文件路径搜索函数
 * 该函数用于构建并搜索一个UI项的完整路径，将其添加到给定的路径字符串中
 * 
 * @param item UI项的指针，代表需要查找路径的UI元素
 * @param path 输出参数，一个足够长的字符串缓冲区，用于存储构建的路径
 * 
 * @return 总是返回0，表示成功执行（根据实际需求，可以修改为返回错误代码）
 */
int filePathSearch(ui_item_pt item, char *path)
{
    // 获取UI项所在页面的指针
    ui_page_pt t_page = item->page.location;
    memset(path, 0, _MAX_LFN);
    // 如果UI项的页面与临时页面指针相同，则将UI项的名称添加到路径中
    strcat(path, item->gb2312_name);
    
    // 遍历页面及其父页面，构建完整的路径
    while(t_page != NULL) 
    {
        // 添加路径分隔符
        strcat(path, "/");
        // 添加当前页面的名称到路径中
        strcat(path, t_page->gb2312_name);
        // 移动到父页面
        t_page = t_page->parent;
        
    }
    
    // 添加路径分隔符和内存设备编号到路径末尾
    strcat(path, "/");
    strcat(path, MEMORY_DEVICE_NO);
    
    // 将构建的路径字符串反转
    inverted_str(path);
    
    // 表示成功执行
    return 0;
}

void filefolderRegister(ui_page_pt page)
{
    FRESULT res;
    
    filefolderPathSearch(page, file_driver.path_buffer);
    if(f_stat(file_driver.path_buffer, NULL) != FR_OK)
    {
        res = f_mkdir(file_driver.path_buffer); 
        if(res != FR_OK)UI_LOG_ERROR("Create Filefolder %s Error:%d\n", file_driver.path_buffer, res);
        else UI_LOG_INFO("Create Filefolader:%s\n", file_driver.path_buffer);
    }
}

void contentRead(ui_item_pt item)
{
    FIL file;
    FRESULT res;
    uint32_t br;
    char buff[128] = {0};

    filePathSearch(item, file_driver.path_buffer);
    res = f_stat(file_driver.path_buffer, NULL);
    if(res != FR_OK)UI_LOG_ERROR("File:%s stat:%d\n", file_driver.path_buffer, res);
    else
    {
        res = f_open(&file, file_driver.path_buffer, FA_OPEN_EXISTING|FA_READ);
        if(res != FR_OK)UI_LOG_ERROR("Open File %s Error:%d\n", file_driver.path_buffer, res);
        else 
        {
            switch (item->itemType)
            {
            case UI_ITEM_WORD:
                res = f_read(&file, buff, sizeof(buff), &br);
                if(res != FR_OK)UI_LOG_ERROR("Read File %s Error:%d\n", file_driver.path_buffer, res);
                else UI_LOG_INFO("Read File:%s, size:%d, content:%s\n", file_driver.path_buffer, br, buff);
                break;
            case UI_ITEM_DATA:
                switch(item->element->data->dataType)
                {
                    case UI_DATA_SWITCH:
                        res = f_read(&file, buff, sizeof(uint8_t), &br);
                        if(res != FR_OK)UI_LOG_ERROR("Read File %s Error:%d\n", file_driver.path_buffer, res);
                        else UI_LOG_INFO("Read File:%s, size:%d, content:%d\n", file_driver.path_buffer, br, *(uint8_t *)buff);
                        break;
                    case UI_DATA_INT:
                        res = f_read(&file, buff, sizeof(int), &br);
                        if(res != FR_OK)UI_LOG_ERROR("Read File %s Error:%d\n", file_driver.path_buffer, res);
                        else UI_LOG_INFO("Read File:%s, size:%d, content:%d\n", file_driver.path_buffer, br, *(int *)buff);
                        break;
                    case UI_DATA_FLOAT:
                        res = f_read(&file, buff, sizeof(float), &br);
                        if(res != FR_OK)UI_LOG_ERROR("Read File %s Error:%d\n", file_driver.path_buffer, res);
                        else UI_LOG_INFO("Read File:%s, size:%d, content:%.2f\n", file_driver.path_buffer, br, *(float *)buff);
                        break;
                    case UI_DATA_STRING:
                        res = f_read(&file, buff, sizeof(buff), &br);
                        if(res != FR_OK)UI_LOG_ERROR("Read File %s Error:%d\n", file_driver.path_buffer, res);
                        else UI_LOG_INFO("Read File:%s, size:%d, content:%s\n", file_driver.path_buffer, br, buff);
                        break;
                }
            default:
                break;
            }
        }
        f_close(&file);
    }
}

void f_contentRead(ui_item_pt item)
{
    FIL file;
    FRESULT res;
    FILINFO info;
    uint32_t bw = 0;
    char buff[128] = {0};
    filePathSearch(item, file_driver.path_buffer);
    res = f_stat(file_driver.path_buffer, &info);
    if(res != FR_OK)UI_LOG_ERROR("File:%s stat:%d\n", file_driver.path_buffer, res);
    else
    {
        if(info.fsize == 0) //文件是空的，写入内容
        {
            res = f_open(&file, file_driver.path_buffer, FA_OPEN_EXISTING|FA_WRITE);
            if(res != FR_OK)UI_LOG_ERROR("Open File %s Error:%d\n", file_driver.path_buffer, res);
            else 
            {
                switch (item->itemType) 
                {
                    case UI_ITEM_WORD:
                        res = f_write(&file, item->element->text->ptr, strlen(item->element->text->ptr), &bw);
                        break;
                    case UI_ITEM_WAVE:
                    case UI_ITEM_DATA:
                        switch(item->element->data->dataType)
                        {
                            case UI_DATA_SWITCH:
                                res = f_write(&file, (uint8_t *)item->element->data->ptr, sizeof(uint8_t), &bw);
                                break;
                            case UI_DATA_INT:
                                res = f_write(&file, (int *)item->element->data->ptr, sizeof(int), &bw);
                                break;
                            case UI_DATA_FLOAT:
                                res = f_write(&file, (float *)item->element->data->ptr, sizeof(float), &bw);
                                break;
                            case UI_DATA_STRING:
                                res = f_write(&file, (char *)item->element->data->ptr, strlen((char *)item->element->data->ptr), &bw);
                                break;
                            default:
                                break;
                        }
                        break;
                    default:
                        break;
                }
                if(res != FR_OK)UI_LOG_ERROR("Write File %s Error:%d\n", file_driver.path_buffer, res);
                else UI_LOG_INFO("Write File:%s, size:%d\n", file_driver.path_buffer, bw);
            }
            f_close(&file);
        }
        else //不是空的，就写入到变量里去
        {
            res = f_open(&file, file_driver.path_buffer, FA_OPEN_EXISTING|FA_READ);
            if(res != FR_OK)UI_LOG_ERROR("Open File %s Error:%d\n", file_driver.path_buffer, res);
            else 
            {
                switch (item->itemType) 
                {
                    case UI_ITEM_WORD:
                        // res = f_read(&file, item->element->text->ptr, strlen(item->element->text->ptr), &bw);
                        break;
                    case UI_ITEM_DATA:
                        switch(item->element->data->dataType)
                        {
                            case UI_DATA_SWITCH:
                                res = f_read(&file, buff, sizeof(uint8_t), &bw);
                                if(res != FR_OK)UI_LOG_ERROR("Read File %s Error:%d\n", file_driver.path_buffer, res);
                                else 
                                {
                                    UI_LOG_INFO("Read File:%s, size:%d, content:%d\n", file_driver.path_buffer, bw, *(uint8_t *)buff);
                                    *(uint8_t *)item->element->data->ptr = *(uint8_t *)buff;
                                }
                                break;
                            case UI_DATA_INT:
                                res = f_read(&file, buff, sizeof(int), &bw);
                                if(res != FR_OK)UI_LOG_ERROR("Read File %s Error:%d\n", file_driver.path_buffer, res);
                                else *(int *)item->element->data->ptr = *(int *)buff;
                                break;
                            case UI_DATA_FLOAT:
                                res = f_read(&file, buff, sizeof(float), &bw);
                                if(res != FR_OK)UI_LOG_ERROR("Read File %s Error:%d\n", file_driver.path_buffer, res);
                                else *(float *)item->element->data->ptr = *(float *)buff;
                                break;
                            case UI_DATA_STRING:
                                res = f_read(&file, (char *)item->element->data->ptr, strlen((char *)item->element->data->ptr), &bw);
                                break;
                            default:
                                break;
                        }
                    default:
                        break;
                }
                if(res != FR_OK)UI_LOG_ERROR("Read File %s Error:%d\n", file_driver.path_buffer, res);
                else UI_LOG_INFO("Read File:%s, size:%d\n", file_driver.path_buffer, bw);
            }
            f_close(&file);
        }
    }
}

void contentWrite(ui_item_pt item)
{
    FIL file;
    FRESULT res;
    uint32_t bw;
    
    if(file_driver.fatfs_stat_get() == UI_FILE_SYS_STAT_DEINITED)file_driver.pfinit();

    filePathSearch(item, file_driver.path_buffer);
    res = f_stat(file_driver.path_buffer, NULL);
    if(res != FR_OK)UI_LOG_ERROR("File:%s stat:%d\n", file_driver.path_buffer, res);
    else
    {
        res = f_open(&file, file_driver.path_buffer, FA_OPEN_EXISTING|FA_WRITE);
        if(res != FR_OK)UI_LOG_ERROR("Open File %s Error:%d\n", file_driver.path_buffer, res);
        else 
        {
            switch (item->itemType) 
            {
                case UI_ITEM_WORD:
                    res = f_write(&file, item->element->text->ptr, strlen(item->element->text->ptr), &bw);
                    break;
                case UI_ITEM_WAVE:
                case UI_ITEM_DATA:
                    switch(item->element->data->dataType)
                    {
                        case UI_DATA_SWITCH:
                            res = f_write(&file, (uint8_t *)item->element->data->ptr, sizeof(uint8_t), &bw);
                            break;
                        case UI_DATA_INT:
                            res = f_write(&file, (int *)item->element->data->ptr, sizeof(int), &bw);
                            break;
                        case UI_DATA_FLOAT:
                            res = f_write(&file, (float *)item->element->data->ptr, sizeof(float), &bw);
                            break;
                        case UI_DATA_STRING:
                            res = f_write(&file, (char *)item->element->data->ptr, strlen((char *)item->element->data->ptr), &bw);
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
            if(res != FR_OK)UI_LOG_ERROR("Write File %s Error:%d\n", file_driver.path_buffer, res);
            else UI_LOG_INFO("Write File:%s, size:%d\n", file_driver.path_buffer, bw);
        }
        f_close(&file);
    }
    
    if(file_driver.fatfs_stat_get() == UI_FILE_SYS_STAT_DEINITED)file_driver.pfdeinit();
}

void fileRegister(ui_item_pt item)
{
    if((item->itemType == UI_ITEM_PARENTS) || (item->itemType == UI_ITEM_LOOP_FUNCTION) || (item->itemType == UI_ITEM_ONCE_FUNCTION) || (item->itemType == UI_ITEM_RETURN))
    return ;

    FIL file;
    FRESULT res;

    filePathSearch(item, file_driver.path_buffer);
    if(f_stat(file_driver.path_buffer, NULL) != FR_OK)
    {
        res = f_open(&file, file_driver.path_buffer, FA_CREATE_ALWAYS); 
        if(res != FR_OK)UI_LOG_ERROR("Create File %s Error:%d\n", file_driver.path_buffer, res);
        else UI_LOG_INFO("Create File:%s\n", file_driver.path_buffer);
        f_close(&file);
    }
}

void item_file_delete(ui_item_pt item) 
{
    FRESULT ff_result;
    char file_path[_MAX_LFN];

    filePathSearch(item, file_path);
    ff_result = f_unlink(file_path);
    if (ff_result)
    {
        UI_LOG_ERROR("Delete %s failed. %d", file_path, ff_result);
        return;
    }
    UI_LOG_INFO("Deleted File:%s.", file_path);
}

/* 递归遍历目录函数 */
FRESULT list_dir(const char* path) {
    FRESULT res;
    DIR dir;
    FILINFO fno;
    char full_path[_MAX_LFN];
    /* 打开目录 */
    res = f_opendir(&dir, path);
    if (res != FR_OK) {
        printf("Open %s Erroe %d\n", path, res);
        return res;
    }

    /* 遍历目录条目 */
    for (;;) {
        res = f_readdir(&dir, &fno);
        if (res != FR_OK || fno.fname[0] == 0) break;  /* 错误或遍历结束 */

        /* 跳过当前目录和父目录项 */
        if (fno.fname[0] == '.') continue;

        /* 构建完整路径 */
        snprintf(full_path, _MAX_LFN, "%s/%s", path, fno.fname);

        /* 判断文件类型 */
        if (fno.fattrib & AM_DIR) {
            /* 如果是目录：递归遍历 */
            printf("[DIR] %s\n", full_path);
            list_dir(full_path);
        } else {
            /* 如果是文件：显示信息 */
            printf("[FILE] %-40s\tSize: %10lu\n", full_path, fno.fsize);
        }
    }

    /* 关闭目录 */
    f_closedir(&dir);
    return FR_OK;
}

#endif
