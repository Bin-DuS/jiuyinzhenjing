/*
 * admintool.c	-- admin tools, moves from SYSOP-only-tool
 *
 * A part of Firebird BBS Project
 *
 * Copyright (c) 1999, Edward Ping-Da Chuang <edwardc@firebird.dhs.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * CVS: $Id: admintool.c,v 1.1 1999/09/07 03:30:32 edwardc Exp $
 */

#ifndef WITHOUT_ADMIN_TOOLS
#include "bbs.h"

extern int cmpbnames();
extern int numboards;
extern int cleanmail();
extern char *chgrp();
extern int dowall();
extern int cmpuids();
extern int t_cmpuids();

int  showperminfo(int, int);
char    cexplain[STRLEN];
char	buf2[STRLEN];
char    lookgrp[30];
FILE   *cleanlog;

int
m_info()
{
	struct userec uinfo;
	int     id;
	modify_user_mode(ADMIN);
	if (!check_systempasswd()) {
		return;
	}
	clear();
	stand_title("�޸�ʹ��������");
	move(1, 0);
	usercomplete("������ʹ���ߴ���: ", genbuf);
	if (*genbuf == '\0') {
		clear();
		return -1;
	}
	if (!(id = getuser(genbuf))) {
		move(3, 0);
		prints("�����ʹ���ߴ���");
		clrtoeol();
		pressreturn();
		clear();
		return -1;
	}
	memcpy(&uinfo, &lookupuser, sizeof(uinfo));

	move(1, 0);
	clrtobot();
	disply_userinfo(&uinfo, 1);
	uinfo_query(&uinfo, 1, id);
	return 0;
}

int
m_newbrd()
{
	struct boardheader newboard;
	extern int numboards;
	char    ans[4];
	char    vbuf[100];
	char   *group;
	int     bid;


	modify_user_mode(ADMIN);
	if (!check_systempasswd()) {
		return;
	}
	clear();
	stand_title("������������");
	memset(&newboard, 0, sizeof(newboard));
	move(2, 0);
	ansimore2("etc/boardref", NA, 3, 7);
	while (1) {
		getdata(10, 0, "����������:   ", newboard.filename, 18, DOECHO, YEA);
		if (newboard.filename[0] != 0) {
			struct boardheader dh;
			if (search_record(BOARDS, &dh, sizeof(dh), cmpbnames, newboard.filename)) {
				prints("\n����! ���������Ѿ�����!!");
				pressanykey();
				return -1;
			}
		} else
			return -1;
		if (valid_brdname(newboard.filename))
			break;
		prints("\n���Ϸ�����!!");
	}
	newboard.flag = 0;
	while (1) {
		getdata(11, 0, "������˵��:   ", newboard.title, 60, DOECHO, YEA);
		if (newboard.title[0] == '\0')
			return -1;
		if (strstr(newboard.title, "��") || strstr(newboard.title, "��")) {
			newboard.flag |= OUT_FLAG;
			break;
		} else if (strstr(newboard.title, "��")) {
			newboard.flag &= ~OUT_FLAG;
			break;
		} else
			prints("����ĸ�ʽ, �޷��ж��Ƿ�ת��!!");
	}
	strcpy(vbuf, "vote/");
	strcat(vbuf, newboard.filename);
	setbpath(genbuf, newboard.filename);
	if (getbnum(newboard.filename) > 0 || mkdir(genbuf, 0755) == -1
		|| mkdir(vbuf, 0755) == -1) {
		prints("\n���������������!!\n");
		pressreturn();
		clear();
		return -1;
	}
	getdata(12, 0, "����������Ա: ", newboard.BM, BM_LEN - 1, DOECHO, YEA);
	move(13, 0);
	if (askyn("�Ƿ����ƴ�ȡȨ��", NA, NA) == YEA) {
		getdata(14, 0, "���� Read/Post? [R]: ", ans, 2, DOECHO, YEA);
		if (*ans == 'P' || *ans == 'p')
			newboard.level = PERM_POSTMASK;
		else
			newboard.level = 0;
		move(1, 0);
		clrtobot();
		move(2, 0);
		prints("�趨 %s Ȩ��. ������: '%s'\n",
			(newboard.level & PERM_POSTMASK ? "POST" : "READ"),
			newboard.filename);
		newboard.level = setperms(newboard.level, "Ȩ��", NUMPERMS, showperminfo);
		clear();
	} else
		newboard.level = 0;
	move(14, 0);
	if (askyn("�Ƿ����������", NA, NA) == YEA)
		newboard.flag |= ANONY_FLAG;
	else
		newboard.flag &= ~ANONY_FLAG;
	if ((bid = getbnum("")) > 0) {
		substitute_record(BOARDS, &newboard, sizeof(newboard), bid);
	} else if (append_record(BOARDS, &newboard, sizeof(newboard)) == -1) {
		pressreturn();
		clear();
		return -1;
	}
	group = chgrp();
	if (group != NULL) {
		if (newboard.BM[0] != '\0')
			sprintf(vbuf, "%-38.38s(BM: %s)", newboard.title + 8, newboard.BM);
		else
			sprintf(vbuf, "%-38.38s", newboard.title + 8);

		if (add_grp(group, cexplain, newboard.filename, vbuf) == -1)
			prints("\n����������ʧ��....\n");
		else
			prints("�Ѿ����뾫����...\n");
	}
	numboards = -1;
	prints("\n������������\n");
	{
		char    secu[STRLEN];
		sprintf(secu, "�����°壺%s", newboard.filename);
		securityreport(secu);
	}
	pressreturn();
	clear();
	return 0;
}

