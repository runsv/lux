##
## default is to build on Linux using GNU make
##

sinclude mk/*?.mk

all :	src Lua

lua :
	cd ./lua && $(MAKE) all

sh :
	cd ./sh && $(MAKE) all

src :
	cd ./src && $(MAKE) all

install-exe :
	cd ./src && $(MAKE) install

install-lua :
	cd ./lua && $(MAKE) install

install :	install-exe install-lua

clean :

check :

test :		check

help :
	@echo valid make targets:

#all install uninstall clean:
#	cd src && $(MAKE) $@
