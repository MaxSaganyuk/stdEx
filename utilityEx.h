#ifndef UTILITYEX
#define UTILITYEX

#include <cassert>
#include <utility>

#if __cplusplus >= 201703L || _HAS_CXX17
#include <optional>
#endif

namespace stdEx
{
#if __cplusplus >= 202002L || _HAS_CXX20
	namespace _stdEx
	{
		template<typename MArray, size_t rank, size_t n>
		constexpr void GetExtent(std::array<size_t, rank>& extent)
		{
			extent[n] = std::extent_v<MArray, n>;

			if constexpr (n + 1 < rank)
			{
				GetExtent<MArray, rank, n + 1>(extent);
			}
		}

		template<typename MArray>
		constexpr auto GetDimentionExtent()
		{
			constexpr size_t rank = std::rank_v<MArray>;
			std::array<size_t, rank> extent;

			GetExtent<MArray, rank, 0>(extent);

			return extent;
		}

		template<typename MArray, size_t rank, size_t n>
		void PrintNDimention(MArray& mArray, std::array<size_t, rank> extent)
		{
			for (size_t i = 0; i < extent[n]; ++i)
			{
				if constexpr (n + 1 < rank)
				{
					std::cout << '{';
					PrintNDimention<std::remove_extent_t<MArray>, rank, n + 1>(mArray[i], extent);
					std::cout << '}';
				}
				else
				{
					std::cout << mArray[i];
					if (i < extent[n] - 1)
					{
						std::cout << ' ';
					}
				}
			}
		}
	}

	template<typename MArray>
	constexpr void PrintMArray(MArray& mArray)
	{
		using namespace _stdEx;

		constexpr auto extent = GetDimentionExtent<MArray>();

		PrintNDimention<MArray, extent.size(), 0>(mArray, extent);
	}
#endif

	template<typename Type>
	class ValWithBackup
	{
#if __cplusplus >= 201703L || _HAS_CXX17
		std::optional<Type> value;
#else
		template<typename Type>
		class PseudoOptional
		{
			Type val;
			bool check;

		public:
			PseudoOptional()
				: val(Type{}), check(false) {}

			bool has_value() const
			{
				return check;
			}

			Type value() const
			{
				return val;
			}

			PseudoOptional& operator=(Type&& value)
			{
				check = true;
				val = std::move(value);

				return *this;
			}

			PseudoOptional& operator=(const PseudoOptional& pseudoOpt)
			{
				val = pseudoOpt.val;
				check = pseudoOpt.check;

				return *this;
			}
		};

		PseudoOptional<Type> value;
#endif
		Type* backup;

	public:
		void ResetValue()
		{
#if __cplusplus >= 201703L || _HAS_CXX17
			this->value = std::optional<Type>();
#else
			this->value = PseudoOptional<Type>();
#endif
		}

		ValWithBackup(Type* backup = nullptr)
			: backup(backup) {}

		void ResetBackup(Type* backup = nullptr)
		{
			this->backup = backup;
		}

		bool HasValue() const
		{
			return value.has_value();
		}

		ValWithBackup& operator=(const ValWithBackup& valWb)
		{
			value = valWb.value;
			backup = valWb.backup;

			return *this;
		}

		ValWithBackup& operator=(Type&& value)
		{
			this->value = std::move(value);

			return *this;
		}

		operator Type() const
		{
			assert(backup && "Backup value is nullptr");
			return value.has_value() ? value.value() : *backup;
		}
	};
}

#endif