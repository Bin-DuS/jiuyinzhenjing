/**		
 * @file    	types.h
 * @date:       2017/10/9	
 * @brief  	���õ�����ת��
 * @author:		xhtian
 * �޸�������	001)[�޸�ԭ��1]		
 * �޸����ߣ�	001)�޸��� YYYY/MM/DD		
 * @version   	1.0
 * Copyright(c) All Right Reserved, Copyright(c) 2017.		
 */
#ifndef __TYPES_H_INC__
#define __TYPES_H_INC__

#if defined(_WIN32) || defined(_WIN64)
#define UX_INLINE _inline
#else
#define UX_INLINE inline
#endif


#define IN
#define OUT
#define INOUT

#define MAX_BUFF_SIZE 1024
#define FILEPATH_MAX_SIZE   256


#ifndef UX_MIN
#define UX_MIN(a,b) (((a)>(b))?(b):(a))
#endif
#ifndef UX_MAX
#define UX_MAX(a,b) (((a)>(b))?(a):(b))
#endif

/**
  * @brief  char
  */
typedef char s8;

/**
  * @brief  unsigned char
  */
typedef unsigned char u8;

/**
  * @brief  short
  */
typedef short s16;

/**
  * @brief  unsigned short
  */
typedef unsigned short u16;

/**
  * @brief  int
  */
typedef int s32;

/**
  * @brief  unsigned int
  */
typedef unsigned int u32;

typedef signed long long s64;
typedef unsigned long long u64;


/**
  * @brief  int
  */
typedef int BOOL;

/**
  * @brief  TRUE
  */
#ifndef TRUE
#define TRUE    1
#endif

/**
  * @brief  FALSE
  */
#ifndef FALSE
#define FALSE   0
#endif

/**
 * @def offsetof
 * @brief ����ָ�������г�Ա�ڴ������е�ƫ����
 */
#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

/**
 * @def container_of
 * @brief ͨ��struct�ĳ�Ա������ȡstruct����
 * @ptr: struct��Ա����ָ��
 * @type: struct������
 * @member:	ptr��struct�е�����
 */
#ifndef container_of
#define container_of(ptr, type, member) ({			\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)( (char *)__mptr - offsetof(type,member) );})
#endif

#ifndef RET_AND_RETURN
#define RET_AND_RETURN(ret) \
    do{                     \
        if ((ret) < 0) return -1;     \
    }while(0)
#endif

#ifndef RET_AND_CONTINUE
#define RET_AND_CONTINUE(ret) \
    do{                     \
        if ((ret) < 0) continue;     \
    }while(0)
#endif


#ifdef __KERNEL__           /** �ں�ͷ�ļ�*/
#include <linux/spinlock.h>

#else                                /** �û�̬��ͷ�ļ�*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#endif

#ifdef __KERNEL__           /** �ں�*/
typedef struct file uxsino_file;
typedef struct file_lock uxsino_file_lock;
typedef spinlock_t uxsino_lock;

#define uxsino_malloc(type, size, sign) \
    ((type*)kmalloc((size), (sign)))

#define uxsino_free(ptr)    \
    do{if((ptr)){kfree((ptr));(ptr) = NULL;}}while(0)

#define uxsino_fopen(filename, mode, fileauth)  \
    (filp_open((filename), (mode), (fileauth)))

#define uxsino_fclose(fp) \
    do {    \
        filp_close(fp, NULL);   \
    }while(0)

#define uxsino_fseek(fp, offset, whence)  \
    ((fp)->f_op->llseek((fp), (offset), (whence)))

#define uxsino_fread(buf, size, count, fp)  \
    ((fp)->f_op->read((fp), (buf), (size), &((fp)->f_pos)))

#define uxsino_fwrite(buf, size, count, fp) \
    ((fp)->f_op->write((fp), (buf), (size), &((fp)->f_pos)))

#define uxsino_fcntl(fp, mode, arg)  \
    ((fp)->f_op->lock((fp), (mode), &(arg)))

#define uxsino_fflush(fp) \
    ((fp)->f_op->flush((fp), NULL))

#define uxsino_ftell(fp)	\
	(fp->f_pos)

#define DEFINE_UXSINO_LOCKER(x) \
	DEFINE_SPINLOCK(x)

#define uxsino_locker_lock_init(locker)	\
	do {					\
		spin_lock_init(locker);	\
	} while (0);

#define uxsino_locker_lock(locker)	\
	do {					\
		spin_lock(locker);	\
	} while (0);

#define uxsino_locker_unlock(locker)	\
	do {					\
	spin_unlock(locker);	\
	} while (0);
	
#else       /** ���ں˵��ú���*/
typedef FILE uxsino_file;
typedef struct flock uxsino_file_lock;
typedef pthread_mutex_t uxsino_lock;

/**
 * @brief   �����ڴ�
 */
#define uxsino_malloc(type, size, sign) \
    ((type*)malloc((size)))

/**
 * @brief   �ͷ��ڴ�
 */
#define uxsino_free(ptr)    \
    do{if((ptr)){free((ptr));(ptr) = NULL;}}while(0)

/**
 * @brief   ���ļ�
 */
#define uxsino_fopen(filename, mode, fileauth)  \
    (fopen((filename), (mode)))
/**
 * @brief   �ر��ļ�
 */
#define uxsino_fclose(fp) \
    do {    \
        fclose(fp);   \
    }while(0)
/**
 * @brief   �ض�λ�ļ�
 */
#define uxsino_fseek(fp, offset, whence)  \
    (fseek((fp), (offset), (whence)))
/**
 * @brief   ���ļ�
 */
#define uxsino_fread(buf, size, count, fp)  \
    (fread((buf), (size), (count), (fp)))
/**
 * @brief   д�ļ�
 */
#define uxsino_fwrite(buf, size, count, fp) \
   (fwrite((buf), (size), (count), (fp)))

#define uxsino_fcntl(fp, mode, arg)  \
    (fcntl((fp), (mode), (arg)))

#define uxsino_fflush(fp) \
    (fflush((fp)))

#define uxsino_ftell(fp)	\
	(ftell(fp))

#define DEFINE_UXSINO_LOCKER(x) \
	uxsino_lock x = PTHREAD_MUTEX_INITIALIZER

#define uxsino_locker_lock_init(locker)	\
	do {					\
		pthread_mutex_init(locker, NULL);	\
	} while (0);

#define uxsino_locker_lock(locker)	\
	do {					\
		pthread_mutex_lock(locker);	\
	} while (0);

#define uxsino_locker_unlock(locker)	\
	do {					\
		pthread_mutex_unlock(locker);	\
	} while (0);


#define __GFP_WAIT  0x10u
#define __GFP_IO       0x40u
#define __GFP_FS       0x80u
#define GFP_KERNEL  (__GFP_WAIT | __GFP_IO | __GFP_FS)
#define GFP_USER 0x00
#endif



#endif

