#pragma once

#include "../identifier_handler.hpp"

namespace deobf::ironbrew_devirtualizer::vm_handler_identifiers {
	using namespace ast;

	struct test_identifier final : public basic_opcode_handler {
		vm_arch::opcode handle(ir::statement::block* path) override {
			if (path->body.size() == 1) {
				if (auto test_statement = path->find_first_of<ir::statement::if_statement>()) {
					auto condition = test_statement.value().get().condition->to_string();

					bool is_c0 = (condition == "stack[instruction_opcode_a]");
					bool is_c1 = (condition == "not( stack[instruction_opcode_a] )");

					if (is_c0) {
						return vm_arch::opcode::op_test;
					}
					else if (is_c1) {
						return vm_arch::opcode::op_test1;
					}
				}
			}

			return __super::handle(path);
		}
	};
}