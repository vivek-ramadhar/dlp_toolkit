# ── src/CMakeLists.txt ────────────────────────────────────────────────────────
# Defines 'myproject_core' — the static lib everything links against.
# Drop .cpp files anywhere here; CONFIGURE_DEPENDS re-globs on rebuild.

file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS "*.cpp" "*.h" "*.hpp")

option(NATIVE_OPTIMIZATIONS "Enable -O3 -march-native" OFF)
if (NATIVE_OPTIMIZATIONS)
	target_compile_options(myproject_core PRIVATE -O3 -march=native)
endif()

add_library(myproject_core STATIC ${CORE_SOURCES})

target_include_directories(myproject_core
    PUBLIC  ${CMAKE_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_options(myproject_core PRIVATE -Wall -Wextra)

if(ENABLE_TRACY)
    target_link_libraries(myproject_core PUBLIC TracyClient)
    target_compile_definitions(myproject_core PUBLIC TRACY_ENABLE)
endif()
