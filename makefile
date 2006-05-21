#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
 
export TOPDIR	:=	$(CURDIR)
 
DATESTRING	:=	$(shell date +%Y)$(shell date +%m)$(shell date +%d)
 
 
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
	@tar --exclude=*CVS* -cvjf dswifi-src-$(DATESTRING).tar.bz2 arm7 arm9 common include makefile 

dist-bin: all
	@tar --exclude=*CVS* -cvjf dswifi-$(DATESTRING).tar.bz2 include lib

dist: dist-bin dist-src

