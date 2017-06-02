file(GLOB_RECURSE EVENT_BUS_SOURCES *.h *.hpp *.c *.cpp)


file(GLOB EVENT_BUS_SOURCES *.h *.hpp *.c *.cpp)



SOURCE_GROUP("event_bus" FILES ${EVENT_BUS_SOURCES})





add_library(event_bus STATIC ${EVENT_BUS_SOURCES})
set_target_properties(event_bus PROPERTIES FOLDER alive)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(event_bus)
