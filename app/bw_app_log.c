#include "bw_app_log.h"
#include "nrf_log.h"
#include "nrf_log_default_backends.h"
#include "nrf_log_ctrl.h"
#include "app_error.h"
#include "sdcard.h"

#define BW_LOG_FILE_PATH "0:"
#define BW_LOG_FILE_NAME "log.txt"
#define BW_LOG_FILE_SIZE 1024*1024    // 1MB

typedef struct
{
    uint32_t      file_size;
    uint32_t      file_offset;
    char          file_name[_MAX_LFN];
} bw_log_t;

/**@brief Function for initializing logging. */
void BW_LOG_Init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);

    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

void BW_LOG_Write(char *p_log)
{
    // ret_code_t err_code;

    // err_code = sdcard_write(BW_LOG_FILE_PATH, BW_LOG_FILE_NAME, p_log, strlen(p_log));
    // APP_ERROR_CHECK(err_code);
}