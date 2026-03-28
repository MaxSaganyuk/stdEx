#ifndef UTILITYEX
#define UTILITYEX

#include <cassert>
#include <utility>
#include <atomic>
#include <vector>
#include <set>
#include <memory>

#if __cplusplus >= 201703L || _HAS_CXX17
#include <optional>
#endif

#if __cplusplus >= 202002L || _HAS_CXX20
#include <iostream>
#include <array>
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
		constexpr auto GetDimensionExtent()
		{
			constexpr size_t rank = std::rank_v<MArray>;
			std::array<size_t, rank> extent;

			GetExtent<MArray, rank, 0>(extent);

			return extent;
		}

		template<typename MArray, size_t rank, size_t n>
		void PrintNDimension(MArray& mArray, std::array<size_t, rank> extent)
		{
			for (size_t i = 0; i < extent[n]; ++i)
			{
				if constexpr (n + 1 < rank)
				{
					std::cout << '{';
					PrintNDimension<std::remove_extent_t<MArray>, rank, n + 1>(mArray[i], extent);
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

		constexpr auto extent = GetDimensionExtent<MArray>();

		PrintNDimension<MArray, extent.size(), 0>(mArray, extent);
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
				: val(Type{}), check(false) {
			}

			bool has_value() const
			{
				return check;
			}

			Type value() const
			{
				return val;
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
			: backup(backup) {
		}

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

		operator Type& ()
		{
			assert(backup && "Backup value is nullptr");
			return value.has_value() ? value.value() : *backup;
		}
	};


	enum class ObjectCounterType
	{
		General,
		Stack,
		Heap
	};

	template<typename Type>
	class ObjectCounter
	{
	private:

		struct ObjectCounterInfo
		{
			size_t created;
			size_t active;

			ObjectCounterInfo& operator++()
			{
				++created;
				++active;

				return *this;
			}
		};

		struct ObjectCounterStorage
		{
			ObjectCounterInfo generalCounter;
			ObjectCounterInfo stackCounter;
			ObjectCounterInfo heapCounter;
		};

		static ObjectCounterStorage allThreadStorage;
		static thread_local ObjectCounterStorage localThreadStorage;

		static void UpdateLocalThreadStorage()
		{
			localThreadStorage = allThreadStorage;
		}

		static thread_local bool stackCheck;

		static ObjectCounterInfo& GetCorrectStorageAndInfo(ObjectCounterType objType, bool localThread)
		{
			if (localThread)
			{
				UpdateLocalThreadStorage();
			}

			return GetCorrectInfo(objType, localThread ? localThreadStorage : allThreadStorage);
		}

		static ObjectCounterInfo& GetCorrectInfo(ObjectCounterType objType, ObjectCounterStorage& objStorage)
		{
			switch (objType)
			{
			case ObjectCounterType::General:
				return objStorage.generalCounter;
			case ObjectCounterType::Stack:
				return objStorage.stackCounter;
			case ObjectCounterType::Heap:
				return objStorage.heapCounter;
			default:
				assert(false && "Unreachable");
			}
		}

		ObjectCounter(std::nullptr_t) {}

	public:

		static size_t GetCreatedObjAmount(ObjectCounterType objType, bool localThread = false)
		{
			return GetCorrectStorageAndInfo(objType, localThread).created;
		}

		static size_t GetActiveObjAmount(ObjectCounterType objType, bool localThread = false)
		{
			return GetCorrectStorageAndInfo(objType, localThread).active;
		}

		ObjectCounter()
		{
			++allThreadStorage.generalCounter;

			if (stackCheck)
			{
				++allThreadStorage.stackCounter;
			}

			stackCheck = true;
		}

		ObjectCounter(const ObjectCounter&)
			: ObjectCounter() {
		}

		void* operator new(size_t amount)
		{
			stackCheck = false;
			++allThreadStorage.heapCounter;
			return ::new ObjectCounter(nullptr);
		}

		void operator delete(void* pointer)
		{
			++allThreadStorage.stackCounter.active;
			--allThreadStorage.heapCounter.active;

			::delete(pointer);
		}

	protected:
		virtual ~ObjectCounter()
		{
			--allThreadStorage.generalCounter.active;
			--allThreadStorage.stackCounter.active;
		}
	};

	template<typename Type>
	typename ObjectCounter<Type>::ObjectCounterStorage ObjectCounter<Type>::allThreadStorage = { 0, 0, 0 };

	template<typename Type>
	typename thread_local ObjectCounter<Type>::ObjectCounterStorage ObjectCounter<Type>::localThreadStorage = { 0, 0, 0 };

	template<typename Type>
	bool thread_local ObjectCounter<Type>::stackCheck = true;

#define TrackedClass(className) class className : public stdEx::ObjectCounter<className>

	enum class RelationType
	{
		NoRelation,   // A  x  B
		LeftToRight,  // A  -> B
		RightToLeft,  // A <-  B
		Bidirectional // A <-> B
	};

#if __cplusplus >= 202002L || _HAS_CXX20
	template<typename Type>
	concept OnlyComparable = requires(Type t) { t < t; };
	
	template<OnlyComparable Type>
#else
	template<typename Type> // Type must support operator<
#endif
	class RelationGraph
	{
		struct Element;

		class SharedPtrValueComparator
		{
		public:
			using is_transparent = void;

			bool operator()(const std::shared_ptr<Element>& left, const std::shared_ptr<Element>& right) const
			{
				return *left < *right;
			}

			bool operator()(const Element& left, const std::shared_ptr<Element>& right) const
			{
				return left < *right;
			}

			bool operator()(const std::shared_ptr<Element>& left, const Element& right) const
			{
				return *left < right;
			}
		};

		class WeakPtrValueComparator
		{
		public:
			using is_transparent = void;

			bool operator()(const std::weak_ptr<Element>& left, const std::weak_ptr<Element>& right) const
			{
				if (right.expired())
				{
					return false;
				}
				if (left.expired())
				{
					return true;
				}

				return *left.lock() < *right.lock();
			}
		};

		struct Element
		{
			Type value;
			std::set<std::weak_ptr<Element>, WeakPtrValueComparator> relations;

			bool operator<(const Element& otherElement) const
			{
				return value < otherElement.value;
			}
		};

		std::set<std::shared_ptr<Element>, SharedPtrValueComparator> elements;
		bool isBidirEnabled = true;

		std::shared_ptr<Element> AddElementImpl(const Type& value)
		{
			const Element protoElement{ value };
			auto iter = elements.find(protoElement);

			if (iter == elements.end())
			{
				iter = elements.insert(std::make_shared<Element>(protoElement)).first;
			}

			return *iter;
		}
	public:
		void EnableBidirectionality(bool value = true)
		{
			isBidirEnabled = value;
		}

		// Can be used as pre linking
		void AddElement(const Type& value)
		{
			AddElementImpl(value);
		}

		// Links elements, already existent elements do not get re-added
		void AddElements(const Type& firstValue, const Type& secondValue, RelationType relationType)
		{
			if (!isBidirEnabled && relationType == RelationType::Bidirectional)
			{
				return;
			}

			auto leftElement  = AddElementImpl(firstValue);
			auto rightElement = AddElementImpl(secondValue);

			if (relationType == RelationType::Bidirectional || relationType == RelationType::LeftToRight)
			{
				if (isBidirEnabled || rightElement->relations.find(leftElement) == rightElement->relations.end())
				{
					leftElement->relations.insert(rightElement);
				}
			}
			if (relationType == RelationType::Bidirectional || relationType == RelationType::RightToLeft)
			{
				if (isBidirEnabled || leftElement->relations.find(rightElement) == leftElement->relations.end())
				{
					rightElement->relations.insert(leftElement);
				}
			}
		}

		// To maintain speed, remove does not invalidate expired elements
		void RemoveElement(const Type& value)
		{
			auto iter = elements.find(Element{ value });
			if (iter != elements.end())
			{
				elements.erase(iter);
			}
		}

		// When related values are requested, we invalidate expired elements
		std::vector<Type> GetValuesRelatedTo(const Type& value)
		{
			std::vector<Type> values;
			values.reserve(elements.size());

			auto elementIter = elements.find(Element{ value });

			if (elementIter != elements.end())
			{
				auto element = *elementIter;
				auto& relations = element->relations;

				for (auto relationIter = relations.begin(); relationIter != relations.end();)
				{
					if (!relationIter->expired())
					{
						values.push_back(relationIter->lock()->value);
						++relationIter;
					}
					else
					{
						relationIter = relations.erase(relationIter);
					}
				}
			}

			return values;
		}
	};

	template<typename Type, size_t size>
	class RingBuffer
	{
		constexpr static size_t npos = 0 - 1;

		static_assert(size, "Size cannot be 0");
		static_assert(size != npos, "Max size_t size is reserved");

		std::array<Type, size> buffer{};
		size_t frontPoint = npos;
		size_t backPoint = npos;

		constexpr void IncrementPoint(size_t& pointToIncrement, size_t& otherPoint, bool push)
		{
			pointToIncrement = (pointToIncrement + 1) % size;

			if (push && pointToIncrement == otherPoint)
			{
				otherPoint = (otherPoint + 1) % size;
			}
		}

		constexpr void DecrementPoint(size_t& pointToDecrement, size_t& otherPoint, bool push)
		{
			pointToDecrement = pointToDecrement - 1 == npos ? size - 1 : pointToDecrement - 1;

			if (push && pointToDecrement == otherPoint)
			{
				otherPoint = otherPoint - 1 == npos ? size - 1 : otherPoint - 1;
			}
		}

	public:
		constexpr void PushBack(const Type& value)
		{
			if (frontPoint == npos && backPoint == npos)
			{
				frontPoint = backPoint = 0;
			}
			else
			{
				IncrementPoint(backPoint, frontPoint, true);
			}

			buffer[backPoint] = value;
		}

		constexpr void PopBack()
		{
			if (frontPoint == backPoint)
			{
				frontPoint = backPoint = npos;
			}
			else
			{
				DecrementPoint(backPoint, frontPoint, false);
			}
		}

		constexpr void PushFront(const Type& value)
		{
			if (frontPoint == npos && backPoint == npos)
			{
				frontPoint = backPoint = 0;
			}
			else
			{
				DecrementPoint(frontPoint, backPoint, true);
			}

			buffer[frontPoint] = value;
		}

		constexpr void PopFront()
		{
			if (frontPoint == backPoint)
			{
				frontPoint = backPoint = npos;
			}
			else
			{
				IncrementPoint(frontPoint, backPoint, false);
			}
		}

		constexpr Type& GetFront()
		{
			if (frontPoint == npos)
			{
				throw std::logic_error("Buffer has no data");
			}
			else
			{
				return buffer[frontPoint];
			}
		}

		constexpr Type& GetBack()
		{
			if (backPoint == npos)
			{
				throw std::logic_error("Buffer has no data");
			}
			else
			{
				return buffer[backPoint];
			}
		}

		constexpr Type& operator[](size_t index)
		{
			if (index < GetCurrentSize())
			{
				return buffer[(frontPoint + index) % size];
			}
			else
			{
				throw std::logic_error("Index is beyond current size");
			}
		}

		constexpr size_t GetMaxSize()
		{
			return size;
		}

		constexpr size_t GetCurrentSize()
		{
			if (backPoint == npos && frontPoint == npos)
			{
				return 0;
			}

			return (backPoint >= frontPoint ? backPoint - frontPoint : size - frontPoint - backPoint) + 1;
		}
	};
}
#endif