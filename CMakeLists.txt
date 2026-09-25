cmake_minimum_required(VERSION 3.20)
project(YinYangChronicle LANGUAGES C CXX)
enable_testing()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release)
endif()

if(MSVC)
  add_compile_options(/W4 /utf-8)
else()
  add_compile_options(-Wall -Wextra)
endif()

# Lua 5.4.7 (vendored by tools/fetch_lua.py, MIT) for event scripts.
file(GLOB LUA_SRC third_party/lua/*.c)
add_library(lua STATIC ${LUA_SRC})
target_include_directories(lua PUBLIC third_party/lua)
if(NOT MSVC)
  target_compile_options(lua PRIVATE -w)
endif()

# Platform-free core: game logic + software framebuffer.
add_library(core STATIC src/game.cpp src/gfx.cpp src/state.cpp src/world.cpp src/ui.cpp src/menu.cpp
                        src/battle.cpp src/scenes.cpp src/script.cpp src/save.cpp src/data.cpp src/maps.cpp)
target_include_directories(core PUBLIC src)
target_link_libraries(core PUBLIC lua)

# Bake data/ (JSON) into C++ headers before compiling: no runtime parser, single executable.
find_package(Python3 REQUIRED COMPONENTS Interpreter)
add_custom_target(bake ALL
  COMMAND ${Python3_EXECUTABLE} tools/bake_data.py
  COMMAND ${Python3_EXECUTABLE} tools/bake_maps.py
  COMMAND ${Python3_EXECUTABLE} tools/bake_scripts.py
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Baking data/*.json, data/maps/*.tmj and data/scripts/*.lua")
add_dependencies(core bake)

# SDL2: vcpkg/installed CMake config on Windows, pkg-config on macOS/Linux.
find_package(SDL2 CONFIG QUIET)
if(TARGET SDL2::SDL2)
  set(SDL_LIBS SDL2::SDL2)
  if(TARGET SDL2::SDL2main)
    list(PREPEND SDL_LIBS SDL2::SDL2main)
  endif()
else()
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2)
  set(SDL_LIBS PkgConfig::SDL2)
endif()

add_executable(YinYangChronicle WIN32 src/main.cpp)
target_link_libraries(YinYangChronicle PRIVATE core ${SDL_LIBS})

add_executable(CoreTest tests/core_test.cpp)
target_link_libraries(CoreTest PRIVATE core)
add_test(NAME CoreTest COMMAND CoreTest)

add_executable(RenderHashTest tests/render_hash_test.cpp)
target_link_libraries(RenderHashTest PRIVATE core)
add_test(NAME RenderHashTest COMMAND RenderHashTest)
