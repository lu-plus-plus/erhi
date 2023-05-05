#pragma once

#include <list>		// for linear allocator
#include <vector>

#include "../common.hpp"



namespace erhi {

	/*
		An allocator deals with all the details in memory allocation, including memory types, chunks, alignment and page size, etc.
		A memory pool simply manages one or more chunks of memory.
	*/

	struct MemoryPoolDesc {
		uint64_t blockSize;
		uint32_t minBlockCount;
		uint32_t maxBlockCount;
	};

	struct IMemoryPool : IObject {

	private:

		IDeviceHandle mDeviceHandle;
		MemoryPoolDesc mDesc;

	public:

		IMemoryPool(IDevice * pDevice, MemoryPoolDesc const & desc);
		virtual ~IMemoryPool();

		IDevice * GetDevice() const;
		MemoryPoolDesc const & GetDesc() const;

		virtual IMemoryHandle AllocateMemory(uint64_t size, uint64_t alignment) = 0;

	};

	struct IAllocator : IObject {

	private:

		IDeviceHandle mDeviceHandle;

	public:

		IAllocator(IDevice * pDevice);
		virtual ~IAllocator();

		IDevice * GetDevice() const;

		virtual IBufferHandle				CreateBuffer(MemoryHeapType heapType, BufferDesc const & desc) = 0;
		virtual ITextureHandle				CreateTexture(MemoryHeapType heapType, TextureDesc const & desc) = 0;

	};



	struct LinearMemoryPool : IMemoryPool {

		struct Block;

		struct Fragment {
			uint64_t mOffset;
			uint64_t mSize;
		};

		struct FragmentMemory : IMemory {
			IMemoryHandle mBlock;
			Block * mpBlock;
			std::list<Fragment>::iterator mpSelf;
		};

		struct Block {
			uint64_t mFreeRegionBegin;
			uint64_t mFreeRegionEnd;
			std::list<Fragment> mFragments;
		};

		std::vector<Block> mBlocks;
		MemoryPoolDesc mDesc;

		LinearMemoryPool(IDevice * pDevice, MemoryPoolDesc const & desc);
		virtual ~LinearMemoryPool();

		virtual IMemoryHandle AllocateMemory(uint64_t size, uint64_t alignment) override;

	};

	//struct BuddyAllocator : IAllocator {

	//	struct Block;
	//	struct Arena;
	//	struct BlockDesc;

	//	struct Block {
	//		int mOrder;
	//		uint64_t mOffset;
	//		std::list<BlockDesc>::iterator mpFreeListDesc;

	//		uint64_t Size() const { return 1llu << mOrder; }
	//		uint64_t Offset() const { return mOffset; }
	//	};

	//	struct Arena {
	//		int mOrder;
	//		IMemoryHandle mMemoryHandle;
	//		std::list<Block> mBlocks;

	//		uint64_t Size() const { return 1llu << mOrder; }
	//	};

	//	struct BlockDesc {
	//		std::list<Arena>::iterator mpArena;
	//		std::list<Block>::iterator mpBlock;

	//		uint64_t Size() const { return mpBlock->Size(); }
	//		uint64_t Offset() const { return mpBlock->Offset(); }
	//		IMemoryHandle GetMemoryHandle() const { return mpArena->mMemoryHandle; }
	//	};

	//	// The size of an arena is at least 4 MB.
	//	// This magic number is inherent from the maximum resource alignment in DX12.
	//	static constexpr int minArenaOrder = 22;

	//	static constexpr int maxOrder = 32;

	//	std::list<Arena> mArenas;
	//	std::list<BlockDesc> mFreeLists[maxOrder];

	//	[[nodiscard]] BuddyAllocator(IDevice * pDevice, FnAllocateCallback const & allocate);
	//	virtual ~BuddyAllocator();

	//	[[nodiscard]] BlockDesc AllocateBlock(uint64_t size, uint64_t alignment);
	//	[[nodiscard]] void FreeBlock(BlockDesc const & block);

	//protected:

	//	BlockDesc AllocateArenaInternal(int order);

	//	BlockDesc MakeBlock(int order, uint64_t offset);

