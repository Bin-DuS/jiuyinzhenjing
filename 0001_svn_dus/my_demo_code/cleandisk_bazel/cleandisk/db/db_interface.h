//*******************************************************************************
// ��Ȩ����(C) 2015 uxsino All right reserved
// �ļ����� : db_interface.h
// ��ǰ�汾 : 1.0.0.1
// ��    �� : zhaozongpeng (zhaozp@uxsino.com)
// ������� : 2015��12��30��
// ����ժҪ : ���ݿ�����ӿڶ���
// �޸ļ�¼ : 
// ��    ��  ��    ��  �޸���   �޸�ժҪ

//*******************************************************************************

#ifndef __DB_INTERFACE_H__
#define __DB_INTERFACE_H__
#define ON      1
#define OFF     0


#define DATABASE_FULL_PATH          "/opt/data.db"    // ���ݿ��ļ�·�����������Ǽӵ������ļ�
#define DB_TYPE_SQLITE3             1                       // sqlite3 ����
#define DB_SQL_MAX_LEN              256
#define DB_TBL_NAME_MAX_LEN         64
#define DB_BUSY_RETRY_COUNT         3   // ��
#define DB_BUSY_RETRY_DELAY_TIME    10  // ms


// ���ݿ���ɾ��������ݺ��Ƿ��ͷŴ��̿ռ�, �����ļ���ʱӰ������
#define DB_VACUUM_SPACE_SWITCH      ON              
// ���ݿ����ڱ�����������BUSY������������
#define DB_BUSY_RETRY_SWITCH        ON
// ���ݿⷵ��BUSY,����ʱ��ÿ���ӳ�ʱ��
#define DB_BUSY_RETRY_DELAY_SWITCH  ON


// �������
#define CREATE_TBL_UX_TBL_INFO "create table if not exists tbl_ux_tbl_info(tname varchar(128) not null primary key,num integer,update_time integer,op_type int);"
// ������־��
#define CREATE_TBL_SERVOPLOG "CREATE TABLE if not exists 'tbl_servoplog' ('level'  INTEGER NOT NULL,\
'subjectid'  INTEGER NOT NULL,\
'subjectname'  TEXT NOT NULL,\
'objectid'  INTEGER NOT NULL,\
'objectname'  TEXT NOT NULL,\
'time'  TEXT NOT NULL,\
'evtid'  INTEGER NOT NULL,\
'evtdesc'  TEXT NOT NULL,\
'result' INTEGER);"
#define SELECT_TBL_SERVOPLOG_REPEAT_SYS_ALARM "SELECT time FROM tbl_servoplog WHERE \
objectname = '%s' ORDER BY time DESC LIMIT 0,1;"

// ��־�������ñ�
typedef enum 
{
    backup_serv_alarm_log = 0,
    backup_file_integrality_log
}log_backup_type;
#define TBL_LOG_BACKUP_CONFIG "tbl_log_backup_config"
#define CREATE_TBL_LOG_BACKUP_CONFIG "create table if not exists tbl_log_backup_config\
('type' TEXT not null,'filenum' TEXT not null,'filesize' TEXT not null,'backupdir' TEXT,'curfilenum' TEXT);"
#define INSERT_LOG_BACKUP_CFG "INSERT INTO tbl_log_backup_config (type,filenum,filesize,curfilenum,backupdir) \
VALUES ('%d','%d','%d','0','%s');"
#define UPDATE_LOG_BACKUP_CONFIG "UPDATE tbl_log_backup_config SET filenum = '%d', filesize = '%d' , backupdir = '%s' WHERE type = '%d';"

#define UPDATE_LOG_BACKUP_CONFIG_CUR "UPDATE tbl_log_backup_config SET filenum = '%d', filesize = '%d' , backupdir = '%s' , curfilenum = '%d' WHERE type = '%d';"

#define UPDATE_LOG_BACKUP_CONFIG_CUR_NUM "UPDATE tbl_log_backup_config SET curfilenum = '%d' WHERE type = '%d';"
#define QUERY_LOG_BACKUP_CONFIG "SELECT filenum,filesize,backupdir,curfilenum FROM tbl_log_backup_config WHERE type = '%d';"

