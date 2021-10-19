#
# Makefile for the ESP Package Manager (EPM) documentation.
#
# Copyright © 2020 by Jim Jagielski
# Copyright © 1999-2020 by Michael R Sweet
# Copyright © 1999-2006 by Easy Software Products, all rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.#

# Programs...
CC	=	clang
HTMLDOC	=	/usr/bin/echo
RM	=	/usr/bin/rm -f


# Program options...
ARFLAGS		=	crs
ARCHFLAGS	=	
CFLAGS		=	$(ARCHFLAGS) -Wall -Wformat -Wno-format-y2k -Wunused  -D_FORTIFY_SOURCE=2 $(OPTIM)
CPPFLAGS	=	
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
top_srcdir	=	..

BUILDROOT	=	$(DSTROOT)$(RPM_BUILD_ROOT)$(DESTDIR)



# Man page generation rule...
.SUFFIXES:	.html .1 .5
.1.html .5.html:
	$(RM) $@
	./mantohtml $< >$@


# Targets...
MANPAGES	=	epm.html epminstall.html mkepmlist.html setup.html
HTMLFILES	=	title.html preface.html 1-intro.html 2-building.html \
			3-packaging.html 4-advanced.html 5-examples.html \
			a-license.html b-manpages.html c-reference.html \
			$(MANPAGES)
BOOKS		=	epm-book.epub epm-book.html epm-book.pdf


# Make everything (requires HTMLDOC)
all:	$(MANPAGES) $(BOOKS)

# Clean generated programs
clean:
	$(RM) mantohtml mantohtml.o

# Install documentation
install:
	echo Installing EPM manpages in $(BUILDROOT)$(mandir)/man1
	$(INSTALL) -d -m 755 $(BUILDROOT)$(mandir)/man1
	for file in epm.1 epminstall.1 mkepmlist. setup.1; do \
		$(INSTALL) -c -m 644 $$file $(BUILDROOT)$(mandir)/man1; \
	done
	echo Installing EPM manpages in $(BUILDROOT)$(mandir)/man5
	$(INSTALL) -d -m 755 $(BUILDROOT)$(mandir)/man5
	for file in epm.list.5 setup.types.5; do \
		$(INSTALL) -c -m 644 $$file $(BUILDROOT)$(mandir)/man5; \
	done
	echo Installing EPM documentation in $(BUILDROOT)$(docdir)
	$(INSTALL) -d -m 755 $(BUILDROOT)$(docdir)
	for file in $(srcdir)/COPYING $(srcdir)/README.md $(BOOKS); do \
		$(INSTALL) -c -m 644 $$file $(BUILDROOT)$(docdir); \
	done


# Uninstall all targets...
uninstall:
	echo Uninstalling EPM manpages from $(BUILDROOT)$(mandir)/man1
	$(RM) $(BUILDROOT)$(mandir)/man1/epm.1
	$(RM) $(BUILDROOT)$(mandir)/man1/epminstall.1
	$(RM) $(BUILDROOT)$(mandir)/man1/mkepmlist.1
	$(RM) $(BUILDROOT)$(mandir)/man1/setup.1
	echo Uninstalling EPM manpages from $(BUILDROOT)$(mandir)/man5
	$(RM) $(BUILDROOT)$(mandir)/man5/epm.list.5
	$(RM) $(BUILDROOT)$(mandir)/man5/setup.types.5
	echo Uninstalling EPM documentation from $(BUILDROOT)$(docdir)
	$(RM) -r $(BUILDROOT)$(docdir)

# EPUB book
epm-book.epub:	$(HTMLFILES) epm-256.png setup.png title.png epm-book.book
	$(HTMLDOC) --batch epm-book.book --titleimage title.png -f epm-book.epub

# HTML book
epm-book.html:	$(HTMLFILES) epm-256.png setup.png epm-book.book
	$(HTMLDOC) --batch epm-book.book -f epm-book.html

# PDF book
epm-book.pdf:	$(HTMLFILES) epm-256.png setup.png epm-book.book
	$(HTMLDOC) --batch epm-book.book --size universal -f epm-book.pdf

# mantohtml
mantohtml:	mantohtml.o
	$(CC) -o $@ mantohtml.o

# HTML man pages
$(MANPAGES):	mantohtml
