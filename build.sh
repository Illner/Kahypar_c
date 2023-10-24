if [ -d build ]; then rm -Rf build; fi

mkdir build
cd build

# Release
mkdir Release
cd Release
cmake -D CMAKE_BUILD_TYPE=Release -G "Unix Makefiles" ../..
make

cd ..

# Debug
mkdir Debug
cd Debug
cmake -D CMAKE_BUILD_TYPE=Debug -G "Unix Makefiles" ../..
make

cd ..
