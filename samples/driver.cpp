#include <stdlib.h>
void test (int*);
int main(int argc, char* argv[]) {
	int * buffer = malloc(sizeof(int) * 16384);
	test(buffer);
}
