#ifndef __DES_H__
#define __DES_H__

void get_random_bytes(void *buf, int nbytes);


#ifndef WST_CRYPT_CARD
typedef long DWORD;
typedef int WORD;
#endif

typedef  unsigned long PUBHANDLE;
typedef unsigned char key_data_t[128];

extern char PubLastCryptError[256];

extern long CARD_MAIN_KEY;

#define maxn 0x8000		// ����֧�����ĳ���
#define ENCODE 0,16,1		// �����õĺ�
#define DECODE 15,-1,-1		// �����õĺ�

// �ַ�ת�ɶ�����
void ChToBit(char* dest, const char* src, int length);

// ������ת���ַ�
void BitToCh(char* dest, char* src, int length);

// ���û�����offsetΪƫ�ƣ���countΪ����
void BatchSet(char* dest, char* src, char* offset, int count);

// �õ�16���������Կ
void getKeys(char *key, char keyb[][72]);

// �����ܺ������ת��Ϊ�ɶ�������
void msgPro(char* dest, char* src);

// ͨ�ü�/���ܺ������������������ɺ�ENCODE��DECODE�ṩ
void DES(const char* pmsg, int st, int cl, int step, char keyb[][72],char *res);



int PubOpenCryptModule(PUBHANDLE *hMod);
int PubCloseCryptModule(PUBHANDLE *hMod);
int PubLoginCryptModule(PUBHANDLE* hMod,const char* pwd,int pwdlen);
int PubLogoutCryptModule(PUBHANDLE* hMod);

int PubGetBasicKey(PUBHANDLE* hMod,long whatkey,PUBHANDLE* hKey);


int PubGetCryptVersion(char* verinfo,int len);
int PubCryptEnvInit();
int PubCryptEnvClean();

int PubGenKey(PUBHANDLE* hMod,PUBHANDLE * hKey);
int PubImportKey(const key_data_t *key,PUBHANDLE *hMod,PUBHANDLE* hSPECKey,PUBHANDLE *hKey,long mode);
int PubExportKey(key_data_t *key,PUBHANDLE* hMod,PUBHANDLE * hSPECKey,PUBHANDLE *hkey,long mode);
int PubDestroyKey(PUBHANDLE *hMod,PUBHANDLE *hKey);

int PubEncryptInit(PUBHANDLE *hMod,PUBHANDLE* hKey,long mode);
int PubEncryptFinal_old(PUBHANDLE *hMod,const char* inbuf,int inlen,unsigned char* outbuf,int *outlen,int cflag);
int PubEncryptFinal(PUBHANDLE *hMod,PUBHANDLE* hkey,const char* inbuf,int inlen,unsigned char* outbuf,int *outlen);
int PubDecryptInit(PUBHANDLE *hMod,PUBHANDLE* hKey,long mode);
int PubDecryptFinal_old(PUBHANDLE *hMod,const char* inbuf,int inlen,unsigned char* outbuf,int *outlen,int cflag);

int PubDecryptFinal(PUBHANDLE *hMod,PUBHANDLE* hkey,const char* inbuf,int inlen,unsigned char* outbuf,int *outlen);

int PubEncryptFinal(PUBHANDLE *hMod,PUBHANDLE* hkey,const char* inbuf,int inlen,unsigned char* outbuf,int *outlen);
int PubGenRandom(PUBHANDLE *hMod, unsigned char* outbuf,int len);

int PubTestSecModule(PUBHANDLE* hMod,const char* pwd);

int PubHandleInit(PUBHANDLE *h);

int PubGetOpenKeyNumber();
int PubGetMaxKeyNumber();


#endif
