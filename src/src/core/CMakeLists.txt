file(GLOB_RECURSE CORE_SOURCES *.h *.hpp *.c *.cpp)


file(GLOB CORE_SOURCES *.h *.hpp *.c *.cpp)



SOURCE_GROUP("main" FILES ${CORE_SOURCES})





add_library(core STATIC ${CORE_SOURCES})
set_target_properties(core PROPERTIES FOLDER alive)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(core)
