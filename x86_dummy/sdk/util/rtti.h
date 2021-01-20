#pragma once
#include <inc.h>
namespace sdk
{
	namespace util
	{
		struct _base_class
		{
			struct _vftable
			{
				void* pmethods[];
			}*pVFTable;
		};
		struct _TypeDescriptor {
			_base_class::_vftable* pVFTable;
			uintptr_t spare;
			char* pname;
		};
		struct _RTTIClassHierarchyDescriptor;
		struct _RTTIBaseClassDescriptor {
			_TypeDescriptor* pTypeDescriptor;
			unsigned long numContainedBases;
			struct _PMD {
				ptrdiff_t mdisp;
				ptrdiff_t pdisp;
				ptrdiff_t vdisp;
			} where;
			unsigned long attributes;
			_RTTIClassHierarchyDescriptor* pClassDescriptor;
		};
		struct _RTTIClassHierarchyDescriptor {
			unsigned long signature;
			unsigned long attributes;
			unsigned long numBaseClasses;
			struct _RTTIBaseClassArray {
				_RTTIBaseClassDescriptor* ppBaseClassDescriptor[];
			}*pppBaseClassDescriptor;
		};
		struct _RTTICompleteObjectLocator {
			unsigned long signature;
			unsigned long offset;
			unsigned long cdOffset;
			_TypeDescriptor* pTypeDescriptor;
			_RTTIClassHierarchyDescriptor* pClassHierarchyDescriptor;
		};
		_RTTICompleteObjectLocator* get(_base_class* instance);
	}
}