#define ERHI_ENABLE_NATIVE_DECLARATION

#include <format>

#include "erhi/vulkan/message.hpp"
#include "erhi/vulkan/instance.hpp"
#include "erhi/vulkan/physical_device.hpp"



namespace erhi::vk {

	IInstanceHandle createInstance(InstanceDesc const & desc) {
		return MakeHandle<Instance>(desc);
	}



	std::atomic<bool> Instance::gIsVolkInitialized = false;

	Instance::Instance(InstanceDesc const & desc) :
		IInstance{ desc },
		mInstance{ VK_NULL_HANDLE },
		mDebugUtilsMessenger{ VK_NULL_HANDLE },
		mPhysicalDevices{} {

		if (bool expected = false; gIsVolkInitialized.compare_exchange_strong(expected, true)) {
			// <todo> Unsafe operation: once it finds out that volk is not initialized, it immediately tag it as initialized. </todo>
			vkCheckResult(volkInitialize());
		}

		uint32_t layerPropertyCount{ 0u };
		vkCheckResult(vkEnumerateInstanceLayerProperties(&layerPropertyCount, nullptr));
		std::vector<VkLayerProperties> layers{ layerPropertyCount };
		vkCheckResult(vkEnumerateInstanceLayerProperties(&layerPropertyCount, layers.data()));

		mpMessageCallback->verbose("supported Vulkan layers:");
		for (auto const & layer : layers) {
			mpMessageCallback->verbose(std::format("{}, ver.{}.{}.{}", layer.layerName, VK_API_VERSION_MAJOR(layer.specVersion), VK_API_VERSION_MINOR(layer.specVersion), VK_API_VERSION_PATCH(layer.specVersion)));
		}
		mpMessageCallback->verbose("");

		uint32_t extensionPropertyCount{ 0u };
		vkCheckResult(vkEnumerateInstanceExtensionProperties(nullptr, &extensionPropertyCount, nullptr));
		std::vector<VkExtensionProperties> extensions{ extensionPropertyCount };
		vkCheckResult(vkEnumerateInstanceExtensionProperties(nullptr, &extensionPropertyCount, extensions.data()));

		mpMessageCallback->verbose("supported Vulkan extensions:");
		for (auto const & extension : extensions) {
			mpMessageCallback->verbose(std::format("{}, ver.{}.{}.{}", extension.extensionName, VK_API_VERSION_MAJOR(extension.specVersion), VK_API_VERSION_MINOR(extension.specVersion), VK_API_VERSION_PATCH(extension.specVersion)));
		}
		mpMessageCallback->verbose("");

		// Create VkInstance.

		void * instanceCreateInfoPNext{ nullptr };

		VkApplicationInfo applicationInfo{
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pNext = nullptr,
			.pApplicationName = "",
			.applicationVersion = VK_MAKE_VERSION(0, 0, 1),
			.pEngineName = "erhi",
			.engineVersion = VK_MAKE_VERSION(0, 0, 1),
			.apiVersion = VK_MAKE_VERSION(1, 3, 0)
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

		uint32_t physicalDeviceCount{ 0u };
		vkEnumeratePhysicalDevices(mInstance, &physicalDeviceCount, nullptr);
		mPhysicalDevices.resize(physicalDeviceCount);
		vkEnumeratePhysicalDevices(mInstance, &physicalDeviceCount, mPhysicalDevices.data());
	}



	Instance::~Instance() {
		if (mDebugUtilsMessenger != VK_NULL_HANDLE) {
			vkDestroyDebugUtilsMessengerEXT(mInstance, mDebugUtilsMessenger, nullptr);
		}
		vkDestroyInstance(mInstance, nullptr);
	}



	std::vector<IPhysicalDeviceHandle> Instance::listPhysicalDevices() {
		std::vector<IPhysicalDeviceHandle> pPhysicalDevices(mPhysicalDevices.size(), nullptr);

		for (uint32_t i = 0; i < mPhysicalDevices.size(); ++i)
			pPhysicalDevices[i] = MakeHandle<PhysicalDevice>(this, mPhysicalDevices[i]);

		mpMessageCallback->verbose("physical devices:");
		for (auto const & pPhysicalDevice : pPhysicalDevices) {
			mpMessageCallback->verbose(std::format("{} physical device '{}'", pPhysicalDevice->type() == PhysicalDeviceType::Discrete ? "discrete" : "integrated", pPhysicalDevice->name()));
		}
		mpMessageCallback->verbose("");

		return pPhysicalDevices;
	}

	IPhysicalDeviceHandle Instance::selectPhysicalDevice(PhysicalDeviceDesc const & desc) {
		for (auto physicalDevice : mPhysicalDevices) {
			auto handle{ MakeHandle<PhysicalDevice>(this, physicalDevice) };
			if (handle->type() == desc.type) {
				return handle;
			}
		}

		throw std::runtime_error("Failed to find a proper physical device.");

		return nullptr;
	}

}