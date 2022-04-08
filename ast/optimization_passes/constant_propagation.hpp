#pragma once

#include "base_optimizer.hpp"

// semantic analysis
namespace deobf::ast::optimization_passes {
	struct constant_propagation final : private base_optimizer {
		void optimize() override;

		explicit constant_propagation(ast::ir::node* root) :
			base_optimizer(root)
		{ }
	};
}