int
m_editbrd()
{
	char    bname[STRLEN], buf[STRLEN], oldtitle[STRLEN], vbuf[256],
	       *group;
	char    oldpath[STRLEN], newpath[STRLEN], tmp_grp[30];
	int     pos, noidboard, a_mv;
	struct boardheader fh, newfh;
	modify_user_mode(ADMIN);
	if (!check_systempasswd()) {
		return;
	}
	clear();
	stand_title("�޸���������Ѷ");
	move(1, 0);
	make_blist();
	namecomplete("��������������: ", bname);
	if (*bname == '\0') {
		move(2, 0);
		prints("���������������");
		pressreturn();
		clear();
		return -1;
	}
	pos = search_record(BOARDS, &fh, sizeof(fh), cmpbnames, bname);
	if (!pos) {
		move(2, 0);
		prints("���������������");
		pressreturn();
		clear();
		return -1;
	}
	noidboard = fh.flag & ANONY_FLAG;
	move(3, 0);
	memcpy(&newfh, &fh, sizeof(newfh));
	prints("����������:   %s\n", fh.filename);
	prints("������˵��:   %s\n", fh.title);
	prints("����������Ա: %s\n", fh.BM);
	prints("����������:   %s\n", (noidboard) ? "Yes" : "No");
	strcpy(oldtitle, fh.title);
	prints("���� %s Ȩ��: %s",
		(fh.level & PERM_POSTMASK) ? "POST" :
		(fh.level & PERM_NOZAP) ? "ZAP" : "READ",
		(fh.level & ~PERM_POSTMASK) == 0 ? "������" : "������");
	move(8, 0);
	if (askyn("�Ƿ����������Ѷ", NA, NA) == YEA) {
		move(9, 0);
		prints("ֱ�Ӱ� <Return> ���޸Ĵ�����Ѷ...");

		while (1) {
			getdata(10, 0, "������������: ", genbuf, 18, DOECHO, YEA);
			if (*genbuf != 0) {
				struct boardheader dh;
				if (search_record(BOARDS, &dh, sizeof(dh), cmpbnames, genbuf)) {
					move(2, 0);
					prints("����! ���������Ѿ�����!!");
					move(10, 0);
					clrtoeol();
					continue;
				}
				if (valid_brdname(genbuf)) {
					strncpy(newfh.filename, genbuf, sizeof(newfh.filename));
					strcpy(bname, genbuf);
					break;
				} else {
					move(2, 0);
					prints("���Ϸ�������������!");
					move(10, 0);
					clrtoeol();
					continue;
				}
			} else {
				break;
			}
		}
		ansimore2("etc/boardref", NA, 11, 7);
		while (1) {
			getdata(18, 0, "��������˵��: ", genbuf, 60, DOECHO, YEA);
			if (*genbuf != 0)
				strncpy(newfh.title, genbuf, sizeof(newfh.title));
			else
				break;
			if (strstr(newfh.title, "��") || strstr(newfh.title, "��")) {
				newfh.flag |= OUT_FLAG;
				break;
			} else if (strstr(newfh.title, "��")) {
				newfh.flag &= ~OUT_FLAG;
				break;
			} else
				prints("\n����ĸ�ʽ, �޷��ж��Ƿ�ת��!!");
		}
		move(20, 0);
		clrtoeol();
		getdata(19, 0, "����������Ա: ", genbuf, 60, DOECHO, YEA);
		if (*genbuf != 0)
			strncpy(newfh.BM, genbuf, sizeof(newfh.BM));
		if (*genbuf == ' ')
			strncpy(newfh.BM, "\0", sizeof(newfh.BM));
		sprintf(buf, "������ (Y/N)? [%c]: ", (noidboard) ? 'Y' : 'N');
		getdata(20, 0, buf, genbuf, 4, DOECHO, YEA);
		if (*genbuf == 'y' || *genbuf == 'Y' || *genbuf == 'N' || *genbuf == 'n') {
			if (*genbuf == 'y' || *genbuf == 'Y')
				newfh.flag |= ANONY_FLAG;
			else
				newfh.flag &= ~ANONY_FLAG;
		}
		move(21, 0);
		if (askyn("�Ƿ��ƶ���������λ��", NA, NA) == YEA)
			a_mv = 2;
		else
			a_mv = 0;
		move(22, 0);
		if (askyn("�Ƿ���Ĵ�ȡȨ��", NA, NA) == YEA) {
			char    ans[4];
			sprintf(genbuf, "���� (R)�Ķ� �� (P)���� ���� [%c]: ",
				(newfh.level & PERM_POSTMASK ? 'P' : 'R'));
			getdata(23, 0, genbuf, ans, 2, DOECHO, YEA);
			if ((newfh.level & PERM_POSTMASK) && (*ans == 'R' || *ans == 'r'))
				newfh.level &= ~PERM_POSTMASK;
			else if (!(newfh.level & PERM_POSTMASK) && (*ans == 'P' || *ans == 'p'))
				newfh.level |= PERM_POSTMASK;
			clear();
			move(2, 0);
			prints("�趨 %s '%s' ��������Ȩ��\n",
				newfh.level & PERM_POSTMASK ? "����" : "�Ķ�", newfh.filename);
			newfh.level = setperms(newfh.level, "Ȩ��", NUMPERMS, showperminfo);
			clear();
			getdata(0, 0, "ȷ��Ҫ������? (Y/N) [N]: ", genbuf, 4, DOECHO, YEA);
		} else {
			getdata(23, 0, "ȷ��Ҫ������? (Y/N) [N]: ", genbuf, 4, DOECHO, YEA);
		}
		if (*genbuf == 'Y' || *genbuf == 'y') {
			{
				char    secu[STRLEN];
				sprintf(secu, "�޸���������%s(%s)", fh.filename, newfh.filename);
				securityreport(secu);
			}
			if (strcmp(fh.filename, newfh.filename)) {
				char    old[256], tar[256];
				a_mv = 1;
				setbpath(old, fh.filename);
				setbpath(tar, newfh.filename);
				rename(old, tar);
				sprintf(old, "vote/%s", fh.filename);
				sprintf(tar, "vote/%s", newfh.filename);
				rename(old, tar);
			}
			if (newfh.BM[0] != '\0')
				sprintf(vbuf, "%-38.38s(BM: %s)", newfh.title + 8, newfh.BM);
			else
				sprintf(vbuf, "%-38.38s", newfh.title + 8);
			get_grp(fh.filename);
			edit_grp(fh.filename, lookgrp, oldtitle + 8, vbuf);

			if (a_mv >= 1) {
				group = chgrp();
				get_grp(fh.filename);
				strcpy(tmp_grp, lookgrp);
				if (strcmp(tmp_grp, group) || a_mv != 2) {
					del_from_file("0Announce/.Search", fh.filename);
					if (group != NULL) {
						if (newfh.BM[0] != '\0')
							sprintf(vbuf, "%-38.38s(BM: %s)", newfh.title + 8, newfh.BM);
						else
							sprintf(vbuf, "%-38.38s", newfh.title + 8);

						if (add_grp(group, cexplain, newfh.filename, vbuf) == -1)
							prints("\n����������ʧ��....\n");
						else
							prints("�Ѿ����뾫����...\n");
						sprintf(newpath, "0Announce/groups/%s/%s", group, newfh.filename);
						sprintf(oldpath, "0Announce/groups/%s/%s", tmp_grp, fh.filename);
						if (dashd(oldpath)) {
							deltree(newpath);
						}
						rename(oldpath, newpath);
						del_grp(tmp_grp, fh.filename, fh.title + 8);
					}
				}
			}
			substitute_record(BOARDS, &newfh, sizeof(newfh), pos);
			sprintf(genbuf, "���������� %s ������ --> %s",
				fh.filename, newfh.filename);
			report(genbuf);
			numboards = -1;	/* force re-caching */
		}
	}
	clear();
	return 0;
}

