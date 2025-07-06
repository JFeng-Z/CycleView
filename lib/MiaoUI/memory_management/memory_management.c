#include "memory_management.h"

#define M_MEMORY_POOL_SIZE 10*1024

static __align(sizeof(uint32_t)) uint8_t m_memory_pool[M_MEMORY_POOL_SIZE];

typedef struct m_memory_node_t *m_memory_node_pt;

typedef struct m_memory_node_t{
    uint32_t offset;
    uint32_t size;
    m_memory_node_pt next;
    m_memory_node_pt last;
}m_memory_node_t;

static m_memory_node_t m_memory_head, m_memory_tail;

#define M_MEMORY_BASE_ADDR ((uint32_t)m_memory_pool)

void m_memory_pool_init(void)
{
    m_memory_head.offset = 0;
    m_memory_head.size = 0;
    m_memory_head.next = &m_memory_tail;
    m_memory_head.last = NULL;
    m_memory_tail.offset = M_MEMORY_POOL_SIZE;
    m_memory_tail.size = 0;
    m_memory_tail.next = NULL;
    m_memory_tail.last = &m_memory_head;
}

static void m_memset(void *addr, uint8_t value, uint32_t size)
{
    uint8_t *addr_t = (uint8_t *)addr;
    while(size--)
        *addr_t++ = value;
}

void *m_malloc(uint32_t size)
{
    if(size == 0)return NULL;
    m_memory_node_pt node = &m_memory_head;
    m_memory_node_pt new_node = NULL;
    uint32_t relsize = size + sizeof(m_memory_node_t);
    uint32_t new_node_addr = 0;
    if(relsize % sizeof(uint32_t))
        relsize += sizeof(uint32_t) - relsize % sizeof(uint32_t);
    while(node->next != NULL)
    {
        if(node->next->offset - (node->offset + node->size) >= relsize)
        {
            new_node_addr = M_MEMORY_BASE_ADDR + node->offset + node->size;
            new_node = (m_memory_node_pt)new_node_addr;
            new_node->offset = node->offset + node->size;
            new_node->size = relsize;
            new_node->next = node->next;
            new_node->last = node;
            node->next = new_node;
            return (void *)(new_node + 1);
        }
        else node = node->next;
    }
    return NULL;
}

void m_free(void *addr)
{
    if(addr == NULL)return;
    m_memory_node_pt node = (m_memory_node_pt)addr - 1; 
    if(node->last != NULL && node->next != NULL)
    {
        node->last->next = node->next;
        node->next->last = node->last;
        node->last = NULL;
        node->next = NULL;
        m_memset(node, 0, node->size);
    }
}

