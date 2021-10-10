/*
	When the confrigution is "DLL" it creates a dll,
	here we define what functions are exposed to the dll.

	Wraps so the class funciton calls are accessable in a dll
	by passing in a pointer with the call. (C style)

	Is useful if you're for example loading the dll in a C# script.

	e.g GeimBoy_LoadRom( gbGameBoy* _ptr, const char* _path);
*/

#pragma once
#define DLL_EXPORT extern "C" __declspec(dllexport)
