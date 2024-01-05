cmake_minimum_required(VERSION 3.26)

project(erhi)

set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_STANDARD 23)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS true)



set(erhi_include_dir		${PROJECT_SOURCE_DIR}/include)
set(erhi_source_dir			${PROJECT_SOURCE_DIR}/source)
set(erhi_package_dir		${PROJECT_SOURCE_DIR}/packages)
set(erhi_example_dir		${PROJECT_SOURCE_DIR}/examples)



add_library(erhi_abstract_interface INTERFACE)

target_include_directories(erhi_abstract_interface
	INTERFACE ${erhi_include_dir}
)



function(add_file_group group_name group_root group_prefix file_expr)
	file(
		GLOB ${group_name}
		LIST_DIRECTORIES false

		# root directory
		${group_root}/*${file_postfix}

		# modules
		${group_root}/context/*${file_postfix}
		${group_root}/resource/*${file_postfix}
		${group_root}/command/*${file_postfix}

		# miscellaneous
		${group_root}/utility/*${file_postfix}
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



# create Vulkan backend

add_header_group(erhi_vk_headers ${erhi_include_dir}/erhi/vulkan)
add_source_group(erhi_vk_sources ${erhi_source_dir}/vulkan)

add_library(erhi_vulkan STATIC
	${erhi_vk_headers}
	${erhi_vk_sources}
)

# import volk

set(VOLK_STATIC_DEFINES)
if(WIN32)
	set(VK_USE_PLATFORM_WIN32_KHR)
endif()
add_subdirectory(${erhi_package_dir}/volk EXCLUDE_FROM_ALL)

# import VulkanMemoryAllocator

add_subdirectory(${erhi_package_dir}/VulkanMemoryAllocator EXCLUDE_FROM_ALL)

# set up dependencies

target_link_libraries(erhi_vulkan
	PRIVATE erhi_abstract_interface
	PRIVATE volk
	PRIVATE VulkanMemoryAllocator
)



# create D3D12 backend

add_header_group(erhi_dx12_headers ${erhi_include_dir}/erhi/dx12)
add_source_group(erhi_dx12_sources ${erhi_source_dir}/dx12)

add_library(erhi_dx12 STATIC
	${erhi_dx12_headers}
	${erhi_dx12_sources}
)

# import D3D12MemoryAllocator

add_subdirectory(${erhi_package_dir}/D3D12MemoryAllocator EXCLUDE_FROM_ALL)

# set up dependencies

target_link_libraries(erhi_dx12
	PRIVATE erhi_abstract_interface
	PRIVATE dxgi
	PRIVATE d3d12
	PRIVATE D3D12MemoryAllocator
)



# create main library

add_header_group(erhi_common_headers ${erhi_include_dir}/erhi/common)
add_source_group(erhi_common_sources ${erhi_source_dir}/common)

add_library(erhi SHARED
	${erhi_common_headers}
	${erhi_common_sources}
)

target_link_libraries(erhi
	PRIVATE erhi_abstract_interface
	PUBLIC erhi_vulkan
	PUBLIC erhi_dx12
)



add_executable(01_hello_erhi
	${erhi_example_dir}/01_hello_erhi.cpp
)

target_include_directories(01_hello_erhi
	PRIVATE ${erhi_include_dir}
)

target_link_libraries(01_hello_erhi
	PRIVATE erhi
)