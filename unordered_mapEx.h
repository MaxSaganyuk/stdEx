#ifndef UNORDEREDMAPEX
#define UNORDEREDMAPEX

#include <unordered_map>

#include <stdexcept>

namespace stdEx
{
	template<class Key, class Value, class Hash = std::hash<Key>, class KeyEqual = std::equal_to<Key>>
	class unordered_multimap : public std::unordered_multimap<Key, Value, Hash, KeyEqual>
	{
	public:
		std::vector<Value> operator[](const Key&& index)
		{
			std::vector<Value> res{};

			for (auto iter = this->begin(); iter != this->end(); ++iter)
			{
				if (iter->first == index)
				{
					res.push_back(iter->second);
				}
			}

			return res;
		}
	};
}

#endif 