CMAKE_MINIMUM_REQUIRED(VERSION 3.1.0)


PROJECT(IzgProjekt2017)

SET(APPLICATION_NAME izgProjekt2017)
SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")


IF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	SET(APPLE TRUE)
ENDIF ()

IF (MSVC)
	SET(USE_PREBUILD_LIB_PACKAGE ON CACHE BOOL "Use prebuild lib package.")
	IF (USE_PREBUILD_LIB_PACKAGE)
		SET(SDL2_DIR ${CMAKE_CURRENT_SOURCE_DIR}/sdl2/win64/cmake CACHE STRING "" FORCE)
		FILE(COPY ${CMAKE_CURRENT_SOURCE_DIR}/sdl2/win64/bin/SDL2.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
		FILE(COPY ${CMAKE_CURRENT_SOURCE_DIR}/sdl2/win64/bin/SDL2d.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
	ENDIF ()
ENDIF ()


FIND_PACKAGE(SDL2 REQUIRED)


SET(GPU_SOURCES
	gpu/gpu.cpp)
SET(GPU_INCLUDES)

SET(STUDENT_SOURCES
	student/student_cpu.c
	student/student_pipeline.c
	student/student_shader.c
	student/linearAlgebra.c
	student/main.c
	student/camera.c
	student/bunny.c
	student/mouseCamera.c
	student/swapBuffers.c
	student/globals.c)
SET(STUDENT_INCLUDES
	student/student_cpu.h
	student/student_pipeline.h
	student/student_shader.h
	student/gpu.h
	student/uniforms.h
	student/buffer.h
	student/vertexPuller.h
	student/program.h
	student/camera.h
	student/bunny.h
	student/mouseCamera.h
	student/swapBuffers.h
	student/globals.h)

SET(TESTS_SOURCES
	tests/conformanceTests.cpp
	tests/performanceTest.cpp)
SET(TESTS_INCLUDES
	tests/conformanceTests.h
	tests/performanceTest.h)

SET(EXAMPLE_SOURCES
	examples/triangleExample.c)
SET(EXAMPLE_HEADERS
	examples/triangleExample.h)

SET(3RDPARTY_SOURCES)
SET(3RDPARTY_INCLUDES
	3rdParty/catch.hpp)


SET(CMAKE_CXX_STANDARD 14)
SET(CMAKE_C_STANDARD 99)

IF (CMAKE_COMPILER_IS_GNUCXX)
	ADD_COMPILE_OPTIONS(-W)
	ADD_COMPILE_OPTIONS(-Wall)
	ADD_COMPILE_OPTIONS(-pedantic)
	ADD_COMPILE_OPTIONS(-Wconversion)
ENDIF ()

IF (CMAKE_COMPILER_IS_GNUCC)
	ADD_COMPILE_OPTIONS(-W)
	ADD_COMPILE_OPTIONS(-Wall)
	ADD_COMPILE_OPTIONS(-pedantic)
	ADD_COMPILE_OPTIONS(-Wconversion)
ENDIF ()


IF (APPLE)
	EXECUTE_PROCESS(COMMAND sdl2-config --libs OUTPUT_VARIABLE SDL_LIBS)
	STRING(STRIP "${SDL_LIBS}" SDL_LIBS)
	EXECUTE_PROCESS(COMMAND sdl2-config --cflags OUTPUT_VARIABLE SDL_CFLAGS)
	ADD_DEFINITIONS(${SDL_CFLAGS})
ELSE ()
	SET(SDL_LIBS SDL2::SDL2 SDL2::SDL2main)
ENDIF ()


ADD_EXECUTABLE(
	${APPLICATION_NAME}
	${GPU_SOURCES} ${GPU_INCLUDES}
	${STUDENT_SOURCES} ${STUDENT_INCLUDES}
	${TESTS_SOURCES} ${TESTS_INCLUDES}
	${3RDPARTY_SOURCES} ${3RDPARTY_INCLUDES}
	${EXAMPLE_SOURCES} ${EXAMPLE_HEADERS})
TARGET_LINK_LIBRARIES(${APPLICATION_NAME} ${SDL_LIBS})
TARGET_INCLUDE_DIRECTORIES(${APPLICATION_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
