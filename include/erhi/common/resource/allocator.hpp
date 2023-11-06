#pragma once

#include <list>			// for linear pool
#include <vector>		// for linear allocator

#include "../common.hpp"
#include "memory.hpp"



namespace erhi {

	/*
		An allocator deals with all the details in memory allocation, including memory type, alignment, and "page size", etc.
	*/

	struct IAllocator : IObject {

	protected:

		IDeviceHandle mDeviceHandle;

	public:

		IAllocator(IDevice * pDevice);
		virtual ~IAllocator();

		IDevice * GetDevice() const;

		virtual IBufferHandle CreateBuffer(MemoryHeapType heapType, BufferDesc const & desc) = 0;
		virtual ITextureHandle CreateTexture(MemoryHeapType heapType, TextureDesc const & desc) = 0;

	};



	struct IAllocator : IObject {
		struct MemoryInterface : IObject {
			virtual IObjectHandle Allocate(uint64_t size) = 0;
			virtual void Free(IObjectHandle memory) = 0;
		};
		

	};

	namespace VirtualAllocation {

		template <typename T>
		using ListType = std::list<T>;

		struct Fragment {
			uint64_t mOffset;
			uint64_t mSize;
		};

		struct Arena {
			uint64_t mSize;
			uint64_t mFreeRegionBegin;
			uint64_t mFreeRegionEnd;
			ListType<Fragment> mFragments;

			Arena(uint64_t size);
			~Arena();

			ListType<Fragment>::iterator TryToAllocate(uint64_t size, uint64_t alignment);
			void Free(ListType<Fragment>::iterator pFragment);
		};

		struct Pool;

		struct AllocatedRange {
			Pool * mPool;
			ListType<Arena>::iterator mArena;
			ListType<Fragment>::iterator mFragment;

			AllocatedRange(Pool * pPool, ListType<Arena>::iterator pArena, ListType<Fragment>::iterator pFragment);
			~AllocatedRange();
		};

		struct Pool {
			static constexpr uint64_t gDefaultArenaSize = 1u << 16u;
			ListType<Arena> mArenas;

			AllocatedRange Allocate(uint64_t size, uint64_t alignment);
		};

	}

	namespace VirtAlloc::Linear {

		template <typename T>
		using ListType = std::list<T>;

		template <typename T>
		using VectorType = std::vector<T>;

		struct Fragment {
			uint64_t mOffset;
			uint64_t mSize;
		};

		struct FragmentRef {
			struct Arena * mpArena;
			ListType<Fragment>::iterator mpSelf;

			FragmentRef(Arena * pArena, ListType<Fragment>::iterator pSelf);
			FragmentRef(FragmentRef const &) = delete;
			FragmentRef & operator=(FragmentRef const &) = delete;
			FragmentRef(FragmentRef && other) noexcept;
			FragmentRef & operator=(FragmentRef &&) noexcept;
			~FragmentRef();

			void Free();
			void Invalidate();

			bool IsValid() const;
		};

		struct Arena {
			uint64_t mSize;
			uint64_t mFreeRegionBegin;
			uint64_t mFreeRegionEnd;
			ListType<Fragment> mFragments;

			Arena(uint64_t size);
			~Arena();

			FragmentRef TryAllocate(uint64_t size, uint64_t alignment);
			void Free(ListType<Fragment>::iterator pFragment);
		};

		struct Pool {
			VectorType<Arena> mArenas;
		};

	};



	struct ILinearAllocator : IAllocator {

		static constexpr uint64_t gDefaultArenaSize = 1u << 16u;

		ILinearAllocator(IDevice * pDevice);
		virtual ~ILinearAllocator() override;

		virtual IBufferHandle CreateBuffer(MemoryHeapType heapType, BufferDesc const & desc) override;
		virtual ITextureHandle CreateTexture(MemoryHeapType heapType, TextureDesc const & desc) override;

	protected:

		// Each arena is a large block of memory.
		struct Arena {
			VirtAlloc::Linear::Arena mArenaImpl;
			IMemoryHandle mMemoryHandle;
		};

		// Each pool is for a specific memory type.
		struct Pool {
			std::vector<Arena> mArenas;
		};

		std::vector<Pool> mPools;

		VirtAlloc::Linear::FragmentRef CreateFragment(MemoryRequirements requirements);

		virtual IBufferHandle CreateBuffer(VirtAlloc::Linear::FragmentRef && fragment, BufferDesc const & desc) = 0;
		virtual ITextureHandle CreateTexture(VirtAlloc::Linear::FragmentRef && fragement, TextureDesc const & desc) = 0;

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