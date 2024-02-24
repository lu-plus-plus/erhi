#include "erhi/common/resource/descriptor.hpp"

namespace erhi {

	ITextureView::ITextureView(TextureViewDesc const & desc) : mDesc(desc) {}
	ITextureView::~ITextureView() = default;

	ICPUDescriptorHeap::ICPUDescriptorHeap(DescriptorHeapDesc const & desc) : mDesc(desc) {}
	ICPUDescriptorHeap::~ICPUDescriptorHeap() = default;

	IGPUDescriptorHeap::IGPUDescriptorHeap(DescriptorHeapDesc const & desc) : mDesc(desc) {}
	IGPUDescriptorHeap::~IGPUDescriptorHeap() = default;

	IDescriptorSetLayout::IDescriptorSetLayout(DescriptorSetLayoutDesc const & desc) : mBindings(desc.bindings, desc.bindings + desc.bindingCount), mDescriptorHeapType(desc.descriptorHeapType) {}
	IDescriptorSetLayout::~IDescriptorSetLayout() = default;

}