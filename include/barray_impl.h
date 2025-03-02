#ifndef BARRAY_IMPL_H
#define BARRAY_IMPL_H

#include "builder/dyn_var.h"
#include "builder/static_var.h"
#include "blocks/extract_cuda.h"
#include "builder/nd_var.h"
#include "runtime.h"

namespace barray {

// Enum to keep track of where the code is executing
// and where the data is stored
enum current_device_t {
	DEVICE_HOST,
	DEVICE_GPU,
};

// Global variable to keep track of current execution device
extern current_device_t current_device;

// Helper function to check if two arrays are compatible for pointwise operations
// and return the resultant size
std::vector<int> match_expr_sizes(std::vector<int>, std::vector<int>);

// Base class for all expressions that can appear on the RHS of a =
template <typename T>
struct barray_expr {
	virtual const builder::dyn_var<T> get_value_at(std::vector<builder::dyn_var<int>*> indices) const {
		return 0;
	}
	virtual const std::vector<int> get_expr_size(void) const {
		return {};
	}
};

template <typename T>
struct barray_expr_const;
template <typename T>
struct barray_expr_array;

struct barray_base {
	static std::vector<barray_base*> registered_arrays;

	barray_base();
	virtual ~barray_base();


	builder::nd_var<bool> device_required;
	
	virtual void to_device() = 0;
	virtual void to_host() = 0;

};


// Basic barray class. This class provides the programming interface
// for the barray language
template <typename T>
struct barray: public barray_base {

	// Members to keep track of data and sizes
	builder::dyn_var<T*> m_arr = 0;

	// GPU related buffers
	builder::dyn_var<T*> m_device_arr = 0;
	builder::static_var<int> device_allocated = 0;
	builder::static_var<int> current_storage = DEVICE_HOST;
	std::vector<int> m_sizes;


	// Optimization related fields
	builder::static_var<int> is_constant = false;
	builder::static_var<T> constant_val = 0;
	
	// Basic constructor to initialize barray 
	// from underlying buffer and sizes
	barray(builder::dyn_var<T*> arr, std::vector<int> sizes): m_arr(arr), m_sizes(sizes) {}

	// Constructor to allocate a new barray with provided size
	barray(std::vector<int> sizes) {
		allocate(sizes);
	}

	// Function to compute flat array size
	int get_total_size() {	
		int total_size = 1;
		for (auto size: m_sizes) total_size *= size;
		return total_size;
	}

	// Manual allocate function to allocate data with given dimensions
	void allocate(std::vector<int> sizes) {
		m_sizes = sizes;
		m_arr = runtime::malloc((int)sizeof(T) * get_total_size());
	}

	// Function to allocate memory on the GPU
	void allocate_device(std::vector<int> sizes) {
		if (device_allocated) return;
		m_device_arr = runtime::cuda_malloc((int)sizeof(T) * get_total_size());
		device_allocated = 1;
	}

	// Function to transfer the array to the GPU
	void to_device() {
		allocate_device(m_sizes);
		if (current_storage == DEVICE_GPU) return;
		runtime::cuda_memcpy_to_device(m_device_arr, m_arr, (int)sizeof(T) * get_total_size());	
	}

	// Function to transfer the array back to the HOST
	void to_host() {
		if (current_storage == DEVICE_HOST) return;
		runtime::cuda_memcpy_to_host(m_arr, m_device_arr, (int)sizeof(T) * get_total_size());	
	}

	// Delete the default implementations of the operators
	// and copy constructors
	barray(const barray& other) = delete;


	// Helper function to induce loop nests and compute values
	void induce_loop_at(std::vector<builder::dyn_var<int>*> indices, const barray_expr<T>& rhs) {
		unsigned int index = indices.size();

		if (index == m_sizes.size()) {
			if (current_device == DEVICE_HOST)
				m_arr[compute_flat_index(indices)] = rhs.get_value_at(indices);
			else 
				m_device_arr[compute_flat_index(indices)] = rhs.get_value_at(indices);
			return;
		}

		builder::dyn_var<int> i = 0;
		indices.push_back(i.addr());

		// TODO 5: Map loop nests to GPU if current device is GPU

		// TODO 2: Recursively induce loops for all dimensions
		induce_loop_at(indices, rhs);
	}

	// Assignment operator overloads
	void operator=(const barray_expr<T> & rhs) {
		match_expr_sizes(m_sizes, rhs.get_expr_size());
		induce_loop_at({}, rhs);
		is_constant = false;
		current_storage = current_device;
	}
	
	void operator=(const T& value) {
		int total_size = get_total_size();

		// TODO 1: Initialize all elements of the array to value
		
		// TODO 4: Optimize arrays initialized to constants
		current_storage = DEVICE_HOST;

	}

