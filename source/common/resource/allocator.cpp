#include "erhi/common/resource/allocator.hpp"

#include "erhi/common/context/device.hpp"
#include "erhi/common/resource/memory.hpp"

#include "magic_enum.hpp"

#include <cassert>		// for assertion
#include <bit>			// for bit manipulation
#include <format>		// for message formating



namespace erhi {
	
	IAllocator::IAllocator(IDevice * pDevice) : mDeviceHandle(pDevice) {}
	
	IAllocator::~IAllocator() = default;

	IDevice * IAllocator::GetDevice() const { return mDeviceHandle.get(); }



	AllocImpl::Linear::Arena::Arena(uint64_t size) : mSize(size), mFreeRegionBegin(0), mFreeRegionEnd(size), mFragments() {
		assert(std::has_single_bit(size));
	}

	AllocImpl::Linear::Arena::~Arena() {
		assert(mFreeRegionBegin == 0);
		assert(mFreeRegionEnd == mSize);
		assert(mFragments.size() == 0);
	}

	AllocImpl::Linear::FragmentRef AllocImpl::Linear::Arena::TryAllocate(uint64_t size, uint64_t alignment) {
		assert(std::has_single_bit(alignment));

		// mFreeRegionBegin and mFreeRegionEnd may exceed mSize - they are circular counters.

		uint64_t const actualBegin = mFreeRegionBegin % mSize;
		uint64_t const actualEnd = mFreeRegionBegin % mSize;

		uint64_t const alignedBegin = (actualBegin + (alignment - 1)) & -int64_t(alignment);
		uint64_t const alignedEnd = actualEnd & -int64_t(alignment);

		auto SliceFromFront = [&] () -> ListType<Fragment>::iterator {
			mFreeRegionBegin += alignedBegin - actualBegin;
			mFreeRegionBegin += size;
			mFragments.emplace_back(Fragment{ .mOffset = alignedBegin, .mSize = size });
			return std::prev(mFragments.end());
		};

		auto SliceFromBack = [&] () -> ListType<Fragment>::iterator {
			mFreeRegionEnd -= actualEnd - alignedEnd;
			mFreeRegionEnd -= size;
			mFragments.emplace_front(Fragment{ .mOffset = mFreeRegionEnd, .mSize = size });
			return mFragments.begin();
		};

		if (mFreeRegionBegin / mSize == mFreeRegionEnd / mSize) {
			if (alignedBegin + size <= actualEnd) {
				return FragmentRef(this, SliceFromFront());
			}
		}
		else {
			if (alignedBegin + size <= mSize) {
				return FragmentRef(this, SliceFromFront());
			}
			else if (size <= actualEnd) {
				return FragmentRef(this, SliceFromBack());
			}
		}

		return FragmentRef(nullptr, ListType<Fragment>::iterator());
	}

	void AllocImpl::Linear::Arena::Free(ListType<Fragment>::iterator pFragment) {
		assert(not mFragments.empty());
		
		if (mFragments.size() == 1) {
			assert(mFragments.begin() == pFragment);
			mFreeRegionBegin = 0;
			mFreeRegionEnd = mSize;
			mFragments.clear();
		}

		auto pNext = std::next(pFragment);
		auto pPrev = std::prev(mFragments.end());

		// As mFreeRegionBegin and mFreeRegionEnd are circular counters, their values may overflow or underflow.

		if (mFragments.begin() == pFragment) {
			mFreeRegionEnd = mFreeRegionEnd / mSize * mSize + pNext->mOffset;
			if (pNext->mOffset < pFragment->mOffset) mFreeRegionEnd += mSize;
		}
		else if (std::prev(mFragments.end()) == pFragment) {
			mFreeRegionBegin = mFreeRegionBegin / mSize * mSize + pPrev->mOffset + pPrev->mSize;
			if (pPrev->mOffset > pFragment->mOffset) mFreeRegionBegin -= mSize;
		}

		mFragments.erase(pFragment);
	}



	void AllocImpl::Linear::FragmentRef::Free() {
		if (mpArena) mpArena->Free(mpSelf);
	}

	void AllocImpl::Linear::FragmentRef::Invalidate() {
		mpArena = nullptr;
		mpSelf = ListType<Fragment>::iterator();
	}

	AllocImpl::Linear::FragmentRef::FragmentRef(Arena * pArena, ListType<Fragment>::iterator pSelf) : mpArena(pArena), mpSelf(pSelf) {}

