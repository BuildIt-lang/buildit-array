#include "barray.h"
#include "blocks/matchers/patterns.h"
#include "blocks/matchers/matchers.h"
#include "blocks/matchers/replacers.h"
namespace barray {

current_device_t current_device = DEVICE_HOST;

barray_base::barray_base() {
	registered_arrays.push_back(this);
}
barray_base::~barray_base() {
	registered_arrays.erase(std::remove(registered_arrays.begin(), registered_arrays.end(), this), 
		registered_arrays.end());
}
std::vector<barray_base*> barray_base::registered_arrays;

std::vector<int> match_expr_sizes(const std::vector<int> size1, const std::vector<int> size2) {
	if (size1.size() == 0)
		return size2;
	if (size2.size() == 0)
		return size1;
	assert(size1.size() == size2.size() && "Sizes of arrays do not agree for operation");


	// TODO 3: Make sure the sizes of all the dimensions are equal

	return size1;
}
void apply_gpu_fixups(block::block::Ptr ast) {
	// We want to check for simple patterns of the form - 
	// e1 = e1 + A[e2 + threadIdx.x] * B[e3 + threadIdx.x];
	// and replace it with 
	// e1 = e1 + runtime::dot_product(A + e2, B + e3, blockDim.x)
	using namespace block::matcher;
	auto p1 = sq_bkt_expr(expr("A"), expr("e2") + var_with_name("threadIdx.x"));
	auto p2 = sq_bkt_expr(expr("B"), expr("e3") + var_with_name("threadIdx.x"));
	auto p3 = assign_expr(expr("e1"), expr("e1") + p1 * p2);
	
	// TODO 8: // Match patterns and replace them with new patterns
}

}
