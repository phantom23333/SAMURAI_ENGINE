#include "globalAllocator.h"
#include <malloc.h>
#include <fstream>

#include "CustomAllocator.h"


void *DefaultAllocator(size_t size)
{
	return malloc(size);
}
void DefaultFree(void *ptr)
{
	free(ptr);
}

void *DisabeledAllocator(size_t size)
{
	return 0;
}
void DisabeledFree(void *ptr)
{
	(void)ptr;
}

samurai::memory::CustomAllocator *currentCustomAllocator = {};
void *CustomedAllocator(size_t size)
{
	return currentCustomAllocator->allocate(size);
}
void CustomFree(void *ptr)
{
	currentCustomAllocator->free(ptr);
}

void* (*GlobalAllocateFunction)(size_t) = DefaultAllocator;
void  (*GlobalFree)(void *) = DefaultFree;

namespace samurai
{
namespace memory
{
	void setGlobalAllocatorToStandard()
	{
		GlobalAllocateFunction = DefaultAllocator;
		GlobalFree = DefaultFree;
	}

	void dissableAllocators()
	{
		GlobalAllocateFunction = DisabeledAllocator;
		GlobalFree = DisabeledFree;
	}

	void setGlobalAllocator(samurai::memory::CustomAllocator *allocator)
	{
		currentCustomAllocator = allocator;
		GlobalAllocateFunction = CustomedAllocator;
		GlobalFree = CustomFree;
	}
}
}



void *operator new  (size_t count)
{
	return GlobalAllocateFunction(count);
}

void *operator new[](size_t count)
{
	return GlobalAllocateFunction(count);
}

void operator delete  (void *ptr)
{

	GlobalFree(ptr);
}

void operator delete[](void *ptr)
{
	GlobalFree(ptr);
}