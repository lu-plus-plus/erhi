#include "erhi/dx12/context/physical_device.hpp"
#include "erhi/dx12/context/device.hpp"

#include "erhi/dx12/resource/memory.hpp"

#include <bit>			// for memory type bits manipulation
#include <utility>		// for std::pair used in erhi/native mapping



namespace erhi::dx12 {

	Memory::Memory(DeviceHandle deviceHandle, D3D12_HEAP_DESC const & heapDesc) :
		IMemory(heapDesc.SizeInBytes),
		mDeviceHandle(std::move(deviceHandle)),
		mpHeap(nullptr) {
		
		D3D12CheckResult(mDeviceHandle->mpDevice->CreateHeap(&heapDesc, IID_PPV_ARGS(&mpHeap)));
	}

	Memory::~Memory() {
		mpHeap->Release();
	}

	IDeviceHandle Memory::GetDevice() const {
		return mDeviceHandle;
	}



	//Memory::Memory(Device * pDevice, uint32_t size, MemoryLocation location, MemoryHostAccess hostAccess) :
	//	IMemory(pDevice, size, location, hostAccess),
	//	mpHeap(nullptr) {

	//	D3D12_MEMORY_POOL memoryPool;
	//	switch (location) {
	//		case erhi::MemoryLocation::L0_System:
	//			memoryPool = D3D12_MEMORY_POOL_L0;
	//		break;

	//		case erhi::MemoryLocation::L1_Video:
	//			memoryPool = D3D12_MEMORY_POOL_L1;
	//		break;
	//		
	//		default:
	//			memoryPool = D3D12_MEMORY_POOL_UNKNOWN;
	//		break;
	//	}

	//	D3D12_CPU_PAGE_PROPERTY pageProperty;
	//	switch (hostAccess)
	//	{
	//		case erhi::MemoryHostAccess::NotAvailable:
	//			pageProperty = D3D12_CPU_PAGE_PROPERTY_NOT_AVAILABLE;
	//		break;
	//		
	//		case erhi::MemoryHostAccess::SequentialWrite:
	//			pageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE;
	//		break;

	//		case erhi::MemoryHostAccess::Random:
	//			pageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	//		break;
	//		
	//		default:
	//			pageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	//		break;
	//	}

	//	D3D12_HEAP_DESC heapDesc{
	//		.SizeInBytes = size,
	//		.Properties = D3D12_HEAP_PROPERTIES{
	//			.Type = D3D12_HEAP_TYPE_CUSTOM,
	//			.CPUPageProperty = pageProperty,
	//			.MemoryPoolPreference = memoryPool,
	//			.CreationNodeMask = 0u,
	//			.VisibleNodeMask = 0u
	//		},
	//		.Alignment = 0u, 
	//	};

	//	pDevice->mpDevice->CreateHeap()
	//}

	

	D3D12_HEAP_TYPE MapHeapType(MemoryHeapType heapType) {
		switch (heapType) {
			case erhi::MemoryHeapType::Default:
				return D3D12_HEAP_TYPE_DEFAULT;
				break;
			case erhi::MemoryHeapType::Upload:
				return D3D12_HEAP_TYPE_UPLOAD;
				break;
			case erhi::MemoryHeapType::ReadBack:
			default:
				return D3D12_HEAP_TYPE_READBACK;
				break;
		}
	}



	/*
		In DX12, resources are divided into three categories: buffer, textures used as render target or depth stencil, and other textures.
		On devices with D3D12_RESOURCE_HEAP_TIER_1, resources of different categories must be created upon different heaps.
		On devices with D3D12_RESOURCE_HEAP_TIER_2, all kinds of resources may be mixed on a single heap.
	*/

	enum class MemoryNativeCategory : uint32_t {
		General = 0,
		Buffer = 1,
		RTDSTexture = 2,
		NonRTDSTexture = 3,
		MaxEnum = 4
	};

	D3D12_HEAP_FLAGS MapCategoryToD3D12HeapFlags(Device * pDevice, MemoryNativeCategory category) {
		switch (category) {
			case MemoryNativeCategory::Buffer: {
				return D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS;
			} break;

			case MemoryNativeCategory::RTDSTexture: {
				return D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES;
			} break;

			case MemoryNativeCategory::NonRTDSTexture: {
				return D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES;
			} break;

			case MemoryNativeCategory::General:
			default: {
				return D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES;
			}
		}
	}



	uint32_t ToMemoryTypeIndex(Device * pDevice, MemoryHeapType heapType) {
		uint32_t category = 0u;
		
		if (pDevice->mPhysicalDeviceHandle->mFeatureD3D12Options.ResourceHeapTier == D3D12_RESOURCE_HEAP_TIER_1) {
			category = uint32_t(MemoryNativeCategory::Buffer);
		}
		else {
			category = uint32_t(MemoryNativeCategory::General);
		}

		return uint32_t(heapType) * uint32_t(MemoryNativeCategory::MaxEnum) + category;
	}

