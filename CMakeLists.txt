cmake_minimum_required(VERSION 3.20)

project(D3D12_learning LANGUAGES CXX)

if(NOT WIN32)
    message(FATAL_ERROR "D3D12_learning requires Windows and the Direct3D 12 SDK.")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Keep build artifacts outside the source lists. New .cpp files placed in one
# of these tutorial-style source directories are discovered on CMake reconfigure.
file(GLOB_RECURSE D3D12_LEARNING_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/D3D/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Debug/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Support/*.cpp"
)

# Shader files follow the layout used by the tutorial repository.  Their
# compiled blobs are emitted to <build>/Shaders and copied beside the .exe.
file(GLOB_RECURSE D3D12_LEARNING_SHADER_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Shaders/*.hlsl"
)
file(GLOB_RECURSE D3D12_LEARNING_SHADER_INCLUDES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Shaders/*.hlsli"
)

add_executable(D3D12_learning
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
    ${D3D12_LEARNING_SOURCES}
)

# Keep shader sources visible in Visual Studio without treating them as C++.
if(D3D12_LEARNING_SHADER_SOURCES OR D3D12_LEARNING_SHADER_INCLUDES)
    set_source_files_properties(
        ${D3D12_LEARNING_SHADER_SOURCES}
        ${D3D12_LEARNING_SHADER_INCLUDES}
        PROPERTIES HEADER_FILE_ONLY TRUE
    )
    target_sources(D3D12_learning PRIVATE
        ${D3D12_LEARNING_SHADER_SOURCES}
        ${D3D12_LEARNING_SHADER_INCLUDES}
    )
endif()

target_include_directories(D3D12_learning PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_compile_definitions(D3D12_learning PRIVATE
    UNICODE
    _UNICODE
    NOMINMAX
)

if(MSVC)
    # Source files are saved as UTF-8, including Chinese comments.
    target_compile_options(D3D12_learning PRIVATE /utf-8)
endif()

target_link_libraries(D3D12_learning PRIVATE
    d3d12
    dxgi
    dxguid
)

if(D3D12_LEARNING_SHADER_SOURCES)
    find_program(DXC_EXECUTABLE NAMES dxc dxc.exe REQUIRED)

    set(D3D12_LEARNING_SHADER_OUTPUTS)
    foreach(SHADER_SOURCE IN LISTS D3D12_LEARNING_SHADER_SOURCES)
        get_filename_component(SHADER_NAME "${SHADER_SOURCE}" NAME_WE)
        string(TOLOWER "${SHADER_NAME}" SHADER_NAME_LOWER)

        if(SHADER_NAME_LOWER STREQUAL "vertexshader")
            set(SHADER_ENTRY main)
            set(SHADER_PROFILE vs_6_0)
        elseif(SHADER_NAME_LOWER STREQUAL "pixelshader")
            set(SHADER_ENTRY main)
            set(SHADER_PROFILE ps_6_0)
        elseif(SHADER_NAME_LOWER STREQUAL "rootsignature")
            set(SHADER_ENTRY ROOTSIG)
            set(SHADER_PROFILE rootsig_1_1)
        else()
            message(FATAL_ERROR
                "Unknown shader '${SHADER_NAME}'. Add its entry point and target profile to CMakeLists.txt.")
        endif()

        set(SHADER_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/Shaders/${SHADER_NAME}.cso")
        add_custom_command(
            OUTPUT "${SHADER_OUTPUT}"
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/Shaders"
            COMMAND "${DXC_EXECUTABLE}" -nologo -E "${SHADER_ENTRY}" -T "${SHADER_PROFILE}"
                    -Fo "${SHADER_OUTPUT}" "${SHADER_SOURCE}"
            DEPENDS "${SHADER_SOURCE}" ${D3D12_LEARNING_SHADER_INCLUDES}
            COMMENT "Compiling ${SHADER_NAME}.hlsl"
            VERBATIM
        )
        list(APPEND D3D12_LEARNING_SHADER_OUTPUTS "${SHADER_OUTPUT}")
    endforeach()

    add_custom_target(D3D12_learning_shaders ALL DEPENDS ${D3D12_LEARNING_SHADER_OUTPUTS})
    add_dependencies(D3D12_learning D3D12_learning_shaders)
    add_custom_command(TARGET D3D12_learning POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_directory
                "${CMAKE_CURRENT_BINARY_DIR}/Shaders" "$<TARGET_FILE_DIR:D3D12_learning>/Shaders"
        COMMENT "Copying compiled shaders beside D3D12_learning.exe"
    )
endif()

# Reflect the tutorial's D3D / Debug / Support layout in Visual Studio.
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
    ${D3D12_LEARNING_SOURCES}
    ${D3D12_LEARNING_SHADER_SOURCES}
    ${D3D12_LEARNING_SHADER_INCLUDES}
)
