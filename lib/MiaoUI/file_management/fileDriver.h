#ifndef __FILEDRIVER_H
#define __FILEDRIVER_H

#include "ui_conf.h"

#if(UI_USE_FATFS == 1)

#include "ff.h"

typedef struct
{
    FATFS *fatfs_interface;
    char path_buffer[_MAX_LFN];
    uint8_t (*fatfs_stat_get)(void);
    void(*pfinit)(void);
    void(*pfdeinit)(void);
}ui_file_t;

void fileDriverInit(void);
void fileDriverDeInit(void);
int filefolderPathSearch(ui_page_pt page, char *path);
int filePathSearch(ui_item_pt item, char *path);
void filefolderRegister(ui_page_pt page);
void fileRegister(ui_item_pt item);
void contentRead(ui_item_pt item);
void contentWrite(ui_item_pt item);
void f_contentRead(ui_item_pt item);
void item_file_delete(ui_item_pt item);
FRESULT list_dir(const char* path);

#endif

#endif
