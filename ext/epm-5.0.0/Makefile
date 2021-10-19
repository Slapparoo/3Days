#
# Makefile for the ESP Package Manager (EPM).
#
# Copyright © 2020 by Jim Jagielski
# Copyright © 1999-2020 by Michael R Sweet
# Copyright © 1999-2010 by Easy Software Products, all rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Programs...
AR		=	/usr/bin/ar
CC		=	clang
CHMOD		=	/usr/bin/chmod
CODE_SIGN	=	/usr/bin/true
CP		=	/usr/bin/cp
CXX		=	clang++
FLTKCONFIG	=	/usr/bin/fltk-config
INSTALL		=	/usr/bin/install -c
MKDIR		=	/usr/bin/mkdir -p
RANLIB		=	ranlib
RM		=	/usr/bin/rm -f
SHELL		=	/bin/sh
STRIP		=	/usr/bin/strip


# Program options...
ARFLAGS		=	crs
ARCHFLAGS	=	
ASAN_OPTIONS	=	leak_check_at_exit=false
CFLAGS		=	$(ARCHFLAGS) -Wall -Wformat -Wno-format-y2k -Wunused  -D_FORTIFY_SOURCE=2 $(OPTIM)
CODESIGN_IDENTITY =	Developer ID
CPPFLAGS	=	
CSFLAGS		=	-s "$(CODESIGN_IDENTITY)" --timestamp -o runtime
CXXFLAGS	=	$(ARCHFLAGS) -Wall -Wformat -Wno-format-y2k -Wunused -I/usr/include/cairo -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/pixman-1 -I/usr/include/uuid -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/freetype2 -I/usr/include/libpng16   -D_FORTIFY_SOURCE=2 -fno-rtti -fno-exceptions $(OPTIM)
GUILIBS		=	/usr/lib/x86_64-linux-gnu/libfltk_images.a -lpng -lz -ljpeg  /usr/lib/x86_64-linux-gnu/libfltk.a -lXrender -lXcursor -lXfixes -lXext -lXft -lfontconfig -lXinerama -lpthread -ldl -lm  -lX11 
LDFLAGS		=	$(ARCHFLAGS)  $(OPTIM)
LIBS		=	
OPTIM		=	-Os -g


# Directories...
bindir		=	${exec_prefix}/bin
datadir		=	/usr/local/share
datarootdir	=	/usr/local/share
exec_prefix	=	/usr/local
includedir	=	${prefix}/include
infodir		=	${datarootdir}/info
libdir		=	/usr/local/lib
libexecdir	=	${exec_prefix}/libexec
localstatedir	=	${prefix}/var
mandir		=	${datarootdir}/man
oldincludedir	=	/usr/include
prefix		=	/usr/local
privateinclude	=	@privateinclude@
sbindir		=	${exec_prefix}/sbin
sharedstatedir	=	${prefix}/com
srcdir		=	.
sysconfdir	=	${prefix}/etc
top_srcdir	=	.

BUILDROOT	=	$(DSTROOT)$(RPM_BUILD_ROOT)$(DESTDIR)



# Rules...
.SILENT:

.SUFFIXES:	.c .cxx .h .o
.c.o:
	echo Compiling $<...
	$(CC) $(CFLAGS) $(CPPFLAGS) -I. -I$(srcdir) -c $<
.cxx.o:
	echo Compiling $<...
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -I. -I$(srcdir) -c $<


# Targets...
TARGETS		=	libepm.a \
			epm \
			epminstall \
			mkepmlist \
			setup uninst
EPM_OBJS	=	aix.o \
			bsd.o \
			deb.o \
			dist.o \
			file.o \
			inst.o \
			macos.o \
			pkg.o \
			portable.o \
			qprintf.o \
			rpm.o \
			run.o \
			setld.o \
			slackware.o \
			snprintf.o \
			string.o \
			support.o \
			swinstall.o \
			tar.o
SETUP_OBJS	=	setup.o \
			setup2.o \
			gui-common.o
UNINST_OBJS	=	uninst.o \
			uninst2.o \
			gui-common.o
OBJS		=	epm.o \
			$(EPM_OBJS) \
			epminstall.o \
			mkepmlist.o \
			$(SETUP_OBJS) \
			$(UNINST_OBJS)


# Make all targets...
all:	$(TARGETS) Makefile config.h


# Clean all generated files...
clean:
	$(RM) $(OBJS)
	$(RM) $(TARGETS)


# Clean all generated and configuration files...
distclean:
	$(RM) $(OBJS)
	$(RM) $(TARGETS)
	$(RM) config.cache config.h config.log config.status
	$(RM) Makefile doc/Makefile
	$(RM) epm.list


#
# Run the clang.llvm.org static code analysis tool on the C sources.
#

.PHONY: clang clang-changes
clang:
	$(RM) -r clang
	scan-build -V -k -o `pwd`/clang $(MAKE) $(MFLAGS) clean all
clang-changes:
	scan-build -V -k -o `pwd`/clang $(MAKE) $(MFLAGS) all


# Install all targets...
install: all install-guis 
	echo Installing EPM programs in $(BUILDROOT)$(bindir)
	$(INSTALL) -d -m 755 $(BUILDROOT)$(bindir)
	for file in epm epminstall mkepmlist; do \
		$(INSTALL) -c -m 755 $$file $(BUILDROOT)$(bindir); \
	done
	(cd doc; $(MAKE) $(MFLAGS) install)

