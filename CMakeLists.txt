cmake_minimum_required(VERSION 3.20)

project(gk-opengl LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(EXT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external)

set(IMGUI_DIR ${EXT_DIR}/imgui)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backend/imgui_impl_opengl3.cpp
    ${IMGUI_DIR}/backend/imgui_impl_glfw.cpp
)

set(APP_SOURCES
    ${SRC_DIR}/main.cpp
)

add_executable(${PROJECT_NAME}
    ${APP_SOURCES}
    ${IMGUI_SOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${SRC_DIR}/include
    ${IMGUI_DIR}
    ${EXT_DIR}/glfw/include
    ${EXT_DIR}/glew/include
    ${EXT_DIR}
)

target_link_directories(${PROJECT_NAME} PRIVATE
    ${EXT_DIR}/glew/lib/Release/x64
    ${EXT_DIR}/glfw/lib-vc2022
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    GLEW_STATIC
    IMGUI_IMPL_OPENGL_LOADER_GLEW
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    OpenGL::GL
    glfw3_mt
    glew32s
    # user32
    # gdi32
    # shell32
    # advapi32
    # ole32
    # uuid
    # winmm
    # ws2_32
    # bcrypt
)

# Copy shaders to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${SRC_DIR}/shaders/"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders"
)
