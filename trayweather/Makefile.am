
AM_CFLAGS = $(JWMT_CFLAGS)
AM_CPPFLAGS = -DVERSION=\"${VERSION}\" -DPREFIX=\"${PREFIX}\"
AM_LDFLAGS = -lmetar $(JWMT_LIBS)

bin_PROGRAMS = trayweather

trayweather_SOURCES = trayweather.c

pkgdata_DATA = trayweather.glade
