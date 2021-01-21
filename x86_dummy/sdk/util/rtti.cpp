#include "rtti.h"

sdk::util::_RTTICompleteObjectLocator* sdk::util::get(_base_class* instance)
{
	if (!instance || !instance->pVFTable || !instance->pVFTable->pmethods) return 0;
	auto w = *(uint32_t*)(instance->pVFTable - sizeof(_RTTICompleteObjectLocator*));
	if (!w || IsBadCodePtr((FARPROC)w)) return 0;
	//auto a = instance->pVFTable - sizeof(_RTTICompleteObjectLocator*);
	//auto b = *(_RTTICompleteObjectLocator**)(a);
	//if (!b || !b->pTypeDescriptor) return 0;
	return *(_RTTICompleteObjectLocator**)(instance->pVFTable - sizeof(_RTTICompleteObjectLocator*));
}
