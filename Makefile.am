bin_PROGRAMS = xwaxdb
xwaxdb_SOURCES = src/xwaxdb.cpp src/settings.h src/settings.cpp libquasd/file.h libquasd/file.cpp libquasd/gmbrcplaylistexporter.h libquasd/gmbrcplaylistexporter.cpp src/crate.h src/crate.cpp libquasd/m3utool.h libquasd/m3utool.cpp libquasd/tagtool.h libquasd/tagtool.cpp src/crate_audiofiles.h src/crate_audiofiles.cpp src/crate_pls.h src/crate_pls.cpp src/crate_gmbrc.h src/crate_gmbrc.cpp src/db_crates.h src/db_crates.cpp libquasd/settingsfile.h libquasd/settingsfile.cpp libquasd/gmbrctool.h libquasd/gmbrctool.cpp src/uimainwindow.h src/uimainwindow.cpp  src/uigmbrc.h src/uigmbrc.cpp src/uicrates.h src/uicrates.cpp

xwaxdb_CPPFLAGS = -Wall -O3 -std=c++11 -pedantic -DDEBUG `pkg-config --cflags gtkmm-3.0 glibmm-2.4`
xwaxdb_LDFLAGS = `pkg-config --libs gtkmm-3.0 glibmm-2.4 ` -lboost_filesystem -lboost_system -ltag

# m4 convention
ACLOCAL_AMFLAGS = -I m4 --install
# autoreconf 2.68 workaround: touch m4/NOTES ; git add m4/NOTES
EXTRA_DIST = m4/NOTES