// ϵͳ��Ϣ��ֵ��
#define TBL_SYS_INFO_THRESHOLD "tbl_sys_info_threshold"
#define CREATE_TBL_SYS_INFO_THRESHOLD "create table if not exists tbl_sys_info_threshold \
('type' TEXT not null,'eq_name' TEXT not null,'threshold' TEXT not null);"
#define INSERT_TBL_SYS_INFO_THRESHOLD "INSERT INTO tbl_sys_info_threshold ('type','eq_name','threshold') \
VALUES('%d','%s','%d');"
#define UPDATE_TBL_SYS_INFO_THRESHOLD "UPDATE tbl_sys_info_threshold SET type = '%d',eq_name = '%s',threshold = '%d' \
WHERE type = '%d' AND eq_name = '%s';"
#define SELECT_TBL_SYS_INFO_THRESHOLD_WHERE_TYPE_NAME "SELECT COUNT(*) FROM tbl_sys_info_threshold WHERE \
type = '%d' AND eq_name = '%s';"
#define SELECT_TBL_SYS_INFO_THRESHOLD_ALL "SELECT type,eq_name,threshold FROM tbl_sys_info_threshold;"
#define SELCT_TBL_SYS_INFO_THRESHOLD_COUNT_TYPE "SELECT COUNT(*) FROM tbl_sys_info_threshold WHERE \
type = '%d';"
#define SELECT_TBL_SYS_INFO_THRESHOLD_BY_NAME "SELECT type,eq_name,threshold FROM tbl_sys_info_threshold WHERE eq_name = '%s';"
// ���������ñ�
#define TBL_FILE_INTEGRALITY_CONFIG "tbl_file_integrality_config"
#define CREATE_TBL_FILE_INTEGRALITY_CONFIG "create table if not exists tbl_file_integrality_config \
('dir' TEXT not null,'file_name' TEXT not null,'md5' TEXT,'config_time' TEXT);"
#define INSERT_TBL_FILE_INTEGRALITY_CONFIG "INSERT INTO tbl_file_integrality_config ('dir','file_name','md5','config_time') \
VALUES('%s','%s','%s','%s');"
#define UPDATE_TBL_FILE_INTEGRALITY_CONFIG "UPDATE tbl_file_integrality_config SET md5 = '%s',config_time = '%s' WHERE \
dir = '%s' AND file_name = '%s';"
#define SELECT_TBL_FILE_INTEGRALITY_CONFIG_WHERE_DIR_FILENAME "SELECT md5,config_time FROM tbl_file_integrality_config WHERE \
dir = '%s' AND file_name = '%s';"
#define DELETE_TBL_FILE_INTEGRALITY_CONFIG_WHERE_DIR_FILENAME "DELETE FROM tbl_file_integrality_config WHERE \
dir = '%s' AND file_name = '%s';"

// �����Լ����־��
//modify by hx on 2015.05.06
#define DELETE_TBL_FILE_INTEGRALITY_LOG "DELETE FROM tbl_file_integrality_log WHERE dir ='%s' and file_name='%s';"
#define SELECT_TBL_FILE_INTEGRALITY_CONFIG_ALL "SELECT dir,file_name FROM tbl_file_integrality_config;"
//end by hx

#define CREATE_TBL_FILE_INTEGRALITY_LOG "create table if not exists tbl_file_integrality_log \
('dir' TEXT not null,'file_name' TEXT not null,'md5' TEXT,'config_time' INTEGER,'check_result' TEXT);"
#define INSERT_TBL_FILE_INTEGRALITY_LOG "INSERT INTO tbl_file_integrality_log ('dir','file_name','md5','config_time','check_result') \
VALUES('%s','%s','%s',%s,'%d');"
#define SELECT_TBL_FILE_INTEGRALITY_LOG_FILE_NAME "SELECT dir,file_name,md5,config_time,check_result FROM tbl_file_integrality_log \
WHERE file_name LIKE '%%%s%%' ORDER BY config_time DESC LIMIT %d,%d;"
#define SELECT_TBL_FILE_INTEGRALITY_LOG_TIME "SELECT dir,file_name,md5,config_time,check_result FROM tbl_file_integrality_log \
WHERE config_time BETWEEN %s AND %s ORDER BY config_time DESC LIMIT %d,%d;"

