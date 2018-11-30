//*******************************************************************************
// ��Ȩ����(C) 2015 uxsino All right reserved
// �ļ����� : db_interface.c
// ��ǰ�汾 : 1.0.0.1
// ��    �� : zhaozongpeng (zhaozp@uxsino.com)
// ������� : 2015��12��30��
// ����ժҪ : ���ݿ�����ӿڶ���
// �޸ļ�¼ : 
// ��    ��  ��    ��  �޸���   �޸�ժҪ

//*******************************************************************************

#include "db_interface.h"
//#include "ServOpLogCommon.h"
//#include "PubListenHostLog.h"

#include "sqlite3.h"
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#define DEBUG printf
/********************************
****  macro  define        ******
*********************************/
/*  #define _DB_DEBUG_SWITCH_  ON   // ���ļ����Դ��뿪��
#define DEBUG printf*/  

/********************************
**** interface declare     ******
*********************************/

// ��ʼ��sqlite3���ݿ�ʵ��
int db_sqlite3_connect(const char *pszDb_full_path);
int db_sqlite3_connect_ex(const char *pszDb_full_path,int flag);
//��ѯ
int db_sqlite3_query(const char *zSql,     /* SQL to be evaluated */
                char ***pazResult,    /* Results of the query */
                int *pnRow,           /* Number of result rows written here */
                int *pnColumn);       /* Number of result columns written here */   

int db_sqlite3_query_ex(const char *zSql,     /* SQL to be evaluated */
                char ***pazResult,    /* Results of the query */
                int *pnRow,           /* Number of result rows written here */
                int *pnColumn,int flag);       /* Number of result columns written here */   
// ִ��sql��䣬ֻ���ؽ����
int db_sqlite3_exec(const char *zSql);    /* SQL to be evaluated */
int db_sqlite3_exec_ex(const char *zSql,int flag);    /* SQL to be evaluated */
// ɾ����ѯ���صĶ�ά���ڴ�
void db_sqlite3_free_query_result(char **result);
// �ر����ݿ���
int db_sqlite3_close(void);
int db_sqlite3_close_ex(int flag);


// ��������
int db_insert(const char *zSql, const char *pszTbl_name);
int db_insert_ex(const char *zSql, const char *pszTbl_name,int flag);
// ɾ����ֻ���ؽ����
int db_drop_tbl(const char *zSql, const char *pszTbl_name);   
int db_drop_tbl_ex(const char *zSql, const char *pszTbl_name,int flag);   
// ɾ�����ݣ�ֻ���ؽ����
int db_del_data(const char *zSql, const char *pszTbl_name);   
int db_del_data_ex(const char *zSql, const char *pszTbl_name,int flag);   
// �������ݱ�ֻ���ؽ����
int db_create_tbl(const char *zSql, const char *pszTbl_name);   
int db_create_tbl_ex(const char *zSql, const char *pszTbl_name,int flag);   


// ��ʼ�����ݿ��
int db_tbl_init();
int db_tbl_init_ex(int flag);


#ifdef _DB_DEBUG_SWITCH_
// ������������ýӿ�
void db_test_main();
#endif


/********************************
**** global variable define******
*********************************/
static DB_CB_ST *s_pstDb_cb = NULL;                 // ���ݿ�������
static DB_TYPE *s_pDb_instance = NULL;              // ȫ�����ݿ�ʵ��
static DB_CB_ST_EX *s_pstDb_cb_ex[e_max_db_size] = {NULL};
static DB_TYPE *s_pDb_instance_ex[e_max_db_size] = {NULL};

typedef struct _Db_backup_stat
{
    pthread_mutex_t _lock;
    int _stat_flag_serv_op;
    int _stat_flag_file_integrality;
}Db_backup_stat;

static Db_backup_stat s_db_backup_stat;

// �ӿڳ�ʼ���б�, sqlite3�ֶνӿ����ǵ���ԭʼ���ݿ⺯��
#define DB_INTERFACE_LIST  db_sqlite3_connect, \
                           db_sqlite3_query, \
                           db_sqlite3_free_query_result, \
                           db_sqlite3_exec, \
                           db_sqlite3_close, \
                           db_insert, \
                           db_create_tbl, \
                           db_drop_tbl, \
                           db_del_data
                                
#define DB_INTERFACE_LIST_EX  db_sqlite3_connect_ex, \
                           db_sqlite3_query_ex, \
                           db_sqlite3_free_query_result, \
                           db_sqlite3_exec_ex, \
                           db_sqlite3_close_ex, \
                           db_insert_ex, \
                           db_create_tbl_ex, \
                           db_drop_tbl_ex, \
                           db_del_data_ex                                                    

