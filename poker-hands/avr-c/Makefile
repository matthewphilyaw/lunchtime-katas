.PHONY: build 

all: build 

build:
	(cd build; make --no-print-directory)

rebuild: destroy init all

clean:
	(cd build; make clean)

destroy:
	rm -rf build

init: destroy
	mkdir -p build
	(cd build; cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain-avr-gcc.cmake ..)

initd: destroy 
	mkdir -p build
	(cd build; cmake -DDEBUG_MODE=ON -DCMAKE_TOOLCHAIN_FILE=../toolchain-avr-gcc.cmake ..)