int
m_mclean()
{
	modify_user_mode(ADMIN);
	if (!check_systempasswd()) {
		return;
	}
	clear();
	stand_title("���˽���ż�");
	move(1, 0);
	prints("��������Ѷ���δ mark ���ż�\n");
	if (askyn("ȷ����", NA, NA) == NA) {
		clear();
		return 0;
	} {
		char    secu[STRLEN];
		sprintf(secu, "�������ʹ�����Ѷ��ż���");
		securityreport(secu);
	}

	cleanlog = fopen("mailclean.log", "w");
	move(3, 0);
	prints("�����ĵȺ�.\n");
	refresh();
	if (apply_record(PASSFILE, cleanmail, sizeof(struct userec)) == -1) {
		move(4, 0);
		prints("apply PASSFILE err...\n");
		pressreturn();
		clear();
		return -1;
	}
	move(4, 0);
	fclose(cleanlog);
	prints("������! ��¼�� mailclean.log.\n");
	pressreturn();
	clear();
	return 0;
}

int
m_trace()
{
	struct stat ostatb, cstatb;
	int     otflag, ctflag, done = 0;
	char    ans[3];
	char   *msg;
	modify_user_mode(ADMIN);
	if (!check_systempasswd()) {
		return;
	}
	clear();
	stand_title("Set Trace Options");
	while (!done) {
		move(2, 0);
		otflag = stat("trace", &ostatb);
		ctflag = stat("trace.chatd", &cstatb);
		prints("Ŀǰ�趨:\n");
		trace_state(otflag, "һ��", ostatb.st_size);
		trace_state(ctflag, "����", cstatb.st_size);
		move(9, 0);
		prints("<1> �л�һ���¼\n");
		prints("<2> �л������¼\n");
		getdata(12, 0, "��ѡ�� (1/2/Exit) [E]: ", ans, 2, DOECHO, YEA);

		switch (ans[0]) {
		case '1':
			if (otflag) {
				touchfile("trace");
				msg = "һ���¼ ON";
			} else {
				rename("trace", "trace.old");
				msg = "һ���¼ OFF";
			}
			break;
		case '2':
			if (ctflag) {
				touchfile("trace.chatd");
				msg = "�����¼ ON";
			} else {
				rename("trace.chatd", "trace.chatd.old");
				msg = "�����¼ OFF";
			}
			break;
		default:
			msg = NULL;
			done = 1;
		}
		move(t_lines - 2, 0);
		if (msg) {
			prints("%s\n", msg);
			report(msg);
		}
	}
	clear();
	return 0;
}

