cmake_minimum_required(VERSION 3.20)
project(JoseonYokaiRPG LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optimization flags for 1.44MB floppy budget
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /Oi /GL /Gy /GF /DNDEBUG /W4 /utf-8")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/LTCG /OPT:REF /OPT:ICF /MANIFEST:NO")
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS WIN32_LEAN_AND_MEAN NOMINMAX)
else()
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -flto -s -DNDEBUG -Wall -Wextra")
endif()

# Core sources
set(SOURCES
    src/main.cpp
    src/core/renderer.cpp
    src/core/input.cpp
    src/core/window.cpp
    src/core/task_engine.cpp
    src/core/scene_stack.cpp
    src/core/save_system.cpp
    src/ui/font_renderer.cpp
    src/ui/dialogue_box.cpp
    src/ui/ui_widgets.cpp
    src/world/tilemap.cpp
    src/world/grid_controller.cpp
    src/world/world_scene.cpp
    src/gameplay/yokai.cpp
    src/gameplay/party.cpp
    src/gameplay/artifact.cpp
    src/gameplay/encyclopedia.cpp
    src/gameplay/quest.cpp
    src/battle/status_effects.cpp
    src/battle/battle.cpp
    src/battle/battle_sequencer.cpp
    src/battle/battle_scene.cpp
    src/scenes/encyclopedia_scene.cpp
    src/scenes/party_scene.cpp
    src/scenes/artifact_scene.cpp
    src/scenes/quest_scene.cpp
    src/scenes/title_scene.cpp
    src/scenes/yutnori_scene.cpp
    src/gameplay/alchemy.cpp
    src/world/field_obstacle.cpp
    src/data/data_manager.cpp
    src/audio/audio_engine.cpp
    src/world/weather_system.cpp
    src/battle/skill_fx_system.cpp
)

# Main Executable
add_executable(joseon_yokai_rpg ${SOURCES})

if(WIN32)
    target_link_libraries(joseon_yokai_rpg PRIVATE gdi32 user32 winmm)
endif()

# Logic Unit Tests Executable
set(TEST_SOURCES
    tests/test_main.cpp
    tests/test_battle.cpp
    src/core/renderer.cpp
    src/core/task_engine.cpp
    src/core/save_system.cpp
    src/world/tilemap.cpp
    src/world/field_obstacle.cpp
    src/gameplay/yokai.cpp
    src/gameplay/party.cpp
    src/gameplay/artifact.cpp
    src/gameplay/encyclopedia.cpp
    src/gameplay/quest.cpp
    src/gameplay/alchemy.cpp
    src/battle/status_effects.cpp
    src/battle/battle.cpp
    src/data/data_manager.cpp
    src/audio/audio_engine.cpp
    src/world/weather_system.cpp
    src/battle/skill_fx_system.cpp
)

add_executable(run_tests ${TEST_SOURCES})

if(WIN32)
    target_link_libraries(run_tests PRIVATE gdi32 user32 winmm)
endif()
