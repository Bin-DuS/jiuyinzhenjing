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
$Id: register.c,v 1.19 1999/08/27 02:28:15 edwardc Exp $
*/

#include "bbs.h"
char   *sysconf_str();
char   *genpasswd();
char   *Ctime();

extern char fromhost[60];
extern time_t login_start_time;
time_t  system_time;

int
bad_user_id(userid)
char   *userid;
{
	FILE   *fp;
	char    buf[STRLEN];
	char   *ptr;
	char    ptr2[IDLEN + 2];
	char    ch;
	ptr = userid;
	while ((ch = *ptr++) != '\0') {
		if (!isalnum(ch) && ch != '_')
			return 1;
	}

	if ((fp = fopen(".badname", "r")) != NULL) {
		strtolower(ptr2, userid);
		while (fgets(buf, STRLEN, fp) != NULL) {
			ptr = strtok(buf, " \n\t\r");
			if (ptr != NULL && *ptr != '#' && (strstr(ptr2, ptr) != NULL)) {
				fclose(fp);
				return 1;
			}
		}
		fclose(fp);
	}
	return 0;
}

int
compute_user_value(urec)
struct userec *urec;
{
	int     value;
	/* if (urec) has XEMPT permission, don't kick it */
	if ((urec->userlevel & PERM_XEMPT) || strcmp(urec->userid, "guest") == 0)
		return 999;
	value = (time(0) - urec->lastlogin) / 60;	/* min */
	/* new user should register in 30 mins */
	if (strcmp(urec->userid, "new") == 0) {
		return (30 - value) * 60;
	}
	if (urec->numlogins <= 3)
		return (15 * 1440 - value) / 1440;
	if (!(urec->userlevel & PERM_LOGINOK))
		return (30 * 1440 - value) / 1440;
	if (urec->stay > 1000000)
		return (365 * 1440 - value) / 1440;
	return (120 * 1440 - value) / 1440;
}

int
getnewuserid()
{
	struct userec utmp, zerorec;
	struct stat st;
	int     fd, size, val, i;
	system_time = time(NULL);
	if (stat("tmp/killuser", &st) == -1 || st.st_mtime < system_time - 3600) {
		if ((fd = open("tmp/killuser", O_RDWR | O_CREAT, 0600)) == -1)
			return -1;
		write(fd, ctime(&system_time), 25);
		close(fd);
		log_usies("CLEAN", "dated users.");
		printf("Ѱ�����ʺ���, ���Դ�Ƭ��...\n\r");
		memset(&zerorec, 0, sizeof(zerorec));
		if ((fd = open(PASSFILE, O_RDWR | O_CREAT, 0600)) == -1)
			return -1;
		size = sizeof(utmp);
		for (i = 0; i < MAXUSERS; i++) {
			if (read(fd, &utmp, size) != size)
				break;
			val = compute_user_value(&utmp);
			if (utmp.userid[0] != '\0' && val < 0) {
				sprintf(genbuf, "#%d %-12s %15.15s %d %d %d",
					i + 1, utmp.userid, ctime(&(utmp.lastlogin)) + 4,
					utmp.numlogins, utmp.numposts, val);
				log_usies("KILL ", genbuf);
				if (!bad_user_id(utmp.userid)) {
					sprintf(genbuf, "mail/%c/%s",
						toupper(utmp.userid[0]), utmp.userid);
					f_rm(genbuf);
					sprintf(genbuf, "home/%c/%s",
						toupper(utmp.userid[0]), utmp.userid);
					f_rm(genbuf);
				}
				lseek(fd, (off_t) (-size), SEEK_CUR);
				write(fd, &zerorec, sizeof(utmp));
			}
		}
		close(fd);
		touchnew();
	}
	if ((fd = open(PASSFILE, O_RDWR | O_CREAT, 0600)) == -1)
		return -1;
	flock(fd, LOCK_EX);

	i = searchnewuser();
	sprintf(genbuf, "uid %d from %s", i, fromhost);
	log_usies("APPLY", genbuf);

	if (i <= 0 || i > MAXUSERS) {
		flock(fd, LOCK_UN);
		close(fd);
		if (dashf("etc/user_full")) {
			ansimore("etc/user_full", NA);
			oflush();
		} else {
			printf("��Ǹ, ʹ�����ʺ��Ѿ�����, �޷�ע���µ��ʺ�.\n\r");
		}
		val = (st.st_mtime - system_time + 3660) / 60;
		printf("��ȴ� %d ����������һ��, ף�����.\n\r", val);
		sleep(2);
		exit(1);
	}
	memset(&utmp, 0, sizeof(utmp));
	strcpy(utmp.userid, "new");
	utmp.lastlogin = time(NULL);
	if (lseek(fd, (off_t) (sizeof(utmp) * (i - 1)), SEEK_SET) == -1) {
		flock(fd, LOCK_UN);
		close(fd);
		return -1;
	}
	write(fd, &utmp, sizeof(utmp));
	setuserid(i, utmp.userid);
	flock(fd, LOCK_UN);
	close(fd);
	return i;
}

