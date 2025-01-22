/*
MIT License

CX3D Game Framework (https://github.com/PardCode/CX3D)

Copyright (c) 2019-2024, PardCode

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once
#include "types.h"

class vector2
{
  public:
	vector2()
	{
	}
	vector2(f32 x, f32 y) : x(x), y(y)
	{
	}
	vector2(const vector2& vector) : x(vector.x), y(vector.y)
	{
	}

	vector2 operator*(f32 num) const
	{
		return vector2(x * num, y * num);
	}
	vector2 operator+(const vector2& vec) const
	{
		return vector2(x + vec.x, y + vec.y);
	}
	vector2 operator-(const vector2& vec) const
	{
		return vector2(x - vec.x, y - vec.y);
	}

  public:
	f32 x = 0.0f, y = 0.0f;
};