int
m_register()
{
	FILE   *fn;
	char    ans[3], *fname;
	int     x, y, wid, len;
	char    uident[STRLEN];
	modify_user_mode(ADMIN);
	if (!check_systempasswd()) {
		return;
	}
	clear();

	stand_title("�趨ʹ����ע������");
	for (;;) {
		getdata(1, 0, "(0)�뿪  (1)�����ע������  (2)��ѯʹ����ע������ ? : ",
			ans, 2, DOECHO, YEA);
		if (ans[0] == '1' || ans[0] == '2')
			break;
		else
			return 0;
	}
	if (ans[0] == '1') {
		fname = "new_register";
		if ((fn = fopen(fname, "r")) == NULL) {
			prints("\n\nĿǰ������ע������.");
			pressreturn();
		} else {
			y = 3, x = wid = 0;
			while (fgets(genbuf, STRLEN, fn) != NULL && x < 65) {
				if (strncmp(genbuf, "userid: ", 8) == 0) {
					move(y++, x);
					prints(genbuf + 8);
					len = strlen(genbuf + 8);
					if (len > wid)
						wid = len;
					if (y >= t_lines - 2) {
						y = 3;
						x += wid + 2;
					}
				}
			}
			fclose(fn);
			if (askyn("�趨������", NA, YEA) == YEA) {
				securityreport("�趨ʹ����ע������");
				scan_register_form(fname);
			}
		}
	} else {
		move(1, 0);
		usercomplete("������Ҫ��ѯ�Ĵ���: ", uident);
		if (uident[0] != '\0')
			if (!getuser(uident)) {
				move(2, 0);
				prints("�����ʹ���ߴ���...");
			} else {
				sprintf(genbuf, "home/%c/%s/register", toupper(lookupuser.userid[0]), lookupuser.userid);
				if ((fn = fopen(genbuf, "r")) != NULL) {
					prints("\nע����������:\n\n");
					for (x = 1; x <= 15; x++) {
						if (fgets(genbuf, STRLEN, fn))
							prints("%s", genbuf);
						else
							break;
					}
				} else
					prints("\n\n�Ҳ�����/����ע������!!\n");
			}
		pressanykey();
	}
	clear();
	return 0;
}

