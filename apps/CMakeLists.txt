# ── apps/CMakeLists.txt ───────────────────────────────────────────────────────
# Each .cpp with a main() here becomes its own executable.
# Add a new app: just create apps/my_tool.cpp — nothing else to touch.

file(GLOB APP_SOURCES CONFIGURE_DEPENDS "*.cpp")

foreach(APP_SRC ${APP_SOURCES})
    get_filename_component(APP_NAME ${APP_SRC} NAME_WE)
    add_executable(${APP_NAME} ${APP_SRC})
    target_link_libraries(${APP_NAME} PRIVATE myproject_core)
endforeach()
