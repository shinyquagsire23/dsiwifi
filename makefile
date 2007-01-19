ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>devkitPro)
endif
 
export TOPDIR	:=	$(CURDIR)
 
VERSION	:=	0.3d
 
 
all: release debug
 
release: lib
	make -C arm9 BUILD=release
	make -C arm7 BUILD=release
 
debug: lib
	make -C arm9 BUILD=debug
	make -C arm7 BUILD=debug
 
lib:
	mkdir lib
 
clean:
	make -C arm9 clean
	make -C arm7 clean

dist-src: clean
	@tar --exclude=*CVS* -cvjf dswifi-src-$(VERSION).tar.bz2 arm7 arm9 common include makefile 

dist-bin: all
	@tar --exclude=*CVS* -cvjf dswifi-$(VERSION).tar.bz2 include lib

dist: dist-bin dist-src

install: dist-bin
	bzip2 -cd dswifi-$(VERSION).tar.bz2 | tar -xv -C $(DEVKITPRO)/libnds
