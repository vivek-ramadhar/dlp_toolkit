## Project File Structure

---

```
dlp_toolkit/
├── apps/
│   ├── CMakeLists.txt
│   └── main.cpp
├── python/
│   ├── CMakeLists.txt
│   ├── bindings.cpp
│   ├── elliptic_curve_visualizer.py
│   └── tests.py
├── src/
│   ├── CMakeLists.txt
│   ├── dlp_toolkit.cpp
│   └── dlp_toolkit.h
├── CMakeLists.txt
├── Makefile
└── README.md
```

---

## Project Dependencies

- GCC 13+
- CMake >= 3.20
- Git
- Tracy v0.13.1
- pybind11 v2.11.1
- python3-dev ( for python bindings )

Tracy and pybind11 are fetched and installed using CMake's FetchContent_Declare() function. Thus, they are only installed when needed or specified by compile options. To actually observe and interact with the Tracy profiled code, you do need to install the tracy-profiler executable separately. Tracy is very particular about versioning. The executable version MUST MATCH THE EXACT sourcec dependcy version. This project compiles with Tracy v0.13.1 .

## Quick-Setup

Here is the quickest way to setup, test, and run the code in this project. I assume the base machine is a fresh Ubuntu 24.04 install.

```
apt-get update
apt-get install -y build-essential cmake git
git clone https://github.com/vivek-ramadhar/dlp_toolkit.git
cd dlp_toolkit
make
./build/apps/main
```

This compiles and runs the test contained inside `apps/main.cpp`. The Makefile provides a simpler interface to the various CMake project targets and different compilation options.

Here are the most common/important compilation options:

`make debug`

- This sets the CMAKE_BUILD_TYPE to Debug, which in turn defines the macro `DEBUG` to enable certain sections of source guarded by `#ifdef DEBUG`. Additionally, it enables the TRACY compilation options which defines the `TRACY_ENABLE` macro, and builds the Tracy target.

`make tracy`

- This sets the CMAKE_BUILD_TYPE to Release, enables the NATIVE compilation options (just `-O3 -march=native`), and enables the TRACY compilation option.

`make bindings`

- This just enables the BINDINGS compilation option, which compiles the `python/bindings.cpp` file into a python library (PY_MODULE.so) which is then installed and copied to project root so it can be easily imported in python scripts.

`make clean`

- This deletes all content of the build/ folder and and finds the python .so file and deletes that too.

---
