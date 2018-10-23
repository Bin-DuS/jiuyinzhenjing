#ifndef _COMM_SLAB_H_
#define _COMM_SLAB_H_
#include "simple_list.h"
#include <stdio.h>
#define SLAB_MAX 8                                       /**<���������������ֵ*/
#define SLAB_UNIT_SIZE 8                                /**<����������С��Ԫ*/        
struct uxsino_slab_ops;
struct uxsino_slab;
/**��������ṹ*/
typedef struct _tag_mem_unit{
	struct list_head element;                           /**<�������ڴ�����ṹ*/
    void* p;                                             /**<ָ�򻺳����ڴ������ָ��*/
}mem_unit;
/**�������ṹ��*/
typedef struct _tag_uxsino_slab{
    size_t m_unit_max_num[SLAB_MAX];                   /**<����������*/
    size_t m_unit_size[SLAB_MAX];                      /**<��������С*/
    size_t slab_size_sum;                               /**<�������ܴ�С*/
    size_t slab_max_num;                                /**<�������ܿ���*/
    mem_unit* m_mem_uns;                                /**<��������ṹ*/
    struct list_head m_free_mem_objs[SLAB_MAX];       /**<ʣ��ռ��б�*/
    struct list_head m_used_mem_objs[SLAB_MAX];       /**<���ÿռ��б�*/
    struct uxsino_slab_ops* ops;                        /**<�����������ṹ��*/
    void* m_mem_unit;                                   /**<�������ڴ�����*/
}uxsino_slab;
/**�����������ṹ��*/
struct uxsino_slab_ops{
    void* (*slab_malloc)(uxsino_slab* slab,int malloc_size);
    void  (*slab_free)(uxsino_slab* slab,void* ptr);
    void* (*slab_realloc)(uxsino_slab* slab,void* ptr,int malloc_size);
};
uxsino_slab* slab_create();
void slab_release(uxsino_slab* slab);
void* slab_malloc(uxsino_slab* slab,int malloc_size);
void slab_free(uxsino_slab* slab,void* ptr);
void* slab_realloc(uxsino_slab* slab,void* ptr,int malloc_size);
#endif

