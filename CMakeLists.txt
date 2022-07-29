cmake_minimum_required(VERSION 3.12)

project(erhi)

set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS true)



set(ERHI_INCLUDE_DIR		${PROJECT_SOURCE_DIR}/include)
set(ERHI_SOURCE_DIR			${PROJECT_SOURCE_DIR}/source)
set(ERHI_PACKAGES_DIR		${PROJECT_SOURCE_DIR}/packages)
set(ERHI_EXAMPLES_DIR		${PROJECT_SOURCE_DIR}/examples)



add_subdirectory(${ERHI_PACKAGES_DIR}/spdlog EXCLUDE_FROM_ALL)

set(VOLK_STATIC_DEFINES)
if(WIN32)
	set(VK_USE_PLATFORM_WIN32_KHR)
endif()
add_subdirectory(${ERHI_PACKAGES_DIR}/volk EXCLUDE_FROM_ALL)



add_library(erhi SHARED
	${ERHI_INCLUDE_DIR}/erhi/common/handle.hpp
	${ERHI_INCLUDE_DIR}/erhi/common/object.hpp

	${ERHI_INCLUDE_DIR}/erhi/common/message.hpp
	${ERHI_INCLUDE_DIR}/erhi/common/instance.hpp
	${ERHI_INCLUDE_DIR}/erhi/common/physical_device.hpp

	${ERHI_SOURCE_DIR}/common/message.cpp
	${ERHI_SOURCE_DIR}/common/instance.cpp
	${ERHI_SOURCE_DIR}/common/physical_device.cpp

	${ERHI_INCLUDE_DIR}/erhi/vulkan/native.hpp
	
	${ERHI_INCLUDE_DIR}/erhi/vulkan/message.hpp
	${ERHI_INCLUDE_DIR}/erhi/vulkan/instance.hpp
	${ERHI_INCLUDE_DIR}/erhi/vulkan/physical_device.hpp

	${ERHI_SOURCE_DIR}/vulkan/native.cpp
	
	${ERHI_SOURCE_DIR}/vulkan/message.cpp
	${ERHI_SOURCE_DIR}/vulkan/instance.cpp
	${ERHI_SOURCE_DIR}/vulkan/physical_device.cpp
)

target_include_directories(erhi
	PRIVATE ${ERHI_INCLUDE_DIR}
)

target_link_libraries(erhi
	PRIVATE volk::volk
)



add_library(erhi_examples_utils SHARED
	${ERHI_EXAMPLES_DIR}/utility/default_message_callback.hpp
	${ERHI_EXAMPLES_DIR}/utility/default_message_callback.cpp
)

target_include_directories(erhi_examples_utils
	PRIVATE ${ERHI_INCLUDE_DIR}
)

target_link_libraries(erhi_examples_utils
	PRIVATE erhi
	PRIVATE spdlog::spdlog_header_only
)



add_executable(01_hello_erhi
	${ERHI_EXAMPLES_DIR}/01_hello_erhi.cpp
)

target_include_directories(01_hello_erhi
	PRIVATE ${ERHI_INCLUDE_DIR}
)

target_link_libraries(01_hello_erhi
	PRIVATE erhi
	PRIVATE erhi_examples_utils
)