#!/bin/sh
HOST="FTP.academylabru.420.com1.ru"
PORT="21"
USER="zotov@academylabru.420.com1.ru"
PASSWD="zotov228"
FILE=$1
UFILE=$1
ftp -n $HOST $PORT <<INPUT_END
quote USER $USER
quote PASS $PASSWD
bin
put $FILE $UFILE
quit
INPUT_END
exit 0
