#pragma once
#include <cmath>

namespace GROOVE
{
namespace Util
{
	// TODO: fix mapping function
	static int mapping(int val, int in1, int in2, int out1, int out2)
	{
		if (in1 < 0) {
			val += std::abs(in1);
			in2 += std::abs(in1);
			in1 = 0;
		}
		if (val < in1) {
			val = in1;
		}
		if (val > in2) {
			val = in2;
		}
		val -= in1;
		return (int)(out1 + (val * (out2 - out1) / (in2 - in1)));
	}
}
}
