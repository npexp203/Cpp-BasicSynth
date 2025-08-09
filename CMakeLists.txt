cmake_minimum_required(VERSION 3.30)
project(63549)

set(CMAKE_CXX_STANDARD 23)

include_directories(
        "libraries/portaudio/include"
        "libraries/imgui"
        "libraries/sdl/include"
        "libraries/imgui/backends"
)
file(GLOB SOURCES "src/*.cpp")

add_executable(63549 main.cpp
        ./libraries/imgui/imgui.cpp
        ./libraries/imgui/imgui_demo.cpp
        ./libraries/imgui/imgui_draw.cpp
        ./libraries/imgui/imgui_widgets.cpp
        ./libraries/imgui/imgui_tables.cpp
        ./libraries/imgui/backends/imgui_impl_sdl3.cpp
        ./libraries/imgui/backends/imgui_impl_sdlrenderer3.cpp
        ./src/Filter.cpp
        ${SOURCES}
        src/Oscillator.cpp
        include/Oscillator.h
        src/Synth.cpp
        include/Synth.h
        src/Envelope.cpp
        include/Envelope.h
        src/AudioEngine.cpp
        include/AudioEngine.h
        include/SynthetizerConfig.h

)

if (APPLE)
    set(CMAKE_INSTALL_RPATH "${CMAKE_SOURCE_DIR}/../libraries/sdl/lib/macos/SDL3.framework")
    target_link_libraries(63549 PRIVATE
            "-framework AudioToolbox"
            "-framework CoreAudio"
            "-framework Carbon"
            "${CMAKE_SOURCE_DIR}/libraries/portaudio/lib/macos/libportaudio.a"
            "${CMAKE_SOURCE_DIR}/libraries/sdl/lib/macos/SDL3.framework"
            "-framework CoreFoundation"
    )

elseif (WIN32)
    target_link_libraries(63549 PRIVATE
            "${CMAKE_SOURCE_DIR}/libraries/portaudio/lib/win32/portaudio_x64.dll"
            "${CMAKE_SOURCE_DIR}/libraries/sdl/lib/win32/sdl3.dll"

    )

    add_custom_command(TARGET 63549 POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/libraries/portaudio/lib/win32/portaudio_x64.dll"
            "${CMAKE_CURRENT_SOURCE_DIR}/libraries/sdl/lib/win32/sdl3.dll"
            "$<TARGET_FILE_DIR:63549>"
    )

elseif (UNIX)
    target_link_libraries(63549 PRIVATE
            "-ljack"
            "-ldl"
            "${CMAKE_SOURCE_DIR}/libraries/portaudio/lib/linux-x86_64/libportaudio.a"
            "${CMAKE_SOURCE_DIR}/libraries/sdl/lib/linux-x86_64/libSDL3.a"
    )
endif ()
