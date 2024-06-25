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

			bool has_value()
			{
				return check;
			}

			Type& value()
			{
				return val;
			}

			PseudoOptional& operator=(Type&& value)
			{
				check = true;
				val = std::move(value);

				return *this;
			}
		};

		PseudoOptional<Type> value;
#endif
		Type& backup;

	public:
		ValWithBackup(Type& backup)
			: backup(backup) {}

		void Set(Type&& value)
		{
			this->value = std::move(value);
		}

		Type Get()
		{
			return value.has_value() ? value.value() : backup;
		}
	};
}