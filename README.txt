cmake -B build -DCMAKE_TOOLCHAIN_FILE=xmos_cmake_toolchain/xs3a.cmake
cp converted_test_files/<testfile> build/
cd build 
mv <testfile> test_in.wav
make flak_eval
xrun --io flak_eval.xe
