cmake_minimum_required (VERSION 3.0)

set(ALIVE_EXTENSION_NAME "alive")

file(GLOB_RECURSE ALIVE_SOURCES *.h *.hpp *.c *.cpp)

set(ALIVE_NAME "alive")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}../../../build/win32")

file(GLOB ALIVE_SOURCES *.h *.hpp *.c *.cpp)

SOURCE_GROUP("main" FILES ${ALIVE_SOURCES})

add_library( ${ALIVE_EXTENSION_NAME} SHARED ${ALIVE_SOURCES})
add_library( ${ALIVE_EXTENSION_NAME}_static ${ALIVE_SOURCES})

target_link_libraries(${ALIVE_EXTENSION_NAME} common event_bus sys_profile core intercept_client ${LINK_LIBS})
set_target_properties(${ALIVE_EXTENSION_NAME} PROPERTIES PREFIX "")
set_target_properties(${ALIVE_EXTENSION_NAME} PROPERTIES FOLDER alive)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if(CMAKE_COMPILER_IS_GNUCXX)
	set_target_properties(${ALIVE_EXTENSION_NAME} PROPERTIES LINK_SEARCH_START_STATIC 1)
	set_target_properties(${ALIVE_EXTENSION_NAME} PROPERTIES LINK_SEARCH_END_STATIC 1)
    set_target_properties(${ALIVE_EXTENSION_NAME}_static PROPERTIES LINK_SEARCH_START_STATIC 1)
	set_target_properties(${ALIVE_EXTENSION_NAME}_static PROPERTIES LINK_SEARCH_END_STATIC 1)
endif()