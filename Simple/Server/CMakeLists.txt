cmake_minimum_required (VERSION 2.6)

#include path
include_directories("../../../include")

#Setting the name of groups of files
file(GLOB SRC_SERVER
		*.cpp
		*.h)

#Setting the target
add_executable(Server ${SRC_SERVER})
add_dependencies(Server Common)
set_property(TARGET Server PROPERTY OUTPUT_DIRECTORY ../../bin)
set_property(TARGET Server PROPERTY OUTPUT_NAME SimpleServer)

if(MSVC) #Setting about MSVC
	if(${CMAKE_VS_PLATFORM_TOOLSET} STRGREATER_EQUAL "v140")
		set(VS_VERSION_FOLDER_NAME "v140")
	else()
		set(VS_VERSION_FOLDER_NAME ${CMAKE_VS_PLATFORM_TOOLSET})
	endif()
	
	target_link_libraries(Server ../../../lib/${CMAKE_VS_PLATFORM_NAME}/${VS_VERSION_FOLDER_NAME}/${CMAKE_CFG_INTDIR}/ProudNetClient)
	target_link_libraries(Server ../../../lib/${CMAKE_VS_PLATFORM_NAME}/${VS_VERSION_FOLDER_NAME}/${CMAKE_CFG_INTDIR}/ProudNetServer)
	target_link_libraries(Server ../../bin/${CMAKE_CFG_INTDIR}/SimpleCommon)
else(MSVC)#Setting about None-Windows 
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
	target_link_libraries(Server
		${CMAKE_CURRENT_SOURCE_DIR}/../../../lib/x86_x64-linux/Release/libProudNetServer.a
		${CMAKE_CURRENT_SOURCE_DIR}/../../../lib/x86_x64-linux/Release/libProudNetClient.a
#		${CMAKE_CURRENT_SOURCE_DIR}/../../../lib/x86_x64-linux/Release/libProudNetServer.so
#		${CMAKE_CURRENT_SOURCE_DIR}/../../../lib/x86_x64-linux/Release/libProudNetClient.so
		${CMAKE_CURRENT_SOURCE_DIR}/../../bin/libSimpleCommon.a
		ssl
		crypto
		odbc
	)
endif(MSVC)