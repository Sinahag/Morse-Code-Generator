# RUN THIS ON THE HOST!

all: nested-sharedMem pru-copy

nested-sharedMem:
	make --directory=as4-linux

pru-copy:
	mkdir -p $(HOME)/cmpt433/public/pru/
	cp -r * $(HOME)/cmpt433/public/pru/
	@echo "COPY ONLY" > $(HOME)/cmpt433/public/pru/_COPY_ONLY_
	@echo ""
	@echo "You must build the PRU code on the target, then install it:"
	@echo "(bbg)$$ cd /mnt/remote/pru/as4-pru/"
	@echo "(bbg)$$ make"
	@echo "(bbg)$$ sudo make install_PRU0"