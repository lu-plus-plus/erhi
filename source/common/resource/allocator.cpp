#include "erhi/common/resource/allocator.hpp"

#include "erhi/common/context/device.hpp"
#include "erhi/common/resource/memory.hpp"

#include <cassert>		// for assertion
#include <bit>			// for bit manipulation



namespace erhi {

	LinearMemoryPool::LinearMemoryPool(IDevice * pDevice) : mpDevice(pDevice), mArenas() {}

	LinearMemoryPool::~LinearMemoryPool() = default;

	IMemoryHandle LinearMemoryPool::AllocateMemory(uint64_t size, uint64_t alignment) {
		
	}



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
	//		.mBlocks = std::list(1, Block{
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