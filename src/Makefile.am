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
	 -fsanitize=address

bin_PROGRAMS = yd

yd_SOURCES = \
	main.c \
	sudo.c \
	sudo.h

yd_LDFLAGS = -lpcap

yd_LDADD = $(YD_LIBS)