	AllocImpl::Linear::FragmentRef::FragmentRef(FragmentRef && other) noexcept : mpArena(other.mpArena), mpSelf(other.mpSelf) {
		other.Invalidate();
	}

	AllocImpl::Linear::FragmentRef & AllocImpl::Linear::FragmentRef::operator=(FragmentRef && other) noexcept {
		FragmentRef & self = *this;
		self.Free();
		self.mpArena = other.mpArena;
		self.mpSelf = other.mpSelf;
		other.Invalidate();
		return self;
	}

	AllocImpl::Linear::FragmentRef::~FragmentRef() {
		Free();
	}

	bool AllocImpl::Linear::FragmentRef::IsValid() const { return mpArena != nullptr; }



	ILinearAllocator::ILinearAllocator(IDevice * pDevice) : IAllocator(pDevice) {}

	ILinearAllocator::~ILinearAllocator() = default;

	AllocImpl::Linear::FragmentRef ILinearAllocator::CreateFragment(MemoryRequirements requirements) {
		uint32_t const memoryTypeIndex = std::countr_zero(requirements.memoryTypeBits);

		if (memoryTypeIndex >= mPools.size()) mPools.resize(memoryTypeIndex + 1);
		auto & pool = mPools[memoryTypeIndex];

		for (auto & arena : pool.mArenas) {
			auto fragment = arena.mArenaImpl.TryAllocate(requirements.size, requirements.alignment);
			if (not fragment.IsValid()) continue;

			return fragment;
		}

		uint64_t const allocationSize = std::max(gDefaultArenaSize, requirements.size);
		pool.mArenas.emplace_back(Arena{
			.mArenaImpl = AllocImpl::Linear::Arena(allocationSize),
			.mMemoryHandle = mDeviceHandle->AllocateMemory(MemoryDesc{
				.memoryTypeIndex = memoryTypeIndex,
				.size = allocationSize
			})
		});
		auto fragment = pool.mArenas.back().mArenaImpl.TryAllocate(requirements.size, requirements.alignment);
		assert(fragment.IsValid());
		return fragment;

	}

	IBufferHandle ILinearAllocator::CreateBuffer(MemoryHeapType heapType, BufferDesc const & desc) {
		auto const req = mDeviceHandle->GetBufferMemoryRequirements(heapType, desc);

		if (not req.memoryTypeBits) {
			// <todo> reflection </todo>
			mDeviceHandle->Warning(std::format("No memory type is available for {}, {}.", magic_enum::enum_name(heapType), "..."));
			return nullptr;
		}

		auto fragment = CreateFragment(req);

		return CreateBuffer(std::move(fragment), desc);
	}

	ITextureHandle ILinearAllocator::CreateTexture(MemoryHeapType heapType, TextureDesc const & desc) {
		auto const req = mDeviceHandle->GetTextureMemoryRequirements(heapType, desc);

		if (not req.memoryTypeBits) {
			// <todo> reflection </todo>
			mDeviceHandle->Warning(std::format("No memory type is available for {}, {}.", magic_enum::enum_name(heapType), "..."));
			return nullptr;
		}

		auto fragment = CreateFragment(req);

		return CreateTexture(std::move(fragment), desc);
	}



	//AllocImpl::Linear::FragmentRef AllocImpl::Linear::TryAllocate(uint64_t size, uint64_t alignment) {
	//	for (Arena & arena : mArenas) {
	//		auto pFragment = arena.Allocate(size, alignment);
	//		if (pFragment == arena.mFragments.end()) continue;

	//		return FragmentRef(&arena, pFragment);
	//	}

	//	return FragmentRef(nullptr, ListType<Fragment>::iterator());
	//}

	//AllocImpl::Linear::Arena & AllocImpl::Linear::CreateArena(uint64_t size) {
	//	return mArenas.emplace_back(size);
	//}



	//BuddyAllocator::BuddyAllocator(IDevice * pDevice, FnAllocateCallback const & allocate) :
	//	IAllocator(pDevice, allocate), mArenas(), mFreeLists() {}

	//BuddyAllocator::~BuddyAllocator() {
	//	for (auto const & a : mArenas) assert(a.mBlocks.size() == 1);
	//}

	//BuddyAllocator::BlockDesc BuddyAllocator::AllocateArenaInternal(int requestedOrder) {
	//	int const arenaOrder = std::max(requestedOrder, minArenaOrder);
	//	uint64_t const arenaSize = 1llu << arenaOrder;

