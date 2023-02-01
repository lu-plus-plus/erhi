#include "erhi/dx12/context/physical_device.hpp"
#include "erhi/dx12/context/device.hpp"

#include "erhi/dx12/resource/memory.hpp"

#include <bit>			// for memory type bits manipulation
#include <utility>		// for std::pair used in erhi/native mapping
#include <cassert>		// for assert()



namespace erhi::dx12 {

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

	enum class NativeCategory : uint32_t {
		Buffer = 0,
		RT_DS_Texture = 1,
		Non_RT_DS_Texture = 2,
		MaxEnum = 3,

		General = 0,
		None = 3 /* for committed resource, whose heap flags are implicitly specified by driver */
	};

	static D3D12_HEAP_FLAGS MapCategoryToHeapFlags(NativeCategory category) {
		D3D12_HEAP_FLAGS heapFlags = D3D12_HEAP_FLAG_NONE;

		// heapFlags |= D3D12_HEAP_FLAG_ALLOW_SHADER_ATOMICS;
		// heapFlags |= D3D12_HEAP_FLAG_CREATE_NOT_ZEROED;

		switch (category) {
			case NativeCategory::Buffer: {
				heapFlags |= D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS;
			} break;

			case NativeCategory::RT_DS_Texture: {
				heapFlags |= D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES;
			} break;

			case NativeCategory::Non_RT_DS_Texture: {
				heapFlags |= D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES;
			} break;

			default: break;
		}

		return heapFlags;
	}

	static NativeCategory GetBufferCategory(Device * pDevice) {
		if (pDevice->mPhysicalDeviceHandle->mFeatureD3D12Options.ResourceHeapTier == D3D12_RESOURCE_HEAP_TIER_1) {
			return NativeCategory::Buffer;
		}
		else {
			return NativeCategory::General;
		}
	}

	static uint32_t GetMemoryTypeIndex(MemoryHeapType heapType, NativeCategory category) {
		return uint32_t(heapType) * uint32_t(NativeCategory::MaxEnum) + uint32_t(category);
	}

	std::pair<MemoryHeapType, NativeCategory> FromMemoryTypeIndex(Device * pDevice, uint32_t memoryTypeIndex) {
		return std::make_pair(
			MemoryHeapType(memoryTypeIndex / uint32_t(MemoryHeapType::MaxEnum)),
			NativeCategory(memoryTypeIndex % uint32_t(MemoryHeapType::MaxEnum))
		);
	}



	static D3D12_RESOURCE_FLAGS MapBufferUsage(BufferUsageFlags bufferUsage) {
		D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;

		if (bufferUsage & BufferUsageBits::StorageBuffer) flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		return flags;
	}



