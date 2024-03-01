#ifndef BARRAY_H
#define BARRAY_H

#include "barray_impl.h"
#include "operators.h"
#include "blocks/c_code_generator.h"
#include "blocks/rce.h"

namespace barray {

template <typename F, typename...OtherArgs>
void generate_barray_program(F func_input, std::string fname, std::ostream &oss, OtherArgs...args) {
	builder::builder_context ctx;
	auto ast = ctx.extract_function_ast(func_input, fname, args...);
	block::eliminate_redundant_vars(ast);
	block::c_code_generator::generate_code(ast, oss, 0);
}

}


#endif