	//	void SliceBlockInternal(BlockDesc & freeBlock, int slicedOrder);
	//	void MergeBuddyBlocksInternal(BlockDesc & blockDesc);
	//	//BlockDesc HalveBlockInternal(BlockDesc const & blockDesc);
	//	
	//	bool IsBlockLeftBuddy(Block const & block);
	//	bool IsBlockFree(Block const & block);
	//	bool GetBuddy(Block const & block);

	//};

	//struct FreeSetAllocator {

	//	template <typename T>
	//	using List = std::list<T>;

	//	template <typename Key, typename Compare>
	//	using Multiset = std::multiset<Key, Compare>;

	//	struct Fragment {
	//		uint64_t mOffset;
	//		uint64_t mSize;
	//	};

	//	struct Chunk {
	//		IMemoryHandle mMemoryHandle;
	//		List<Fragment> mFragments;

	//		Chunk(IMemory * pMemory);
	//		~Chunk();
	//	};

	//	struct FragmentFatPtr {
	//		List<Chunk>::iterator mpChunk;
	//		List<Fragment>::iterator mpFragment;

	//		Fragment * operator->() const { return &(*mpFragment); }
	//		operator List<Fragment>::iterator const & () const { return mpFragment; }

	//		struct Sorter {
	//			using is_transparent = std::true_type;

	//			bool operator()(FragmentFatPtr const & p, Fragment const & f) const {
	//				return p->mSize < f.mSize || (p->mSize == f.mSize && p->mOffset < f.mOffset);
	//			}

	//			bool operator()(Fragment const & f, FragmentFatPtr const & p) const {
	//				return f.mSize < p->mSize || (f.mSize == p->mSize && f.mOffset < p->mOffset);
	//			}

	//			bool operator()(FragmentFatPtr const & a, FragmentFatPtr const & b) const {
	//				return a->mSize < b->mSize || (a->mSize == b->mSize && a->mOffset < b->mOffset);
	//			}
	//		};
	//	};

	//	IDevice * pDevice;
	//	List<Chunk> mChunks;
	//	Multiset<FragmentFatPtr, FragmentFatPtr::Sorter> mAvailableFragments;

	//	FreeSetAllocator() = default;
	//	FreeSetAllocator(FreeSetAllocator const &) = delete;
	//	FreeSetAllocator & operator=(FreeSetAllocator const &) = delete;
	//	~FreeSetAllocator() = default;

	//	Multiset<FragmentFatPtr, FragmentFatPtr::Sorter>::iterator FindFragment(uint64_t size, uint64_t alignment);
	//	FragmentFatPtr SliceFragment(FragmentFatPtr & pFragment, uint64_t absoluteOffset, uint64_t size);
	//	FragmentFatPtr CreateFragment(uint64_t size);

	//	FragmentFatPtr Allocate(uint64_t size, uint64_t alignment);


	//	struct ChunkPool {

	//		IAllocator & mAllocator;

	//		ChunkPool(IAllocator & allocator);
	//		~ChunkPool();

	//		std::list<Fragment>::iterator SliceFragment(std::list<Fragment>::iterator pFragment, uint64_t absoluteOffset, uint64_t size, ResourceState resourceState);
	//		void CreateChunk(IMemory * pMemory);

	//		IBufferHandle CreateBufferOnOldChunk(BufferDesc const & bufferDesc, MemoryRequirements const & memoryRequirements);
	//		IBufferHandle CreateBufferOnNewChunk(BufferDesc const & bufferDesc, MemoryRequirements const & memoryRequirements);

	//	};

	//protected:

	//	virtual uint64_t GetAlignment(ResourceState const & precedingResource, ResourceState const & followingResource) = 0;
	//	virtual IBufferHandle CreatePlacedBufferInAllocator(IMemoryHandle memoryHandle, uint64_t offset, uint64_t alignment, BufferDesc const & desc) = 0;

	//public:

		//virtual IBufferHandle CreateBuffer(MemoryHeapType heapType, BufferDesc const & bufferDesc);

		//virtual IDeviceHandle GetDevice() const = 0;

	//};

}