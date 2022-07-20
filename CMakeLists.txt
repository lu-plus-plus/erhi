cmake_minimum_required(VERSION 3.12)

project(erhi)

set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_STANDARD 20)



set(ERHI_INCLUDE_DIR		${PROJECT_SOURCE_DIR}/include)
set(ERHI_SOURCE_DIR			${PROJECT_SOURCE_DIR}/source)
set(ERHI_PACKAGES_DIR		${PROJECT_SOURCE_DIR}/packages)
set(ERHI_EXAMPLES_DIR		${PROJECT_SOURCE_DIR}/examples)



find_package(Vulkan REQUIRED)



add_library(erhi SHARED
	${ERHI_INCLUDE_DIR}/erhi/common/object.hpp
	${ERHI_INCLUDE_DIR}/erhi/common/device.hpp
	${ERHI_INCLUDE_DIR}/erhi/common/message.hpp

	${ERHI_SOURCE_DIR}/common/message.cpp

	${ERHI_INCLUDE_DIR}/erhi/vulkan/device.hpp
	${ERHI_INCLUDE_DIR}/erhi/vulkan/message.hpp

	${ERHI_SOURCE_DIR}/vulkan/device.cpp
	${ERHI_SOURCE_DIR}/vulkan/message.cpp
)

target_include_directories(erhi
	PRIVATE ${ERHI_INCLUDE_DIR}
	PRIVATE ${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(erhi
	PRIVATE ${Vulkan_LIBRARIES}
)