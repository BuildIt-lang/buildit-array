#ifndef BARRAY_OPERATOR_H
#define BARRAY_OPERATOR_H

#include "barray.h"

namespace barray {


// A simple type trait to identify if 
// operators should be overloaded
template <typename T>
struct is_acceptable_rhs {
	static const bool value = false;
		
};

template <typename T>
struct is_acceptable_rhs<barray<T>> {
	static const bool value = true;
	using output_type = T;
	static const barray_expr<T>& cast(const barray<T>& v) {
		return *new barray_expr_array<T>(v);
	}
};

template <typename T>
struct is_acceptable_rhs<barray_expr<T>> {
	static const bool value = true;
	using output_type = T;
	static const barray_expr<T>& cast(const barray_expr<T>& v) {
		return v;
	}
};

template <>
struct is_acceptable_rhs<int> {
	static const bool value = true;
	using output_type = int;
	static const barray_expr<int>& cast(const int& v) {
		return * new barray_expr_const<int>(v);
	}
};

template <>
struct is_acceptable_rhs<float> {
	static const bool value = true;
	using output_type = float;
	static const barray_expr<float>& cast(const float& v) {
		return * new barray_expr_const<float>(v);
	}
};


template <typename T1, typename T2>
typename std::enable_if<is_acceptable_rhs<T2>::value, const barray_expr<typename is_acceptable_rhs<T1>::output_type>&>::type operator + (const T1& v1, const T2& v2) {
	return *new barray_expr_add<typename is_acceptable_rhs<T1>::output_type>(is_acceptable_rhs<T1>::cast(v1), is_acceptable_rhs<T2>::cast(v2));
}

template <typename T1, typename T2>
typename std::enable_if<is_acceptable_rhs<T2>::value, const barray_expr<typename is_acceptable_rhs<T1>::output_type>&>::type operator * (const T1& v1, const T2& v2) {
	return *new barray_expr_mul<typename is_acceptable_rhs<T1>::output_type>(is_acceptable_rhs<T1>::cast(v1), is_acceptable_rhs<T2>::cast(v2));
}

template <typename T1, typename T2>
typename std::enable_if<is_acceptable_rhs<T2>::value, const barray_expr<typename is_acceptable_rhs<T1>::output_type>&>::type cross (const T1& v1, const T2& v2) {
	return *new barray_expr_cross<typename is_acceptable_rhs<T1>::output_type>(is_acceptable_rhs<T1>::cast(v1), is_acceptable_rhs<T2>::cast(v2));
}


}
#endif
