#ifndef TYPE_TRAITSEX
#define TYPE_TRAITSEX

#include <type_traits>

template<size_t n, typename Type, typename... Types>
struct GeneralizedConditional : GeneralizedConditional<n - 1, Types...>
{
	static_assert((n > 0 && n < sizeof...(Types) + 1), "Invalid index for the amount of listed types");
};

template<typename Type, typename... Types>
struct GeneralizedConditional<0, Type, Types...>
{
	using TypeToUse = Type;
};

template<typename>
struct RemoveAllMemberQualifiers;

#define DefineRemoveAllMemberQualifiers(qualifier)                        \
template<typename ClassT, typename ReturnT, typename... Params>           \
struct RemoveAllMemberQualifiers<ReturnT(ClassT::*)(Params...) qualifier> \
{                                                                         \
	using Type = ReturnT(ClassT::*)(Params...);                           \
};                                                                        \

// std does not provide any way to remove member function qualifiers? (like std::remove_cv)
// Fine... I'll do it myself
DefineRemoveAllMemberQualifiers(&)
DefineRemoveAllMemberQualifiers(&&)
DefineRemoveAllMemberQualifiers(const)
DefineRemoveAllMemberQualifiers(const &)
DefineRemoveAllMemberQualifiers(const &&)
DefineRemoveAllMemberQualifiers(noexcept)
DefineRemoveAllMemberQualifiers(volatile)
DefineRemoveAllMemberQualifiers(& noexcept)
DefineRemoveAllMemberQualifiers(volatile &)
DefineRemoveAllMemberQualifiers(&& noexcept)
DefineRemoveAllMemberQualifiers(volatile &&)
DefineRemoveAllMemberQualifiers(const noexcept)
DefineRemoveAllMemberQualifiers(const volatile)
DefineRemoveAllMemberQualifiers(const & noexcept)
DefineRemoveAllMemberQualifiers(const && noexcept)
DefineRemoveAllMemberQualifiers(const volatile &)
DefineRemoveAllMemberQualifiers(const volatile &&)
DefineRemoveAllMemberQualifiers(volatile noexcept)
DefineRemoveAllMemberQualifiers(volatile & noexcept)
DefineRemoveAllMemberQualifiers(volatile && noexcept)
DefineRemoveAllMemberQualifiers(const volatile noexcept)
DefineRemoveAllMemberQualifiers(const volatile & noexcept)
DefineRemoveAllMemberQualifiers(const volatile && noexcept)

template<typename Type>
struct MemberFuncInfo : MemberFuncInfo<typename RemoveAllMemberQualifiers<Type>::Type>{};

template<typename ClassT, typename ReturnT, typename... Params>
struct MemberFuncInfo<ReturnT(ClassT::*)(Params...)>
{
	using ClassType = ClassT;
	using ReturnType = ReturnT;
};

template<size_t ParamIndex, typename Type>
struct MemberFuncInfoWParams : MemberFuncInfoWParams<ParamIndex, typename RemoveAllMemberQualifiers<Type>::Type>{};

template<size_t ParamIndex, typename ClassT, typename ReturnT, typename... Params>
struct MemberFuncInfoWParams<ParamIndex, ReturnT(ClassT::*)(Params...)>
{
	using ClassType = ClassT;
	using ReturnType = ReturnT;
	using ParamType = GeneralizedConditional<ParamIndex, Params...>;
};


#endif TYPE_TRAITSEX
