## Process this file with automake to produce Makefile.in

## Created by Anjuta

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(YD_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g \
	 -pthread

bin_PROGRAMS = yd \
	listen \
	sockstress

yd_SOURCES = \
	main.c \
	sudo.c \
	sudo.h \
	yd.h \
	yd.c \
	wio.h \
	wio.c \
	ydmainwindow.h \
	ydmainwindow.c \
	ydtcpdetail.h \
	ydtcpdetail.c \
	procnet.h \
	procnet.c \
	wdetect.c \
	wdetect.h \
	notify.c \
	notify.h

yd_LDFLAGS = -lpcap

yd_LDADD = $(YD_LIBS)

listen_SOURCES =  \
	listen.c

sockstress_SOURCES =  \
	sockstress.c