	std::pair<MemoryHeapType, MemoryNativeCategory> FromMemoryTypeIndex(Device * pDevice, uint32_t memoryTypeIndex) {
		return std::make_pair(
			MemoryHeapType(memoryTypeIndex / uint32_t(MemoryHeapType::MaxEnum)),
			MemoryNativeCategory(memoryTypeIndex % uint32_t(MemoryHeapType::MaxEnum))
		);
	}



	static D3D12_RESOURCE_FLAGS MapBufferUsage(BufferUsageFlags bufferUsage) {
		D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;

		if (bufferUsage & BufferUsageBits::StorageBuffer) flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		return flags;
	}



	MemoryRequirements Device::GetBufferMemoryRequirements(MemoryHeapType heapType, BufferDesc const & bufferDesc) {
		uint32_t const memoryTypeIndex = ToMemoryTypeIndex(this, heapType);
		
		/*
			<todo>
			Per D3D12 document,
			"However, applications can use the most amount of capability support without concern about the efficiency impact on buffers."
			A test is needed to verify this statement.
			</todo>
		*/

		D3D12_RESOURCE_DESC const resourceDesc{
			.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
			.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
			.Width = bufferDesc.size,
			.Height = 1,
			.DepthOrArraySize = 1,
			.MipLevels = 1,
			.Format = DXGI_FORMAT_UNKNOWN,
			.SampleDesc = DXGI_SAMPLE_DESC{
				.Count = 1,
				.Quality = 0
			},
			.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
			.Flags = MapBufferUsage(bufferDesc.bufferUsage)
		};

		D3D12_RESOURCE_ALLOCATION_INFO const allocationInfo = mpDevice->GetResourceAllocationInfo(0, 1, &resourceDesc);

		return MemoryRequirements{
			.memoryTypeIndex = memoryTypeIndex,
			.size = allocationInfo.SizeInBytes,
			.alignment = allocationInfo.Alignment
		};
	}



	IMemoryHandle Device::AllocateMemory(MemoryDesc const & desc) {
		auto [heapType, memoryCategory] = FromMemoryTypeIndex(this, desc.memoryTypeIndex);

		D3D12_HEAP_PROPERTIES heapProperty{
			.Type = MapHeapType(heapType),
			.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
			.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
			.CreationNodeMask = 0,
			.VisibleNodeMask = 0
		};

		D3D12_HEAP_FLAGS heapFlags = MapCategoryToD3D12HeapFlags(this, memoryCategory);
		heapFlags |= D3D12_HEAP_FLAG_ALLOW_SHADER_ATOMICS;
		heapFlags |= D3D12_HEAP_FLAG_CREATE_NOT_ZEROED;

		D3D12_HEAP_DESC heapDesc{
			.SizeInBytes = desc.size,
			.Properties = heapProperty,
			.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
			.Flags = heapFlags
		};

		return MakeHandle<Memory>(this, heapDesc);
	}



	Buffer::Buffer(MemoryHandle memoryHandle, uint64_t offset, BufferDesc const & desc) :
		IBuffer(offset, desc.size),
		mMemoryHandle(std::move(memoryHandle)),
		mpBuffer(nullptr) {

		D3D12_RESOURCE_DESC const resourceDesc{
			.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
			.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
			.Width = desc.size,
			.Height = 1,
			.DepthOrArraySize = 1,
			.MipLevels = 1,
			.Format = DXGI_FORMAT_UNKNOWN,
			.SampleDesc = DXGI_SAMPLE_DESC{
				.Count = 1,
				.Quality = 0
			},
			.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
			.Flags = MapBufferUsage(desc.bufferUsage)
		};

		D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;

		auto heapType = mMemoryHandle->mpHeap->GetDesc().Properties.Type;
		switch (heapType) {
			case D3D12_HEAP_TYPE_DEFAULT: {
				state = D3D12_RESOURCE_STATE_COMMON;
			} break;

			case D3D12_HEAP_TYPE_UPLOAD: {
				state = D3D12_RESOURCE_STATE_GENERIC_READ;
			} break;

			case D3D12_HEAP_TYPE_READBACK: {
				state = D3D12_RESOURCE_STATE_COPY_DEST;
			} break;

			default: break;
		}

		D3D12CheckResult(mMemoryHandle->mDeviceHandle->mpDevice->CreatePlacedResource(mMemoryHandle->mpHeap, offset, &resourceDesc, state, nullptr, IID_PPV_ARGS(&mpBuffer)));
	}

	Buffer::~Buffer() {
		mpBuffer->Release();
	}

	IMemoryHandle Buffer::GetMemory() const {
		return mMemoryHandle;
	}



	IBufferHandle Device::CreateBuffer(IMemoryHandle memoryHandle, uint64_t offset, BufferDesc const & bufferDesc) {
		return MakeHandle<Buffer>(dynamic_handle_cast<Memory>(memoryHandle), offset, bufferDesc);
	}

}