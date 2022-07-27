cmake_minimum_required(VERSION 3.0)


project(ClassroomModel)

configure_file(config/rootDirectory.h.in config/rootDirectory.h)

if(MSVC AND NOT "${MSVC_VERSION}" LESS 1400)
	add_definitions( "/MP" )
endif()



if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
set(OPENGL_LIBRARY
	${OPENGL_LIBRARY}
	-lGL -lGLU -lXrandr -lXext -lX11 -lrt
	${CMAKE_DL_LIBS}
	${GLFW_LIBRARIES}
)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
set(OPENGL_LIBRARY
	${OPENGL_LIBRARY}
	${CMAKE_DL_LIBS}
	${GLFW_LIBRARIES}
)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

### GLEW ###

set(GLEW_SOURCE
	external_libraries/glew-1.13.0/src/glew.c
)

set(GLEW_HEADERS
)


add_library( GLEW_1130 STATIC
	${GLEW_SOURCE}
	${GLEW_INCLUDE}
)

target_link_libraries(GLEW_1130
	${OPENGL_LIBRARY}
	${EXTRA_LIBS}
)

### GLEW ###

set(GLAD_SOURCE
	external_libraries/glad/src/glad.c
)

set(GLAD_HEADERS
)


add_library( GLAD_LIB STATIC
	${GLAD_SOURCE}
	${GLAD_INCLUDE}
)

target_link_libraries(GLAD_LIB
	${OPENGL_LIBRARY}
	${EXTRA_LIBS}
)

set(OpenGL_GL_PREFERENCE GLVND)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

### stb_image ###
add_library(STB_IMAGE "external_libraries/stb_image/src/stb_image.c")
SET_TARGET_PROPERTIES(STB_IMAGE PROPERTIES LINKER_LANGUAGE C)


find_package(OpenGL REQUIRED)

### ASSIMP ###
# AssImp already has a CMakeLists.txt so let's use these

# Compile built-in, modified version of Zlib
include(CheckIncludeFile)
include(CheckTypeSize)
include(CheckFunctionExists)
add_subdirectory( external_libraries/assimp-3.0.1270/contrib/zlib )

# Compile without Boost
include_directories( external_libraries/assimp-3.0.1270/code/BoostWorkaround )
add_definitions( -DASSIMP_BUILD_BOOST_WORKAROUND )

# Compile AssImp
set( LIB_INSTALL_DIR "lib")
set(LIBASSIMP_COMPONENT libassimp3.0-r1270-OGLtuts)
set(ZLIB_LIBRARIES zlib)
set(BUILD_STATIC_LIB ON)
#set(ZLIB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/zlib)
add_subdirectory( external_libraries/assimp-3.0.1270/code )

add_executable(
	main src/main.cpp
	src/mesh.h
	src/model.hpp

)
add_subdirectory(external_libraries/glfw)
add_subdirectory(external_libraries/glm)
include_directories( 
	${OPENGL_INCLUDE_DIRS}
	external_libraries/glew-1.13.0/include
	external_libraries/glad/include
	inc
	external_libraries/stb_image/include
	external_libraries/glm
	external_libraries/assimp-3.0.1270/include/
	${CMAKE_BINARY_DIR}/config
)



target_include_directories(main
    PUBLIC external_libraries/glfw/include
	PUBLIC external_libraries/glew-1.13.0/include
	PUBLIC external_libraries/glad/include
	PUBLIC external_libraries/glm
	PUBLIC inc
	PUBLIC external_libraries/stb_image/include
	
)
target_link_directories(main
    PRIVATE external_libraries/glfw/src
	PRIVATE external_libraries/glew-1.13.0/src
	PRIVATE external_libraries/glad/src
	PRIVATE external_libraries/stb_image/src
)
target_link_libraries(main glfw ${OPENGL_LIBRARY}  GLEW_1130 GLAD_LIB STB_IMAGE assimp)

