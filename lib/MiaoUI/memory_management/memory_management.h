#ifndef _MEMORY_MANAGEMENT_H_
#define _MEMORY_MANAGEMENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

void m_memory_pool_init(void);
void *m_malloc(uint32_t size);
void m_free(void *ptr);

#ifdef __cplusplus
}
#endif

#endif