static DB_CB_ST s_stDCB_init = {DB_INTERFACE_LIST};
static DB_CB_ST_EX s_stDCB_init_ex = {DB_INTERFACE_LIST_EX};
/********************************
**** interface define      ******
*********************************/

// ��ȡ���ݿ�ʵ������
DB_CB_ST* get_db_instance()
{
    return s_pstDb_cb;
}
DB_CB_ST_EX* get_db_instance_ex(int flag)
{
    return s_pstDb_cb_ex[flag];
}
// ��ʼ�����ݿ�
int db_init_ex(int flag)
{
    int iret = DB_ERR_SUC;
    //static DB_CB_ST_EX s_stDCB_init = {DB_INTERFACE_LIST_EX};

    if(NULL != s_pstDb_cb_ex[flag])
    {
        return DB_ERR_SUC;
    }

#if (DB_TYPE_USED == DB_TYPE_SQLITE3)
    // ��ʼ�����ݿ�ʵ���ṹ
    s_pstDb_cb_ex[flag] = &s_stDCB_init_ex;
#else
    return DB_ERR_DB_TYPE_UNKNOW;
#endif
    
    // �����ݿ�
    switch(flag)
    {
        case e_query_db:
            iret = s_pstDb_cb_ex[flag]->db_connect_fun(DATABASE_FULL_PATH,flag);        
            break;
    }
    if(DB_ERR_SUC != iret)
    {
        return iret;
    }

    // ��ʼ�����ݱ�
    iret = db_tbl_init_ex(flag);
    if(DB_ERR_SUC != iret)
    {
        return iret;
    }
    DEBUG("LYJ--- FINISH db_init_ex,\n");
    return DB_ERR_SUC;
}
int db_init()
{
    int iret = DB_ERR_SUC;
    

    if(NULL != s_pstDb_cb)
    {
        return DB_ERR_SUC;
    }

//#if (DB_TYPE_USED == DB_TYPE_SQLITE3)
    // ��ʼ�����ݿ�ʵ���ṹ
    s_pstDb_cb = &s_stDCB_init;
//#else
//    return DB_ERR_DB_TYPE_UNKNOW;
//#endif
    
    // �����ݿ�
    iret = s_pstDb_cb->db_connect_fun(DATABASE_FULL_PATH);
    if(DB_ERR_SUC != iret)
    {
        return iret;
    }

    // ��ʼ�����ݱ�
    //iret = db_tbl_init();
    //if(DB_ERR_SUC != iret)
    //{
    //    return iret;
    //}
#ifdef _DB_DEBUG_SWITCH_
    // ������������ýӿ�
    db_test_main();
#endif

    return DB_ERR_SUC;
}