int
id_with_num(userid)
char    userid[IDLEN + 1];
{
	char   *s;
	for (s = userid; *s != '\0'; s++) {
		if (*s < 1 || !isalpha(*s))
			return 1;
	}
	return 0;
}

void
new_register()
{
	struct userec newuser;
	char    passbuf[STRLEN];
	int     allocid, try;
	if (1) {
		time_t  now;
		now = time(0);
		sprintf(genbuf, "etc/no_register_%3.3s", ctime(&now));
		if (dashf(genbuf)) {
			ansimore(genbuf, NA);
			pressreturn();
			exit(1);
		}
	}

	ansimore("etc/register", NA);
	try = 0;
	while (1) {
		if (++try >= 9) {
			prints("\n��������̫����  <Enter> ��...\n");
			refresh();
			longjmp(byebye, -1);
		}
		getdata(0, 0, "�������ʺ����� (Enter User ID, \"0\" to abort): ", passbuf, IDLEN + 1, DOECHO, YEA);
		if (passbuf[0] == '0') {
			longjmp(byebye, -1);
		}
		if (id_with_num(passbuf)) {
			prints("�ʺű���ȫΪӢ����ĸ!\n");
		} else if (strlen(passbuf) < 2) {
			prints("�ʺ�������������Ӣ����ĸ!\n");
		} else if ((*passbuf == '\0') || bad_user_id(passbuf)) {
			prints("��Ǹ, ������ʹ���������Ϊ�ʺš� ���������һ����\n");
		} else if (dosearchuser(passbuf)) {
			prints("���ʺ��Ѿ�����ʹ��\n");
		} else
			break;
	}

	memset(&newuser, 0, sizeof(newuser));
	allocid = getnewuserid();
	if (allocid > MAXUSERS || allocid <= 0) {
		printf("No space for new users on the system!\n\r");
		exit(1);
	}

	strcpy(newuser.userid, passbuf);
	strcpy(passbuf, "");
	
	while (1) {
		getdata(0, 0, "���趨�������� (Setup Password): ", passbuf, PASSLEN, NOECHO, YEA);
		if (strlen(passbuf) < 4 || !strcmp(passbuf, newuser.userid)) {
			prints("����̫�̻���ʹ���ߴ�����ͬ, ����������\n");
			continue;
		}
		strncpy(newuser.passwd, passbuf, PASSLEN);
		getdata(0, 0, "��������һ��������� (Reconfirm Password): ", passbuf, PASSLEN, NOECHO, YEA);
		if (strncmp(passbuf, newuser.passwd, PASSLEN) != 0) {
			prints("�����������, ��������������.\n");
			continue;
		}
		passbuf[8] = '\0';
		strncpy(newuser.passwd, genpasswd(passbuf), ENCPASSLEN);
		break;
	}

	strcpy(newuser.termtype, "vt100");
	newuser.userdefine = -1;
	if (!strcmp(newuser.userid, "guest")) {
		newuser.userlevel = 0;
		newuser.flags[0] = CURSOR_FLAG;
		newuser.userdefine &= ~(DEF_FRIENDCALL | DEF_ALLMSG | DEF_FRIENDMSG);
	} else {
		newuser.userlevel = PERM_BASIC;
		newuser.flags[0] = CURSOR_FLAG | PAGER_FLAG;
	}
	newuser.userdefine &= ~(DEF_NOLOGINSEND);
	newuser.flags[1] = 0;
	newuser.firstlogin = newuser.lastlogin = time(NULL);

	if (substitute_record(PASSFILE, &newuser, sizeof(newuser), allocid) == -1) {
		fprintf(stderr, "too much, good bye!\n");
		exit(1);
	}
	setuserid(allocid, newuser.userid);
	if (!dosearchuser(newuser.userid)) {
		fprintf(stderr, "User failed to create\n");
		exit(1);
	}
	report("new account");
}

