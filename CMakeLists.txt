cmake_minimum_required(VERSION 3.25.0)
set(CMAKE_C_STANDARD 11)
project(spectrum LANGUAGES C)

set(SOURCES
    core/spectrum.c
)

find_package(Vulkan REQUIRED)

if(WIN32)
    set(PLATFORM
        platforms/win32/win32_spectrum.c
    )
    
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
	
	
	
    add_executable("win32_spectrum" ${PLATFORM})
	add_library(${PROJECT_NAME} SHARED ${SOURCES})

	target_include_directories("win32_spectrum"  PRIVATE ${Vulkan_INCLUDE_DIRS})
	target_include_directories(${PROJECT_NAME}  PRIVATE ${Vulkan_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} kernel32.lib user32.lib)
	target_link_libraries("win32_spectrum" kernel32.lib user32.lib)
	target_link_options(${PROJECT_NAME} PRIVATE "-EXPORT:update_and_render")
	set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/data")
	set_property(TARGET "win32_spectrum" PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/data")
    set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "win32_spectrum")
endif(WIN32)