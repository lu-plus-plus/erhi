#ifndef ERHI_MEMORY_HPP
#define ERHI_MEMORY_HPP

#include "../common.hpp"



namespace erhi {

	struct IMemory : IObject {
		
		IMemory();
		virtual ~IMemory() override;

		virtual IDeviceHandle	GetDevice() const = 0;
		virtual uint64_t		Size() const = 0;

	};

}



#endif // ERHI_MEMORY_HPP