	//	IMemoryHandle memory = mAllocateCallback(mDeviceHandle.get(), arenaSize);

	//	mArenas.push_front(Arena{
	//		.mOrder = arenaOrder,
	//		.mMemoryHandle = memory,
	//		.mBlocks = ListType(1, Block{
	//			.mOrder = arenaOrder,
	//			.mOffset = 0,
	//			.mpFreeListDesc = mFreeLists[arenaOrder].end()
	//		})
	//	});
	//	auto pArena = mArenas.begin();
	//	auto pInitialBlock = pArena->mBlocks.begin();

	//	// from free list item to arena and block
	//	mFreeLists[arenaOrder].push_front(BlockDesc{
	//		.mpArena = pArena,
	//		.mpBlock = pInitialBlock
	//	});
	//	auto pBlockDesc = mFreeLists[arenaOrder].begin();

	//	// from arena and block to free list
	//	pInitialBlock->mpFreeListDesc = pBlockDesc;

	//	return *pBlockDesc;
	//}

	//BuddyAllocator::BlockDesc BuddyAllocator::MakeBlock(Arena & arena, int order, uint64_t offset) {
	//	arena.mBlocks.insert()
	//}

	//void BuddyAllocator::SliceBlockInternal(BlockDesc & blockDesc, int slicedOrder) {
	//	auto pArena = blockDesc.mpArena;
	//	auto pBlock = blockDesc.mpBlock;

	//	mFreeLists[pBlock->mOrder].erase(pBlock->mpFreeListDesc);

	//	while (pBlock->mOrder > slicedOrder) {
	//		int const halfOrder = pBlock->mOrder - 1;

	//		auto pSecondHalf = pArena->mBlocks.insert(pBlock, Block{
	//			.mOrder = halfOrder,
	//			.mOffset = pBlock->Offset() + pBlock->Size() / 2,
	//			.mpFreeListDesc = mFreeLists[halfOrder].end()
	//		});

	//		// bidirectional query
	//		mFreeLists[halfOrder].push_front(BlockDesc{
	//			.mpArena = pArena,
	//			.mpBlock = pSecondHalf
	//		});
	//		pSecondHalf->mpFreeListDesc = mFreeLists[halfOrder].begin();

	//		pBlock->mOrder -= 1;
	//	}

	//	mFreeLists[slicedOrder].push_front(BlockDesc{
	//		.mpArena = pArena,
	//		.mpBlock = pBlock
	//	});
	//	pBlock->mpFreeListDesc = mFreeLists[slicedOrder].begin();

	//	blockDesc = BlockDesc{ .mpArena = pArena, .mpBlock = pBlock };
	//}



	//BuddyAllocator::BlockDesc BuddyAllocator::AllocateBlock(uint64_t size, uint64_t alignment) {
	//	assert(size > 0);
	//	assert(std::has_single_bit(alignment));

	//	int const blockOrder = std::bit_width(size) - 1;
	//	
	//	//auto & freeList = mFreeLists[blockOrder];
	//	//if (not freeList.empty()) {
	//	//	BlockDesc result = freeList.front();
	//	//	freeList.pop_front();
	//	//	return result;
	//	//}

	//	for (int order = blockOrder; order < maxOrder; ++order) {
	//		auto & freeList = mFreeLists[order];

	//		if (not freeList.empty()) {
	//			BlockDesc blockDesc = freeList.front();
	//			freeList.pop_front();

	//			SliceInternal(blockDesc, blockOrder);
	//			blockDesc.mpBlock->mpFreeListDesc = mFreeLists[blockOrder].end();
	//			
	//			return blockDesc;
	//		}
	//	}

	//	BlockDesc blockDesc = AllocateArenaInternal(blockOrder);

	//	SliceInternal(blockDesc, blockOrder);
	//	blockDesc.mpBlock->mpFreeListDesc = mFreeLists[blockOrder].end();

	//	return blockDesc;
	//}

	//void BuddyAllocator::FreeBlock(BlockDesc const & blockDesc) {
	//	auto pArena = blockDesc.mpArena;
	//	auto & blocks = pArena->mBlocks;

	//	auto pBlock = blockDesc.mpBlock;
	//	int const initialOrder = pBlock->mOrder;

