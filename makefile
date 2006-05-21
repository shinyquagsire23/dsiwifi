#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
 
export TOPDIR	:=	$(CURDIR)
 
 
 
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


