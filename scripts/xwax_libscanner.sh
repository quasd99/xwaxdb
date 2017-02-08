#!/bin/bash

oIFS=$IFS
IFS="
"

XWAXDBFILES_DIR="$HOME/.config/quasd/xwaxdb/db"
DBFILE=`find $XWAXDBFILES_DIR -name "${1}.xwaxdb"`
cat $DBFILE

IFS=$oIFS

