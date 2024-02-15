CC:=gcc-13
CXX:=g++-13

# CC:=clang-17
# CXX:=clang++-17

project_dir = $(shell pwd)

debug:
	/usr/bin/cmake --build ${project_dir}/build --target Debug | tee build/build.log

release:
	/usr/bin/cmake --build ${project_dir}/build --target Release | tee build/build.log

all:
	/usr/bin/cmake --build ${project_dir}/build --target all | tee build/build.log

conf:
	rm -r build/* && /usr/bin/cmake -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/${CC} -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/${CXX} -S${project_dir} -B${project_dir}/build -G Ninja

clean:
	/usr/bin/cmake --build ${project_dir}/build --target clean --

run: debug
	${project_dir}/build/Debug

run-release: release
	${project_dir}/build/Release
