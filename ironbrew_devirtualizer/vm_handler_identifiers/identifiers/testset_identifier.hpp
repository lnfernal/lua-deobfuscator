#pragma once

#include "../identifier_handler.hpp"

namespace deobf::ironbrew_devirtualizer::vm_handler_identifiers {
	using namespace ast;

	struct testset_identifier final : public basic_opcode_handler {
		vm_arch::opcode handle(ir::statement::block* path) override {
			if (path->body.size() > 1) {
				if (auto test_statement = path->find_first_of<ir::statement::if_statement>(); test_statement.has_value()) {
					if (auto variable = test_statement->get().find_first_of<ir::expression::variable>()) {
						if (auto symbol = path->find_symbol(variable->get().to_string())) {
							if (symbol->symbol_value->to_string() == "stack[instruction_opcode_c]") {
	
								if (test_statement->get().condition->is<ir::expression::unary_expression>())
									return vm_arch::opcode::op_testset1;

								return vm_arch::opcode::op_testset;
							}
						}
					}
				}
			}

			return __super::handle(path);
		}
	};
}