cmake_minimum_required(VERSION 3.16)
project(MonSynthe VERSION 1.0 LANGUAGES CXX)

# On exige le standard C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ------------------------------------------------------
# 1) Inclure les dossiers d'en-têtes (inclut ImGui/SDL3/PortAudio)
# ------------------------------------------------------
include_directories(
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/libraries/SDL3/include
        ${PROJECT_SOURCE_DIR}/libraries/DearImGui
        ${PROJECT_SOURCE_DIR}/libraries/DearImGui/backends
        ${PROJECT_SOURCE_DIR}/libraries/PortAudio/include
)

# ------------------------------------------------------
# 2) Indiquer où trouver les bibliothèques à linker
# ------------------------------------------------------
link_directories(
        ${PROJECT_SOURCE_DIR}/libraries/SDL3/lib
        ${PROJECT_SOURCE_DIR}/libraries/PortAudio/lib
)

# ------------------------------------------------------
# 3) Rassembler tous les .cpp de src/ dans SOURCES
# ------------------------------------------------------
file(GLOB_RECURSE SOURCES
        ${PROJECT_SOURCE_DIR}/src/*.cpp
)

# ------------------------------------------------------
# 4) Créer l'exécutable "MonSynthe" à partir de ces sources
# ------------------------------------------------------
add_executable(MonSynthe ${SOURCES})

# ------------------------------------------------------
# 5) Ajouter les sources ImGui (compile à la volée)
# ------------------------------------------------------
target_sources(MonSynthe PRIVATE
        ${PROJECT_SOURCE_DIR}/libraries/DearImGui/imgui.cpp
        ${PROJECT_SOURCE_DIR}/libraries/DearImGui/imgui_draw.cpp
        ${PROJECT_SOURCE_DIR}/libraries/DearImGui/imgui_tables.cpp
        ${PROJECT_SOURCE_DIR}/libraries/DearImGui/imgui_widgets.cpp
        ${PROJECT_SOURCE_DIR}/libraries/DearImGui/backends/imgui_impl_sdl.cpp
        ${PROJECT_SOURCE_DIR}/libraries/DearImGui/backends/imgui_impl_opengl3.cpp
)

# ------------------------------------------------------
# 6) Lier ImGui, SDL3, PortAudio, OpenGL
# ------------------------------------------------------
# Trouver OpenGL (nécessaire pour ImGui backend)
find_package(OpenGL REQUIRED)
target_link_libraries(MonSynthe PRIVATE ${OPENGL_gl_LIBRARY})

# Selon l'OS, on link manuellement SDL3 et PortAudio
if (WIN32)
    target_link_libraries(MonSynthe PRIVATE SDL3main SDL3 portaudio)
elseif (UNIX)
    # Sur Linux/Mac, on suppose que libSDL3.a se trouve dans libraries/SDL3/lib,
    # et libportaudio.a dans libraries/PortAudio/lib
    target_link_libraries(MonSynthe PRIVATE SDL3 portaudio pthread dl)
endif()

# ------------------------------------------------------
# 7) Placer l'exécutable dans le dossier bin/
# ------------------------------------------------------
set_target_properties(MonSynthe PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)
