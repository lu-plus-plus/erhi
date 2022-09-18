#ifndef ERHI_MEMORY_HPP
#define ERHI_MEMORY_HPP

#include "../object.hpp"
#include "../handle.hpp"



namespace erhi {

	enum class MemoryProperty : uint32_t {
		DeviceLocal = 0b0001,
		HostUpload = 0b0010,
		HostReadBack = 0b0100
	};
	//MemoryProperty const i = MemoryProperty::DeviceLocal | MemoryProperty::DeviceLocal;

	//struct MemoryProperty {
	//	enum class Flags : uint32_t {
	//		DeviceLocal = 0b0001,
	//		HostUpload = 0b0010,
	//		HostReadBack = 0b0100
	//	} mFlags;

	//	MemoryProperty() : mFlags(DeviceLocal | HostUpload) {}
	//};

	struct Memory {

	private:

		uint32_t mRequiredProperties;
		uint32_t mExpectedProperties;

	public:

		Memory() : mRequiredProperties(0u), mExpectedProperties(0u) {}

		Memory & require(MemoryProperty property) {
			mRequiredProperties |= static_cast<uint32_t>(property);
			return *this;
		}

		Memory & expect(MemoryProperty property) {
			mExpectedProperties |= static_cast<uint32_t>(property);
			return *this;
		}

	};

	struct Allocator {

		IDeviceHandle mpDevice;

	};

}



#endif // ERHI_MEMORY_HPP