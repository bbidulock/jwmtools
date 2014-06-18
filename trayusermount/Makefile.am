
AM_CFLAGS = $(JWMT_CFLAGS) -I$(top_srcdir)/s -I$(top_srcdir)/scandirstr
AM_CPPFLAGS = -DVERSION=\"${VERSION}\" -DPREFIX=\"${PREFIX}\"
AM_LDFLAGS = $(JWMT_LIBS)

bin_PROGRAMS = trayusermount blkid_test

trayusermount_SOURCES = trayusermount.c

blkid_test_SOURCES = blkid_test.c

pkgdata_DATA = trayusermount.glade
