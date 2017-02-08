#!/bin/bash
# Reset all db-files

XWAXDB_DIR="$HOME/.config/quasd/xwaxdb"

rm ${XWAXDB_DIR}/crates.db 
echo "" > ${XWAXDB_DIR}/xwax.db 
rm ${XWAXDB_DIR}/db/*