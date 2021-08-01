#ifdef GIFFI_DLL_EXPORT

#include "wrapper.hpp"
#include "../gbPPU.hpp"

using namespace Giffi;

struct gbColorFloat {
	float r, g, b;
};

DLL_EXPORT gbColor* GeimBoi_GetFrameBuffer(gbPPU* _ptr)
{
	return (gbColor*)&_ptr->front_buffer;
}

DLL_EXPORT gbColorFloat GeimBoi_GetColorFloat(gbPPU* _ptr, int _x, int _y)
{
	if (_x >= 160 || _y >= 144)
	{
		return { 0,0,0 };
	}
	gbColorFloat out;
	out.r = (float)(_ptr->front_buffer[_y][_x].r) / 255.0f;
	out.g = (float)(_ptr->front_buffer[_y][_x].g) / 255.0f;
	out.b = (float)(_ptr->front_buffer[_y][_x].b) / 255.0f;
	return out;
}

DLL_EXPORT gbColor GeimBoi_GetColorU8(gbPPU* _ptr, int _x, int _y)
{
	if (_x >= 160 || _y >= 144)
	{
		return { 0,0,0 };
	}
	return _ptr->front_buffer[_y][_x];
}

#endif
