#ifdef GB_DLL_EXPORT

#include "wrapper.hpp"
#include "../gbZ80.hpp"

using namespace GeimBoi;

DLL_EXPORT bool Z80_GetFlag(gbZ80* _ptr, gbFlag _flag)
{
	return _ptr->GetFlag(_flag);
}

DLL_EXPORT bool Z80_GetIE(gbZ80* _ptr, gbInterrupt _interrupt)
{
	return _ptr->GetIE(_interrupt);
}

DLL_EXPORT bool Z80_GetIF(gbZ80* _ptr, gbInterrupt _interrupt)
{
	return _ptr->GetIF(_interrupt);
}

DLL_EXPORT bool Z80_GetIME(gbZ80* _ptr)
{
	return _ptr->GetIME();
}

DLL_EXPORT bool Z80_IsHalted(gbZ80* _ptr)
{
	return _ptr->IsHalted();
}

#endif