char   *
trim(s)
char   *s;
{
	char   *buf;
	char   *l, *r;
	buf = (char *) malloc(256);

	buf[0] = '\0';
	r = s + strlen(s) - 1;

	for (l = s; strchr(" \t\r\n", *l) && *l; l++);

	/* if all space, *l is null here, we just return null */
	if (*l != '\0') {
		for (; strchr(" \t\r\n", *r) && r >= l; r--);
		strncpy(buf, l, r - l + 1);
	}
	return buf;
}

int
invalid_email(addr)
char   *addr;
{
	FILE   *fp;
	char    temp[STRLEN], tmp2[STRLEN];
	if ((fp = fopen(".bad_email", "r")) != NULL) {
		while (fgets(temp, STRLEN, fp) != NULL) {
			strtok(temp, "\n");

			/* edwardc.990425 so, now is case insencible */
			strtolower(genbuf, temp);
			strtolower(tmp2, addr);
			
			/* 990802.edwardc ˵���� tmp2 �� genbuf ������, �����͵������� 
			   ����ֻ�ö��Կ���, �Է�©��֮�� �п��� user email �� 
			   xx@xx.net.tw, ����ֻд xx.net.tw, �ɷ�����û�취����ס ..*/
			   
			if (strstr(tmp2, genbuf) != NULL || strstr(genbuf,tmp2) != NULL ) {
				fclose(fp);
				return 1;
			}
		}
		fclose(fp);
	}
	return 0;
}

int
invalid_realmail(userid, email)
char   *userid, *email;
{
	FILE   *fn;
	char   *emailfile, fname[STRLEN];
	struct stat st;
	time_t  now;
	if ((emailfile = sysconf_str("EMAILFILE")) == NULL)
		return 0;

	if (strchr(email, '@') && valid_ident(email) && HAS_PERM(PERM_LOGINOK))
		return 0;

	now = time(0);
	sethomefile(fname, userid, "register");
	if (stat(fname, &st) == 0) {
		if (now - st.st_mtime >= REG_EXPIRED * 86400) {
			sethomefile(fname, userid, "register.old");
			if (stat(fname, &st) == -1 || now - st.st_mtime >= REG_EXPIRED * 86400)
				return 1;
		}
	}
	sethomefile(fname, userid, "register");
	if ((fn = fopen(fname, "r")) != NULL) {
		fgets(genbuf, STRLEN, fn);
		fclose(fn);
		strtok(genbuf, "\n");
		if (valid_ident(genbuf) &&
			((strchr(genbuf, '@') != NULL) || strstr(genbuf, "usernum"))) {
			if (strchr(genbuf, '@') != NULL) {
				genbuf[STRLEN - 17] = '\0';
				strncpy(email, genbuf, STRLEN - 17);
			}
			move(21, 0);
			prints("������!! ����˳����ɱ�վ��ʹ����ע������,\n");
			prints("������������ӵ��һ��ʹ���ߵ�Ȩ��������...\n");
			pressanykey();
			return 0;
		}
	}
	return 1;
}
#ifdef CODE_VALID
char   *
genrandpwd(int seed)
{
	char    panel[] = "1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	char   *result;
	int     i, rnd;
	result = (char *) malloc(RNDPASSLEN + 1);

	srand((unsigned) (time(0) * seed));
	bzero(result, RNDPASSLEN + 1);

	for (i = 0; i < RNDPASSLEN; i++) {
		rnd = rand() % sizeof(panel);
		result[i] = panel[rnd];
	}

	sethomefile(genbuf, currentuser.userid, ".regpass");
	unlink(genbuf);
	file_append(genbuf, result);

	return ((char *) result);
}
#endif

