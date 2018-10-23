/**/

#ifndef _CONSOLE_H
#define _CONSOLE_H
/*
#define dprint(format,args...) \
    printf("[%s-%s-L%d] "format, __FILE__, __FUNCTION__, __LINE__, ##args);
*/
#define COLUMNS                 80
#define LINES                   24
#define ATTR                    7
#define ATTR_OK                 10
#define ATTR_ERROR              12
#define VIDEO                   0xB8000

static void itoa (char *buf, int base, int d);
static void putchar (int c, int attr);

extern void dprint (int attr, const char *format, ...);
extern void cls (void);

#endif
/*
0 Black ��ɫ
1 Blue ��ɫ
2 Green ��ɫ
3 Cyan ��ɫ
4 Red ��ɫ
5 Magenta ���
6 Brown ��ɫ
7 Light Gray ������ɫ
8 Dark Gray ����ɫ
9 Light Blue ������ɫ
A Light Green ������ɫ
B Light Cyan ������ɫ
C Light Red ������ɫ
D Light Magenta �������
E Yellow ��ɫ
F White ��ɫ
*/
