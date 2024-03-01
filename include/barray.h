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
	auto new_decls = block::extract_cuda_from(block::to<block::func_decl>(ast)->body);
	for (auto a : new_decls)
		block::c_code_generator::generate_code(a, oss, 0);
	block::c_code_generator::generate_code(ast, oss, 0);
}

}


#endif