	MemoryRequirements Device::GetBufferMemoryRequirements(MemoryHeapType heapType, BufferDesc const & bufferDesc) {
		uint32_t const memoryTypeIndex = GetMemoryTypeIndex(heapType, GetBufferCategory(this));
		
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
			.memoryTypeBits = (1u << memoryTypeIndex),
			.size = allocationInfo.SizeInBytes,
			.alignment = allocationInfo.Alignment
		};
	}



	Memory::Memory(DeviceHandle deviceHandle, MemoryDesc const & desc) :
		IMemory(desc),
		mDeviceHandle(std::move(deviceHandle)),
		mpHeap(nullptr) {

		// Map ERHI memory type index to ERHI heap type, and D3D12's "native memory categories".
		// Resources of different categories may NOT be allocated on the same heap.

		auto [heapType, memoryCategory] = FromMemoryTypeIndex(mDeviceHandle.get(), desc.memoryTypeIndex);

		// Map ERHI heap type to D3D12 heap type.

		D3D12_HEAP_PROPERTIES heapProperty{
			.Type = MapHeapType(heapType),
			.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
			.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
			.CreationNodeMask = 0,
			.VisibleNodeMask = 0
		};

		// Map native category to D3D12 heap flags.

		D3D12_HEAP_FLAGS heapFlags = MapCategoryToHeapFlags(memoryCategory);
		heapFlags |= D3D12_HEAP_FLAG_ALLOW_SHADER_ATOMICS;
		heapFlags |= D3D12_HEAP_FLAG_CREATE_NOT_ZEROED;

		// Allocate heap.

		D3D12_HEAP_DESC heapDesc{
			.SizeInBytes = desc.size,
			.Properties = heapProperty,
			.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
			.Flags = heapFlags
		};

		D3D12CheckResult(mDeviceHandle->mpDevice->CreateHeap(&heapDesc, IID_PPV_ARGS(&mpHeap)));
	}

	Memory::~Memory() {
		mpHeap->Release();
	}

	IDeviceHandle Memory::GetDevice() const {
		return mDeviceHandle;
	}

	IMemoryHandle Device::AllocateMemory(MemoryDesc const & desc) {
		return MakeHandle<Memory>(DeviceHandle(this), desc);
	}



	D3D12_RESOURCE_DESC GetResourceState(BufferDesc const & desc) {
		return D3D12_RESOURCE_DESC{
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
	}

	D3D12_RESOURCE_STATES GetInitialState(D3D12_HEAP_TYPE heapType) {
		D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;

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

		return state;
	}



	PlacedBuffer::PlacedBuffer(BufferDesc const & desc, Memory * pMemory, uint64_t offset, uint64_t alignment) :
		IPlacedBuffer(desc, offset, alignment),
		mMemoryHandle(pMemory),
		mpBuffer(nullptr) {

		assert(offset % alignment == 0);

		D3D12_RESOURCE_DESC const resourceDesc = GetResourceState(desc);

		D3D12_RESOURCE_STATES state = GetInitialState(mMemoryHandle->mpHeap->GetDesc().Properties.Type);

		D3D12CheckResult(mMemoryHandle->mDeviceHandle->mpDevice->CreatePlacedResource(mMemoryHandle->mpHeap, offset, &resourceDesc, state, nullptr, IID_PPV_ARGS(&mpBuffer)));
	}

	PlacedBuffer::~PlacedBuffer() {
		mpBuffer->Release();
	}

	IMemoryHandle PlacedBuffer::GetMemory() const {
		return mMemoryHandle;
	}

	IPlacedBufferHandle Device::CreatePlacedBuffer(IMemoryHandle memoryHandle, uint64_t offset, uint64_t alignment,BufferDesc const & bufferDesc) {
		auto const pMemory = dynamic_cast<Memory *>(memoryHandle.get());
		assert(pMemory != nullptr);

		return MakeHandle<PlacedBuffer>(bufferDesc, pMemory, offset, alignment);
	}



	CommittedBuffer::CommittedBuffer(Device * pDevice, MemoryHeapType heapType, BufferDesc const & desc) :
		ICommittedBuffer(desc), mDeviceHandle(pDevice), mpBuffer(nullptr) {
		
		D3D12_HEAP_PROPERTIES heapProperty{
			.Type = MapHeapType(heapType),
			.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
			.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
			.CreationNodeMask = 0,
			.VisibleNodeMask = 0
		};

		auto const heapFlags = MapCategoryToHeapFlags(NativeCategory::None);
		auto const resourceDesc = GetResourceState(desc);
		auto const initialState = GetInitialState(MapHeapType(heapType));

		D3D12CheckResult(pDevice->mpDevice->CreateCommittedResource(&heapProperty, heapFlags, &resourceDesc, initialState, nullptr, IID_PPV_ARGS(&mpBuffer)));
	}

	CommittedBuffer::~CommittedBuffer() {
		mpBuffer->Release();
	}

	ICommittedBufferHandle Device::CreateCommittedBuffer(MemoryHeapType heapType, BufferDesc const & desc) {
		return MakeHandle<CommittedBuffer>(this, heapType, desc);
	}

}