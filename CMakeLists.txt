cmake_minimum_required(VERSION 3.20)
project(dlp_toolkit LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 99)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ============================================================
#  OPTIONS
# ============================================================
option(BUILD_PYTHON_BINDINGS "Build Python bindings"  OFF)
option(BUILD_APPS            "Build standalone apps"  ON)
option(ENABLE_TRACY          "Enable Tracy profiler"  OFF)

# ============================================================
#  FETCHCONTENT — all deps fetched automatically, no submodules/pip
# ============================================================
include(FetchContent)

if(ENABLE_TRACY)
    FetchContent_Declare(
        tracy
        GIT_REPOSITORY https://github.com/wolfpld/tracy.git
        GIT_TAG        v0.13.1
        GIT_SHALLOW    TRUE
        GIT_PROGRESS   TRUE
    )
    FetchContent_MakeAvailable(tracy)
    add_compile_definitions(TRACY_ENABLE)
endif()

if(BUILD_PYTHON_BINDINGS)
    FetchContent_Declare(
        pybind11
        GIT_REPOSITORY https://github.com/pybind/pybind11.git
        GIT_TAG        v2.11.1
    )
    FetchContent_MakeAvailable(pybind11)
endif()

# ============================================================
#  CORE LIBRARY  (src/CMakeLists.txt defines 'myproject_core')
#  Both the app and Python module link against this.
# ============================================================
add_subdirectory(src)


# ============================================================
#  STANDALONE APPS  (apps/CMakeLists.txt)
# ============================================================
if(BUILD_APPS)
    add_subdirectory(apps)
endif()

# ============================================================
#  PYTHON BINDINGS
#  Binding source lives in python/bindings.cpp
#  Result: import dlp_toolkit_py
# ============================================================
if(BUILD_PYTHON_BINDINGS)
    pybind11_add_module(dlp_toolkit_py python/bindings.cpp)

    target_link_libraries(dlp_toolkit_py PRIVATE myproject_core)

    # Required so the core lib can be linked into a shared .so
    set_target_properties(myproject_core PROPERTIES POSITION_INDEPENDENT_CODE ON)

    # Uncomment for any C deps you pull in (e.g. fastobj, tinyobj):
    # set_target_properties(fastobj PROPERTIES POSITION_INDEPENDENT_CODE ON)

    if(WIN32)
        target_link_libraries(dlp_toolkit_py PRIVATE gdi32 user32)
    endif()

    # Install .so to project root → `import dlp_toolkit_py` works from anywhere in project
    install(TARGETS dlp_toolkit_py
        LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}
    )
endif()
