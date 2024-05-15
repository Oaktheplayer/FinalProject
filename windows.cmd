cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE -DCMAKE_MAKE_PROGRAM="%cd%/ninja" -S "%cd%/" -B "%cd%/build" -G "Unix Makefiles" &&
cmake --build "./build" --target all --config Debug -j 8 &&
start "./build/2024_I2P2_TowerDefense_with_answer"