// ��ʼ�����ݿ��
int db_tbl_init_ex(int flag)
{
    int iret = DB_ERR_SUC;
    
    char sql_tbl_info[512] = {0};
    switch(flag)
    {
        case e_query_db:
        {
            sprintf(sql_tbl_info,"%s",CREATE_TBL_UX_TBL_INFO);
            break;
        }
        case e_serv_log_db:
        {
            sprintf(sql_tbl_info,"%s",CREATE_TBL_SERVOPLOG);
            break;
        }
        case e_file_integrality_check_log_db:
        {
            sprintf(sql_tbl_info,"%s",CREATE_TBL_FILE_INTEGRALITY_LOG);
            break;
        }
        default:
            break;
    }
    if(NULL == s_pstDb_cb_ex[flag])
    {
        return DB_ERR_ERROR;
    }

    iret = s_pstDb_cb_ex[flag]->db_exec_fun(sql_tbl_info,flag);
    return iret;
}
int db_tbl_init()
{
    int iret = DB_ERR_SUC;
    char *sql_tbl_info = "create table if not exists tbl_ux_tbl_info(tname varchar(128) not null primary key,num integer,update_time integer,op_type int);";    
    char sql_query[256] = {0};
    char sql_insert[256] = {0};
    char **query_rst = NULL;
    int row = 0;
    int col = 0;
    if(NULL == s_pstDb_cb)
    {
        return DB_ERR_SUC;
    }
    
    iret = s_pstDb_cb->db_exec_fun(sql_tbl_info);
    if (DB_ERR_SUC != iret)
    {
        return iret;
    }
    // ������־�������ݿ���ʼ��
    iret = s_pstDb_cb->db_exec_fun(CREATE_TBL_LOG_BACKUP_CONFIG);
    if (DB_ERR_SUC != iret)
    {
        return iret;
    }
    sprintf(sql_query,"select * from %s",TBL_LOG_BACKUP_CONFIG);
    s_pstDb_cb->db_query_fun(sql_query,&query_rst,&row,&col);
    if (row < 1)
    {
        sprintf(sql_insert,INSERT_LOG_BACKUP_CFG,backup_serv_alarm_log,10,100,DATABASE_FULL_PATH);
        s_pstDb_cb->db_insert_fun(sql_insert,TBL_LOG_BACKUP_CONFIG);
        
        memset(sql_insert,0x0,256);
        sprintf(sql_insert,INSERT_LOG_BACKUP_CFG,backup_file_integrality_log,10,100,DATABASE_FULL_PATH);
        s_pstDb_cb->db_insert_fun(sql_insert,TBL_LOG_BACKUP_CONFIG);
    }
    s_pstDb_cb->db_free_query_result_fun(query_rst);
    // ϵͳ��Ϣ�����ֵ���ݿ���ʼ��
    iret = s_pstDb_cb->db_exec_fun(CREATE_TBL_SYS_INFO_THRESHOLD);
    if (DB_ERR_SUC != iret)
    {
        return iret;
    }
    // �ļ������Լ�����ñ��ʼ��
    iret = s_pstDb_cb->db_exec_fun(CREATE_TBL_FILE_INTEGRALITY_CONFIG);
    if (DB_ERR_SUC != iret)
    {
        return iret;
    }

	//��ȫ���м���ļ�md5����ʼ��
	iret = s_pstDb_cb->db_exec_fun(CREATE_TBL_SECURITY_OPER_CHECK);
    if (DB_ERR_SUC != iret)
    {
        return iret;
    }

	//��ȫ���߳�ʼ���ñ��ʼ��
	iret = s_pstDb_cb->db_exec_fun(CREATE_TBL_BASE_LINE_CHECK);
    if (DB_ERR_SUC != iret)
    {
        return iret;
    }

    //�˿ڹ����ʼ��
    iret = s_pstDb_cb->db_exec_fun(CREATE_TBL_PORT_STATUS);
    if (DB_ERR_SUC != iret)
    {
        return iret;    
    }
    return iret;
}

// �ر����ݿ�
void db_close_ex(int flag)
{
    int iret = DB_ERR_SUC;
    
    if(NULL == s_pstDb_cb_ex[flag])
    {
        return ;
    }

    // �ر����ݿ�
    iret = s_pstDb_cb_ex[flag]->db_close_fun(flag);
    if(DB_ERR_SUC != iret)
    {
        printf("db_close err[%d]\n", iret);
    }

    // �ͷ����ݿ�����ڴ�
    //uxsino_free(s_pstDb_cb); // ʹ�þֲ���̬������ʼ���������ͷ�
    s_pstDb_cb_ex[flag] = NULL;
    
    return;
}
void db_close()
{
    int iret = DB_ERR_SUC;
    
    if(NULL == s_pstDb_cb)
    {
        return ;
    }

    // �ر����ݿ�
    iret = s_pstDb_cb->db_close_fun();
    if(DB_ERR_SUC != iret)
    {
        printf("db_close err[%d]\n", iret);
    }

    // �ͷ����ݿ�����ڴ�
    //uxsino_free(s_pstDb_cb); // ʹ�þֲ���̬������ʼ���������ͷ�
    s_pstDb_cb = NULL;
    
    return;
}


