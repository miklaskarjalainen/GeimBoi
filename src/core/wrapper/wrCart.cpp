#ifdef GB_DLL_EXPORT

#include "wrapper.hpp"
#include "../gbCart.hpp"

using namespace GeimBoi;

DLL_EXPORT const char* Cart_GetGameName(gbCart* _ptr)
{
	return _ptr->GetGameName().c_str();
}

DLL_EXPORT uint8_t Cart_GetGameVersion(gbCart* _ptr)
{
	return _ptr->GetGameVersion();
}

DLL_EXPORT uint16_t Cart_GetRomSize(gbCart* _ptr)
{
	return _ptr->GetRomSize();
}

DLL_EXPORT uint16_t Cart_GetCurRomBank(gbCart* _ptr)
{
	return _ptr->GetCurRomBank();
}

DLL_EXPORT uint16_t Cart_GetRomBankCount(gbCart* _ptr)
{
	return _ptr->GetRomBankCount();
}

DLL_EXPORT uint16_t Cart_GetRamSize(gbCart* _ptr)
{
	return _ptr->GetRamSize();
}

DLL_EXPORT uint16_t Cart_GetCurRamBank(gbCart* _ptr)
{
	return _ptr->GetCurRamBank();
}

DLL_EXPORT uint16_t Cart_GetRamBankCount(gbCart* _ptr)
{
	return _ptr->GetRamBankCount();
}

DLL_EXPORT gbCartType Cart_GetCartType(gbCart* _ptr)
{
	return _ptr->GetCartType();
}

DLL_EXPORT const char* Cart_GetCartTypeText(gbCart* _ptr)
{
	return _ptr->GetCartTypeText().c_str();
}

DLL_EXPORT bool Cart_IsGameLoaded(gbCart* _ptr)
{
	return _ptr->IsGameLoaded();
}

DLL_EXPORT bool Cart_HasBattery(gbCart* _ptr)
{
	return _ptr->HasBattery();
}

DLL_EXPORT bool Cart_SupportsGBC(gbCart* _ptr)
{
	return _ptr->SupportsGBC();
}

DLL_EXPORT bool Cart_SaveBattery(gbCart* _ptr, const char* _path)
{
	return _ptr->SaveBattery(_path);
}

DLL_EXPORT bool Cart_LoadBattery(gbCart* _ptr, const char* _path)
{
	return _ptr->LoadBattery(_path);
}

#endif