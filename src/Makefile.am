lib_LTLIBRARIES = libXrandr.la

libXrandr_la_SOURCES = 	\
	Xrandr.c 	\
	Xrandr.h	\
	Xrandrint.h

libXrandr_la_LIBADD = @X_LIBS@ -lX11 -lXext -lXrender

INCLUDES = -I$(top_srcdir)/include/X11/extensions

#
# Library version info.  Check the libtool docs for
# instructions on when and how to change this value
#
libXrandr_la_LDFLAGS = -version-info 2:1:0 -no-undefined

libXrandrincludedir = $(includedir)/X11/extensions
libXrandrinclude_HEADERS = $(top_srcdir)/include/X11/extensions/Xrandr.h 

