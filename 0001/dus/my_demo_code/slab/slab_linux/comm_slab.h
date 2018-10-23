#ifndef _COMM_SLAB_H_
#define _COMM_SLAB_H_
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
/**��������ṹ*/
typedef struct _tag_mem_unit{
    void * p;
    struct _tag_mem_unit* next;
}mem_unit;
struct uxsino_slab_ops;
struct uxsino_slab;
/**�������ṹ��*/
typedef struct _tag_uxsino_slab{
    size_t m_unit_max_num;/**<����������,����ʱ�ɵ���������*/
    size_t m_unit_size;/**<����������,����ʱ�ɵ���������*/
    mem_unit* m_mem_uns;/**<��������ṹ*/
    struct _tag_mem_unit* m_free_mem_objs;/**<ʣ��ռ��б�*/
    struct _tag_mem_unit* m_used_mem_objs;/**<���ÿռ��б�*/
    struct uxsino_slab_ops* ops;/**<�����������ṹ��*/
    void* m_mem_unit;/**<�������ڴ�����*/
}uxsino_slab;
int create_slab(uxsino_slab* slab,size_t max_num,size_t size);
int release_slab(uxsino_slab* slab);
void* alloc_slab(uxsino_slab* slab,size_t malloc_size);
void free_slab(uxsino_slab* slab,void* ptr);
/**�����������ṹ��*/
struct uxsino_slab_ops{
    int (*create_slab)(uxsino_slab* slab,size_t max_num,size_t size);
    int (*release_slab)(uxsino_slab* slab);
    void* (*alloc_slab)(uxsino_slab* slab,size_t malloc_size);
    void (*free_slab)(uxsino_slab* slab,void* ptr);
};
/**����������ȫ�ֱ���������ָ��g_uxsino_slab_ops*/
struct uxsino_slab_ops g_uxsino_slab_ops = {
    .create_slab = create_slab,
    .release_slab = release_slab,
    .alloc_slab = alloc_slab,
    .free_slab = free_slab,
};
/**************************************************************		
* �������ƣ�create_slab()
* ���ܸ�Ҫ����uxsino_slab�����ڴ棬����uxsino_slab��ʼ��
* ������ϸ����uxsino_slab�����ڴ棬����uxsino_slab��ʼ��
* ���������uxsino_slab* slab,size_t max_num,size_t size
* �����������
* ����ֵ��  0:�ɹ���-1:ʧ��
* �������ڣ�2017/10/19		
* �������ߣ�duzhengbin
* **************************************************************/
int create_slab(uxsino_slab* slab,size_t max_num,size_t size ){
    slab->m_unit_size = size;
    slab->m_unit_max_num = max_num;
    slab->m_mem_unit = NULL;
    slab->m_mem_uns = NULL;
    slab->m_mem_unit = (void*)malloc(max_num*size);
    if(slab->m_mem_unit == NULL)return -1;
    memset(slab->m_mem_unit,0x00,max_num*size);
    slab->m_mem_uns = (mem_unit*)malloc(sizeof(mem_unit)*max_num);
    if(slab->m_mem_uns == NULL){
        free(slab->m_mem_unit);
        slab->m_mem_unit = NULL;
        return -1;
    }
    int i = 0;
    for(i = 0;i < slab->m_unit_max_num;i++){
        slab->m_mem_uns[i].p = NULL;
        slab->m_mem_uns[i].next = NULL;
    }
    for(i = 0;i < slab->m_unit_max_num;i++){
        (slab->m_mem_uns[i].p) = (void*)(slab->m_mem_unit+i*slab->m_unit_size);
        slab->m_mem_uns[i].next = &slab->m_mem_uns[i+1];
    }
    slab->m_mem_uns[slab->m_unit_max_num - 1].next = NULL;
    slab->m_free_mem_objs = &slab->m_mem_uns[0];
    slab->m_used_mem_objs = NULL;
    return 0;
}
/**************************************************************		
* �������ƣ�release_slab()
* ���ܸ�Ҫ���ͷ�uxsino_slab������ڴ�
* ������ϸ���ͷ�uxsino_slab������ڴ�
* ���������uxsino_slab* slab
* �����������
* ����ֵ��  0:�ɹ���-1:ʧ��
* �������ڣ�2017/10/19		
* �������ߣ�duzhengbin
* **************************************************************/
int release_slab(uxsino_slab* slab){
    free(slab->m_mem_unit);
    free(slab->m_mem_uns);
    slab->m_mem_unit = NULL;
    slab->m_mem_uns = NULL;
    slab->m_free_mem_objs = NULL;
    slab->m_used_mem_objs = NULL;
    return 0;
}
/**************************************************************		
* �������ƣ�alloc_slab()
* ���ܸ�Ҫ��ʹ��uxsino_slab�����ڴ�
* ������ϸ��ʹ��uxsino_slab�����ڴ�
* ���������uxsino_slab* slab,size_t malloc_size
* �����������
* ����ֵ��  void*
* �������ڣ�2017/10/19		
* �������ߣ�duzhengbin
* **************************************************************/
void* alloc_slab(uxsino_slab* slab,size_t malloc_size){
    if(malloc_size > slab->m_unit_size){
        //printf("malloc by self\n");
        return malloc(malloc_size);
    }
    if(NULL == slab->m_free_mem_objs){
        //printf("malloc by self\n");
        return malloc(slab->m_unit_size);
    }
    struct _tag_mem_unit* free = slab->m_free_mem_objs;
    slab->m_free_mem_objs = slab->m_free_mem_objs->next;
    free->next = slab->m_used_mem_objs;
    slab->m_used_mem_objs = free;
    //printf("malloc by slab\n");
    return (free->p);
}
/**************************************************************		
* �������ƣ�free_slab()
* ���ܸ�Ҫ��ʹ��uxsino_slab�ͷ�������ڴ�
* ������ϸ��ʹ��uxsino_slab�ͷ�������ڴ棬�ͷ����ڴ����Ҫ��ptrָ��ָ��NULL
* ���������uxsino_slab* slab,void* ptr
* �����������
* ����ֵ��  void
* �������ڣ�2017/10/19		
* �������ߣ�duzhengbin
* **************************************************************/

void free_slab(uxsino_slab* slab,void* ptr){
    if(ptr == NULL){
        return;
    }
    if(NULL == slab->m_used_mem_objs){
        if(ptr != NULL){
            //printf("free by self\n");
            free(ptr);
        }
        return;
    }
    struct _tag_mem_unit* temp = slab->m_used_mem_objs;
    if((temp->p) == ptr){
        memset(ptr,0x00,slab->m_unit_size);
        slab->m_used_mem_objs = slab->m_used_mem_objs->next;
        temp->next = slab->m_free_mem_objs;
        slab->m_free_mem_objs = temp;
        //printf("free by slab\n");
        return;
    }
    struct _tag_mem_unit* temp1 = temp->next;
    do{
        if((temp1->p) == ptr){
            memset(ptr,0x00,slab->m_unit_size);
            temp->next = temp->next->next;
            temp1->next = slab->m_free_mem_objs;
            slab->m_free_mem_objs = temp1;
            //printf("free by slab\n");
            return;
        }
        temp = temp->next;
        temp1 = temp->next;
    }while(temp1 != NULL);
    if(ptr != NULL){
        free(ptr);
        //printf("free by self\n");
    }
    return;
}
#endif