	//	while (blocks.size() != 1) {
	//		bool isLeftBuddy = (pBlock->mOffset & (1llu << pBlock->mOrder)) != 0;
	//		
	//		if (isLeftBuddy) assert(pBlock != std::prev(blocks.end()));
	//		else assert(pBlock != blocks.begin());
	//		auto pBuddyBlock = isLeftBuddy ? std::next(pBlock) : std::prev(pBlock);

	//		assert(pBuddyBlock->Size() == pBlock->Size());

	//		// The buddy block has been occupied.
	//		if (pBuddyBlock->mpFreeListDesc == mFreeLists[pBuddyBlock->mOrder].end()) break;

	//		pBlock->mOrder += 1;
	//		pBlock->mOffset = std::min(pBlock->mOffset, pBuddyBlock->mOffset);
	//		// pBlock->mpFreeListDesc is still to be updated.

	//		mFreeLists[pBuddyBlock->mOrder].erase(pBuddyBlock->mpFreeListDesc);
	//		blocks.erase(pBuddyBlock);
	//	}

	//	mFreeLists[initialOrder].erase(pBlock->mpFreeListDesc);
	//	mFreeLists[pBlock->mOffset].push_front(BlockDesc{
	//		.mpArena = pArena,
	//		.mpBlock = pBlock
	//	});
	//	pBlock->mpFreeListDesc = mFreeLists[pBlock->mOffset].begin();
	//}



	//FreeSetAllocator::Chunk::Chunk(IMemory * pMemory) : mMemoryHandle(pMemory), mFragments() {
	//	mFragments.push_back(Fragment{
	//		.mOffset = 0,
	//		.mSize = mMemoryHandle->GetDesc().size
	//	});
	//}

	//FreeSetAllocator::Chunk::~Chunk() = default;



	//FreeSetAllocator::Multiset<FreeSetAllocator::FragmentFatPtr, FreeSetAllocator::FragmentFatPtr::Sorter>::iterator FreeSetAllocator::FindFragment(uint64_t size, uint64_t alignment) {
	//	auto ppFragment = mAvailableFragments.lower_bound(Fragment{ .mOffset = 0llu, .mSize = size });

	//	while (ppFragment != mAvailableFragments.end()) {
	//		auto alignedOffset = (ppFragment->mpFragment->mOffset + alignment - 1) / alignment * alignment;
	//	
	//		if (alignedOffset + size < ppFragment->mpFragment->mSize) {
	//			++ppFragment;
	//		}
	//		else {
	//			return ppFragment;
	//		}
	//	}

	//	return mAvailableFragments.end();
	//}

	//FreeSetAllocator::FragmentFatPtr FreeSetAllocator::SliceFragment(FragmentFatPtr & pFragment, uint64_t absoluteOffset, uint64_t size) {
	//	assert(pFragment->mOffset <= absoluteOffset);
	//	assert(absoluteOffset + size <= pFragment->mOffset + pFragment->mSize);

	//	auto pChunk = pFragment.mpChunk;
	//	auto & fragments = pChunk->mFragments;

	//	Fragment sliced{
	//		.mOffset = absoluteOffset,
	//		.mSize = size
	//	};

	//	if (pFragment->mOffset < sliced.mOffset) {
	//		Fragment preceding{
	//			.mOffset = pFragment->mOffset,
	//			.mSize = absoluteOffset - pFragment->mOffset,
	//		};
	//		mAvailableFragments.insert(FragmentFatPtr{
	//			.mpChunk = pChunk,
	//			.mpFragment = fragments.insert(pFragment, preceding)
	//		});
	//	}

	//	auto pSliced = fragments.insert(pFragment, sliced);

	//	if (sliced.mOffset + sliced.mSize < pFragment->mOffset + pFragment->mSize) {
	//		Fragment following{
	//			.mOffset = sliced.mOffset + sliced.mSize,
	//			.mSize = (pFragment->mOffset + pFragment->mSize) - (sliced.mOffset + sliced.mSize)
	//		};
	//		mAvailableFragments.insert(FragmentFatPtr{
	//			.mpChunk = pChunk,
	//			.mpFragment = fragments.insert(pFragment, following)
	//		});
	//	}

	//	fragments.erase(pFragment);

	//	return FragmentFatPtr{ .mpChunk = pChunk, .mpFragment = pSliced };
	//}

	//FreeSetAllocator::FragmentFatPtr FreeSetAllocator::CreateFragment(uint64_t size) {
	//	IMemoryHandle memory = pDevice->AllocateMemory(size);
	//	mChunks.emplace_back(memory.get());

