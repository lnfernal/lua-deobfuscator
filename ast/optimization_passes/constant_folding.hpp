#pragma once

#include "base_optimizer.hpp"

namespace deobf::ast::optimization_passes {
	struct constant_folding final : private base_optimizer {
		void optimize() override;

		explicit constant_folding(ast::ir::node* root) :
			base_optimizer(root)
		{ }
	};
}