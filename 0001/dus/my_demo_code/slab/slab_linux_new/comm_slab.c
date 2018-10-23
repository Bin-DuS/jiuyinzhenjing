#include "comm_slab.h"
#include "stdbool.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/**����������ȫ�ֱ���������ָ��g_uxsino_slab_ops*/
struct uxsino_slab_ops g_uxsino_slab_op = {
	.slab_malloc = slab_malloc,
	.slab_free = slab_free,
	.slab_realloc = slab_realloc,
};
void print(uxsino_slab* slab){
    mem_unit* mem_un; 
    int i = 0;
    struct list_head *pos,*n;
    for(i = 0;i < SLAB_MAX;i++){
        printf("-----------------slab->m_used_mem_objs[%d]--------------------\n",i);
        if(!list_empty(&(slab->m_used_mem_objs[i]))){
            list_for_each_safe(pos,n,&slab->m_used_mem_objs[i]){
                mem_un = container_of(pos,mem_unit,element);
                if(mem_un){
                    printf("mem_un->p = 0x%x\n",mem_un->p);
                }
            }
            printf("------------------------------------------------------------- \n");
        }else{
            printf("slab->m_used_mem_objs[%d] is empty.\n",i);
            printf("------------------------------------------------------------- \n");
        }
        printf("-----------------slab->m_free_mem_objs[%d]--------------------\n",i);
        if(!list_empty(&(slab->m_free_mem_objs[i]))){
            list_for_each_safe(pos,n,&slab->m_free_mem_objs[i]){
                mem_un = container_of(pos,mem_unit,element);
                if(mem_un){
                    printf("mem_un->p = 0x%x\n",mem_un->p);
                }
            }
            printf("------------------------------------------------------------- \n");
        }else{
            printf("slab->m_free_mem_objs[%d] is empty.\n",i);
            printf("------------------------------------------------------------- \n");
        }
    }
}
/**************************************************************    
* �������ƣ�slab_create()
* ���ܸ�Ҫ������ռ䣬��ʼ��slab
* ������ϸ���Դ�����slab���г�ʼ��
* ���������
* �����������
* ����ֵ��  uxsino_slab *
* �������ڣ�2017/10/23    
* �������ߣ�duzhengbin
* **************************************************************/