	void operator=(const barray& other) {
		*this = barray_expr_array<T>(other);
	}


	// Function to convert multi-dimensional index into a single index
	// based on the sizes
	builder::dyn_var<int> compute_flat_index(std::vector<builder::dyn_var<int>*> indices, unsigned int index = -1) const {
		if (index == -1)
			index = m_sizes.size() - 1;
		if (index == 0) 
			return *(indices[index]);
		return *(indices[index]) + (int) m_sizes[index] * compute_flat_index(indices, index - 1);
	}

	
	// Function to write the constant value to the array if
	// it is optimized
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

	const std::vector<int> get_expr_size(void) const {
		return {};
	}
};

// RHS class for holding array operands
template <typename T>
struct barray_expr_array: public barray_expr<T>{
	const struct barray<T>& m_array;
	barray_expr_array(const struct barray<T>& array): m_array(array) {}

	const builder::dyn_var<T> get_value_at(std::vector<builder::dyn_var<int>*> indices) const {
		// TODO 6: Make sure the array is on the GPU if requested from GPU

		// TODO 4.2: Optimize arrays initialized to constants
		if (current_device == DEVICE_HOST)
			return const_cast<builder::dyn_var<T*>&>(m_array.m_arr)[m_array.compute_flat_index(indices)];
		else
			return const_cast<builder::dyn_var<T*>&>(m_array.m_device_arr)[m_array.compute_flat_index(indices)];
	}

	const std::vector<int> get_expr_size(void) const {
		return m_array.m_sizes;
	}
	
};

// Non leaf RHS expr classes

// Class for storing addition of two expressions
template <typename T>
struct barray_expr_add: public barray_expr<T> {
	const struct barray_expr<T>& expr1;
	const struct barray_expr<T>& expr2;

	std::vector<int> computed_sizes;

	barray_expr_add(const struct barray_expr<T>& expr1, const struct barray_expr<T>& expr2):
		expr1(expr1), expr2(expr2) {
		computed_sizes = match_expr_sizes(expr1.get_expr_size(), expr2.get_expr_size());
	}

	const std::vector<int> get_expr_size(void) const {
		return computed_sizes;
	}

	const builder::dyn_var<T> get_value_at(std::vector<builder::dyn_var<int>*> indices) const {
		return expr1.get_value_at(indices) + expr2.get_value_at(indices);
	}
};


// Class for pointwise multiplication of two expressions
template <typename T>
struct barray_expr_mul: public barray_expr<T> {
	const struct barray_expr<T>& expr1;
	const struct barray_expr<T>& expr2;

	std::vector<int> computed_sizes;

	barray_expr_mul(const struct barray_expr<T>& expr1, const struct barray_expr<T>& expr2):
		expr1(expr1), expr2(expr2) {
		computed_sizes = match_expr_sizes(expr1.get_expr_size(), expr2.get_expr_size());
	}

	const std::vector<int> get_expr_size(void) const {
		return computed_sizes;
	}

	const builder::dyn_var<T> get_value_at(std::vector<builder::dyn_var<int>*> indices) const {
		return expr1.get_value_at(indices) * expr2.get_value_at(indices);
	}
};

// Class for cross product of two expressions
template <typename T>
struct barray_expr_cross: public barray_expr<T> {
	const struct barray_expr<T>& expr1;
	const struct barray_expr<T>& expr2;

	std::vector<int> computed_sizes;

	barray_expr_cross(const struct barray_expr<T>& expr1, const struct barray_expr<T>& expr2):
		expr1(expr1), expr2(expr2) {
		std::vector<int> size1 = expr1.get_expr_size();
		std::vector<int> size2 = expr2.get_expr_size();
		assert(size1.size() == 2 && size2.size() == 2 && "Cross can only be performed on 2 dimensional arrays");
		assert(size1[1] == size2[0] && "Dimensions should match for cross product");
		computed_sizes.push_back(size1[0]);
		computed_sizes.push_back(size2[1]);
	}

	const std::vector<int> get_expr_size(void) const {
		return computed_sizes;
	}

	const builder::dyn_var<T> get_value_at(std::vector<builder::dyn_var<int>*> indices) const {
		builder::dyn_var<T> sum = 0;
		builder::dyn_var<int> i = 0; 
		std::vector<builder::dyn_var<int>*> indices1 = {indices[0], i.addr()};
		std::vector<builder::dyn_var<int>*> indices2 = {i.addr(), indices[1]};
		int len = expr1.get_expr_size()[1];

		// TODO: Implement cross product operator		

		return sum;
	}
	
};
}

#endif