#define SELECT_TBL_FILE_INTEGRALITY_LOG_RESULT "SELECT dir,file_name,md5,config_time,check_result FROM tbl_file_integrality_log \
WHERE check_result = '%s' ORDER BY config_time DESC LIMIT %d,%d;"
//��ȫ���м���
#define TBL_SECURITY_OPER_CHECK "tbl_security_oper_check"
#define CREATE_TBL_SECURITY_OPER_CHECK "create table if not exists tbl_security_oper_check(file_path varchar(256),md5 varchar(256));"
#define INSERT_TBL_SECURITY_OPER_CHECK "INSERT INTO tbl_security_oper_check (file_path,md5) VALUES(\"%s\",\"%s\");"

#define TBL_BASE_LINE_CHECK "tbl_base_line_check"
#define CREATE_TBL_BASE_LINE_CHECK "create table if not exists tbl_base_line_check(class varchar(256),value TEXT);"
#define INSERT_TBL_BASE_LINE_CHECK "INSERT INTO tbl_base_line_check (class,value) VALUES(\"%s\",\"%s\");"
#define INSERT_TBL_PASSWD_DAY "INSERT INTO tbl_base_line_check (class,value) VALUES(\"%s\",\"%s\");"
#define SELECT_TBL_PASSWD_DAY "SELECT value FROM tbl_base_line_check WHERE class = '%s';"

#define TBL_SERVER_BASE_INFO "tbl_server_base_info"
#define CREATE_TBL_SERVER_BASE_INFO "create table if not exists tbl_server_base_info(class varchar(256),value TEXT);"
#define INSERT_TBL_SERVER_BASE_INFO "INSERT INTO tbl_server_base_info (class,value) VALUES(\"%s\",\"%s\");"
#define SELECT_TBL_SERVER_BASE_INFO "SELECT value FROM tbl_server_base_info WHERE class = '%s';"
#define DELETE_TBL_SERVER_BASE_INFO "delete from tbl_server_base_info;"

#define TBL_PORT_STATUS "tbl_port_status"
#define CREATE_TBL_PORT_STATUS "create table if not exists tbl_port_status(class varchar(256),value TEXT);"
#define INSERT_TBL_PORT_STATUS "INSERT INTO tbl_port_status (class,value) VALUES(\"%s\",\"%s\");"
#define SELECT_TBL_PORT_STATUS "SELECT * FROM tbl_port_status;"
#define UPDATE_TBL_PORT_STATUS "UPDATE tbl_port_status SET value = '%s' WHERE class = '%s';"
#define DELETE_TBL_PORT_STATUS "DELETE FROM tbl_port_status WHERE class ='%s' ;"


#if (DB_TYPE_USED == DB_TYPE_SQLITE3)
struct sqlite3;
#define DB_TYPE struct sqlite3                              // ���ݿ����Ͷ���
#else
#define DB_TYPE void
#endif


typedef enum db_flag
{
    e_query_db = 0,
    e_serv_log_db,
    e_file_integrality_check_log_db,        // �ļ������Լ����־
    e_max_db_size
}DB_FLAG;

#define DB_MODULE_ID        0x20                            // ģ��ID
#define MAKECODE(module, code) (module << 16 | code)

//#define SERV_ALARM_LOG_BACKUP_PATH ALARM_LOG_FULL_PATH
//#define SERV_FILE_INTEGRALITY_LOG_BACKUP_PATH FILE_LOG_FULL_PATH


