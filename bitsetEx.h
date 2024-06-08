#include <bitset>
#include <vector>
#include <functional>

namespace stdEx
{
	enum class ShiftTypes
	{
		Logical,
		Arithmetic,
		Circular
	};

	template<size_t BitAmount>
	class bitset : public std::bitset<BitAmount>
	{
	private:
		enum class LeftRight
		{
			Left,
			LeftMem,
			Right,
			RightMem
		};

		std::vector<std::function<std::bitset<BitAmount>(const size_t)>> operatorCollection
		{
			[this](const size_t amount) { return std::bitset<BitAmount>::operator<< (amount); },
			[this](const size_t amount) { return std::bitset<BitAmount>::operator<<=(amount); },
			[this](const size_t amount) { return std::bitset<BitAmount>::operator>> (amount); },
			[this](const size_t amount) { return std::bitset<BitAmount>::operator>>=(amount); }
		};

		std::vector<std::function<void(std::bitset<BitAmount>&, const size_t, const LeftRight)>> shiftCollection
		{
			[this](std::bitset<BitAmount>& bitset, const size_t amount, const LeftRight leftright) { LogicalShiftImpl(bitset, amount, leftright); },
			[this](std::bitset<BitAmount>& bitset, const size_t amount, const LeftRight leftright) { ArithmeticShiftImpl(bitset, amount, leftright); },
			[this](std::bitset<BitAmount>& bitset, const size_t amount, const LeftRight leftright) { CircularShiftImpl(bitset, amount, leftright); }
		};

		ShiftTypes currentShiftType;

		void LogicalShiftImpl(std::bitset<BitAmount>& bitset, const size_t amount, const LeftRight leftright)
		{
			bitset = operatorCollection[static_cast<int>(leftright)](amount);
		}

		void ArithmeticShiftImpl(std::bitset<BitAmount>& bitset, const size_t amount, const LeftRight leftright)
		{
			if (leftright == LeftRight::Left || leftright == LeftRight::LeftMem)
			{
				bitset = operatorCollection[static_cast<int>(leftright)](amount);
			}
			else
			{
				std::bitset<BitAmount>& resR = (leftright == LeftRight::RightMem ? *this : bitset = static_cast<std::bitset<BitAmount>>(*this));

				for (int i = 0; i < amount; ++i)
				{
					bool LMB = resR[BitAmount - 1];
					resR >>= 1;
					resR[BitAmount - 1] = LMB;
				}
			}
		}

		void CircularShiftLeft(std::bitset<BitAmount>& resR, const size_t amount)
		{
			for (int i = 0; i < amount; ++i)
			{
				bool memBool = resR[BitAmount - 1];
				resR <<= 1;
				resR[0] = memBool;
			}
		}

		void CircularShiftRight(std::bitset<BitAmount>& resR, const size_t amount)
		{
			for (int i = 0; i < amount; ++i)
			{
				bool memBool = resR[0];
				resR >>= 1;
				resR[BitAmount - 1] = memBool;
			}
		}

		void CircularShiftImpl(std::bitset<BitAmount>& bitset, const size_t amount, const LeftRight leftright)
		{
			bool left = leftright == LeftRight::Left || leftright == LeftRight::LeftMem;

			std::bitset<BitAmount>& resR = (leftright == LeftRight::LeftMem || leftright == LeftRight::RightMem ? *this : bitset = static_cast<std::bitset<BitAmount>>(*this));

			left ? CircularShiftLeft(resR, amount) : CircularShiftRight(resR, amount);
		}

		bitset<BitAmount> ShiftImpl(const size_t amount, const LeftRight leftright)
		{
			std::bitset<BitAmount> res;

			shiftCollection[static_cast<int>(currentShiftType)](res, amount, leftright);

			return res;
		}

	public:

		bitset()
			: std::bitset<BitAmount>()
		{
			currentShiftType = ShiftTypes::Logical;
		}

		bitset(const int value)
			: std::bitset<BitAmount>(value)
		{
			currentShiftType = ShiftTypes::Logical;
		}

		void SetShiftType(const ShiftTypes shiftTypeToSet)
		{
			currentShiftType = shiftTypeToSet;
		}

		bitset(const std::bitset<BitAmount>& bitset)
			: bitset(BitAmount) {}

		bitset<BitAmount> operator<<(const size_t amount)
		{
			return ShiftImpl(amount, LeftRight::Left);
		}

		bitset<BitAmount>& operator<<=(const size_t amount)
		{
			ShiftImpl(amount, LeftRight::LeftMem);
			return *this;
		}

		bitset<BitAmount> operator>>(const size_t amount)
		{
			return ShiftImpl(amount, LeftRight::Right);
		}

		bitset<BitAmount>& operator>>=(const size_t amount)
		{
			ShiftImpl(amount, LeftRight::RightMem);
			return *this;
		}

	};
}