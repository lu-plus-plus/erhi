cmake_minimum_required(VERSION 3.12)

project(erhi)

set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS true)



set(ERHI_INCLUDE_DIR		${PROJECT_SOURCE_DIR}/include)
set(ERHI_SOURCE_DIR			${PROJECT_SOURCE_DIR}/source)
set(ERHI_PACKAGES_DIR		${PROJECT_SOURCE_DIR}/packages)
set(ERHI_EXAMPLES_DIR		${PROJECT_SOURCE_DIR}/examples)



set(VOLK_STATIC_DEFINES)
if(WIN32)
	set(VK_USE_PLATFORM_WIN32_KHR)
endif()
add_subdirectory(${ERHI_PACKAGES_DIR}/volk EXCLUDE_FROM_ALL)



add_library(erhi_headers INTERFACE)

target_include_directories(erhi_headers
	INTERFACE ${ERHI_INCLUDE_DIR}
)



add_library(erhi_vulkan STATIC
	${ERHI_INCLUDE_DIR}/erhi/vulkan/native.hpp	
	${ERHI_SOURCE_DIR}/vulkan/native.cpp

	${ERHI_INCLUDE_DIR}/erhi/vulkan/message.hpp
	${ERHI_INCLUDE_DIR}/erhi/vulkan/instance.hpp
	${ERHI_INCLUDE_DIR}/erhi/vulkan/physical_device.hpp
	${ERHI_INCLUDE_DIR}/erhi/vulkan/device.hpp
	${ERHI_INCLUDE_DIR}/erhi/vulkan/queue.hpp

	${ERHI_SOURCE_DIR}/vulkan/message.cpp
	${ERHI_SOURCE_DIR}/vulkan/instance.cpp
	${ERHI_SOURCE_DIR}/vulkan/physical_device.cpp
	${ERHI_SOURCE_DIR}/vulkan/device.cpp
	${ERHI_SOURCE_DIR}/vulkan/queue.cpp

	# ${ERHI_INCLUDE_DIR}/erhi/vulkan/resource/resource.hpp
	# ${ERHI_INCLUDE_DIR}/erhi/vulkan/resource/memory.hpp

	# ${ERHI_SOURCE_DIR}/vulkan/resource/memory.cpp
)

target_link_libraries(erhi_vulkan
	PRIVATE erhi_headers
	PUBLIC volk::volk
)



add_library(erhi_dx12 STATIC
	${ERHI_INCLUDE_DIR}/erhi/dx12/native.hpp
	${ERHI_SOURCE_DIR}/dx12/native.cpp

	${ERHI_INCLUDE_DIR}/erhi/dx12/message.hpp
	${ERHI_INCLUDE_DIR}/erhi/dx12/instance.hpp
	${ERHI_INCLUDE_DIR}/erhi/dx12/physical_device.hpp
	${ERHI_INCLUDE_DIR}/erhi/dx12/device.hpp
	${ERHI_INCLUDE_DIR}/erhi/dx12/queue.hpp

	${ERHI_SOURCE_DIR}/dx12/message.cpp
	${ERHI_SOURCE_DIR}/dx12/instance.cpp
	${ERHI_SOURCE_DIR}/dx12/physical_device.cpp
	${ERHI_SOURCE_DIR}/dx12/device.cpp
	${ERHI_SOURCE_DIR}/dx12/queue.cpp

	# ${ERHI_INCLUDE_DIR}/erhi/dx12/resource/resource.hpp
	# ${ERHI_INCLUDE_DIR}/erhi/dx12/resource/memory.hpp

	# ${ERHI_SOURCE_DIR}/dx12/resource/memory.cpp
)

target_link_libraries(erhi_dx12
	PRIVATE erhi_headers
	PRIVATE dxgi
	PRIVATE d3d12
)



add_library(erhi SHARED
	${ERHI_INCLUDE_DIR}/erhi/common/handle.hpp
	${ERHI_INCLUDE_DIR}/erhi/common/object.hpp

	${ERHI_INCLUDE_DIR}/erhi/common/common.hpp

	${ERHI_INCLUDE_DIR}/erhi/common/context.hpp
	${ERHI_INCLUDE_DIR}/erhi/common/message.hpp
	${ERHI_INCLUDE_DIR}/erhi/common/instance.hpp
	${ERHI_INCLUDE_DIR}/erhi/common/physical_device.hpp
	${ERHI_INCLUDE_DIR}/erhi/common/device.hpp
	${ERHI_INCLUDE_DIR}/erhi/common/queue.hpp

	${ERHI_SOURCE_DIR}/common/message.cpp
	${ERHI_SOURCE_DIR}/common/instance.cpp
	${ERHI_SOURCE_DIR}/common/physical_device.cpp
	${ERHI_SOURCE_DIR}/common/device.cpp
	${ERHI_SOURCE_DIR}/common/queue.cpp

	# ${ERHI_INCLUDE_DIR}/erhi/common/resource/resource.hpp
	# ${ERHI_INCLUDE_DIR}/erhi/common/resource/memory.hpp

	# ${ERHI_SOURCE_DIR}/common/resource/memory.cpp
)

target_link_libraries(erhi
	PRIVATE erhi_headers
	PUBLIC erhi_vulkan
	PUBLIC erhi_dx12
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