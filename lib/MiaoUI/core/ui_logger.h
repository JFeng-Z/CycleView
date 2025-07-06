#ifndef _UI_LOGGER_H_
#define _UI_LOGGER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ui_conf.h"

typedef enum 
{
    UI_LOG_LEVEL_DEBUG = 0,
    UI_LOG_LEVEL_INFO,
    UI_LOG_LEVEL_WARN,
    UI_LOG_LEVEL_ERROR,
    UI_LOG_LEVEL_FATAL,
    UI_LOG_LEVEL_ALL
}UI_LOG_LEVEL;

extern const UI_LOG_LEVEL LOG_LEVEL;

#define UI_LOG_INFO(format, ...)\
    do {\
        if(LOG_LEVEL >= UI_LOG_LEVEL_INFO)\
        UI_LOG("->INFO:\n"format"\n", ##__VA_ARGS__);\
    }while(0)

#define UI_LOG_WARN(format, ...)\
    do {\
        if(LOG_LEVEL >= UI_LOG_LEVEL_WARN)\
        UI_LOG("->WARN:\n@%s:%d:%s:"format"\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);\
    }while(0)

#define UI_LOG_ERROR(format, ...)\
    do {\
        if(LOG_LEVEL >= UI_LOG_LEVEL_ERROR)\
        UI_LOG("->ERROR:\n@%s:%d:%s:"format"\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);\
    }while(0)

#define UI_LOG_FATAL(format, ...)\
    do {\
        if(LOG_LEVEL >= UI_LOG_LEVEL_FATAL)\
        UI_LOG("->FATAL:\n@%s:%d:%s:"format"\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);\
    }while(0)

#ifdef __cplusplus
}
#endif

#endif
