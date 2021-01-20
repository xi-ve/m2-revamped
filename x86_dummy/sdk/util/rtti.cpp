#include "rtti.h"

sdk::util::_RTTICompleteObjectLocator* sdk::util::get(_base_class* instance)
{
	return *(_RTTICompleteObjectLocator**)(instance->pVFTable - sizeof(_RTTICompleteObjectLocator*));
}
