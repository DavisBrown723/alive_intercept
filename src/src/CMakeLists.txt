cmake_minimum_required (VERSION 3.1)



if(CMAKE_COMPILER_IS_GNUCXX)
	SET(CMAKE_CXX_FLAGS "-std=c++latest -march=i686 -m32 -O2 -s -fPIC -fpermissive")
	set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
	set(CMAKE_SHARED_LINKER_FLAGS "-static-libgcc -static-libstdc++")
else()
	#set(CMAKE_CXX_FLAGS_DEBUG "/D _DEBUG /MDd /Zi /Ob0 /Od /RTC1 /EHsc")
	#set(CMAKE_CXX_FLAGS_MINSIZEREL     "/MT /O1 /Ob1 /D /EHsc")
	#set(CMAKE_CXX_FLAGS_RELEASE       "/MT /O2 /Ob2 /D /EHsc")
	#set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MT /Zi /O2 /Ob1 /D /EHsc")
endif()

# includes

include_directories(${PROJECT_SOURCE_DIR}/ext/intercept/src/client/headers)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# build components

add_subdirectory(common)
add_subdirectory(event_bus)
add_subdirectory(core)
add_subdirectory(sys_profile)
add_subdirectory(alive)