#ifndef ERHI_MEMORY_HPP
#define ERHI_MEMORY_HPP

#include "../object.hpp"
#include "../handle.hpp"

#include "../device.hpp"



namespace erhi {

	struct IMemoryView {
		virtual uint32_t size() const = 0;
		virtual uint32_t offset() const = 0;
	};

	struct IMemory : IObject {
		
		IMemory();
		virtual ~IMemory() override;

		virtual IDevice *	pDevice() const = 0;

		virtual uint32_t	size() const = 0;
		virtual uint32_t	offset() const = 0;
		virtual uint32_t	alignment() const = 0;

	};

}



#endif // ERHI_MEMORY_HPP