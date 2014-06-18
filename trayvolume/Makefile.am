
AM_CFLAGS = $(JWMT_CFLAGS)
AM_CPPFLAGS = -DVERSION=\"${VERSION}\" -DPREFIX=\"${PREFIX}\"
AM_LDFLAGS = $(JWMT_LIBS)

bin_PROGRAMS = pavolumeup pavolumedown alsavolumeup alsavolumedown trayvolume-alsa trayvolume-pulse

pavolumeup_SOURCES = pavolumeup.c
pavolumedown_SOURCES = pavolumedown.c
alsavolumeup_SOURCES = alsavolumeup.c
alsavolumedown_SOURCES = alsavolumedown.c
trayvolume_alsa_SOURCES = trayvolume-alsa.c
trayvolume_pulse_SOURCES = trayvolume-pulse.c

pkgdata_DATA = trayvolume.glade