int
d_board()
{
	struct boardheader binfo;
	int     bid, ans;
	char    bname[STRLEN];
	extern char lookgrp[];
	extern int numboards;
	if (!HAS_PERM(PERM_BLEVELS)) {
		return 0;
	}
	modify_user_mode(ADMIN);
	if (!check_systempasswd()) {
		return;
	}
	clear();
	stand_title("ɾ��������");
	make_blist();
	move(1, 0);
	namecomplete("������������: ", bname);
	if (bname[0] == '\0')
		return 0;
	bid = getbnum(bname);
	if (get_record(BOARDS, &binfo, sizeof(binfo), bid) == -1) {
		move(2, 0);
		prints("����ȷ��������\n");
		pressreturn();
		clear();
		return 0;
	}
	ans = askyn("��ȷ��Ҫɾ�����������", NA, NA);
	if (ans != 1) {
		move(2, 0);
		prints("ȡ��ɾ���ж�\n");
		pressreturn();
		clear();
		return 0;
	} {
		char    secu[STRLEN];
		sprintf(secu, "ɾ����������%s", binfo.filename);
		securityreport(secu);
	}
	if (seek_in_file("0Announce/.Search", bname)) {
		move(4, 0);
		if (askyn("�Ƴ�������", NA, NA) == YEA) {
			get_grp(binfo.filename);
			del_grp(lookgrp, binfo.filename, binfo.title + 8);
		}
	}
	if (seek_in_file("etc/junkboards", bname))
		del_from_file("etc/junkboards", bname);
	if (seek_in_file("0Announce/.Search", bname))
		del_from_file("0Announce/.Search", bname);

	if (binfo.filename[0] == '\0')
		return -1;	/* rrr - precaution */
	sprintf(genbuf, "boards/%s", binfo.filename);
	f_rm(genbuf);
	sprintf(genbuf, "vote/%s", binfo.filename);
	f_rm(genbuf);
	sprintf(genbuf, " << '%s' �� %s ɾ�� >>",
		binfo.filename, currentuser.userid);
	memset(&binfo, 0, sizeof(binfo));
	strncpy(binfo.title, genbuf, STRLEN);
	binfo.level = PERM_SYSOP;
	substitute_record(BOARDS, &binfo, sizeof(binfo), bid);

	move(4, 0);
	prints("\n���������Ѿ�ɾ��...\n");
	pressreturn();
	numboards = -1;
	clear();
	return 0;
}

