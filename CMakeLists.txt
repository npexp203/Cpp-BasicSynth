cmake_minimum_required(VERSION 3.30)
project(synth)

set(CMAKE_CXX_STANDARD 23)

include_directories(
        "libraries/portaudio/include"
        "libraries/imgui"
        "libraries/sdl/include"
        "libraries/imgui/backends"
        "include"
)

file(GLOB_RECURSE SOURCES
        "src/*.cpp"
        "src/audio/*.cpp"
        "src/ui/*.cpp"
)

set(IMGUI_SOURCES
        ./libraries/imgui/imgui.cpp
        ./libraries/imgui/imgui_demo.cpp
        ./libraries/imgui/imgui_draw.cpp
        ./libraries/imgui/imgui_widgets.cpp
        ./libraries/imgui/imgui_tables.cpp
        ./libraries/imgui/backends/imgui_impl_sdl3.cpp
        ./libraries/imgui/backends/imgui_impl_sdlrenderer3.cpp
)

add_executable(synth
        main.cpp
        ${IMGUI_SOURCES}
        ${SOURCES}
)

if (APPLE)
    set(CMAKE_INSTALL_RPATH "${CMAKE_SOURCE_DIR}/../libraries/sdl/lib/macos/SDL3.framework")
    target_link_libraries(synth PRIVATE
            "-framework AudioToolbox"
            "-framework CoreAudio"
            "-framework Carbon"
            "${CMAKE_SOURCE_DIR}/libraries/portaudio/lib/macos/libportaudio.a"
            "${CMAKE_SOURCE_DIR}/libraries/sdl/lib/macos/SDL3.framework"
            "-framework CoreFoundation"
    )

elseif (WIN32)
    target_link_libraries(synth PRIVATE
            "${CMAKE_SOURCE_DIR}/libraries/portaudio/lib/win32/portaudio_x64.dll"
            "${CMAKE_SOURCE_DIR}/libraries/sdl/lib/win32/sdl3.dll"
    )

    add_custom_command(TARGET synth POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/libraries/portaudio/lib/win32/portaudio_x64.dll"
            "${CMAKE_CURRENT_SOURCE_DIR}/libraries/sdl/lib/win32/sdl3.dll"
            "$<TARGET_FILE_DIR:synth>"
    )

elseif (UNIX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(ALSA alsa)

    target_link_libraries(synth PRIVATE
            "-ljack"
            "-ldl"
            "${CMAKE_SOURCE_DIR}/libraries/portaudio/lib/linux-x86_64/libportaudio.a"
            "${CMAKE_SOURCE_DIR}/libraries/sdl/lib/linux-x86_64/libSDL3.a"
            ${ALSA_LIBRARIES}
    )
endif ()


