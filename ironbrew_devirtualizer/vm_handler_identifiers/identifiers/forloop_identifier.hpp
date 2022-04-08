#pragma once

#include "../identifier_handler.hpp"

namespace deobf::ironbrew_devirtualizer::vm_handler_identifiers {
	using namespace ast;

	struct forloop_identifier final : public basic_opcode_handler {
		vm_arch::opcode handle(ir::statement::block* path) override {
			/*auto binary_exp = path->find_first_of<ir::expression::binary_expression>();
			if (binary_exp.has_value() && binary_exp->get().operation == ir::expression::binary_expression::operation_t::add) {
				if (auto symbol = path->find_symbol(binary_exp->get().right->to_string())) {
					if (symbol->symbol_value->to_string() == "stack[( instruction_opcode_a + 2 )]") {
						return vm_arch::opcode::op_forloop;
					}
				}
			}*/
			/*if (path->body.size() > 2) {
				if (auto declare_statement = path->body.at(1)->as<ir::statement::local_declaration>()) {
					if (declare_statement->body.size() == 1) {
						if (declare_statement->body.at(0)->to_string() == "stack[( instruction_opcode_a + 2 )]") {
							return vm_arch::opcode::op_forprep;
						}
					}
				}
			}*/

			// DEPRECATED CEHCK FORPREP

			return __super::handle(path);
		}
	};
}