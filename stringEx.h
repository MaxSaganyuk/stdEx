#ifndef STRINGEX
#define STRINGEX

namespace stdEx
{
	/*
	* By default, the safest and most memory inefficient method is chosen called "Extend"
	* To change it #define FixedStringPreferLarger or FixedStringPreferSmaller in your translation unit
	* before including the header.
	* Such modes assume that buffers for FixedString will exceed de facto sizes greatly
	* allowing to avoid evergrowing buffer on compounded concatenations.
	* Additionally, it is possible to simply change function definition and provide your own rule.
	*/
	template<size_t FirstBufferSize, size_t SecondBufferSize>
	consteval size_t _GetConcatBufferSize() noexcept
	{
#ifdef FixedStringPreferLarger
		bool preferLarger = true;
#elifdef FixedStringPreferSmaller
		bool preferLarger = false;
#endif

#ifdef FixedStringPreferLarger || FixedStringPreferSmaller
		return preferLarger ? std::max(FirstBufferSize, SecondBufferSize) : std::min(FirstBufferSize, SecondBufferSize);
#else
		// The "Extend". Two string have two null terminators, only one is needed in the resulting one, hence -1
		return FirstBufferSize + SecondBufferSize - 1;
#endif
	}

	template <typename Type>
	concept CharType =
		std::same_as<Type, char> || std::same_as<Type, wchar_t> || std::same_as<Type, char8_t> ||
		std::same_as<Type, char16_t> || std::same_as<Type, char32_t>;

	template<size_t BufferSize, CharType Type = char>
	class FixedString
	{
		static_assert(BufferSize > 0, "Buffer size must be non zero");

		// Assumes that buffer bounds are checked
		template<size_t ThisBufferSize>
		constexpr void WriteToStr(
			const Type(&str)[ThisBufferSize], size_t amount, size_t start = 0, bool abortOnNullTer = false
		) noexcept
		{
			size_t startSize = size;

			for (size_t i = 0; i < amount; ++i)
			{
				if (abortOnNullTer && str[start + i] == '\0') return;

				this->str[startSize + i] = str[start + i];
				++this->size;
			}
		}
	public:
		Type str[BufferSize]{}; // Raw array is prefered because getting "str" directly allows getting Type[] intead of Type*
		size_t size{};

		constexpr FixedString() noexcept = default;

		constexpr FixedString(const Type(&str)[BufferSize]) noexcept
		{
			WriteToStr(str, BufferSize, 0, true);
		}

		template<size_t ThisBufferSize>
		constexpr FixedString(const Type(&str)[ThisBufferSize]) noexcept
		{
			static_assert(BufferSize >= ThisBufferSize, "Passed character array's buffer must fix FixedString buffer");

			WriteToStr(str, ThisBufferSize, 0, true);
		}

		constexpr auto operator<=>(const FixedString<BufferSize, Type>&) const = delete;

		constexpr void Clean() noexcept
		{
			if consteval
			{
				// std::fill_n is not noexcept due to internal complexity
				for (size_t i = 0; i < BufferSize; ++i)
				{
					str[i] = 0;
				}
			}
			else
			{
				std::memset(str, 0, BufferSize);
			}
		}

		constexpr size_t GetBufferSize() const noexcept
		{
			return BufferSize;
		}

		constexpr void SetSize(size_t size)
		{
			if (size < BufferSize)
			{
				this->size = size;
			}
			else
			{
				throw std::exception("Cannot set size larger than buffer");
			}
		}

		template<typename Self>
		constexpr auto&& Begin(this Self&& self) noexcept
		{
			return self.str;
		}

		template<typename Self>
		constexpr auto&& End(this Self&& self) noexcept
		{
			return self.str + self.size;
		}

		template<size_t ThisBufferSize = BufferSize>
		constexpr FixedString<ThisBufferSize, Type> Substr(size_t start, size_t end) const
		{
			size_t amount = end - start;

			if (start < end && start < BufferSize && end < BufferSize && amount < ThisBufferSize)
			{
				FixedString<ThisBufferSize, Type> res;

				res.WriteToStr(str, amount, start);

				return res;
			}
			else
			{
				throw std::exception("Invalid start and end for substring creation");
			}
		}

		template<size_t FirstBufferSize, size_t SecondBufferSize, CharType ThisType>
		constexpr friend FixedString<_GetConcatBufferSize<FirstBufferSize, SecondBufferSize>(), ThisType> operator+(
			const FixedString<FirstBufferSize, ThisType>& firstStr, const FixedString<SecondBufferSize, ThisType>& secondStr
			);

		template<size_t ThisBufferSize>
		constexpr FixedString<BufferSize>& operator+=(const Type(&otherStr)[ThisBufferSize])
		{
			return operator+=(FixedString{ otherStr });
		}

		template<size_t ThisBufferSize>
		constexpr FixedString<BufferSize>& operator+=(const FixedString<ThisBufferSize>& otherStr)
		{
			if (BufferSize - size > otherStr.size)
			{
				WriteToStr(otherStr.str, otherStr.size);
			}
			else
			{
				throw std::exception("Cannot append beyond buffer");
			}

			return *this;
		}

		template<typename Self>
		constexpr char At(this Self&& self, size_t index)
		{
			if (index < BufferSize)
			{
				return self.str[index];
			}
			else
			{
				throw std::out_of_range("Cannot get index outside buffer size");
			}
		}

		template<typename Self>
		constexpr char operator[](this Self&& self, size_t index) noexcept
		{
			return self.str[index];
		}

		// Some convenience methods are not provided, but std::string_view can be used
		template<typename Self>
		constexpr std::basic_string_view<Type> View(this Self&& self) noexcept
		{
			return std::basic_string_view(self.str, self.size);
		}

		std::basic_string<Type> Unfix() const
		{
			return std::basic_string<Type>(str, str + size);
		}

		template<typename Self>
		constexpr operator std::basic_string_view<Type>(this Self&& self)
		{
			return self.View();
		}

		operator std::basic_string<Type>() const
		{
			return Unfix();
		}
	};

	template<size_t FirstBufferSize, size_t SecondBufferSize, CharType ThisType>
	constexpr FixedString<_GetConcatBufferSize<FirstBufferSize, SecondBufferSize>(), ThisType> operator+(
		const FixedString<FirstBufferSize, ThisType>& firstStr, const FixedString<SecondBufferSize, ThisType>& secondStr
		)
	{
		constexpr size_t ResultingBufferSize = _GetConcatBufferSize<FirstBufferSize, SecondBufferSize>();

		if (firstStr.size + secondStr.size < ResultingBufferSize)
		{
			FixedString<ResultingBufferSize, ThisType> res;

			res.WriteToStr(firstStr.str, firstStr.size);
			res.WriteToStr(secondStr.str, secondStr.size);

			return res;
		}
		else
		{
			throw std::exception("Resulting buffer size on operator+ is not enough for concat-ed string");
		}
	}
}

#endif STRINGEX
