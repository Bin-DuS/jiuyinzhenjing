/*
 * functions.h		-- include file for define functions individual
 *	
 * Copyright (c) 1999, Edward Ping-Da Chuang <edwardc@edwardc.dhs.org>
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
 * CVS: $Id: functions.h,v 1.14 1999/10/01 14:29:22 edwardc Exp $
 */

#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#define RNDSIGN
/* ����ǩ���� */

//#define MAGIC_PASS
/* վ���� ID ��վѯ�� Magic Pass */

#define REFUSE_LESS60SEC
/* 60 ���ڲ����ظ� login */

#define TALK_LOG
/* �����¼���� */

#define CODE_VALID
/* ������֤ */

//#define MSG_CANCLE_BY_CTRL_C
/* �� ctrl-c ������ѶϢ */

#define LOG_MY_MESG
/* ѶϢ��¼�м�¼�Լ���������ѶϢ */

#define COLOR_POST_DATE
/* ����������ɫ */

#define BIGGER_MOVIE
/* �Ӵ�����ռ� (����) */

#define ALWAYS_SHOW_BRDNOTE
/* ÿ�ν��嶼�� show �����廭�� */

#define BBSD
/* ʹ�� BBS daemon, �ص����ʹ�� in.zbbsd, telnetd ��ʹ�� bbsrf */

//#define MUDCHECK_BEFORELOGIN
/* ����ĳЩ MUD server ��Ҫ��ʹ���ߵ���֮ǰ������ȷ������ʹ�ó�ʽ������
   Ctrl + C ��ʱ�ɸ�Ϊ Ctrl + D, Ctrl + Z .. */
   
#endif /* _FUNCTIONS_H_ */
