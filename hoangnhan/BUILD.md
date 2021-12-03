# HOW TO BUILD:
## Prerequisite:
- CMake 3.15 and up
- CCache

## Build process:
To build the main program, from `hoangnhan` folder, run the following command
```bash
$ cmake -S . -B ./cmake-build -DCMAKE_BUILD_TYPE=RelWithDebInfo -DENABLE_CACHE=OFF -DWARNINGS_AS_ERRORS=OFF -DENABLE_TESTING=OFF
$ cmake --build ./cmake-build
```

The program will be build in `./cmake-build/src` directory
