/*
    Pirate Bulletin Board System
    Copyright (C) 1990, Edward Luke, lush@Athena.EE.MsState.EDU
    Eagles Bulletin Board System
    Copyright (C) 1992, Raymond Rocker, rocker@rock.b11.ingr.com
                        Guy Vega, gtvega@seabass.st.usm.edu
                        Dominic Tynes, dbtynes@seabass.st.usm.edu
    Firebird Bulletin Board System
    Copyright (C) 1996, Hsien-Tsung Chang, Smallpig.bbs@bbs.cs.ccu.edu.tw
                        Peng Piaw Foong, ppfoong@csie.ncu.edu.tw

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*/
/*
$Id: delete.c,v 1.6 1999/09/07 03:30:32 edwardc Exp $
*/

#include "bbs.h"
#ifdef WITHOUT_ADMIN_TOOLS
#define kick_user
#endif

int
offline()
{
	char    buf[STRLEN];
	modify_user_mode(OFFLINE);
	clear();
	if (HAS_PERM(PERM_SYSOP) || HAS_PERM(PERM_BOARDS) || HAS_PERM(PERM_ADMINMENU)
		|| HAS_PERM(PERM_SEEULEVELS)) {
		move(1, 0);
		prints("\n\n������������, ���������ɱ��!!\n");
		pressreturn();
		clear();
		return;
	}
	if (currentuser.stay < 86400) {
		move(1, 0);
		prints("\n\n�Բ���, ����δ���ʸ�ִ�д�����!!\n");
		prints("�� mail �� SYSOP ˵����ɱԭ��, лл��\n");
		pressreturn();
		clear();
		return;
	}
	getdata(1, 0, "�������������: ", buf, PASSLEN, NOECHO, YEA);
	if (*buf == '\0' || !checkpasswd(currentuser.passwd, buf)) {
		prints("\n\n�ܱ�Ǹ, ����������벻��ȷ��\n");
		pressreturn();
		clear();
		return;
	}
	getdata(3, 0, "�������ʲ������? ", buf, NAMELEN, DOECHO, YEA);
	if (*buf == '\0' || strcmp(buf, currentuser.realname)) {
		prints("\n\n�ܱ�Ǹ, �Ҳ�����ʶ�㡣\n");
		pressreturn();
		clear();
		return;
	}
	clear();
	move(1, 0);
	prints("[1;5;31m����[0;1;31m�� ��ɱ��, �����޷����ô��ʺŽ��뱾վ����");
	prints("\n\n\n[1;32m���ʺ�Ҫ�� 30 ����Ż�ɾ�������ѹ�� :( .....[m\n\n\n");
	if (askyn("��ȷ��Ҫ�뿪������ͥ", NA, NA) == 1) {
		clear();
		currentuser.userlevel = 0;
		substitute_record(PASSFILE, &currentuser, sizeof(struct userec), usernum);
		mail_info();
		modify_user_mode(OFFLINE);
		kick_user(&uinfo);
		exit(0);
	}
}
getuinfo(fn)
FILE   *fn;
{
	int     num;
	char    buf[40];
	fprintf(fn, "\n\n���Ĵ���     : %s\n", currentuser.userid);
	fprintf(fn, "�����ǳ�     : %s\n", currentuser.username);
	fprintf(fn, "��ʵ����     : %s\n", currentuser.realname);
	fprintf(fn, "��ססַ     : %s\n", currentuser.address);
	fprintf(fn, "�����ʼ����� : %s\n", currentuser.email);
	fprintf(fn, "��ʵ E-mail  : %s\n", currentuser.reginfo);
	fprintf(fn, "Ident ����   : %s\n", currentuser.ident);
	fprintf(fn, "�ʺŽ������� : %s", ctime(&currentuser.firstlogin));
	fprintf(fn, "����������� : %s", ctime(&currentuser.lastlogin));
	fprintf(fn, "������ٻ��� : %s\n", currentuser.lasthost);
	fprintf(fn, "��վ����     : %d ��\n", currentuser.numlogins);
	fprintf(fn, "������Ŀ     : %d\n", currentuser.numposts);
	fprintf(fn, "��վ��ʱ��   : %d Сʱ %d ����\n",
		currentuser.stay / 3600, (currentuser.stay / 60) % 60);
	strcpy(buf, "bTCPRp#@XWBA#VS-DOM-F012345678");
	for (num = 0; num < 30; num++)
		if (!(currentuser.userlevel & (1 << num)))
			buf[num] = '-';
	buf[num] = '\0';
	fprintf(fn, "ʹ����Ȩ��   : %s\n\n", buf);
}
mail_info()
{
	FILE   *fn;
	time_t  now;
	char    filename[STRLEN];
	now = time(0);
	sprintf(filename, "tmp/suicide.%s", currentuser.userid);
	if ((fn = fopen(filename, "w")) != NULL) {
		fprintf(fn, "[1m%s[m �Ѿ��� [1m%24.24s[m �Ǽ���ɱ�ˣ��������������ϣ��뱣��...", currentuser.userid
			,ctime(&now));
		getuinfo(fn);
		fclose(fn);
		postfile(filename, "syssecurity", "�Ǽ���ɱ֪ͨ(30������Ч)...", 2);
		unlink(filename);
	}
	if ((fn = fopen(filename, "w")) != NULL) {
		fprintf(fn, "��Һ�,\n\n");
		fprintf(fn, "���� %s (%s)���Ҽ����Ǽ���30�����뿪�����ˡ�\n\n",
			currentuser.userid, currentuser.username);
		fprintf(fn, "�Ҳ���������������� %s�����ڱ�վ %d �� login ���ܹ� %d ���Ӷ����ڼ�ĵ��εΡ�\n",
			ctime(&currentuser.firstlogin), currentuser.numlogins, currentuser.stay / 60);
		fprintf(fn, "���ҵĺ��Ѱ� %s �����ǵĺ����������õ��ɡ���Ϊ�Ҽ���û��Ȩ������վ��!\n\n",
			currentuser.userid);
		fprintf(fn, "�����г�һ���һ�����ġ� ����!! �ټ�!!\n\n\n");
		fprintf(fn, "%s � %24.24s ��.\n\n", currentuser.userid, ctime(&now));
		fclose(fn);
		postfile(filename, "notepad", "�Ǽ���ɱ����...", 2);
		unlink(filename);
	}
}
