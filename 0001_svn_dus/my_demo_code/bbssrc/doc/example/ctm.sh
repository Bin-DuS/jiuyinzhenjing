#!/bin/sh
# �� shell script ���� ctm һ�θ��¶�� ctm Delta
# NAME Ϊ����֮ǰ�ĵ���
# �÷� : �� ctm.sh �ŵ�Ҫ�⿪��Ŀ¼ ex. /home/bbsadm/bbssrc��CTM���������
#	 /home/bbsadm/CTM����Ҫ���±�� 2 - 23 �� Delta
#
#	cd /home/bbsadm/bbssrc
#	ctm.sh /home/bbsadm/CTM 2 23
#					�κ������� mail: skyo@mgt.ncu.edu.tw
#
#   $Id: ctm.sh,v 1.2 1999/06/06 04:35:24 skyo Exp $    
NAME="fb3src-"
A=$2
while [ $3 -ge $A ]
do
	ctm -v $1/$NAME$A.gz
        A=`expr $A + 1 `
done