install-guis:	setup uninst
	echo Installing EPM setup/uninst in $(BUILDROOT)$(libdir)/epm
	$(INSTALL) -d -m 755 $(BUILDROOT)$(libdir)/epm
	for file in setup uninst; do \
		$(INSTALL) -c -m 755 $$file $(BUILDROOT)$(libdir)/epm; \
	done

install-macos:
	echo Installing EPM macOS data files in $(BUILDROOT)$(datadir)/epm
	$(INSTALL) -d -m 755 $(BUILDROOT)$(datadir)/epm
	$(INSTALL) -c -m 644 default.icns $(BUILDROOT)$(datadir)/epm


# Uninstall all targets...
uninstall:
	echo Uninstalling EPM programs from $(BUILDROOT)$(bindir)
	$(RM) $(BUILDROOT)$(bindir)/epm
	$(RM) $(BUILDROOT)$(bindir)/epminstall
	$(RM) $(BUILDROOT)$(bindir)/mkepmlist
	echo Uninstalling EPM setup/uninstall from $(BUILDROOT)$(libdir)/epm
	$(RM) -r $(BUILDROOT)$(libdir)/epm
	echo Uninstalling EPM macOS data files from $(BUILDROOT)$(datadir)/epm
	$(RM) -r $(BUILDROOT)$(datadir)/epm


# Makefile
Makefile:	Makefile.in configure
	if test -f config.status; then \
		./config.status --recheck; \
		./config.status; \
	else \
		./configure; \
	fi
	touch config.h


# config.h
config.h:	config.h.in configure
	if test -f config.status; then \
		./config.status --recheck; \
		./config.status; \
	else \
		./configure; \
	fi
	touch config.h


# Test EPM...
test:	$(TARGETS)
	echo Starting portable distribution build test...
	if ./epm -vv epm >test.log; then \
		echo Portable distribution build test PASSED.; \
	else \
		echo Portable distribution build test FAILED.; \
		cat test.log; \
	fi
	echo Starting native distribution build test...
	if ./epm -vv -s default.png -f native \
		--setup-program setup --uninstall-program uninst \
		epm >test.log; then \
		echo Native distribution build test PASSED.; \
	else \
		echo Native distribution build test FAILED.; \
		cat test.log; \
	fi
	$(RM) test.log


# Make distributions in different formats using EPM...
aix: $(TARGETS)
	./epm -f aix -v epm

bsd: $(TARGETS)
	./epm -f bsd -v epm

slackware: $(TARGETS)
	./epm -f slackware -v epm

deb: $(TARGETS)
	./epm -f deb -v epm

inst tardist: $(TARGETS)
	./epm -f tardist -v epm

gui: $(TARGETS)
	./epm -f portable -v -s default.png --setup-program setup \
		--uninstall-program uninst --data-dir . epm

lsb macos portable rpm: $(TARGETS)
	./epm -f $@ -v -s default.png --setup-program setup \
		--uninstall-program uninst --data-dir . epm

pkg: $(TARGETS)
	./epm -f pkg -v epm

native: $(TARGETS)
	./epm -f native -v epm

swinstall depot: $(TARGETS)
	./epm -f depot -v epm

# libepm.a
libepm.a:	$(EPM_OBJS)
	echo Archiving libepm.a...
	$(RM) libepm.a
	$(AR) $(ARFLAGS) libepm.a $(EPM_OBJS)
	$(RANLIB) libepm.a

$(EPM_OBJS):	epm.h epmstring.h


# epm
epm:	epm.o libepm.a
	echo Linking epm...
	$(CC) $(LDFLAGS) -o epm epm.o libepm.a $(LIBS)
	#echo Code signing $@...
	#$(CODE_SIGN) $(CSFLAGS) -i com.jimjag.epm.$@ $@

epm.o:	epm.h epmstring.h


# epminstall
epminstall:	epminstall.o libepm.a
	echo Linking epminstall...
	$(CC) $(LDFLAGS) -o epminstall epminstall.o libepm.a $(LIBS)
	#echo Code signing $@...
	#$(CODE_SIGN) $(CSFLAGS) -i com.jimjag.epm.$@ $@

epminstall.o:	epm.h epmstring.h


# mkepmlist
mkepmlist:	mkepmlist.o libepm.a
	echo Linking mkepmlist...
	$(CC) $(LDFLAGS) -o mkepmlist mkepmlist.o libepm.a $(LIBS)
	#echo Code signing $@...
	#$(CODE_SIGN) $(CSFLAGS) -i com.jimjag.epm.$@ $@

mkepmlist.o:	epm.h epmstring.h


# setup (GUI)
setup:	$(SETUP_OBJS) libepm.a
	echo Linking setup...
	$(CXX) $(LDFLAGS) -o setup $(SETUP_OBJS) libepm.a $(GUILIBS) $(LIBS)

setup.o:	setup.h gui-common.h epmstring.h
setup2.o:	setup.h gui-common.h epmstring.h
gui-common.o:	gui-common.h


# uninst (GUI)
uninst:	$(UNINST_OBJS) libepm.a
	echo Linking uninst...
	$(CXX) $(LDFLAGS) -o uninst $(UNINST_OBJS) libepm.a $(GUILIBS) $(LIBS)

uninst.o:	uninst.h epmstring.h gui-common.h
uninst2.o:	uninst.h epmstring.h gui-common.h
gui-common.o:	gui-common.h


# Common dependencies...
$(OBJS):	Makefile config.h
