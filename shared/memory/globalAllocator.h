#pragma once


#include "CustomAllocator.h"

namespace samurai
{
namespace memory
{
	void setGlobalAllocatorToStandard();
	
	void setGlobalAllocator(samurai::memory::CustomAllocator *allocator);

	void dissableAllocators();

}
}
