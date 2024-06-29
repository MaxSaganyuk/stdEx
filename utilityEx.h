#ifndef UTILITYEX
#define UTILITYEX

#include <utility>

#if __cplusplus >= 201703L || _HAS_CXX17
#include <optional>
#endif

namespace stdEx
{
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
		ValWithBackup(Type& backup)
			: backup(&backup) {}

		ValWithBackup()
			: backup(nullptr) {}

		void Set(Type&& value)
		{
			this->value = std::move(value);
		}

		Type Get() const
		{
			assert(backup && "Backup value is nullptr");
			return value.has_value() ? value.value() : *backup;
		}

		void ResetBackup(Type& backup)
		{
			this->backup = &backup;
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
	};
}

#endif