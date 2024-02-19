#include "erhi/vulkan/context/context.hpp"
#include "erhi/vulkan/command/command.hpp"

#include <vector>
#include <format>



namespace erhi::vk {

	IDeviceHandle CreateDevice(DeviceDesc const & desc, std::shared_ptr<IMessageCallback> pMessageCallback) {
		return new Device(desc, pMessageCallback);
	}



	Device::Device(DeviceDesc const & desc, std::shared_ptr<IMessageCallback> pMessageCallback) :
		IDevice(desc, pMessageCallback),
		mInstance(VK_NULL_HANDLE),
		mDebugUtilsMessenger(VK_NULL_HANDLE),
		mpPhysicalDevice(),
		mDevice(VK_NULL_HANDLE),
		mGraphicsQueueFamilyIndex(std::numeric_limits<uint32_t>::max()),
		mComputeQueueFamilyIndex(std::numeric_limits<uint32_t>::max()),
		mCopyQueueFamilyIndex(std::numeric_limits<uint32_t>::max()),
		mAllocator(VK_NULL_HANDLE) {

		// GLFW as Window System Interface

		if (GLFW_FALSE == glfwInit()) {
			throw std::runtime_error("failed to initialize the GLFW library");
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		// Vulkan loader

		vkCheckResult(volkInitialize());

		// layers

		uint32_t layerPropertyCount = 0u;
		vkCheckResult(vkEnumerateInstanceLayerProperties(&layerPropertyCount, nullptr));
		std::vector<VkLayerProperties> layers(layerPropertyCount);
		vkCheckResult(vkEnumerateInstanceLayerProperties(&layerPropertyCount, layers.data()));

		mpMessageCallback->Verbose("supported Vulkan layers:");
		for (auto const & layer : layers) {
			mpMessageCallback->Verbose(std::format("{}, ver.{}.{}.{}", layer.layerName, VK_API_VERSION_MAJOR(layer.specVersion), VK_API_VERSION_MINOR(layer.specVersion), VK_API_VERSION_PATCH(layer.specVersion)));
		}
		mpMessageCallback->Verbose("");

		// extensions

		uint32_t extensionPropertyCount{ 0u };
		vkCheckResult(vkEnumerateInstanceExtensionProperties(nullptr, &extensionPropertyCount, nullptr));
		std::vector<VkExtensionProperties> extensions{ extensionPropertyCount };
		vkCheckResult(vkEnumerateInstanceExtensionProperties(nullptr, &extensionPropertyCount, extensions.data()));

		mpMessageCallback->Verbose("supported Vulkan extensions:");
		for (auto const & extension : extensions) {
			mpMessageCallback->Verbose(std::format("{}, ver.{}.{}.{}", extension.extensionName, VK_API_VERSION_MAJOR(extension.specVersion), VK_API_VERSION_MINOR(extension.specVersion), VK_API_VERSION_PATCH(extension.specVersion)));
		}
		mpMessageCallback->Verbose("");

		// create VkInstance

		void * instanceCreateInfoPNext = nullptr;

		VkApplicationInfo applicationInfo{
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pNext = nullptr,
			.pApplicationName = "",
			.applicationVersion = VK_MAKE_VERSION(0, 0, 1),
			.pEngineName = "erhi",
			.engineVersion = VK_MAKE_VERSION(0, 0, 1),
			.apiVersion = VK_API_VERSION_1_3
		};

		std::vector<char const *> pEnabledLayerNames;

		std::vector<char const *> pEnabledExtensionNames;

		VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
			.pNext = nullptr,
			.flags = 0,
			.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
			.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
			.pfnUserCallback = adaptToMessageCallback,
			.pUserData = mpMessageCallback.get()
		};

		if (desc.enableDebug) {
			pEnabledLayerNames.push_back("VK_LAYER_KHRONOS_validation");
			pEnabledExtensionNames.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			instanceCreateInfoPNext = &debugUtilsMessengerCreateInfo;
		}

		uint32_t glfwExtensionCount = 0;
		char const * * glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		for (uint32_t i = 0; i < glfwExtensionCount; ++i)
			pEnabledExtensionNames.push_back(glfwExtensions[i]);

		VkInstanceCreateInfo instanceCreateInfo{
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pNext = instanceCreateInfoPNext,
			.pApplicationInfo = &applicationInfo,
			.enabledLayerCount = uint32_t(pEnabledLayerNames.size()),
			.ppEnabledLayerNames = pEnabledLayerNames.data(),
			.enabledExtensionCount = uint32_t(pEnabledExtensionNames.size()),
			.ppEnabledExtensionNames = pEnabledExtensionNames.data()
		};

		vkCheckResult(vkCreateInstance(&instanceCreateInfo, nullptr, &mInstance));

		volkLoadInstance(mInstance);

		if (desc.enableDebug) {
			vkCheckResult(vkCreateDebugUtilsMessengerEXT(mInstance, &debugUtilsMessengerCreateInfo, nullptr, &mDebugUtilsMessenger));
		}

		// list physical devices

		std::vector<VkPhysicalDevice> availableVkPhysicalDevices;

		uint32_t physicalDeviceCount{ 0u };
		vkEnumeratePhysicalDevices(mInstance, &physicalDeviceCount, nullptr);
		availableVkPhysicalDevices.resize(physicalDeviceCount);
		vkEnumeratePhysicalDevices(mInstance, &physicalDeviceCount, availableVkPhysicalDevices.data());

		// select a physical device

		for (auto pd : availableVkPhysicalDevices) {
			std::unique_ptr<PhysicalDevice> pPhysicalDevice = std::make_unique<PhysicalDevice>(pd);

			bool const is_high_performance =
				desc.physicalDevicePreference == PhysicalDevicePreference::HighPerformance
				and pPhysicalDevice->DeviceType() == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
			bool const is_minimal_power =
				desc.physicalDevicePreference == PhysicalDevicePreference::MinimalPower
				and pPhysicalDevice->DeviceType() == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;

			if (is_high_performance or is_minimal_power) {
				mpPhysicalDevice = std::move(pPhysicalDevice);
				break;
			}
		}

		if (not mpPhysicalDevice)
			throw std::runtime_error("Failed to find a proper physical device.");

		// create a Vulkan device, along with
		// queues for Graphics & Compute & Transfer, asynchronous compute, and asynchronous transfer

		constexpr VkQueueFlags graphicsQueueFlags{ VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT };
		constexpr VkQueueFlags computeQueueFlags{ VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT };
		constexpr VkQueueFlags copyQueueFlags{ VK_QUEUE_TRANSFER_BIT };

		for (uint32_t queueFamilyIndex = 0; queueFamilyIndex < mpPhysicalDevice->mQueueFamilies.size(); ++queueFamilyIndex) {
			auto const & queueFamilyProperties = mpPhysicalDevice->mQueueFamilies[queueFamilyIndex];

			if ((queueFamilyProperties.queueFamilyProperties.queueFlags & graphicsQueueFlags) == graphicsQueueFlags) {
				mGraphicsQueueFamilyIndex = queueFamilyIndex;
			}
			else if ((queueFamilyProperties.queueFamilyProperties.queueFlags & computeQueueFlags) == computeQueueFlags) {
				mComputeQueueFamilyIndex = queueFamilyIndex;
			}
			else if ((queueFamilyProperties.queueFamilyProperties.queueFlags & copyQueueFlags) == copyQueueFlags) {
				mCopyQueueFamilyIndex = queueFamilyIndex;
			}
		}

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

		auto GetDeviceQueueCreateInfo = [] (uint32_t queueFamilyIndex) -> VkDeviceQueueCreateInfo {
			static float const queuePriority{ 1.0f };
			VkDeviceQueueCreateInfo createInfo{
				.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.queueFamilyIndex = queueFamilyIndex,
				.queueCount = 1,
				.pQueuePriorities = &queuePriority
			};
			return createInfo;
		};

		if (mGraphicsQueueFamilyIndex == std::numeric_limits<uint32_t>::max()) [[unlikely]] {
			throw std::runtime_error(std::format("Failed to find a graphics & compute & transfer queue family on device '{}'.", mpPhysicalDevice->DeviceName()));
		}
		queueCreateInfos.push_back(GetDeviceQueueCreateInfo(mGraphicsQueueFamilyIndex));

		if (mComputeQueueFamilyIndex == std::numeric_limits<uint32_t>::max()) [[unlikely]] {
			throw std::runtime_error(std::format("Failed to find a asynchronous compute queue family on device '{}'.", mpPhysicalDevice->DeviceName()));
		}
		queueCreateInfos.push_back(GetDeviceQueueCreateInfo(mComputeQueueFamilyIndex));

		if (mCopyQueueFamilyIndex == std::numeric_limits<uint32_t>::max()) [[unlikely]] {
			throw std::runtime_error(std::format("Failed to find a asynchronous transfer queue family on device '{}'.", mpPhysicalDevice->DeviceName()));
		}
		queueCreateInfos.push_back(GetDeviceQueueCreateInfo(mCopyQueueFamilyIndex));

		std::vector<char const *> const enabledDeviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		VkDeviceCreateInfo deviceCreateInfo{
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
			.pQueueCreateInfos = queueCreateInfos.data(),
			.enabledLayerCount = 0,
			.ppEnabledLayerNames = nullptr,
			.enabledExtensionCount = uint32_t(enabledDeviceExtensions.size()),
			.ppEnabledExtensionNames = enabledDeviceExtensions.data(),
			.pEnabledFeatures = nullptr
		};

		VkPhysicalDeviceFeatures2 physicalDeviceFeature{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };

		if (not mpPhysicalDevice->mBufferDeviceAddressFeatures.bufferDeviceAddress) {
			throw std::runtime_error("Vulkan feature `buffer device address` is not supported");
		}
		VkPhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddressExtension{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES };
		bufferDeviceAddressExtension.bufferDeviceAddress = true;

		NextChain(deviceCreateInfo)
			.Next(physicalDeviceFeature)
			.Next(bufferDeviceAddressExtension);

		vkCheckResult(vkCreateDevice(*mpPhysicalDevice, &deviceCreateInfo, nullptr, &mDevice));

		mPrimaryQueue = std::make_shared<Queue>(mDevice, QueueType::Primary, mGraphicsQueueFamilyIndex);
		mAsyncComputeQueue = std::make_shared<Queue>(mDevice, QueueType::AsyncCompute, mComputeQueueFamilyIndex);
		mAsyncCopyQueue = std::make_shared<Queue>(mDevice, QueueType::AsyncCopy, mCopyQueueFamilyIndex);

		// create Vulkan memory allocator

		VmaVulkanFunctions const vulkanFunctions = {
			.vkGetInstanceProcAddr = vkGetInstanceProcAddr,
			.vkGetDeviceProcAddr = vkGetDeviceProcAddr,
			.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties,
			.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties,
			.vkAllocateMemory = vkAllocateMemory,
			.vkFreeMemory = vkFreeMemory,
			.vkMapMemory = vkMapMemory,
			.vkUnmapMemory = vkUnmapMemory,
			.vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges,
			.vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges,
			.vkBindBufferMemory = vkBindBufferMemory,
			.vkBindImageMemory = vkBindImageMemory,
			.vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements,
			.vkGetImageMemoryRequirements = vkGetImageMemoryRequirements,
			.vkCreateBuffer = vkCreateBuffer,
			.vkDestroyBuffer = vkDestroyBuffer,
			.vkCreateImage = vkCreateImage,
			.vkDestroyImage = vkDestroyImage,
			.vkCmdCopyBuffer = vkCmdCopyBuffer,
			.vkGetBufferMemoryRequirements2KHR = vkGetBufferMemoryRequirements2,
			.vkGetImageMemoryRequirements2KHR = vkGetImageMemoryRequirements2,
			.vkBindBufferMemory2KHR = vkBindBufferMemory2,
			.vkBindImageMemory2KHR = vkBindImageMemory2,
			.vkGetPhysicalDeviceMemoryProperties2KHR = vkGetPhysicalDeviceMemoryProperties2,
			.vkGetDeviceBufferMemoryRequirements = vkGetDeviceBufferMemoryRequirements,
			.vkGetDeviceImageMemoryRequirements = vkGetDeviceImageMemoryRequirements
		};
		
		VmaAllocatorCreateInfo allocatorCreateInfo = {};
		allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_3;
		allocatorCreateInfo.physicalDevice = *mpPhysicalDevice;
		allocatorCreateInfo.device = mDevice;
		allocatorCreateInfo.instance = mInstance;
		allocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;
		allocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

		vkCheckResult(vmaCreateAllocator(&allocatorCreateInfo, &mAllocator));
	}



	Device::~Device() {
		vmaDestroyAllocator(mAllocator);

		vkDestroyDevice(mDevice, nullptr);

		if (mDebugUtilsMessenger != VK_NULL_HANDLE) {
			vkDestroyDebugUtilsMessengerEXT(mInstance, mDebugUtilsMessenger, nullptr);
		}
		vkDestroyInstance(mInstance, nullptr);

		// Vulkan loader

		volkFinalize();

		// GLFW

		glfwTerminate();
	}



	Device::operator VkDevice() const {
		return mDevice;
	}

}