// ��ѯ  ��ṹ�ǣ���һ���������������в���ֵ��
// �����ķ�ʽ�Ͷ�ά������ͬ, ��һά���ݴ洢
/*
    ���ݱ�����ʾ��:
    Name        | Age
    -----------------------
    Alice       | 43
    Bob         | 28
    Cindy       | 21

    ���ؽ������:
    azResult[0] = "Name";
    azResult[1] = "Age";
    azResult[2] = "Alice";
    azResult[3] = "43";
    azResult[4] = "Bob";
    azResult[5] = "28";
    azResult[6] = "Cindy";
    azResult[7] = "21";

*/
int db_sqlite3_query_ex(const char *zSql,     /* SQL to be evaluated */
                char ***pazResult,    /* Results of the query */
                int *pnRow,           /* Number of result rows written here */
                int *pnColumn,int flag)        /* Number of result columns written here */  
{
    int iret = DB_ERR_SUC;
    char *pzErrmsg = NULL;
    
    if(NULL == s_pDb_instance_ex[flag])
    {
        return DB_ERR_NULL_DBCB_ERR;
    }

    if(NULL == zSql)
    {
        return DB_ERR_SQL_ERR;
    }

    // ��ѯ���ݿ�
    iret = sqlite3_get_table(s_pDb_instance_ex[flag], 
                    zSql, 
                    pazResult, 
                    pnRow, 
                    pnColumn, 
                    &pzErrmsg);
    if(SQLITE_OK != iret)
    {
        if(NULL == pzErrmsg)
        {
            printf("sqlite3_get_table err[%d]\n", iret);
        } else {
            printf("sqlite3_get_table err[%d][%s]\n", iret, pzErrmsg);
            sqlite3_free(pzErrmsg);
            pzErrmsg = NULL;
        }
        return iret;
    }
    return DB_ERR_SUC;
}
int db_sqlite3_query(const char *zSql,     /* SQL to be evaluated */
                char ***pazResult,    /* Results of the query */
                int *pnRow,           /* Number of result rows written here */
                int *pnColumn)        /* Number of result columns written here */   
{
    int iret = DB_ERR_SUC;
    char *pzErrmsg = NULL;
    
    if(NULL == s_pDb_instance)
    {
        return DB_ERR_NULL_DBCB_ERR;
    }

    if(NULL == zSql)
    {
        return DB_ERR_SQL_ERR;
    }

    // ��ѯ���ݿ�
    iret = sqlite3_get_table(s_pDb_instance, 
                    zSql, 
                    pazResult, 
                    pnRow, 
                    pnColumn, 
                    &pzErrmsg);
    if(SQLITE_OK != iret)
    {
        if(NULL == pzErrmsg)
        {
            printf("sqlite3_get_table err[%d]\n", iret);
        }
        else
        {
            printf("sqlite3_get_table err[%d][%s]\n", iret, pzErrmsg);
            sqlite3_free(pzErrmsg);
            pzErrmsg = NULL;
        }
        
        return iret;
    }
    return DB_ERR_SUC;
}


// ��������
int db_insert(const char *zSql, const char *pszTbl_name) 
{
    int iret = DB_ERR_SUC;
    
    if(NULL == s_pstDb_cb)
    {
        return DB_ERR_NULL_DBCB_ERR;
    }

    if((NULL == zSql) || (NULL == pszTbl_name))
    {
        return DB_ERR_AGRS_ERR;
    }

    // ִ��SQL���
    iret = s_pstDb_cb->db_exec_fun(zSql);
    if(DB_ERR_SUC != iret)
    {
        return iret;
    }       

    return DB_ERR_SUC;
}

int db_insert_ex(const char *zSql, const char *pszTbl_name,int flag) 
{
    int iret = DB_ERR_SUC;
    
    if(NULL == s_pstDb_cb_ex[flag])
    {
        return DB_ERR_NULL_DBCB_ERR;
    }

    if((NULL == zSql) || (NULL == pszTbl_name))
    {
        return DB_ERR_AGRS_ERR;
    }

    // ִ��SQL���
    iret = s_pstDb_cb_ex[flag]->db_exec_fun(zSql,flag);
    if(DB_ERR_SUC != iret)
    {
        return iret;
    }       

    return DB_ERR_SUC;
}
// ִ��sql��䣬ֻ���ؽ����
int db_sqlite3_exec(const char *zSql)   /* SQL to be evaluated */
{
    int iret = DB_ERR_SUC;
    char *pzErrmsg = NULL;      // ������Ϣ
    
    if(NULL == s_pDb_instance)
    {
        return DB_ERR_NULL_DBCB_ERR;
    }

    if(NULL == zSql)
    {
        return DB_ERR_AGRS_ERR;
    }

    // ִ��SQL���
    iret = sqlite3_exec(s_pDb_instance, 
                        zSql, 
                        NULL, 
                        NULL, 
                        &pzErrmsg);
    if(SQLITE_OK != iret)
    {
        if(NULL == pzErrmsg)
        {
            printf("sqlite3_exec err[%d]\n", iret);
        }
        DEBUG("sqlite3_exec err[%d][%s]\n", iret, pzErrmsg);
        sqlite3_free(pzErrmsg);
        pzErrmsg = NULL;
        return iret;
    }       

    return DB_ERR_SUC;
}
int db_sqlite3_exec_ex(const char *zSql,int flag)   /* SQL to be evaluated */
{
    int iret = DB_ERR_SUC;
    char *pzErrmsg = NULL;      // ������Ϣ
    
    if(NULL == s_pDb_instance_ex[flag])
    {
        return DB_ERR_NULL_DBCB_ERR;
    }

    if(NULL == zSql)
    {
        return DB_ERR_AGRS_ERR;
    }

    // ִ��SQL���
    iret = sqlite3_exec(s_pDb_instance_ex[flag], 
                        zSql, 
                        NULL, 
                        NULL, 
                        &pzErrmsg);
    if(SQLITE_OK != iret)
    {
        if(NULL == pzErrmsg)
        {
            DEBUG("sqlite3_exec err[%d]\n", iret);
        }
        DEBUG("sqlite3_exec err[%d][%s]\n", iret, pzErrmsg);
        sqlite3_free(pzErrmsg);
        pzErrmsg = NULL;
        return iret;
    }       

    return DB_ERR_SUC;
}

