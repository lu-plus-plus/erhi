#include <format>

#include "erhi/vulkan/context/message.hpp"
#include "erhi/vulkan/context/instance.hpp"
#include "erhi/vulkan/context/physical_device.hpp"



namespace erhi::vk {

	IInstanceHandle CreateInstance(InstanceDesc const & desc) {
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

		mMessageCallbackHandle->Verbose("supported Vulkan layers:");
		for (auto const & layer : layers) {
			mMessageCallbackHandle->Verbose(std::format("{}, ver.{}.{}.{}", layer.layerName, VK_API_VERSION_MAJOR(layer.specVersion), VK_API_VERSION_MINOR(layer.specVersion), VK_API_VERSION_PATCH(layer.specVersion)));
		}
		mMessageCallbackHandle->Verbose("");

		uint32_t extensionPropertyCount{ 0u };
		vkCheckResult(vkEnumerateInstanceExtensionProperties(nullptr, &extensionPropertyCount, nullptr));
		std::vector<VkExtensionProperties> extensions{ extensionPropertyCount };
		vkCheckResult(vkEnumerateInstanceExtensionProperties(nullptr, &extensionPropertyCount, extensions.data()));

		mMessageCallbackHandle->Verbose("supported Vulkan extensions:");
		for (auto const & extension : extensions) {
			mMessageCallbackHandle->Verbose(std::format("{}, ver.{}.{}.{}", extension.extensionName, VK_API_VERSION_MAJOR(extension.specVersion), VK_API_VERSION_MINOR(extension.specVersion), VK_API_VERSION_PATCH(extension.specVersion)));
		}
		mMessageCallbackHandle->Verbose("");

		// Create VkInstance.

		void * instanceCreateInfoPNext{ nullptr };

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
			.pUserData = mMessageCallbackHandle.get()
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



	Instance::operator VkInstance() const {
		return mInstance;
	}



	std::vector<IPhysicalDeviceHandle> Instance::ListPhysicalDevices() {
		std::vector<IPhysicalDeviceHandle> pPhysicalDevices(mPhysicalDevices.size(), nullptr);

		for (uint32_t i = 0; i < mPhysicalDevices.size(); ++i)
			pPhysicalDevices[i] = MakeHandle<PhysicalDevice>(this, mPhysicalDevices[i]);

		mMessageCallbackHandle->Verbose("physical devices:");
		for (auto const & pPhysicalDevice : pPhysicalDevices) {
			mMessageCallbackHandle->Verbose(std::format("{} physical device '{}'", pPhysicalDevice->type() == PhysicalDeviceType::Discrete ? "discrete" : "integrated", pPhysicalDevice->name()));
		}
		mMessageCallbackHandle->Verbose("");

		return pPhysicalDevices;
	}



	IPhysicalDeviceHandle Instance::SelectPhysicalDevice(PhysicalDeviceDesc const & desc) {
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