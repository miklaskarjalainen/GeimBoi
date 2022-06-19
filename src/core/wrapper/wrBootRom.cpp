#ifdef GB_DLL_EXPORT

#include "wrapper.hpp"
#include "../gbBootRom.hpp"

using namespace GeimBoi;

DLL_EXPORT bool BootRom_LoadBios(gbBootRom* _ptr, const char* _path)
{
	return _ptr->LoadBios(_path);
}

DLL_EXPORT bool BootRom_IsBiosLoaded(gbBootRom* _ptr)
{
	return _ptr->IsBiosLoaded();
}

#endif