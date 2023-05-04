#include "erhi/dx12/context/physical_device.hpp"
#include "erhi/dx12/context/device.hpp"

#include "erhi/dx12/resource/memory.hpp"

#include <bit>			// for memory type bits manipulation
#include <utility>		// for std::pair used in erhi/native mapping
#include <cassert>		// for assert()
#include <format>		// for std::format

#include "magic_enum.hpp"		// for enum_count()



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

	enum class ResourceCategory : uint32_t {
		Buffer = 0,
		RT_DS_Texture = 1,
		Non_RT_DS_Texture = 2,
		
		None = 0,					// For all kinds of placed resources on tier 2 device.
	};

	static D3D12_HEAP_FLAGS MapResourceCategoryToD3D12HeapFlags(Device * pDevice, ResourceCategory category) {
		if (pDevice->mPhysicalDeviceHandle->mFeatureD3D12Options.ResourceHeapTier == D3D12_RESOURCE_HEAP_TIER_1) {
			D3D12_HEAP_FLAGS heapFlags = D3D12_HEAP_FLAG_NONE;

			switch (category) {
				case ResourceCategory::Buffer:
					heapFlags |= D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS;
					break;

				case ResourceCategory::RT_DS_Texture:
					heapFlags |= D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES;
					break;

				case ResourceCategory::Non_RT_DS_Texture:
					heapFlags |= D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES;
					break;

				default:
					break;
			}

			return heapFlags;
		}
		else {
			return D3D12_HEAP_FLAG_NONE;
		}		
	}

	//static ResourceCategory MakeCategory_Buffer(Device * pDevice) {
	//	if (pDevice->mPhysicalDeviceHandle->mFeatureD3D12Options.ResourceHeapTier == D3D12_RESOURCE_HEAP_TIER_1) {
	//		return ResourceCategory::Buffer;
	//	}
	//	else {
	//		return ResourceCategory::None;
	//	}
	//}

	//static ResourceCategory MakeCategory_RT_DS_Texture(Device * pDevice) {
	//	if (pDevice->mPhysicalDeviceHandle->mFeatureD3D12Options.ResourceHeapTier == D3D12_RESOURCE_HEAP_TIER_1) {
	//		return ResourceCategory::RT_DS_Texture;
	//	}
	//	else {
	//		return ResourceCategory::None;
	//	}
	//}

	//static ResourceCategory MakeCategory_Non_RT_DS_Texture(Device * pDevice) {
	//	if (pDevice->mPhysicalDeviceHandle->mFeatureD3D12Options.ResourceHeapTier == D3D12_RESOURCE_HEAP_TIER_1) {
	//		return ResourceCategory::Non_RT_DS_Texture;
	//	}
	//	else {
	//		return ResourceCategory::None;
	//	}
	//}

	static uint32_t MakeMemoryTypeIndex(Device * pDevice, MemoryHeapType heapType, ResourceCategory category) {
		if (pDevice->mPhysicalDeviceHandle->mFeatureD3D12Options.ResourceHeapTier == D3D12_RESOURCE_HEAP_TIER_1) {
			return uint32_t(heapType) * uint32_t(magic_enum::enum_count<ResourceCategory>()) + uint32_t(category);
		}
		else {
			return uint32_t(heapType);
		}
	}

	static std::pair<MemoryHeapType, ResourceCategory> ParseMemoryTypeIndex(Device * pDevice, uint32_t memoryTypeIndex) {
		if (pDevice->mPhysicalDeviceHandle->mFeatureD3D12Options.ResourceHeapTier == D3D12_RESOURCE_HEAP_TIER_1) {
			return {
				MemoryHeapType(memoryTypeIndex / magic_enum::enum_count<ResourceCategory>()),
				ResourceCategory(memoryTypeIndex % magic_enum::enum_count<ResourceCategory>())
			};
		}
		else {
			return {
				MemoryHeapType(memoryTypeIndex),
				ResourceCategory::None
			};
		}
	}



	static D3D12_RESOURCE_FLAGS MapBufferUsageFlags(BufferUsageFlags bufferUsage) {
		D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;

		// In D3D12, resource flags are mostly for textures.
		if (bufferUsage & BufferUsageStorageBuffer) flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		return flags;
	}



	MemoryRequirements Device::GetBufferMemoryRequirements(MemoryHeapType heapType, BufferDesc const & bufferDesc) {
		uint32_t const memoryTypeIndex = MakeMemoryTypeIndex(this, heapType, ResourceCategory::Buffer);
		
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
			.Flags = MapBufferUsageFlags(bufferDesc.usage)
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

		auto [heapType, memoryCategory] = ParseMemoryTypeIndex(mDeviceHandle.get(), desc.memoryTypeIndex);

		// Map ERHI heap type to D3D12 heap type.

		D3D12_HEAP_PROPERTIES heapProperty{
			.Type = MapHeapType(heapType),
			.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
			.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
			.CreationNodeMask = 0,
			.VisibleNodeMask = 0
		};

		// Map native category to D3D12 heap flags.

		D3D12_HEAP_FLAGS heapFlags = MapResourceCategoryToD3D12HeapFlags(mDeviceHandle.get(), memoryCategory);
		heapFlags |= D3D12_HEAP_FLAG_CREATE_NOT_ZEROED;
		// <todo>
		// Does atomic operation cost zero when it's not used at all?
		// 
		// heapFlags |= D3D12_HEAP_FLAG_ALLOW_SHADER_ATOMICS;
		// 
		// </todo>
		
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
			.Flags = MapBufferUsageFlags(desc.usage)
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



	PlacedBuffer::PlacedBuffer(Memory * pMemory, uint64_t offset, uint64_t actualSize, BufferDesc const & desc) :
		IBuffer(desc), mMemoryHandle(pMemory), mOffset(offset), mActualSize(actualSize), mpBuffer(nullptr) {

		auto const resourceDesc = GetResourceState(desc);
		auto const initialState = GetInitialState(mMemoryHandle->mpHeap->GetDesc().Properties.Type);

		D3D12CheckResult(mMemoryHandle->mDeviceHandle->mpDevice->CreatePlacedResource(mMemoryHandle->mpHeap, offset, &resourceDesc, initialState, nullptr, IID_PPV_ARGS(&mpBuffer)));
	}

	PlacedBuffer::~PlacedBuffer() {
		mpBuffer->Release();
	}

	IBufferHandle Device::CreatePlacedBuffer(IMemory * pMemory, uint64_t offset, uint64_t actualSize, BufferDesc const & bufferDesc) {
		assert(pMemory != nullptr);
		assert(pMemory->GetDesc().size >= offset + actualSize);
		assert(actualSize >= bufferDesc.size);

		return MakeHandle<PlacedBuffer>(dynamic_cast<Memory *>(pMemory), offset, actualSize, bufferDesc);
	}



	CommittedBuffer::CommittedBuffer(Device * pDevice, MemoryHeapType heapType, BufferDesc const & desc) :
		IBuffer(desc), mDeviceHandle(pDevice), mpBuffer(nullptr) {
		
		D3D12_HEAP_PROPERTIES heapProperty{
			.Type = MapHeapType(heapType),
			.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
			.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
			.CreationNodeMask = 0,
			.VisibleNodeMask = 0
		};

		D3D12_HEAP_FLAGS heapFlags = D3D12_HEAP_FLAG_NONE;
		heapFlags |= D3D12_HEAP_FLAG_CREATE_NOT_ZEROED;
		// <todo>
		// Does atomic operation cost zero when it's not used at all?
		// 
		// heapFlags |= D3D12_HEAP_FLAG_ALLOW_SHADER_ATOMICS;
		// 
		// </todo>

		auto const resourceDesc = GetResourceState(desc);
		auto const initialState = GetInitialState(MapHeapType(heapType));

		D3D12CheckResult(pDevice->mpDevice->CreateCommittedResource(&heapProperty, heapFlags, &resourceDesc, initialState, nullptr, IID_PPV_ARGS(&mpBuffer)));
	}

	CommittedBuffer::~CommittedBuffer() {
		mpBuffer->Release();
	}

	IBufferHandle Device::CreateCommittedBuffer(MemoryHeapType heapType, BufferDesc const & desc) {
		return MakeHandle<CommittedBuffer>(this, heapType, desc);
	}



	DXGI_FORMAT MapFormat(Format format) {
		switch (format) {
			case erhi::Format::Unknown:
				return DXGI_FORMAT_UNKNOWN;
				break;

			case erhi::Format::R32G32B32A32_Typeless:
				return DXGI_FORMAT_R32G32B32A32_TYPELESS;
				break;
			case erhi::Format::R32G32B32A32_Float:
				return DXGI_FORMAT_R32G32B32A32_FLOAT;
				break;
			case erhi::Format::R32G32B32A32_UInt:
				return DXGI_FORMAT_R32G32B32A32_UINT;
				break;
			case erhi::Format::R32G32B32A32_SInt:
				return DXGI_FORMAT_R32G32B32A32_SINT;
				break;

			case erhi::Format::R32G32B32_Typeless:
				return DXGI_FORMAT_R32G32B32_TYPELESS;
				break;
			case erhi::Format::R32G32B32_Float:
				return DXGI_FORMAT_R32G32B32_FLOAT;
				break;
			case erhi::Format::R32G32B32_UInt:
				return DXGI_FORMAT_R32G32B32_UINT;
				break;
			case erhi::Format::R32G32B32_SInt:
				return DXGI_FORMAT_R32G32B32_SINT;
				break;

			case erhi::Format::R16G16B16A16_Typeless:
				return DXGI_FORMAT_R16G16B16A16_TYPELESS;
				break;
			case erhi::Format::R16G16B16A16_Float:
				return DXGI_FORMAT_R16G16B16A16_FLOAT;
				break;
			case erhi::Format::R16G16B16A16_UNorm:
				return DXGI_FORMAT_R16G16B16A16_UNORM;
				break;
			case erhi::Format::R16G16B16A16_UInt:
				return DXGI_FORMAT_R16G16B16A16_UINT;
				break;
			case erhi::Format::R16G16B16A16_SNorm:
				return DXGI_FORMAT_R16G16B16A16_SNORM;
				break;
			case erhi::Format::R16G16B16A16_SInt:
				return DXGI_FORMAT_R16G16B16A16_SINT;
				break;

			case erhi::Format::R32G32_Typeless:
				return DXGI_FORMAT_R32G32_TYPELESS;
				break;
			case erhi::Format::R32G32_Float:
				return DXGI_FORMAT_R32G32_FLOAT;
				break;
			case erhi::Format::R32G32_UInt:
				return DXGI_FORMAT_R32G32_UINT;
				break;
			case erhi::Format::R32G32_SInt:
				return DXGI_FORMAT_R32G32_SINT;
				break;

			case erhi::Format::R8G8B8A8_Typeless:
				return DXGI_FORMAT_R8G8B8A8_TYPELESS;
				break;
			case erhi::Format::R8G8B8A8_UNorm:
				return DXGI_FORMAT_R8G8B8A8_UNORM;
				break;
			case erhi::Format::R8G8B8A8_UInt:
				return DXGI_FORMAT_R8G8B8A8_UINT;
				break;
			case erhi::Format::R8G8B8A8_SNorm:
				return DXGI_FORMAT_R8G8B8A8_SNORM;
				break;
			case erhi::Format::R8G8B8A8_SInt:
				return DXGI_FORMAT_R8G8B8A8_SINT;
				break;

			case erhi::Format::R16G16_Typeless:
				return DXGI_FORMAT_R16G16_TYPELESS;
				break;
			case erhi::Format::R16G16_Float:
				return DXGI_FORMAT_R16G16_FLOAT;
				break;
			case erhi::Format::R16G16_UNorm:
				return DXGI_FORMAT_R16G16_UNORM;
				break;
			case erhi::Format::R16G16_UInt:
				return DXGI_FORMAT_R16G16_UINT;
				break;
			case erhi::Format::R16G16_SNorm:
				return DXGI_FORMAT_R16G16_SNORM;
				break;
			case erhi::Format::R16G16_SInt:
				return DXGI_FORMAT_R16G16_SINT;
				break;

			case erhi::Format::R32_Float:
				return DXGI_FORMAT_R32_FLOAT;
				break;
			case erhi::Format::R32_UInt:
				return DXGI_FORMAT_R32_UINT;
				break;
			case erhi::Format::R32_SInt:
				return DXGI_FORMAT_R32_SINT;
				break;
			
			case erhi::Format::D32_Float:
				return DXGI_FORMAT_D32_FLOAT;
				break;
			case erhi::Format::D16_UNorm:
				return DXGI_FORMAT_D16_UNORM;
				break;
			case erhi::Format::D24_UNorm_S8_UInt:
				return DXGI_FORMAT_D24_UNORM_S8_UINT;
				break;

			default:
				return DXGI_FORMAT_UNKNOWN;
				break;
		}
	}



	static uint32_t MapSampleCount(TextureSampleCount sampleCount) {
		return 1u << static_cast<uint32_t>(sampleCount);
	}



	static D3D12_RESOURCE_FLAGS MapTextureUsageFlags(TextureUsageFlags flags) {
		D3D12_RESOURCE_FLAGS result = D3D12_RESOURCE_FLAG_NONE;

		if (flags & TextureUsageStorage) {
			result |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		}
		if (flags & TextureUsageRenderTargetAttachment) {
			result |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		}
		if (flags & TextureUsageDepthStencilAttachment) {
			result |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		}

		return result;
	}



	static D3D12_RESOURCE_DESC MapTextureDesc(Device * pDevice, TextureDesc const & desc) {
		D3D12_RESOURCE_DIMENSION dimension = D3D12_RESOURCE_DIMENSION_UNKNOWN;
		switch (desc.dimension) {
			case TextureDimension::Texture1D:
				dimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D;
				break;

			case TextureDimension::Texture2D:
				dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
				break;

			case TextureDimension::Texture3D:
				dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
				break;

			default:
				break;
		}

		// <todo> small texture optimization </todo>
		uint64_t alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		if (desc.sampleCount != TextureSampleCount::Count_1) {
			alignment = D3D12_DEFAULT_MSAA_RESOURCE_PLACEMENT_ALIGNMENT;
		}

		DXGI_FORMAT const format = MapFormat(desc.format);
		uint32_t const sampleCount = MapSampleCount(desc.sampleCount);
		{
			D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msaaQualityLevels{
				.Format = format,
				.SampleCount = sampleCount
			};
			D3D12CheckResult(pDevice->mpDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msaaQualityLevels, sizeof(msaaQualityLevels)));
			if (msaaQualityLevels.NumQualityLevels == 0) {
				throw invalid_arguments(std::format(
					"Texture with DXGI_FORMAT = {} and sample count = {} is not supported.",
					magic_enum::enum_name(format), sampleCount
				));
			}
		}

		return D3D12_RESOURCE_DESC{
			.Dimension = dimension,
			.Alignment = alignment,
			.Width = desc.extent[0],
			.Height = desc.extent[1],
			.DepthOrArraySize = uint16_t(desc.extent[2]),
			.MipLevels = uint16_t(desc.mipLevels),
			.Format = format,
			.SampleDesc = DXGI_SAMPLE_DESC{
				.Count = sampleCount,
				.Quality = 0
			},
			.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
			.Flags = MapTextureUsageFlags(desc.usage)
		};
	}



	CommittedTexture::CommittedTexture(Device * pDevice, MemoryHeapType heapType, TextureDesc const & desc) :
		ITexture(desc), mDeviceHandle(pDevice), mpTexture(nullptr) {

		D3D12_HEAP_PROPERTIES heapProperty{
			.Type = MapHeapType(heapType),
			.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
			.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
			.CreationNodeMask = 0,
			.VisibleNodeMask = 0
		};

		D3D12_HEAP_FLAGS heapFlags = D3D12_HEAP_FLAG_NONE;
		heapFlags |= D3D12_HEAP_FLAG_CREATE_NOT_ZEROED;
		// <todo>
		// Does atomic operation cost zero when it's not used at all?
		// 
		// heapFlags |= D3D12_HEAP_FLAG_ALLOW_SHADER_ATOMICS;
		// 
		// </todo>

		auto const resourceDesc = MapTextureDesc(pDevice, desc);
		auto const initialState = GetInitialState(MapHeapType(heapType));

		D3D12CheckResult(pDevice->mpDevice->CreateCommittedResource(&heapProperty, heapFlags, &resourceDesc, initialState, nullptr, IID_PPV_ARGS(&mpTexture)));
	}

	CommittedTexture::~CommittedTexture() {
		mpTexture->Release();
	}

	ITextureHandle Device::CreateCommittedTexture(MemoryHeapType heapType, TextureDesc const & desc) {
		return MakeHandle<CommittedTexture>(this, heapType, desc);
	}



	PlacedTexture::PlacedTexture(Memory * pMemory, uint64_t offset, uint64_t actualSize, TextureDesc const & desc) :
		ITexture(desc), mMemoryHandle(pMemory), mOffset(offset), mActualSize(actualSize), mpTexture(nullptr) {

		Device * pDevice = pMemory->mDeviceHandle.get();

		D3D12_RESOURCE_DESC const resourceDesc = MapTextureDesc(pDevice, desc);
		D3D12_RESOURCE_STATES const initialState = GetInitialState(pMemory->mpHeap->GetDesc().Properties.Type);

		D3D12CheckResult(pDevice->mpDevice->CreatePlacedResource(pMemory->mpHeap, offset, &resourceDesc, initialState, nullptr, IID_PPV_ARGS(&mpTexture)));
	}

	PlacedTexture::~PlacedTexture() {
		mpTexture->Release();
	}

	ITextureHandle Device::CreatePlacedTexture(IMemory * pMemory, uint64_t offset, uint64_t actualSize, TextureDesc const & desc) {
		return MakeHandle<PlacedTexture>(dynamic_cast<Memory *>(pMemory), offset, actualSize, desc);
	}



	MemoryRequirements Device::GetTextureMemoryRequirements(MemoryHeapType heapType, TextureDesc const & textureDesc) {
		uint32_t memoryTypeIndex = 0xFFFF'FFFFu;
		if (textureDesc.usage & (TextureUsageRenderTargetAttachment | TextureUsageDepthStencilAttachment)) {
			memoryTypeIndex = MakeMemoryTypeIndex(this, heapType, ResourceCategory::RT_DS_Texture);
		}
		else {
			memoryTypeIndex = MakeMemoryTypeIndex(this, heapType, ResourceCategory::Non_RT_DS_Texture);
		}

		auto const resourceDesc = MapTextureDesc(this, textureDesc);

		D3D12_RESOURCE_ALLOCATION_INFO const allocationInfo = mpDevice->GetResourceAllocationInfo(0, 1, &resourceDesc);

		return MemoryRequirements{
			.memoryTypeBits = (1u << memoryTypeIndex),
			.prefersCommittedResource = false,
			.requiresCommittedResource = false,
			.size = allocationInfo.SizeInBytes,
			.alignment = allocationInfo.Alignment,
		};
	}

}