// ���ݿ�ʵ�����󣬼���ز����ӿ�
typedef struct db_context_block
{
    int (*db_connect_fun)(const char*);             // �������ݿ�
    int (*db_query_fun)(const char *,               // ��ѯ���ݿ⣬���ؽ��Ϊ��ά��
                    char ***, 
                    int *, 
                    int *);
    void (*db_free_query_result_fun)(char **result);            // ɾ����ѯ���صĶ�ά���ڴ�
    int (*db_exec_fun)(const char *);                           // ִ��sql���,ֻ����ִ�н��
    int (*db_close_fun)(void);                                  // �ر����ݿ�ʵ��
    int (*db_insert_fun)(const char *, const char *);           // �������ݿ⣬ִ�в���sql���
    int (*db_create_tbl_fun)(const char *, const char *);       // ִ�д����� sql���
    int (*db_drop_tbl_fun)(const char *, const char *);         // ɾ���������ռ�õĴ��̿ռ�
    int (*db_del_data_fun)(const char *, const char *);         // ִ��ɾ������ sql���
}DB_CB_ST;

typedef struct db_context_block_ex
{
    int (*db_connect_fun)(const char*,int);             // �������ݿ�
    int (*db_query_fun)(const char *,               // ��ѯ���ݿ⣬���ؽ��Ϊ��ά��
                    char ***, 
                    int *, 
                    int *,
                    int);
    void (*db_free_query_result_fun)(char **result);            // ɾ����ѯ���صĶ�ά���ڴ�
    int (*db_exec_fun)(const char *,int);                           // ִ��sql���,ֻ����ִ�н��
    int (*db_close_fun)(int);                                  // �ر����ݿ�ʵ��
    int (*db_insert_fun)(const char *, const char *,int);           // �������ݿ⣬ִ�в���sql���
    int (*db_create_tbl_fun)(const char *, const char *,int);       // ִ�д����� sql���
    int (*db_drop_tbl_fun)(const char *, const char *,int);         // ɾ���������ռ�õĴ��̿ռ�
    int (*db_del_data_fun)(const char *, const char *,int);         // ִ��ɾ������ sql���    
}DB_CB_ST_EX;
// ���ݿ���������붨��
typedef enum db_errcode
{
    DB_ERR_SUC                      = 0x00,
    DB_ERR_ERROR                    = MAKECODE(DB_MODULE_ID, 0x00), // ��������ʼֵ 0x20 0000
    DB_ERR_MALLOC_ERR               = MAKECODE(DB_MODULE_ID, 0x01), // ������ֵ 0x20 0001 malloc ʧ��
    DB_ERR_AGRS_ERR                 = MAKECODE(DB_MODULE_ID, 0x02), // ������ֵ 0x20 0002 ��δ���
    DB_ERR_NULL_DBCB_ERR            = MAKECODE(DB_MODULE_ID, 0x03), // ������ֵ 0x20 0003 DB_CB_ST*ָ��Ϊ�գ�û�г�ʼ��
    DB_ERR_SQL_ERR                  = MAKECODE(DB_MODULE_ID, 0x04), // ������ֵ 0x20 0004 SQL������
    DB_ERR_QUERY_ERR                = MAKECODE(DB_MODULE_ID, 0x05), // ������ֵ 0x20 0005 ��ѯʧ��
    DB_ERR_DB_TYPE_UNKNOW           = MAKECODE(DB_MODULE_ID, 0x06), // ������ֵ 0x20 0006 ���ݿ����ʹ���
    DB_ERR_DB_OPENED                = MAKECODE(DB_MODULE_ID, 0x07), // ������ֵ 0x20 0007 ���ݿ��Ѿ���
}DB_ERR_EN;





// ��ȡ���ݿ�ʵ��
DB_CB_ST* get_db_instance();
DB_CB_ST_EX* get_db_instance_ex(int flag);
// ��ʼ�����ݿ�
int db_init();
int db_init_ex(int);
// �ر����ݿ�
void db_close();
void db_close_ex(int);

// ��ʼ������״̬
void init_backup_db_stat_flag(void);
// ����ָ�����ݿ�ı���״̬0δ���б��� 1���б���
void set_backup_db_stat_flag(int stat,int db_type);
// ��ȡָ�����ݿ�ı���״̬
int get_backup_db_stat_flag(int db_type);
// �ͷű���״̬���
void release_backup_db_stat_flag(void);
// ��ȡ������������Ϣ��uuid
int get_server_base_info_HostUuid(char *ret);


#endif