// ɾ����ѯ���صĶ�ά���ڴ�
void db_sqlite3_free_query_result(char **result)
{
    if((NULL == result) || (NULL == *result))
    {
        return;
    }
    
    sqlite3_free_table(result);
}

// �ر����ݿ���
int db_sqlite3_close(void)
{
    int iret = DB_ERR_SUC;
    if(NULL == s_pDb_instance)
    {
        return iret;
    }
    
    iret = sqlite3_close(s_pDb_instance);
    if(SQLITE_OK != iret)
    {
        printf("sqlite3_close err:%d\n", iret);
        return iret;
    }
    s_pDb_instance = NULL;
    
    return DB_ERR_SUC;
}

int db_sqlite3_close_ex(int flag)
{
    int iret = DB_ERR_SUC;
    if(NULL == s_pDb_instance_ex[flag])
    {
        return iret;
    }
    
    iret = sqlite3_close(s_pDb_instance_ex[flag]);
    if(SQLITE_OK != iret)
    {
        printf("sqlite3_close err:%d\n", iret);
        return iret;
    }
    s_pDb_instance_ex[flag] = NULL;
    
    return DB_ERR_SUC;
}

// ��ʼ��sqlite3���ݿ�ʵ��
int db_sqlite3_connect(const char *pszDb_full_path)
{
    int iret = DB_ERR_SUC;
    if(NULL != s_pDb_instance)
    {
        return DB_ERR_DB_OPENED;
    }
    
    if(NULL == pszDb_full_path)
    {
        return DB_ERR_AGRS_ERR;
    }

    // �����ݿ�
    iret = sqlite3_open(pszDb_full_path, &s_pDb_instance);
    if(SQLITE_OK != iret)
    {
        s_pDb_instance = NULL;
        printf("sqlite3_open err:%d\n", iret);
        return iret;
    }
    
    return DB_ERR_SUC;
}

int db_sqlite3_connect_ex(const char *pszDb_full_path,int flag)
{
    int iret = DB_ERR_SUC;
    if(NULL != s_pDb_instance_ex[flag])
    {
        return DB_ERR_DB_OPENED;
    }
    
    if(NULL == pszDb_full_path)
    {
        return DB_ERR_AGRS_ERR;
    }

    // �����ݿ�
    iret = sqlite3_open(pszDb_full_path, &s_pDb_instance_ex[flag]);
    if(SQLITE_OK != iret)
    {
        s_pDb_instance_ex[flag] = NULL;
        printf("sqlite3_open err:%d\n", iret);
        return iret;
    }
    
    return DB_ERR_SUC;
}

// ɾ����ֻ���ؽ����
int db_drop_tbl(const char *zSql, const char *pszTbl_name)   
{
    int iret = DB_ERR_SUC;
    char sql[DB_SQL_MAX_LEN] = {0};
    
    if(NULL == s_pstDb_cb)
    {
        return DB_ERR_NULL_DBCB_ERR;
    }
    
    if((NULL == zSql) || (NULL == pszTbl_name))
    {
        return DB_ERR_AGRS_ERR;
    }

    // ִ��ɾ����sql
    iret = s_pstDb_cb->db_exec_fun(zSql);
    if(DB_ERR_SUC != iret)
    {
        return iret;
    }

    // ���±���Ϣ���е�����
    sprintf(sql, "delete from tbl_ux_tbl_info where tname=\"%s\";", pszTbl_name);
    iret = s_pstDb_cb->db_exec_fun(sql);
    if(DB_ERR_SUC != iret)
    {
        return iret;
    }
    
#if (ON == DB_VACUUM_SPACE_SWITCH)
    // �ͷŴ��̿ռ�
    sprintf(sql, "vacuum;");
    iret = s_pstDb_cb->db_exec_fun(sql);
    if(DB_ERR_SUC != iret)
    {
        return iret;
    }
#endif
    
    return DB_ERR_SUC;
}

