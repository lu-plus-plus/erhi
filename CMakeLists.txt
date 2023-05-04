cmake_minimum_required(VERSION 3.12)

project(erhi)

set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS true)



set(erhi_include_dir		${PROJECT_SOURCE_DIR}/include)
set(erhi_source_dir			${PROJECT_SOURCE_DIR}/source)
set(erhi_packages_dir		${PROJECT_SOURCE_DIR}/packages)
set(erhi_examples_dir		${PROJECT_SOURCE_DIR}/examples)



set(VOLK_STATIC_DEFINES)
if(WIN32)
	set(VK_USE_PLATFORM_WIN32_KHR)
endif()
add_subdirectory(${erhi_packages_dir}/volk EXCLUDE_FROM_ALL)

add_subdirectory(${erhi_packages_dir}/magic_enum EXCLUDE_FROM_ALL)



add_library(erhi_headers_interface INTERFACE)

target_include_directories(erhi_headers_interface
	INTERFACE ${erhi_include_dir}
)

target_link_libraries(erhi_headers_interface
	INTERFACE magic_enum::magic_enum
)



function(add_file_group group_name group_root group_prefix file_expr)
	file(
		GLOB ${group_name}
		LIST_DIRECTORIES false

		${group_root}/*${file_postfix}
		${group_root}/utils/*${file_postfix}
		${group_root}/context/*${file_postfix}
		${group_root}/resource/memory*${file_postfix}
		# ${group_root}/resource/allocator*${file_postfix}
		${group_root}/command/*${file_postfix}
	)
	
	source_group(
		TREE ${group_root}
		PREFIX ${group_prefix}
		FILES ${${group_name}}
	)

	set(${group_name} ${${group_name}} PARENT_SCOPE)
endfunction()

function(add_header_group group_name group_root)
	add_file_group(${group_name} ${group_root} "Header Files" ".hpp")
	set(${group_name} ${${group_name}} PARENT_SCOPE)
endfunction()

function(add_source_group group_name group_root)
	add_file_group(${group_name} ${group_root} "Source Files" ".cpp")
	set(${group_name} ${${group_name}} PARENT_SCOPE)
endfunction()



add_header_group(erhi_vk_headers ${erhi_include_dir}/erhi/vulkan)

add_source_group(erhi_vk_sources ${erhi_source_dir}/vulkan)

add_library(erhi_vulkan STATIC
	${erhi_vk_headers}
	${erhi_vk_sources}
)

target_link_libraries(erhi_vulkan
	PRIVATE erhi_headers_interface
	PUBLIC volk::volk
)



add_header_group(erhi_dx12_headers ${erhi_include_dir}/erhi/dx12)

add_source_group(erhi_dx12_sources ${erhi_source_dir}/dx12)

add_library(erhi_dx12 STATIC
	${erhi_dx12_headers}
	${erhi_dx12_sources}
)

target_link_libraries(erhi_dx12
	PRIVATE erhi_headers_interface
	PRIVATE dxgi
	PRIVATE d3d12
)



add_header_group(erhi_common_headers ${erhi_include_dir}/erhi/common)

add_source_group(erhi_common_sources ${erhi_source_dir}/common)

add_library(erhi SHARED
	${erhi_common_headers}
	${erhi_common_sources}
)

target_link_libraries(erhi
	PRIVATE erhi_headers_interface
	PUBLIC erhi_vulkan
	PUBLIC erhi_dx12
)



add_library(erhi_examples_utils SHARED
	${erhi_examples_dir}/utility/default_message_callback.hpp
	${erhi_examples_dir}/utility/default_message_callback.cpp
)

target_include_directories(erhi_examples_utils
	PRIVATE ${erhi_include_dir}
)

target_link_libraries(erhi_examples_utils
	PRIVATE erhi
)



add_executable(01_hello_erhi
	${erhi_examples_dir}/01_hello_erhi.cpp
)

target_include_directories(01_hello_erhi
	PRIVATE ${erhi_include_dir}
)

target_link_libraries(01_hello_erhi
	PRIVATE erhi
	PRIVATE erhi_examples_utils
)