int
d_user(cid)
char   *cid;
{
	int     id;
	char    secu[STRLEN];
	modify_user_mode(ADMIN);
	if (!check_systempasswd()) {
		return;
	}
	clear();
	stand_title("ɾ��ʹ�����ʺ�");
	move(1, 0);
	usercomplete("��������ɾ����ʹ���ߴ���: ", genbuf);
	if (*genbuf == '\0') {
		clear();
		return 0;
	}
	if (!(id = getuser(genbuf))) {
		move(3, 0);
		prints("�����ʹ���ߴ���...");
		pressreturn();
		clear();
		return 0;
	}
	move(1, 0);
	prints("ɾ��ʹ���� [%s].", genbuf);
	clrtoeol();
	move(2, 0);
	if (askyn(genbuf, NA, NA) == NA) {
		move(2, 0);
		prints("ȡ��ɾ��ʹ����...\n");
		pressreturn();
		clear();
		return 0;
	}
	if (lookupuser.userid[0] == '\0' || !strcmp(lookupuser.userid, "SYSOP")) {
		prints("�޷�ɾ��!!\n");
		pressreturn();
		clear();
		return 0;
	}
	sprintf(secu, "ɾ��ʹ���ߣ�%s", lookupuser.userid);
	securityreport(secu);
	sprintf(genbuf, "mail/%c/%s", toupper(lookupuser.userid[0]), lookupuser.userid);
	f_rm(genbuf);
	sprintf(genbuf, "home/%c/%s", toupper(lookupuser.userid[0]), lookupuser.userid);
	f_rm(genbuf);
	lookupuser.userlevel = 0;
	strcpy(lookupuser.address, "");
	strcpy(lookupuser.username, "");
	strcpy(lookupuser.realname, "");
	strcpy(lookupuser.termtype, "");
	move(2, 0);
	prints("%s �Ѿ��������...\n", lookupuser.userid);
	lookupuser.userid[0] = '\0';
	substitute_record(PASSFILE, &lookupuser, sizeof(lookupuser), id);
	setuserid(id, lookupuser.userid);
	pressreturn();
	clear();
	return 1;
}

int
x_level()
{
	int     id;
	unsigned int newlevel;
	modify_user_mode(ADMIN);
	if (!check_systempasswd()) {
		return;
	}
	clear();
	move(0, 0);
	prints("����ʹ����Ȩ��\n");
	clrtoeol();
	move(1, 0);
	usercomplete("���������ĵ�ʹ�����ʺ�: ", genbuf);
	if (genbuf[0] == '\0') {
		clear();
		return 0;
	}
	if (!(id = getuser(genbuf))) {
		move(3, 0);
		prints("Invalid User Id");
		clrtoeol();
		pressreturn();
		clear();
		return 0;
	}
	move(1, 0);
	clrtobot();
	move(2, 0);
	prints("�趨ʹ���� '%s' ��Ȩ�� \n", genbuf);
	newlevel = setperms(lookupuser.userlevel, "Ȩ��", NUMPERMS, showperminfo);
	move(2, 0);
	if (newlevel == lookupuser.userlevel)
		prints("ʹ���� '%s' Ȩ��û�б��\n", lookupuser.userid);
	else {
		lookupuser.userlevel = newlevel;
		{
			char    secu[STRLEN];
			sprintf(secu, "�޸� %s ��Ȩ��", lookupuser.userid);
			securityreport(secu);
		}

		substitute_record(PASSFILE, &lookupuser, sizeof(struct userec), id);
		prints("ʹ���� '%s' Ȩ���Ѿ��������.\n", lookupuser.userid);
	}
	pressreturn();
	clear();
	return 0;
}