int db_drop_tbl_ex(const char *zSql, const char *pszTbl_name,int flag)   
{
    int iret = DB_ERR_SUC;
    char sql[DB_SQL_MAX_LEN] = {0};
    
    if(NULL == s_pstDb_cb_ex[flag])
    {
        return DB_ERR_NULL_DBCB_ERR;
    }
    
    if((NULL == zSql) || (NULL == pszTbl_name))
    {
        return DB_ERR_AGRS_ERR;
    }

    // ִ��ɾ����sql
    iret = s_pstDb_cb_ex[flag]->db_exec_fun(zSql,flag);
    if(DB_ERR_SUC != iret)
    {
        return iret;
    }

    // ���±���Ϣ���е�����
    if (flag == e_query_db)
    {
        sprintf(sql, "delete from tbl_ux_tbl_info where tname=\"%s\";", pszTbl_name);
        iret = s_pstDb_cb_ex[flag]->db_exec_fun(sql,flag);
        if(DB_ERR_SUC != iret)
        {
            return iret;
        }
    }
    
    
#if (ON == DB_VACUUM_SPACE_SWITCH)
    // �ͷŴ��̿ռ�
    sprintf(sql, "vacuum;");
    iret = s_pstDb_cb_ex[flag]->db_exec_fun(sql,flag);
    if(DB_ERR_SUC != iret)
    {
        return iret;
    }
#endif
    
    return DB_ERR_SUC;
}

// ɾ�����ݣ�ֻ���ؽ����
int db_del_data(const char *zSql, const char *pszTbl_name)
{
    int iret = DB_ERR_SUC;
    
    if(NULL == s_pstDb_cb)
    {
        return DB_ERR_NULL_DBCB_ERR;
    }

    if((NULL == zSql) || (NULL == pszTbl_name))
    {
        return DB_ERR_AGRS_ERR;
    }

    // ִ��SQL���
    iret = s_pstDb_cb->db_exec_fun(zSql);
    if(DB_ERR_SUC != iret)
    {
        return iret;
    }       

    return DB_ERR_SUC;
}

int db_del_data_ex(const char *zSql, const char *pszTbl_name,int flag)
{
    int iret = DB_ERR_SUC;
    
    if(NULL == s_pstDb_cb_ex[flag])
    {
        return DB_ERR_NULL_DBCB_ERR;
    }

    if((NULL == zSql) || (NULL == pszTbl_name))
    {
        return DB_ERR_AGRS_ERR;
    }

    // ִ��SQL���
    iret = s_pstDb_cb_ex[flag]->db_exec_fun(zSql,flag);
    if(DB_ERR_SUC != iret)
    {
        return iret;
    }       

    return DB_ERR_SUC;
}

// �������ݱ�ֻ���ؽ����
int db_create_tbl(const char *zSql, const char *pszTbl_name)  
{
    int iret = DB_ERR_SUC;
    char sql[DB_SQL_MAX_LEN] = {0};
    
    if(NULL == s_pstDb_cb)
    {
        return DB_ERR_NULL_DBCB_ERR;
    }

    if((NULL == zSql) || (NULL == pszTbl_name))
    {
        return DB_ERR_AGRS_ERR;
    }

    // ִ��SQL���
    iret = s_pstDb_cb->db_exec_fun(zSql);
    if(DB_ERR_SUC != iret)
    {
        return iret;
    }       

    // ���±���Ϣ���е����ݣ�����Ԥ�������Ż�,����Ƶ��ˢ�¿��
    sprintf(sql, "insert into tbl_ux_tbl_info(tname, num, update_time, op_type) values(\"%s\",0,0,0);", pszTbl_name);
    iret = s_pstDb_cb->db_exec_fun(sql);
    if(DB_ERR_SUC != iret)
    {
        return iret;
    }
    
    return DB_ERR_SUC;
}

int db_create_tbl_ex(const char *zSql, const char *pszTbl_name,int flag)  
{
    int iret = DB_ERR_SUC;
    char sql[DB_SQL_MAX_LEN] = {0};
    
    if(NULL == s_pstDb_cb_ex[flag])
    {
        return DB_ERR_NULL_DBCB_ERR;
    }

    if((NULL == zSql) || (NULL == pszTbl_name))
    {
        return DB_ERR_AGRS_ERR;
    }

    // ִ��SQL���
    iret = s_pstDb_cb_ex[flag]->db_exec_fun(zSql,flag);
    if(DB_ERR_SUC != iret)
    {
        return iret;
    }       

    // ���±���Ϣ���е����ݣ�����Ԥ�������Ż�,����Ƶ��ˢ�¿��
    if (flag == e_query_db)
    {
        sprintf(sql, "insert into tbl_ux_tbl_info(tname, num, update_time, op_type) values(\"%s\",0,0,0);", pszTbl_name);
        iret = s_pstDb_cb_ex[flag]->db_exec_fun(sql,flag);
        if(DB_ERR_SUC != iret)
        {
            return iret;
        }
    }
    
    
    return DB_ERR_SUC;
}

