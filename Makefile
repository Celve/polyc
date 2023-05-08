build: 
	cmake --build build -j 24

run: build
	./build/poc $(FILE)

.PHONY: build run