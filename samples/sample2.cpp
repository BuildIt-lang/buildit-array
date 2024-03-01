#include "barray.h"


// A simple program to compute the cross product of two matricies
void cross_product(builder::dyn_var<int*> arr1, builder::dyn_var<int*> arr2, builder::dyn_var<int*> arr_out, 
		int M, int N, int O) {

	barray::barray<int> x(arr1, {M, N});
	barray::barray<int> y(arr2, {N, O});
	barray::barray<int> z(arr_out, {M, O});

	// Invoke the cross operator
	z = 5 * cross(x, y);
	
}


int main(int argc, char* argv[]) {
	barray::generate_barray_program(cross_product, "cross_product", std::cout, 32, 16, 8);
	return 0;
}
