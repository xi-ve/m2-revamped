#pragma once
#include <inc.h>
namespace sdk
{
	namespace util
	{
		namespace math
		{
			class c_vector3
			{
			public:
				c_vector3() {};
				c_vector3(float a, float b, float c) { x = a; y = b; z = c; }
				bool cmp(c_vector3 a)
				{
					if ((int)a.x == (int)x &&
						(int)a.y == (int)y &&
						(int)a.z == (int)z) return true;
					return false;
				}
				bool cmpf(c_vector3 a)
				{
					if (a.x == x &&
						a.y == y &&
						a.z == z) return true;
					return false;
				}
				bool valid()
				{
					if (x == 0.f && y == 0.f && z == 0.f) return false;
					return true;
				}
				void clear()
				{
					x = 0; y = 0; z = 0;
				}
				c_vector3 mult()
				{
					return c_vector3(x * 100.f, y * 100.f, z * 100.f);
				}
				bool hasZero()
				{
					if (x == 0 || y == 0 || z == 0) return true;
					return false;
				}
				float x = 0, y = 0, z = 0;
				float pause = 0;
			};

		}
	}
}