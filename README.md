# xwaxdb

This software can be used to create a music-database for [xwax](http://xwax.org/).
On startup, new files will be scanned for ID3-Tags and, if wished, the BPM will be calculated using [bpm-tag](http://wiki.xwax.org/bpm-tools).

With this database xwax can be launched much faster because the ID3-Tags of the files will not be scanned each time xwax is launched.

There are three ways to create _crates_ to use within xwax

* Import [gmusicbrowser](https://gmusicbrowser.org/) playlists, easily selected by the xwaxdb-GUI
* Import all m3u-playlists from your specified xwax-music-folder
* Import recursively all folders containing music-files as _crate_ from your specified xwax-music-folder

Once the database is build, xwaxdb can be even used as commandline-tool just importing and scanning new files.

Launching xwax using the xwaxdb-database can be done like this:

```
XWAXDB_DIR="$HOME/.config/quasd/xwaxdb/"
LIBRARY_SCANNER="${XWAXDB_DIR}/bin/xwax_libscanner.sh"
F_XWAXDB_LPARAMS="${XWAXDB_DIR}/xwax_lparams.lst"
CRATES=""

while read line
do
  CRATES="-l ${line} ${CRATES}"
done < $F_XWAXDB_LPARAMS

xwax -k -m 5 -r 44100 -t mixvibes_v2 -j Deck_A -j Deck_B -s $LIBRARY_SCANNER $CRATES
```

![screenshot](http://www.soundiction.de/images/xwaxdb.png)