uxsino_slab *slab_create(){
	uxsino_slab *slab = (uxsino_slab*)malloc(sizeof(uxsino_slab));
	int i = 0;
	int slab_max_num_array[SLAB_MAX]={1,1,1,1,8,4,2,1};
	slab->slab_max_num = slab_max_num_array[0];
	slab->m_unit_size[0] = SLAB_UNIT_SIZE;
	slab->m_unit_max_num[0] = slab_max_num_array[0];
	slab->slab_size_sum = slab->m_unit_size[0]*slab->m_unit_max_num[0];
	for(i = 1;i < SLAB_MAX;i++){
        slab->m_unit_size[i] = slab->m_unit_size[i-1]*2;
        slab->m_unit_max_num[i] = slab_max_num_array[i];
        slab->slab_size_sum += slab->m_unit_size[i]*slab->m_unit_max_num[i];
        slab->slab_max_num += slab_max_num_array[i];
        //printf("slab->m_unit_size[%d] = %d\n",i,slab->m_unit_size[i]);
	}
	slab->m_mem_uns = (mem_unit*)malloc(sizeof(mem_unit)*(slab->slab_max_num));
	if(slab->m_mem_uns == NULL)
        return (uxsino_slab *)NULL;
    //printf("slab->m_unit_size_sum = %d slab->slab_max_num = %d\n",slab->slab_size_sum,slab->slab_max_num);
	slab->m_mem_unit = (void*)malloc(slab->slab_size_sum);
	//printf("slab->m_mem_unit 0x%x\n",slab->m_mem_unit);
	if(slab->m_mem_unit == NULL){
        free(slab->m_mem_uns);
        slab->m_mem_uns = NULL;
        return (uxsino_slab *)NULL;
	}
	slab->ops = &g_uxsino_slab_op;
    memset(slab->m_mem_unit,0x00,slab->slab_size_sum);
    //printf("slab->m_unit_size_sum = %d\n",slab->slab_size_sum);
    for(i = 0;i < SLAB_MAX;i++){
	    INIT_LIST_HEAD(&(slab->m_free_mem_objs[i]));
	    INIT_LIST_HEAD(&(slab->m_used_mem_objs[i]));
	}
	int j = 0;
	int count = 0;
	int offset = 0;
	for(i = 0;i < slab->slab_max_num;i++){
	    if(j < SLAB_MAX){
	        if(count < slab->m_unit_max_num[j]){
                slab->m_mem_uns[i].p = (void*)(slab->m_mem_unit + offset);
                //printf("slab->m_mem_uns[%d].p 0x%x   %d\n", i,slab->m_mem_uns[i].p,offset);
                list_add_tail(&(slab->m_mem_uns[i]).element,&(slab->m_free_mem_objs[j]));
                offset += slab->m_unit_size[j];
                count++;
            }else{
                count = 0;
                j++;
                slab->m_mem_uns[i].p = (void*)(slab->m_mem_unit + offset);
                //printf("slab->m_mem_uns[%d].p 0x%x   %d\n",i  ,slab->m_mem_uns[i].p,offset);
                list_add_tail(&(slab->m_mem_uns[i]).element,&(slab->m_free_mem_objs[j]));
                offset += slab->m_unit_size[j];
                count++;
            }
        }
	}
	//print(slab);
	return slab;
}
/**************************************************************    
* �������ƣ�slab_release()
* ���ܸ�Ҫ��ʹ����ϣ��ͷ�����Ŀռ�
* ������ϸ��ʹ����ϣ��ͷ�����Ŀռ�
* ���������uxsino_slab* slab(��Ҫ�ͷŵ�uxsino_slab*)
* �����������
* ����ֵ��  void
* �������ڣ�2017/10/23    
* �������ߣ�duzhengbin
* **************************************************************/
void slab_release(uxsino_slab* slab){
	if(slab->m_mem_uns != NULL){
        free(slab->m_mem_uns);
        slab->m_mem_uns = NULL;
	}
	if(slab->m_mem_unit != NULL){
        free(slab->m_mem_unit);
        slab->m_mem_unit = NULL;
	}
	if(slab != NULL){
        free(slab);
        slab=NULL;
	}
	return;
}
/**************************************************************    
* �������ƣ�slab_malloc()
* ���ܸ�Ҫ������slab�������ڴ�
* ������ϸ������slab�������ڴ�
* ���������uxsino_slab* slab(slab_malloc��������slab����)��malloc_size(����Ĵ�С)
* �����������
* ����ֵ��  void*
* �������ڣ�2017/10/23    
* �������ߣ�duzhengbin
* **************************************************************/
void* slab_malloc(uxsino_slab* slab,int malloc_size){
	mem_unit* mem_un;
	int i = 0; 
	for(i = 0;i < SLAB_MAX;i++){
	    if((malloc_size <= slab->m_unit_size[i])&&(!list_empty(&(slab->m_free_mem_objs[i])))){
	        mem_un = container_of(slab->m_free_mem_objs[i].next,mem_unit,element);
            list_del(&mem_un->element);
            list_add_tail(&mem_un->element,&(slab->m_used_mem_objs[i]));
            //printf("mem_un->p = 0x%x\n",mem_un->p);
            memset(mem_un->p,0x00,slab->m_unit_size[i]);
            //printf("slab_malloc,i = %d,size = %d\n",i,malloc_size);
            //print(slab);
            return (void*)(mem_un->p);
        }else{
            //printf("slab->m_unit_size[%d] = %d\n",i,slab->m_unit_size[i]);
        }
	}
	//print(slab);
	//printf("malloc,%d\n",malloc_size);
    return (void*)malloc(malloc_size);
}
/**************************************************************    
* �������ƣ�is_in_slab()
* ���ܸ�Ҫ���ж�ָ���Ƿ���slab��������
* ������ϸ���ж�ָ���Ƿ���slab��������
* ���������uxsino_slab* slab(slab_malloc��������slab����)��const void* ptr(��Ҫ�жϵ�ָ��)
* �����������
* ����ֵ��  int -1 δ�ҵ���>=0 �������ڵ�������
* �������ڣ�2017/10/23    
* �������ߣ�duzhengbin
* **************************************************************/
int is_in_slab(uxsino_slab* slab,const void* ptr)//�жϴ����ڻ�������
{
    if (ptr < slab->m_mem_unit || ptr > slab->m_mem_unit + slab->slab_size_sum) {
        return -1;
    }
    int i = 0;
    void* ptr_tmp;
    ptr_tmp = slab->m_mem_unit + slab->m_unit_max_num[0]*slab->m_unit_size[0];
    if(ptr >= slab->m_mem_unit && ptr < ptr_tmp){
        return 0;
    }
    for(i = 1; i < SLAB_MAX;i++){
        if(ptr >= ptr_tmp && ptr <ptr_tmp + slab->m_unit_max_num[i]*slab->m_unit_size[i])
        {
            return i;
        }else{
            ptr_tmp += slab->m_unit_max_num[i]*slab->m_unit_size[i];
        }
    }
    return -1;
}
/**************************************************************    
* �������ƣ�slab_free()
* ���ܸ�Ҫ������slab�������ڴ�
* ������ϸ������slab�������ڴ�(������Ϻ���Ҫ���н�ptr��ָ��ָ��NULL)
* ���������uxsino_slab* slab(slab_malloc��������slab����)��void * ptr(��Ҫ���յ��ڴ��ַ)
* �����������
* ����ֵ��  void
* �������ڣ�2017/10/23    
* �������ߣ�duzhengbin
* **************************************************************/
void slab_free(uxsino_slab* slab,void * ptr){
	mem_unit* mem_un;
	int i = is_in_slab(slab,ptr);
	if(i >= 0 && i < SLAB_MAX){
        mem_un = container_of(slab->m_used_mem_objs[i].next,mem_unit,element);
        memset(mem_un->p,0x00,slab->m_unit_size[i]);
        list_del(&mem_un->element);
        list_add_tail(&mem_un->element,&(slab->m_free_mem_objs[i]));
	}else{
        free(ptr);
	}
	//print(slab);
	return;
}
/**************************************************************    
* �������ƣ�slab_realloc()
* ���ܸ�Ҫ�������ڴ�����slab�������ڴ�
* ������ϸ�������ڴ�slab�������ڴ�
* ���������uxsino_slab* slab(slab_malloc��������slab����)��ptr ԭָ�룬malloc_size(����Ĵ�С)
* �����������
* ����ֵ��  void*
* �������ڣ�2017/12/08    
* �������ߣ�duzhengbin
* **************************************************************/
void* slab_realloc(uxsino_slab* slab,void* ptr,int malloc_size){
    int flag = is_in_slab(slab,ptr);
    if(malloc_size <= slab->m_unit_size[flag]){
        return ptr;
    }
    void* newptr = slab->ops->slab_malloc(slab,malloc_size);
    memcpy(newptr,ptr,slab->m_unit_size[flag]);
    slab->ops->slab_free(slab,ptr);
    return newptr;
}

