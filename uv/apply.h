#ifndef __LIBRA_APPLY_H__
#define __LIBRA_APPLY_H__

#include <tuple>
#include <type_traits>
#include <utility>

namespace Libra
{
	//helper from http://www.cnblogs.com/qicosmos/p/3318070.html. thank you!
	//
	namespace internal
	{
		template<size_t N>
		struct Apply {
			template<typename F, typename T, typename... A>
			static inline auto apply(F && f, T && t, A &&... a)
				-> decltype(Apply<N - 1>::apply(
				::std::forward<F>(f), ::std::forward<T>(t),
				::std::get<N - 1>(::std::forward<T>(t)),
				::std::forward<A>(a)...
				))
			{
				return Apply<N - 1>::apply(::std::forward<F>(f),
					::std::forward<T>(t),
					::std::get<N - 1>(::std::forward<T>(t)),
					::std::forward<A>(a)...);
			}
		};

		template<>
		struct Apply<0> {
			template<typename F, typename T, typename... A>
			static inline auto apply(F && f, T &&, A &&... a)
				-> decltype(::std::forward<F>(f)	(::std::forward<A>(a)...))
			{
				return ::std::forward<F>(f)(::std::forward<A>(a)...);
			}
		};
	}
	

	template<typename F, typename T>
	inline auto apply(F && f, T && t)
		-> decltype(internal::Apply< ::std::tuple_size<typename ::std::decay<T>::type
		>::value>::apply(::std::forward<F>(f), ::std::forward<T>(t)))
	{
		return internal::Apply< ::std::tuple_size<typename ::std::decay<T>::type
		>::value>::apply(::std::forward<F>(f), ::std::forward<T>(t));
	}

}

#endif