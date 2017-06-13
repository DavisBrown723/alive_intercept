file(GLOB_RECURSE SYS_PROFILE_SOURCES *.h *.hpp *.c *.cpp)


file(GLOB SYS_PROFILE_SOURCES *.h *.hpp *.c *.cpp)



SOURCE_GROUP("main" FILES ${SYS_PROFILE_SOURCES})





add_library(sys_profile STATIC ${SYS_PROFILE_SOURCES})
set_target_properties(sys_profile PROPERTIES FOLDER alive)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(sys_profile)
