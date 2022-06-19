#ifdef GB_DLL_EXPORT

#include "wrapper.hpp"
#include "../gbGameBoy.hpp"
#include "../gbPPU.hpp"
#include "../gbZ80.hpp"
#include "../gbCart.hpp"
#include "../gbBootRom.hpp"

using namespace GeimBoi;

// gbGameBoy //
DLL_EXPORT gbGameBoy* GameBoy_Create()
{
	return new gbGameBoy();
}

DLL_EXPORT void GameBoy_Destroy(gbGameBoy* _ptr)
{
	delete _ptr;
}

DLL_EXPORT void GameBoy_Clock(gbGameBoy* _ptr)
{
	_ptr->Clock();
}

DLL_EXPORT void GameBoy_FrameAdvance(gbGameBoy* _ptr)
{
	_ptr->FrameAdvance();
}

DLL_EXPORT void GameBoy_Reset(gbGameBoy* _ptr)
{
	_ptr->Reset();
}

DLL_EXPORT bool GameBoy_LoadRom(gbGameBoy* _ptr, const char* _path)
{
	return _ptr->LoadRom(_path);
}

DLL_EXPORT void GameBoy_SetPalette(gbGameBoy* _ptr, uint8_t _white, uint8_t _lgray, uint8_t _dgray, uint8_t _black)
{
	_ptr->SetPalette(_white, _lgray, _dgray, _black);
}

DLL_EXPORT void GameBoy_PressButton(gbGameBoy* _ptr, gbButton _button)
{
	_ptr->PressButton(_button);
}

DLL_EXPORT void GameBoy_ReleaseButton(gbGameBoy* _ptr, gbButton _button)
{
	_ptr->ReleaseButton(_button);
}

DLL_EXPORT uint8_t GameBoy_ReadByte(gbGameBoy* _ptr, uint16_t _addr)
{
	return _ptr->ReadByte(_addr);
}

// Getters for the other classes
DLL_EXPORT gbPPU* GameBoy_GetPpu(gbGameBoy* _ptr)
{
	return &_ptr->mPpu;
}

DLL_EXPORT gbZ80* GameBoy_GetCpu(gbGameBoy* _ptr)
{
	return &_ptr->mCpu;
}

DLL_EXPORT gbCart* GameBoy_GetCart(gbGameBoy* _ptr)
{
	return &_ptr->mCart;
}

DLL_EXPORT gbBootRom* GameBoy_GetBootRom(gbGameBoy* _ptr)
{
	return &_ptr->mBootRom;
}

#endif