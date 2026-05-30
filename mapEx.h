#ifndef MAPEX
#define MAPEX

#include <map>
#include <vector>
#include <stdexcept>

namespace stdEx
{
	template<typename Key, typename Value, typename Compare = std::less<Key>, typename Allocator = std::allocator<std::pair<const Key, Value>>>
	class map : public std::map<Key, Value, Compare, Allocator>
	{
		Value defaultValue;
		bool  defaultValueSet = false;

		template<typename Self>
		static decltype(auto) atImpl(Self& self, const Key& key)
		{
			auto iter = self.find(key);

			if (iter != self.end())
			{
				return (iter->second);
			}
			else if (self.defaultValueSet)
			{
				return (self.defaultValue);
			}
			else
			{
				throw std::out_of_range("Key does not exist and default value is not set");
			}
		}

	public:
		Value& at(const Key& key)
		{
			return atImpl(*this, key);
		}

		const Value& at(const Key& key) const
		{
			return atImpl(*this, key);
		}

		void SetDefaultValue(const Value& value)
		{
			defaultValue = value;
			defaultValueSet = true;
		}
	};

	template<class Key, class Value, class Compare = std::less<Key>>
	class multimap : public std::multimap<Key, Value, Compare>
	{
	public:
		std::vector<Value> operator[](const Key&& index)
		{
			std::vector<Value> res{};
			bool found = false;

			for (auto iter = this->begin(); iter != this->end(); ++iter)
			{
				if (iter->first == index)
				{
					res.push_back(iter->second);
					found = true;
				}
				else if (found)
				{
					break;
				}
			}

			return res;
		}
	};
}

#endif