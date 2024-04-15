#include <containers.h>
#include <assert/assert.h>




#define SAMURAI_DECLARE_CONTAINER(x)	if (std::strcmp(name, #x ) == 0)						\
{																							\
if (sizeof(x) != memoryArena->containerStructMemory.size) { return nullptr; }				\
	return new(memoryArena->containerStructMemory.block)  x ();								\
}																							\
else


//this should not allocate memory
Container *getContainer(const char *name, samurai::memory::MemoryArena *memoryArena)
{


	SAMURAI_ALL_CONTAINERS()
	{
		//"invalid container name: "
		return nullptr;
	}

}

#undef SAMURAI_DECLARE_CONTAINER