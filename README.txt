cmake -B build -DCMAKE_TOOLCHAIN_FILE=xmos_cmake_toolchain/xs3a.cmake
cp converted_test_files/<testfile> build/
cd build 
mv <testfile> test_in.wav
make <target>
make run_<target>

Targets are
flak_eval_2
flak_eval_4
flak_eval_6
flak_eval_8
flak_eval_ddr_2
flak_eval_ddr_4
flak_eval_ddr_6
flak_eval_ddr_8