// ��ʼ������״̬
void init_backup_db_stat_flag(void)
{
    pthread_mutex_init(&(s_db_backup_stat._lock),NULL);
    s_db_backup_stat._stat_flag_file_integrality = 0;
    s_db_backup_stat._stat_flag_serv_op = 0;
}
// ����ָ�����ݿ�ı���״̬
void set_backup_db_stat_flag(int stat,int db_type)
{
    pthread_mutex_lock(&(s_db_backup_stat._lock));
    if (db_type == backup_serv_alarm_log)
        s_db_backup_stat._stat_flag_serv_op = stat;
    else if (db_type == backup_file_integrality_log)
        s_db_backup_stat._stat_flag_file_integrality = stat;
    pthread_mutex_unlock(&(s_db_backup_stat._lock));
}
// ��ȡָ�����ݿ�ı���״̬
int get_backup_db_stat_flag(int db_type)
{
    int stat = 0;
    pthread_mutex_lock(&(s_db_backup_stat._lock));
    if (db_type == backup_serv_alarm_log)
        stat = s_db_backup_stat._stat_flag_serv_op;
    else if (db_type == backup_file_integrality_log)
        stat = s_db_backup_stat._stat_flag_file_integrality;
    pthread_mutex_unlock(&(s_db_backup_stat._lock));
    return stat;
}
// �ͷű���״̬���
void release_backup_db_stat_flag(void)
{
    pthread_mutex_destroy(&(s_db_backup_stat._lock));
    s_db_backup_stat._stat_flag_file_integrality = 0;
    s_db_backup_stat._stat_flag_serv_op = 0;
}

///////////////////////////////////////////////////////////////
// �������������
// ����db_test_main �ɲ��Խ������룬��ѯ�����ɾ��
//#ifdef _DB_DEBUG_SWITCH_
//  db_test_main();
//#endif    
//
///////////////////////////////////////////////////////////////
#ifdef _DB_DEBUG_SWITCH_    //���Դ���
#include <stdio.h>
#include <time.h>
const int const_data_count = 99999;
time_t timep;

#define TIME_PRINT(x) do{\
        time (&timep);\
        DEBUG("%s:%s\n",x, asctime(gmtime(&timep)));\
    }while(0)
    
int db_test_insert_commit();
int db_test_create_table();
void db_test_show_data(char **pszData, int row, int column);
int db_test_query();
int db_test_drop_table_data();
int db_test_drop_table();



void db_test_main()
{
    int iret = 0;

    db_test_drop_table();
    do
    {
        // ����
        iret = db_test_create_table();
        if(0 != iret)
        {
            break;
        }

        // ��������
        iret = db_test_insert_commit();
        if(0 != iret)
        {
            break;
        }

        // ��ѯ
        iret = db_test_query();
        if(0 != iret)
        {
            printf("db_test_query err\n");
            break;
        }

        // ɾ����������,Ч�ʱȽϵ�
        iret = db_test_drop_table_data();
        if(0 != iret)
        {
            printf("db_test_drop_table_data err\n");
            break;
        }

        // ɾ�����������ݲ���ֱ��ɾ����
        iret = db_test_drop_table();
        if(0 != iret)
        {
            printf("db_test_drop_table err\n");
            break;
        }
        
    }while(1000);
    printf("db_test_main %d\n", iret);

    return;
}

// ����ʽ�����������
int db_test_insert_commit()
{

    char sql[1024] = {0};
    int iret = 0;
    int icount = 0; // ���ݼ�¼����

    const DB_CB_ST *pstDB = get_db_instance();
    if(NULL == pstDB)
    {
        printf("get_db_instance NULL\n");
        return -1;
    }   

    do
    {
        TIME_PRINT("insert begin");
        
        // ��������
        iret = pstDB->db_insert_fun("begin;", "db_test");
        if(0 != iret)
        {
            break;
        }
        
        for(icount = 0; icount < const_data_count; icount++)
        {
            memset(sql, 0x00, sizeof(sql));
            sprintf(sql, 
                    "insert into db_test(sid, sname,time) values(%d, \"n_%d\", \"2015-01-01\");\n", 
                    icount, icount);

            iret = pstDB->db_insert_fun(sql, , "db_test");
            if(0 != iret)
            {
                break;
            }
        }       
    }
    while(0);

    if(0 != iret)
    {
        iret = pstDB->db_insert_fun("rollback;", "db_test");
        return iret;
    }
    iret = pstDB->db_insert_fun("commit;", "db_test");
    TIME_PRINT("insert end");
    return iret;
}