void
send_regmail(trec)
struct userec *trec;
{
	time_t  code;
	FILE   *fin, *fout, *dp;
#ifdef CODE_VALID
	char    buf[RNDPASSLEN + 1];
#endif

	sethomefile(genbuf, trec->userid, "mailcheck");
	if ((dp = fopen(genbuf, "w")) == NULL) {
		fclose(dp);
		return;
	}
	code = time(0);
	fprintf(dp, "%9.9d:%d\n", code, getpid());
	fclose(dp);

	sprintf(genbuf, "%s -f %s.bbs@%s %s ", SENDMAIL,
		trec->userid, BBSHOST, trec->email);
	fout = popen(genbuf, "w");
	fin = fopen(sysconf_str("EMAILFILE"), "r");
	if ((fin != NULL) && (fout != NULL)) {
		fprintf(fout, "Reply-To: SYSOP.bbs@%s\n", BBSHOST);
		fprintf(fout, "From: SYSOP.bbs@%s\n", BBSHOST);
		fprintf(fout, "To: %s\n", trec->email);
		fprintf(fout, "Subject: @%s@[-%9.9d:%d-]%s mail check.\n", trec->userid, code, getpid(), BBSID);
		fprintf(fout, "X-Purpose: %s registration mail.\n", BBSNAME);
		fprintf(fout, "\n");
		fprintf(fout, "[����]\n");
		fprintf(fout, "BBS λַ         : %s (%s)\n", BBSHOST, BBSIP);
		fprintf(fout, "��ע��� BBS ID  : %s\n", trec->userid);
		fprintf(fout, "��������         : %s", ctime(&trec->firstlogin));
		fprintf(fout, "������Դ         : %s\n", fromhost);
		fprintf(fout, "������ʵ����/�ǳ�: %s (%s)\n", trec->realname, trec->username);
#ifdef CODE_VALID
		sprintf(buf, "%s", (char *) genrandpwd((int) getpid()));
		fprintf(fout, "��֤����         : %s (��ע���Сд)\n", buf);
#endif
		fprintf(fout, "��֤�ŷ�������   : %s\n", ctime(&code));

		fprintf(fout, "[English]\n");
		fprintf(fout, "BBS LOCATION     : %s (%s)\n", BBSHOST, BBSIP);
		fprintf(fout, "YOUR BBS USER ID : %s\n", trec->userid);
		fprintf(fout, "APPLICATION DATE : %s", ctime(&trec->firstlogin));
		fprintf(fout, "LOGIN HOST       : %s\n", fromhost);
		fprintf(fout, "YOUR NICK NAME   : %s\n", trec->username);
		fprintf(fout, "YOUR NAME        : %s\n", trec->realname);
#ifdef CODE_VALID
		fprintf(fout, "VALID CODE       : %s (case sensitive)\n", buf);
#endif
		fprintf(fout, "THIS MAIL SENT ON: %s\n", ctime(&code));

		while (fgets(genbuf, 255, fin) != NULL) {
			if (genbuf[0] == '.' && genbuf[1] == '\n')
				fputs(". \n", fout);
			else
				fputs(genbuf, fout);
		}
		fprintf(fout, ".\n");
		fclose(fin);
		fclose(fout);
	}
}

