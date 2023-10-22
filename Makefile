CC  = gcc
CXX = g++
G = Unix Makefiles
J = 16

compile-step: build-step
	cmake --build ./build -j $(J)

build-step:
	cmake -B ./build "-DCMAKE_C_COMPILER:FILEPATH=$(CC)" "-DCMAKE_CXX_COMPILER:FILEPATH=$(CXX)" -G "$(G)" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

clean:
	git clean -d -f -x ./build