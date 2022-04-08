#pragma once

#include "../identifier_handler.hpp"

namespace deobf::ironbrew_devirtualizer::vm_handler_identifiers {
	using namespace ast;

	struct concat_identifier final : public basic_opcode_handler {
		vm_arch::opcode handle(ir::statement::block* path) override {
			auto for_step = path->find_first_of<ir::statement::for_step>();
			if (for_step.has_value()) {
				auto binary_exp = for_step->get().body->find_first_of<ir::expression::binary_expression>();
				if (binary_exp.has_value() && binary_exp->get().operation == ir::expression::binary_expression::operation_t::concat) {
					return vm_arch::opcode::op_concat;
				}
			}

			return __super::handle(path);
		}
	};
}