	//	auto pChunk = std::prev(mChunks.end());
	//	auto pFragment = pChunk->mFragments.begin();

	//	return FragmentFatPtr{
	//		.mpChunk = pChunk,
	//		.mpFragment = pFragment
	//	};
	//}

	//FreeSetAllocator::FragmentFatPtr FreeSetAllocator::Allocate(uint64_t size, uint64_t alignment) {
	//	auto ppFragment = FindFragment(size, alignment);
	//	
	//	if (ppFragment != mAvailableFragments.end()) {
	//		auto pFragment = *ppFragment;
	//		auto alignedOffset = (alignment + pFragment->mOffset - 1) / pFragment->mOffset * pFragment->mOffset;
	//		auto pSlice = SliceFragment(pFragment, alignedOffset, size);
	//		return pSlice;
	//	}
	//	else {
	//		auto pFragment = CreateFragment(size);
	//		return pFragment;
	//	}
	//}



	//IAllocator::ChunkPool::ChunkPool(IAllocator & allocator) : mAllocator(allocator), mChunks(), mAvailableFragments() {}

	//IAllocator::ChunkPool::~ChunkPool() = default;

	//void IAllocator::ChunkPool::CreateChunk(IMemory * pMemory) {
	//	mChunks.push_back(Chunk(pMemory, mChunks.size()));
	//}

	//IBufferHandle IAllocator::ChunkPool::CreateBufferOnOldChunk(BufferDesc const & bufferDesc, MemoryRequirements const & memoryRequirements) {

	//	Fragment newFragment(0u, bufferDesc.size, ResourceState());

	//	for (auto ppFragment = mAvailableFragments.lower_bound(newFragment); ppFragment != mAvailableFragments.end(); ++ppFragment) {

	//		auto pFragment = *ppFragment;
	//		auto & chunk = mChunks[pFragment->mChunkIndex];
	//		auto & mFragments = chunk.mFragments;

	//		// The minimum valid offset, required by the "page size" of the preceding resource.

	//		uint64_t alignment = memoryRequirements.alignment;

	//		if (pFragment != mFragments.begin()) {
	//			auto precedingAlignment = mAllocator.GetAlignment(
	//				std::prev(pFragment)->mResourceState,
	//				pFragment->mResourceState
	//			);
	//			alignment = std::max(alignment, precedingAlignment);
	//		}

	//		uint64_t const availableOffset = (pFragment->mOffset + alignment - 1) / alignment * alignment;

	//		// The maximum available size, required by the "page size" of the following resource.

	//		int64_t availableSize = int64_t(pFragment->mSize) - int64_t(availableOffset - pFragment->mOffset);
	//		availableSize = std::max(0ll, availableSize);

	//		if (pFragment != mFragments.end()) {
	//			auto followingAlignment = mAllocator.GetAlignment(
	//				pFragment->mResourceState,
	//				std::next(pFragment)->mResourceState
	//			);

	//			auto alignedOffset = std::next(pFragment)->mOffset / followingAlignment * followingAlignment;

	//			availableSize = std::max(int64_t(alignedOffset) - int64_t(pFragment->mOffset), availableSize);
	//		}

	//		// If this fragment provides enough space, create a buffer at here.

	//		if (availableSize >= memoryRequirements.size) {
	//			auto bufferHandle = mAllocator.CreatePlacedBufferInAllocator(chunk.mMemoryHandle, availableOffset, alignment, bufferDesc);

	//			SliceFragment(pFragment, availableOffset, availableSize, ResourceState(bufferHandle.get()));

	//			return bufferHandle;
	//		}

	//	} // ! for

	//	return nullptr;

	//}



	//IAllocator::IAllocator() : mMemoryPools() {}

	//IAllocator::~IAllocator() = default;

	//IBufferHandle IAllocator::CreateBuffer(MemoryHeapType heapType, BufferDesc const & bufferDesc) {

	//	auto deviceHandle = GetDevice();

	//	auto memoryRequirements = deviceHandle->GetBufferMemoryRequirements(heapType, bufferDesc);
	//	auto & memoryTypeBits = memoryRequirements.memoryTypeBits;

	//	while (memoryTypeBits != 0) {
	//		uint32_t memoryTypeIndex = std::countr_zero(memoryTypeBits);
	//		memoryTypeBits &= ~(1u << memoryTypeIndex);


	//	}

	//}

}