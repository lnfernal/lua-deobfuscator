#pragma once

#include "./ast/optimization_passes/base_optimizer.hpp"
#include "./ast/ir/abstract_visitor_pattern.hpp"
#include "./ast/ir/node.hpp"
#include "./ast/ir/statement.hpp"

//#include "ironbrew_devirtualizer/devirtualizer_context/devirtualizer_context.hpp"

namespace deobf::ironbrew_devirtualizer::devirtualizer_ast_markers {
	using namespace ast;

	struct mark_wrapper final : private ast::optimization_passes::base_optimizer {
		void optimize() override;

		explicit mark_wrapper(ast::ir::statement::block* root) :
			base_optimizer(root)
		{ }
	};
}