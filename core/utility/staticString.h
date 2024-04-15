#pragma once


#include <staticVector.h>
#include <string>

namespace samurai
{

	template<size_t N>
	struct StaticString: public StaticVector<char, N>
	{
		StaticString() {};

		StaticString(const char *c)
		{
			for (; *c != 0; c++)
			{
				push_back(*c);
			}
		}

		std::string to_string()
		{
			return std::string(beg_, beg_ + size_);
		}
	};


}