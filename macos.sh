cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_MAKE_PROGRAM="ninja" -G Ninja -S "$(pwd)" -B "$(pwd)/cmake-build-debug" &&
cmake --build "$(pwd)/cmake-build-debug" --target 2024_I2P2_TowerDefense_with_answer &&
"$(pwd)/cmake-build-debug/2024_I2P2_TowerDefense_with_answer"
