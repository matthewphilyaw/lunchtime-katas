.PHONY: build 

all: build 

build:
	(cd build; make)

rebuild: destroy init all

clean:
	(cd build; make clean)

destroy:
	rm -rf build

init:
	mkdir -p build
	(cd build; cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain-avr-gcc.cmake ..)