void
check_gender()
{
	char    ans[5];
	time_t  now;
	struct tm *tmnow;
	clear();

	now = time(0);
	tmnow = localtime(&now);

	ans[0] = '\0';
	while (ans[0] < '1' || ans[0] > '2') {
		getdata(2, 0, "����������Ա�: [1]�е� [2]Ů�� (1 or 2): ",
			ans, 2, DOECHO, YEA);
	}
	switch (ans[0]) {
	case '1':
		currentuser.gender = 'M';
		break;
	case '2':
		currentuser.gender = 'F';
		break;
	}
	move(4, 0);
	prints("���������ĳ�������");
	while (currentuser.birthyear < tmnow->tm_year - 98 || currentuser.birthyear > tmnow->tm_year - 3) {
		ans[0] = '\0';
		getdata(5, 0, "��λ����Ԫ��: ", ans, 5, DOECHO, YEA);
		if (atoi(ans) < 1900)
			continue;
		currentuser.birthyear = atoi(ans) - 1900;
	}
	while (currentuser.birthmonth < 1 || currentuser.birthmonth > 12) {
		ans[0] = '\0';
		getdata(6, 0, "������: (1-12) ", ans, 3, DOECHO, YEA);
		currentuser.birthmonth = atoi(ans);
	}
	while (currentuser.birthday < 1 || currentuser.birthday > 31) {
		ans[0] = '\0';
		getdata(7, 0, "������: (1-31) ", ans, 3, DOECHO, YEA);
		currentuser.birthday = atoi(ans);
	}

	substitute_record(PASSFILE, &currentuser, sizeof(currentuser), usernum);
}
void
check_register_info()
{
	struct userec *urec = &currentuser;
	char   *newregfile;
	FILE   *fout;
	int     perm;
	char    ans[4], buf[192], buf2[STRLEN];
	extern int showansi;
	time_t  now;
	struct tm *tmnow;
#ifdef CODE_VALID
	int     i;
#endif

	clear();
	sprintf(buf, "%s", BBSHOST);
	if (!(urec->userlevel & PERM_BASIC)) {
		urec->userlevel = 0;
		return;
	}
	perm = PERM_DEFAULT & sysconf_eval("AUTOSET_PERM");

	while ((strlen(urec->username) < 2)
		|| (strstr(urec->username, "  "))
		|| (strstr(urec->username, "  "))) {
		getdata(2, 0, "�����������ǳ� (Enter nickname): ", urec->username, NAMELEN, DOECHO, YEA);
		strcpy(uinfo.username, urec->username);
		update_utmp();
	}
	while ((strlen(urec->realname) < 4)
		|| (strstr(urec->realname, "  "))
		|| (strstr(urec->realname, "  "))) {
		move(3, 0);
		prints("������������ʵ���� (Enter realname):\n");
		getdata(4, 0, "> ", urec->realname, NAMELEN, DOECHO, YEA);
	}
	while ((strlen(urec->address) < 10) || (strstr(urec->address, "   "))) {
		move(5, 0);
		prints("����������ͨѶ��ַ (Enter home address)��\n");
		getdata(6, 0, "> ", urec->address, STRLEN - 10, DOECHO, YEA);
	}
	if (strchr(urec->email, '@') == NULL) {
		move(8, 0);
		prints("���������ʽΪ: [1;37muserid@your.domain.name[m\n");
		prints("������������� (�����ṩ�߰� <Enter>)");
		getdata(10, 0, "> ", urec->email, NAMELEN, DOECHO, YEA);
		if (strchr(urec->email, '@') == NULL) {
			sprintf(genbuf, "%s.bbs@%s", urec->userid, buf);
			strncpy(urec->email, genbuf, STRLEN);
		}
	}
	now = time(0);
	tmnow = localtime(&now);

	if (!HAS_PERM(PERM_LOGINOK)) {
		if (!invalid_realmail(urec->userid, urec->reginfo)) {
			set_safe_record();
			urec->lastjustify = time(0);
			urec->userlevel |= PERM_DEFAULT;
			if (HAS_PERM(PERM_DENYPOST) && !HAS_PERM(PERM_SYSOP))
				urec->userlevel &= ~PERM_POST;
			substitute_record(PASSFILE, urec, sizeof(struct userec), usernum);
#ifdef CODE_VALID
		} else {
			/* edwardc.990426 ������֤ */
			sethomefile(buf, currentuser.userid, ".regpass");
			if (dashf(buf)) {

				move(13, 0);
				prints("����δͨ�����ȷ��... \n");
				prints("�����ڱ�������ע��ȷ������, \"��֤����\"����Ϊ���ȷ��\n");
				prints("һ���� %d ����Ԫ, ��Сд���в���, ��ע��. \n", RNDPASSLEN);
				prints("������ȡ�������������� [Enter] ��, ϵͳ�������·���һ��ȷ����.\n");
				prints("[1;33m��ע��, ������������һ����֤�������������������룡[m\n");

				if ((fout = fopen(buf, "r")) != NULL) {
					fscanf(fout, "%s", buf2);
					fclose(fout);
				}
				for (i = 0; i < 3; i++) {
					move(18, 0);
					prints("�㻹�� %d �λ���\n", 3 - i);
					getdata(19, 0, "��������֤����: ", genbuf, (RNDPASSLEN + 1), DOECHO, YEA);

					if (strcmp(genbuf, "") != 0) {
						if (strcmp(genbuf, buf2) != 0)
							continue;
						else
							break;
					}
				}

				if (i == 3) {
					prints("������֤ʧ��! ϵͳ�����·�����֤��, ������������ SYSOP �巢��\n");
					send_regmail(&currentuser);
					pressanykey();
				} else {
					set_safe_record();
					urec->userlevel |= PERM_DEFAULT;
					if (HAS_PERM(PERM_DENYPOST) && !HAS_PERM(PERM_SYSOP))
						urec->userlevel &= ~PERM_POST;
					/*
					 * edwardc.990502
					 * ������һֱҪ������ע�������
					 */
					urec->lastjustify = time(0);
					substitute_record(PASSFILE, urec, sizeof(struct userec), usernum);
					prints("������!! ����˳����ɱ�վ��ʹ����ע������,\n");
					prints("������������ӵ��һ��ʹ���ߵ�Ȩ��������...\n");
					unlink(buf);
					mail_file("etc/smail", currentuser.userid, "��ӭ���뱾վ����");
					pressanykey();
				}

			} else
#endif
				if ((sysconf_str("EMAILFILE") != NULL) &&
					(!strstr(urec->email, buf)) &&
					(!invalidaddr(urec->email)) &&
				(!invalid_email(urec->email))) {
				move(13, 0);
				prints("���ĵ������� ����ͨ��������֤...  \n");
				prints("    ��վ�����ϼ�һ����֤�Ÿ���,\n");
				prints("    ��ֻҪ�� %s ����, �Ϳ��Գ�Ϊ��վ�ϸ���.\n\n", urec->email);
				prints("    ��Ϊ��վ�ϸ���, �������и����Ȩ���!\n");
				move(20, 0);
				if (askyn("��Ҫ�������ھͼ���һ������", YEA, NA) == YEA) {
					send_regmail(urec);
					getdata(21, 0, "ȷ�����Ѽĳ�, ��������Ŷ!! �밴 <Enter> : ", ans, 2, DOECHO, YEA);
				}
			} else {
				showansi = 1;
				if (sysconf_str("EMAILFILE") != NULL) {
					prints("\n������д�ĵ����ʼ���ַ ��[1;33m%s[m��\n", urec->email);
					prints("���ǺϷ�֮ UNIX �ʺţ�ϵͳ����Ͷ��ע���ţ��뵽[1;32mInfoEdit->Info[m���޸�...\n");
					pressanykey();
				}
			}
		}
	}
	if (urec->lastlogin - urec->firstlogin < 3 * 86400) {
		if (urec->numlogins == 1) {

			clear();
			ans[0] = '\0';
			while (ans[0] < '1' || ans[0] > '2') {
				getdata(2, 0, "����������Ա�: [1]�е� [2]Ů�� (1 or 2): ",
					ans, 2, DOECHO, YEA);
			}
			switch (ans[0]) {
			case '1':
				urec->gender = 'M';
				break;
			case '2':
				urec->gender = 'F';
				break;
			}
			move(4, 0);
			prints("���������ĳ�������");
			while (urec->birthyear < tmnow->tm_year - 98 || urec->birthyear > tmnow->tm_year - 3) {
				ans[0] = '\0';
				getdata(5, 0, "��λ����Ԫ��: ", ans, 5, DOECHO, YEA);
				if (atoi(ans) < 1900)
					continue;
				urec->birthyear = atoi(ans) - 1900;
			}
			while (urec->birthmonth < 1 || urec->birthmonth > 12) {
				ans[0] = '\0';
				getdata(6, 0, "������: (1-12) ", ans, 3, DOECHO, YEA);
				urec->birthmonth = atoi(ans);
			}
			while (urec->birthday < 1 || urec->birthday > 31) {
				ans[0] = '\0';
				getdata(7, 0, "������: (1-31) ", ans, 3, DOECHO, YEA);
				urec->birthday = atoi(ans);
			}
			sprintf(buf, "tmp/newcomer.%s", currentuser.userid);
			if ((fout = fopen(buf, "w")) != NULL) {
				fprintf(fout, "��Һ�,\n\n");
				fprintf(fout, "���� %s (%s), ���� %s\n"
					,currentuser.userid, urec->username, fromhost);
				fprintf(fout, "����%s������վ����, ���Ҷ��ָ�̡�\n",
					(urec->gender == 'M') ? "С��" : "СŮ��");
				move(9, 0);
				prints("��������̵ĸ��˼��, ��վ����ʹ���ߴ���к�\n");
				prints("(�������, д���ֱ�Ӱ� <Enter> ����)....");
				getdata(11, 0, ":", buf2, 75, DOECHO, YEA);
				if (buf2[0] != '\0') {
					fprintf(fout, "\n\n���ҽ���:\n\n");
					fprintf(fout, "%s\n", buf2);
					getdata(12, 0, ":", buf2, 75, DOECHO, YEA);
					if (buf2[0] != '\0') {
						fprintf(fout, "%s\n", buf2);
						getdata(13, 0, ":", buf2, 75, DOECHO, YEA);
						if (buf2[0] != '\0') {
							fprintf(fout, "%s\n", buf2);
						}
					}
				}
				fclose(fout);
				sprintf(buf2, "������·: %s", urec->username);
				postfile(buf, "newcomers", buf2, 2);
				unlink(buf);
			}
			pressanykey();
		}
		newregfile = sysconf_str("NEWREGFILE");
		if (!HAS_PERM(PERM_SYSOP) && newregfile != NULL) {
			set_safe_record();
			urec->userlevel &= ~(perm);
			substitute_record(PASSFILE, urec, sizeof(struct userec), usernum);
			ansimore(newregfile, YEA);
		}
	}
	set_safe_record();
	if (HAS_PERM(PERM_DENYPOST) && !HAS_PERM(PERM_SYSOP)) {
		currentuser.userlevel &= ~PERM_POST;
		substitute_record(PASSFILE, urec, sizeof(struct userec), usernum);
	}
}