void
a_edits()
{
	int     aborted;
	char    ans[7], buf[STRLEN], buf2[STRLEN];
	int     ch, num, confirm;
	static char *e_file[] =
	{"../Welcome", "../Welcome2", "issue", "logout", "../vote/notes",
		"menu.ini", "../.badname", "../.bad_email", "../.bansite", "../.blockmail",
	"autopost", "junkboards", "sysops", "expire.ctl", "whatdate", "../NOLOGIN", "../innd/black.list.general", NULL};
	static char *explain_file[] =
	{"�����վ������", "��վ����", "��վ��ӭ��", "��վ����"
		,"���ñ���¼", "menu.ini", "����ע��� ID", "����ȷ��֮E-Mail", "������վ֮λַ"
		,"����E-mail������", "ÿ���Զ����ŵ�", "����POST���İ�", "����������", "��ʱ�����趨��",
	"�������嵥", "NOLOGIN", "ת�ź�����", NULL};
	modify_user_mode(ADMIN);
	if (!check_systempasswd()) {
		return;
	}
	clear();
	move(1, 0);
	prints("����ϵͳ����\n\n");
	for (num = 0; HAS_PERM(PERM_SYSOP) ? e_file[num] != NULL && explain_file[num] != NULL : explain_file[num] != "menu.ini"; num++) {
		prints("[[1;32m%2d[m] %s\n", num + 1, explain_file[num]);
	}
	prints("[[1;32m%2d[m] �������\n", num + 1);

	getdata(num + 5, 0, "��Ҫ������һ��ϵͳ����: ", ans, 3, DOECHO, YEA);
	ch = atoi(ans);
	if (!isdigit(ans[0]) || ch <= 0 || ch > num || ans[0] == '\n' || ans[0] == '\0')
		return;
	ch -= 1;
	sprintf(buf2, "etc/%s", e_file[ch]);
	move(3, 0);
	clrtobot();
	sprintf(buf, "(E)�༭ (D)ɾ�� %s? [E]: ", explain_file[ch]);
	getdata(3, 0, buf, ans, 2, DOECHO, YEA);
	if (ans[0] == 'D' || ans[0] == 'd') {
		confirm = askyn("��ȷ��Ҫɾ�����ϵͳ��", NA, NA);
		if (confirm != 1) {
			move(5, 0);
			prints("ȡ��ɾ���ж�\n");
			pressreturn();
			clear();
			return;
		} {
			char    secu[STRLEN];
			sprintf(secu, "ɾ��ϵͳ������%s", explain_file[ch]);
			securityreport(secu);
		}
		unlink(buf2);
		move(5, 0);
		prints("%s ��ɾ��\n", explain_file[ch]);
		pressreturn();
		clear();
		return;
	}
	modify_user_mode(EDITSFILE);
	aborted = vedit(buf2, NA);
	clear();
	if (aborted != -1) {
		prints("%s ���¹�", explain_file[ch]);
		{
			char    secu[STRLEN];
			sprintf(secu, "�޸�ϵͳ������%s", explain_file[ch]);
			securityreport(secu);
		}

		if (!strcmp(e_file[ch], "../Welcome")) {
			unlink("Welcome.rec");
			prints("\nWelcome ��¼������");
		}
	}
	pressreturn();
}

int
wall()
{
	if (!HAS_PERM(PERM_SYSOP))
		return 0;
	modify_user_mode(MSG);
	move(2, 0);
	clrtobot();
	if (!get_msg("����ʹ����", buf2, 1)) {
		return 0;
	}
	if (apply_ulist(dowall) == 0) {
		move(2, 0);
		prints("���Ͽ���һ��\n");
		pressanykey();
	}
	prints("\n�Ѿ��㲥���...\n");
	pressanykey();
	return 1;
}

int
setsystempasswd()
{
	FILE   *pass;
	char    passbuf[20], prepass[20];
	modify_user_mode(ADMIN);
	if (strcmp(currentuser.userid, "SYSOP"))
		return;
	if (!check_systempasswd())
		return;
	getdata(2, 0, "�������µ�ϵͳ����: ", passbuf, 19, NOECHO, YEA);
	getdata(3, 0, "ȷ���µ�ϵͳ����: ", prepass, 19, NOECHO, YEA);
	if (strcmp(passbuf, prepass))
		return;
	if ((pass = fopen("etc/.syspasswd", "w")) == NULL) {
		move(4, 0);
		prints("ϵͳ�����޷��趨....");
		pressanykey();
		return;
	}
	fprintf(pass, "%s\n", genpasswd(passbuf));
	fclose(pass);
	move(4, 0);
	prints("ϵͳ�����趨���....");
	pressanykey();
	return;
}

