cmake_minimum_required(VERSION 3.13)
project(gfx)

file(GLOB_RECURSE SRC "src/*.cpp" "src/*.c")
file(GLOB_RECURSE LIB "gfx/*.cpp" "gfx/*.c")
file(GLOB_RECURSE VENDOR "vendor/*.cpp" "vendor/*.c")

file(GLOB_RECURSE SRC ${SRC} ${LIB} ${VENDOR})

add_executable(${PROJECT_NAME} ${SRC})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_include_directories(${PROJECT_NAME} PRIVATE "src/" "gfx/" "vendor/" "vendor/glad/" "vendor/flog/")

if (MSVC)
    # OpenGL
    find_package(OpenGL REQUIRED)
    target_link_libraries(gfx PUBLIC OpenGL::GL)

    # GLFW3
    find_package(glfw3 CONFIG REQUIRED)
    target_link_libraries(gfx PUBLIC glfw)

    # FreeType
    find_package(Freetype REQUIRED)
    target_link_libraries(gfx PRIVATE Freetype::Freetype)
else()
    find_package(PkgConfig)
    find_package(OpenGL REQUIRED)

    pkg_check_modules(GLFW3 REQUIRED glfw3)
    pkg_check_modules(FREETYPE2 REQUIRED freetype2)

    target_include_directories(gfx PUBLIC
        ${OPENGL_INCLUDE_DIRS}
        ${GLFW3_INCLUDE_DIRS}
        ${FREETYPE2_INCLUDE_DIRS}
    )
    
    target_link_directories(gfx PUBLIC
        ${OPENGL_LIBRARY_DIRS}
        ${GLFW3_LIBRARY_DIRS}
        ${FREETYPE2_LIBRARY_DIRS}
    )

    target_link_libraries(gfx PUBLIC
        ${OPENGL_LIBRARIES}
        ${GLFW3_LIBRARIES}
        ${FREETYPE2_LIBRARIES}
    )

    if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        target_link_libraries(gfx PUBLIC stdc++fs dl)
    endif ()
endif ()

