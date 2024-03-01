#ifndef BARRAY_IMPL_H
#define BARRAY_IMPL_H

#include "builder/dyn_var.h"
#include "builder/static_var.h"
#include "blocks/extract_cuda.h"
#include "runtime.h"

namespace barray {


enum current_device_t {
	DEVICE_HOST,
	DEVICE_GPU,
};

current_device_t current_device = DEVICE_HOST;


// Base class for all expressions that can appear on the RHS of a =
template <typename T>
struct barray_expr {
	virtual const builder::dyn_var<T> get_value_at(std::vector<builder::dyn_var<int>*> indices) const {
		return 0;
	}
};
template <typename T>
struct barray_expr_const;
template <typename T>
struct barray_expr_array;


// Basic barray class. This class provides the programming interface
// for the barray language
template <typename T>
struct barray {

	// Members to keep track of data and sizes
	builder::dyn_var<T*> m_arr = 0;
	std::vector<size_t> m_sizes;


	// Optimization related fields
	builder::static_var<int> is_constant = false;
	builder::static_var<T> constant_val = 0;
	
	// Basic constructor to initialize barray 
	// from underlying buffer and sizes
	barray(builder::dyn_var<T*> arr, std::vector<size_t> sizes): m_arr(arr), m_sizes(sizes) {}

	// Constructor to allocate a new barray with provided size
	barray(std::vector<size_t> sizes) {
		allocate(sizes);
	}

	// Manual allocate function to allocate data with given dimensions
	void allocate(std::vector<size_t> sizes) {
		m_sizes = sizes;
		size_t total_size = 1;
		for (auto size: sizes) total_size *= size;
		m_arr = runtime::malloc((int)sizeof(T) * (int)total_size);
	}

	// Delete the default implementations of the operators
	// and copy constructors
	barray(const barray& other) = delete;

	void induce_loop_at(std::vector<builder::dyn_var<int>*> indices, const barray_expr<T>& rhs) {
		if (indices.size() == m_sizes.size()) {
			m_arr[compute_flat_index(indices)] = rhs.get_value_at(indices);
			return;
		}
		unsigned int index = indices.size();
		if (current_device == DEVICE_GPU && indices.size() == 0)
			builder::annotate(CUDA_KERNEL);
		for(builder::dyn_var<int> i = 0; i < (int)m_sizes[index]; ++i) {
			indices.push_back(i.addr());
			induce_loop_at(indices, rhs);
		}
	}

	void operator=(const barray_expr<T> & rhs) {
		induce_loop_at({}, rhs);
		is_constant = false;
	}
	
	void operator=(const T& value) {
		//*this = barray_expr_const<T>(value);
		is_constant = true;	
		constant_val = value;
	}

	void operator=(const barray& other) {
		*this = barray_expr_array<T>(other);
	}

	builder::dyn_var<int> compute_flat_index(std::vector<builder::dyn_var<int>*> indices, unsigned int index = -1) const {
		if (index == -1)
			index = m_sizes.size() - 1;
		if (index == 0) 
			return *(indices[index]);
		return *(indices[index]) + (int) m_sizes[index] * compute_flat_index(indices, index - 1);
	}


	void finalize() {
		if (!is_constant) return;
		*this = barray_expr_const<T>((T)constant_val);
	}

};


// RHS class for constants in the generated code
template <typename T>
struct barray_expr_const: public barray_expr<T> {
	T m_value;
	barray_expr_const (const T& value): m_value(value) {}

	const builder::dyn_var<T> get_value_at(std::vector<builder::dyn_var<int>*> indices) const {
		// Always return the same constant value
		return m_value;
	}
};

// RHS class for holding array operands
template <typename T>
struct barray_expr_array: public barray_expr<T>{
	const struct barray<T>& m_array;
	barray_expr_array(const struct barray<T>& array): m_array(array) {}

	const builder::dyn_var<T> get_value_at(std::vector<builder::dyn_var<int>*> indices) const {
		if (m_array.is_constant) return m_array.constant_val;
		return const_cast<builder::dyn_var<T*>&>(m_array.m_arr)[m_array.compute_flat_index(indices)];
	}
	
};

// Non leaf RHS expr classes

// Class for storing addition of two expressions
template <typename T>
struct barray_expr_add: public barray_expr<T> {
	const struct barray_expr<T>& expr1;
	const struct barray_expr<T>& expr2;


	barray_expr_add(const struct barray_expr<T>& expr1, const struct barray_expr<T>& expr2):
		expr1(expr1), expr2(expr2) {}

	const builder::dyn_var<T> get_value_at(std::vector<builder::dyn_var<int>*> indices) const {
		return expr1.get_value_at(indices) + expr2.get_value_at(indices);
	}
};


// Class for pointwise multiplication of two expressions
template <typename T>
struct barray_expr_mul: public barray_expr<T> {
	const struct barray_expr<T>& expr1;
	const struct barray_expr<T>& expr2;


	barray_expr_mul(const struct barray_expr<T>& expr1, const struct barray_expr<T>& expr2):
		expr1(expr1), expr2(expr2) {}

	const builder::dyn_var<T> get_value_at(std::vector<builder::dyn_var<int>*> indices) const {
		return expr1.get_value_at(indices) * expr2.get_value_at(indices);
	}
};

}

#endif
