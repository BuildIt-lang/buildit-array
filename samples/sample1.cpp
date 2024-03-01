#include "barray.h"

void test_program(builder::dyn_var<int*> arr1, size_t dim1, size_t dim2, size_t dim3) {
	barray::barray<int> y(arr1, {dim1, dim2, dim3});
	barray::barray<int> z({dim1, dim2, dim3});
	barray::barray<int> w({dim1, dim2, dim3});

	z = 0;
	w = 1;
	for (builder::dyn_var<int> x = 0; x < 5; ++x) {
		y = 5 + z + w * 2;
		w = y;
	}
}


int main(int argc, char* argv[]) {
	barray::generate_barray_program(test_program, "test", std::cout, 32, 16, 8);
	return 0;
}
