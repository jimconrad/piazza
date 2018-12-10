#Get the right toolchain for macos/linux
export MAKE=gmake


all:
	$(MAKE) -C src

clean:
	$(MAKE) clean -C src
