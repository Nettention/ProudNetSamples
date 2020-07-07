cmake_minimum_required (VERSION 2.6)

#include path
include_directories("../../../include")

#Setting the name of groups of files
file(GLOB SRC_CLIENT
		*.cpp
		*.h)

#Setting the target
add_executable(Client ${SRC_CLIENT})
add_dependencies(Client Common)
set_property(TARGET Client PROPERTY OUTPUT_DIRECTORY ../../bin)
set_property(TARGET Client PROPERTY OUTPUT_NAME SimpleClient)

#Setting about MSVC
if(MSVC)
	if(${CMAKE_VS_PLATFORM_TOOLSET} STRGREATER_EQUAL "v140")
		set(VS_VERSION_FOLDER_NAME "v140")
	else()
		set(VS_VERSION_FOLDER_NAME ${CMAKE_VS_PLATFORM_TOOLSET})
	endif()
	
	target_link_libraries(Client ../../../lib/${CMAKE_VS_PLATFORM_NAME}/${VS_VERSION_FOLDER_NAME}/${CMAKE_CFG_INTDIR}/ProudNetClient)
	target_link_libraries(Client ../../bin/${CMAKE_CFG_INTDIR}/SimpleCommon)
else(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
	target_link_libraries(Client
		${CMAKE_CURRENT_SOURCE_DIR}/../../../lib/x86_x64-linux/Release/libProudNetClient.a
#		${CMAKE_CURRENT_SOURCE_DIR}/../../../lib/x86_x64-linux/Release/libProudNetClient.so
		${CMAKE_CURRENT_SOURCE_DIR}/../../bin/libSimpleCommon.a
	)
endif(MSVC)