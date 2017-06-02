file(GLOB_RECURSE COMMON_SOURCES *.h *.hpp *.c *.cpp)


file(GLOB COMMON_SOURCES *.h *.hpp *.c *.cpp)



SOURCE_GROUP("common" FILES ${COMMON_SOURCES})





add_library(common STATIC ${COMMON_SOURCES})
set_target_properties(common PROPERTIES FOLDER alive)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(common)