// ������
int db_test_create_table()
{
    int iret = 0;
    char *strsql1 = "create table if not exists db_test(sid integer not null primary key,sname varchar(256),time date)";
    char *strsql2 = "create table if not exists ux_group(GID integer not null primary key,group_name varchar(256),user_list TEXT)";

    DB_CB_ST *pstDB = get_db_instance();
    if(NULL == pstDB)
    {
        printf("get_db_instance NULL\n");
        return -1;
    }   

    iret = pstDB->db_create_tbl_fun(strsql1, "db_test");   
    return iret;
}

int db_test_drop_table()
{
    int iret = 0;
    char *strsql = "drop table db_test;";
    DB_CB_ST *pstDB = get_db_instance();
    if(NULL == pstDB)
    {
        printf("get_db_instance NULL\n");
        return -1;
    }   

    TIME_PRINT("drop table begin");
    iret = pstDB->db_drop_tbl_fun(strsql, "db_test");
    TIME_PRINT("drop table end");
    return iret;
}

int db_test_drop_table_data()
{
    int iret = 0;
    char *strsql = "delete from db_test;";
    DB_CB_ST *pstDB = get_db_instance();
    if(NULL == pstDB)
    {
        printf("get_db_instance NULL\n");
        return -1;
    }   

    TIME_PRINT("drop table data begin");
    iret = pstDB->db_del_data_fun(strsql, "db_test");
    TIME_PRINT("drop table data end");
    return iret;
}

// ��һ��Ϊ��ͷ����һά������������
void db_test_show_data(char **pszData, int row, int column)
{
    int i = 0;
    int j = 0;

    for(; i <= row; i++)
    {
        for(j = 0; j < column; j++)
        {
            printf("%s\t", pszData[i * column + j]);
        }
        printf("\n");
    }
    
    return;
}


int db_test_query()
{
    int iret = 0;
    char sql[1024] = {0};

    int irow = 0;
    int icolumn = 0;
    char **pszresult = NULL;
    int offset = 0;
    
    DB_CB_ST *pstDB = get_db_instance();
    if(NULL == pstDB)
    {
        printf("get_db_instance NULL\n");
        return -1;
    }

    TIME_PRINT("select begin");
    // ��ѯ��������
    do
    {
        memset(sql, 0x00, sizeof(sql));
        sprintf(sql, "select * from db_test order by sid limit 200 offset %d;", offset);

        iret = pstDB->db_query_fun(sql, &pszresult, &irow, &icolumn);
        if(0 != iret)
        {
            printf("db_query_fun err\n");
            break;
        }

        // ��ʾ����
        //db_test_show_data(pszresult, irow, icolumn);
        
        // �ͷŲ�ѯ����ڴ�
        pstDB->db_free_query_result_fun(pszresult);
        pszresult = NULL;

        if(irow < 200)
        {
            break;
        }

        offset += 200;
    }while(!iret);

    // �ͷŲ�ѯ����ڴ�
    pstDB->db_free_query_result_fun(pszresult);
    pszresult= NULL;
    TIME_PRINT("select end");
    return iret;
}


#endif      //���Դ���

/*
* nh 2017.05.19 
* �ɹ�:���ط�����������Ϣ�е�����ΨһID��ʧ��:����NULL
*/
int get_server_base_info_HostUuid(char * ret)
{
    //char* ret = NULL;
	int rt = 0;
    char sql[1024] = {0};
	int row = 0, col = 0;
	DB_CB_ST* db_cb_st = NULL;
	char** data = NULL;
   
	db_cb_st = get_db_instance();
	if(NULL == db_cb_st){
		return NULL;
	}
	
	sprintf(sql, SELECT_TBL_SERVER_BASE_INFO, "HostUuid");
	rt = db_cb_st->db_query_fun(sql, &data, &row, &col);
	if(DB_ERR_SUC != rt){
		db_cb_st->db_exec_fun("rollback;");
	}
	db_cb_st->db_exec_fun("commit;");
	if(row > 0){
		strcpy(ret, data[1]);
	}
	else{
		ret = NULL;
	}

    db_cb_st->db_free_query_result_fun(data);
    data = NULL;

	return rt;
}



