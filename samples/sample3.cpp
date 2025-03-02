#include "barray.h"


// A simple program to compute the dot product of two matricies
void dot_product(builder::dyn_var<int*> arr1, builder::dyn_var<int*> arr2, builder::dyn_var<int*> arr_out, 
		int M, int N) {

	// Reset all values
	for (builder::dyn_var<int> i = 0; i < M; i++) {
		arr_out[i] = 0;
	}

	for (builder::dyn_var<int> i = 0; i < M; i++) {
		for (builder::dyn_var<int> j = 0; j < N; j++) {
			arr_out[i] += arr1[i * M + j] * arr2[i * M + j];
		}
	}
	
	
}


int main(int argc, char* argv[]) {
	barray::generate_barray_program(dot_product, "dot_product", std::cout, 32, 512);
	return 0;
}