int
x_csh()
{
	char    buf[PASSLEN];
	int     save_pager;
	int     magic;
	if (!HAS_PERM(PERM_SYSOP)) {
		return 0;
	}
	if (!check_systempasswd()) {
		return;
	}
	modify_user_mode(SYSINFO);
	clear();
	getdata(1, 0, "������ͨ�а���: ", buf, PASSLEN, NOECHO, YEA);
	if (*buf == '\0' || !checkpasswd(currentuser.passwd, buf)) {
		prints("\n\n���Ų���ȷ, ����ִ�С�\n");
		pressreturn();
		clear();
		return;
	}
	randomize();
	magic = rand() % 1000;
	prints("\nMagic Key: %d", magic * 5 - 2);
	getdata(4, 0, "Your Key : ", buf, PASSLEN, NOECHO, YEA);
	if (*buf == '\0' || !(atoi(buf) == magic)) {
		securityreport("Fail to shell out");
		prints("\n\nKey ����ȷ, ����ִ�С�\n");
		pressreturn();
		clear();
		return;
	}
	securityreport("Shell out");
	modify_user_mode(SYSINFO);
	clear();
	refresh();
	reset_tty();
	save_pager = uinfo.pager;
	uinfo.pager = 0;
	update_utmp();
	do_exec("csh", NULL);
	restore_tty();
	uinfo.pager = save_pager;
	update_utmp();
	clear();
	return 0;
}

int
kick_user(userinfo)
struct user_info *userinfo;
{
	int     id, ind;
	struct user_info uin;
	struct userec kuinfo;
	char    kickuser[40], buffer[40];
	if (uinfo.mode != LUSERS && uinfo.mode != OFFLINE && uinfo.mode != FRIEND) {
		modify_user_mode(ADMIN);
		stand_title("��ʹ������վ");
		move(1, 0);
		usercomplete("����ʹ�����ʺ�: ", kickuser);
		if (*kickuser == '\0') {
			clear();
			return 0;
		}
		if (!(id = getuser(kickuser))) {
			move(3, 0);
			prints("Invalid User Id");
			clrtoeol();
			pressreturn();
			clear();
			return 0;
		}
		move(1, 0);
		prints("�ߵ�ʹ���� : [%s].", kickuser);
		clrtoeol();
		move(2, 0);
		if (askyn(genbuf, NA, NA) == NA) {
			move(2, 0);
			prints("ȡ����ʹ����..\n");
			pressreturn();
			clear();
			return 0;
		}
		search_record(PASSFILE, &kuinfo, sizeof(kuinfo), cmpuids, kickuser);
		ind = search_ulist(&uin, t_cmpuids, id);
	} else {
		uin = *userinfo;
		strcpy(kickuser, uin.userid);
/*        id = getuser(kickuser);
        search_record(PASSFILE, &kuinfo, sizeof(kuinfo), cmpuids, kickuser);
        ind = search_ulist( &uin, t_cmpuids, id ); */
		ind = YEA;
	}
	if (!ind || !uin.active || (kill(uin.pid, 0) == -1)) {
		if (uinfo.mode != LUSERS && uinfo.mode != OFFLINE && uinfo.mode != FRIEND) {
			move(3, 0);
			prints("User Has Logged Out");
			clrtoeol();
			pressreturn();
			clear();
		}
		return 0;
	}
	kill(uin.pid, SIGHUP);
	sprintf(buffer, "kicked %s", kickuser);
	report(buffer);
	sprintf(genbuf, "%s (%s)", kuinfo.userid, kuinfo.username);
	log_usies("KICK ", genbuf);
	uin.active = NA;
	uin.pid = 0;
	uin.invisible = YEA;
	uin.sockactive = 0;
	uin.sockaddr = 0;
	uin.destuid = 0;
	update_ulist(&uin, ind);
	move(2, 0);
	if (uinfo.mode != LUSERS && uinfo.mode != OFFLINE && uinfo.mode != FRIEND) {
		prints("User has been Kicked\n");
		pressreturn();
		clear();
	}
	return 1;
}

#endif
