#!/bin/bash

XWAX_BIN=xwax
XWAXDB_DIR="$HOME/.config/quasd/xwaxdb/"
LIBRARY_SCANNER="${XWAXDB_DIR}/bin/xwax_libscanner.sh"
F_XWAXDB_LPARAMS="${XWAXDB_DIR}/xwax_lparams.lst"
CRATES=""

while read line
do
  CRATES="-l ${line} ${CRATES}"
done < $F_XWAXDB_LPARAMS

# Launch xwax
$XWAX_BIN -i "${XWAXDB_DIR}/bin/gatsxwax-import" -k -m 5 -r 44800 -t mixvibes_v2 -j Deck_A -j Deck_B -s $LIBRARY_SCANNER $CRATES
#$XWAX_BIN -i "$HOME/skripte/xwax-scan/new/gatsxwax-import" -k -m 5 -r 44100 -t mixvibes_v2 -a hw:ScratchAmp_1 -a hw:ScratchAmp -s $LIBRARY_SCANNER $PLS_ENTRIES
