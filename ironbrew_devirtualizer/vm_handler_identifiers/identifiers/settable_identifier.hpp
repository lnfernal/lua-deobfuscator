#pragma once

#include "../identifier_handler.hpp"

namespace deobf::ironbrew_devirtualizer::vm_handler_identifiers {
	using namespace ast;

	struct settable_identifier final : public basic_opcode_handler {
		vm_arch::opcode handle(ir::statement::block* path) override {
			if (path->body.size() == 1) {
				if (auto assign = path->body.at(0)->as<ir::statement::variable_assign>()) {
					const auto assign_string = assign->to_string();

					const auto is_stk = (assign_string == "stack[instruction_opcode_a][stack[instruction_opcode_b]] = stack[instruction_opcode_c]");
					const auto is_kb = (assign_string == "stack[instruction_opcode_a][instruction_opcode_b] = stack[instruction_opcode_c]");
					const auto is_kc = (assign_string == "stack[instruction_opcode_a][stack[instruction_opcode_b]] = instruction_opcode_c");
					const auto is_kbc = (assign_string == "stack[instruction_opcode_a][instruction_opcode_b] = instruction_opcode_c");

					if (is_stk) {
						return vm_arch::opcode::op_settable;
					}
					else if (is_kb) {
						return vm_arch::opcode::op_settable1;
					}
					else if (is_kc) {
						return vm_arch::opcode::op_settable2;
					}
					else if (is_kbc) {
						return vm_arch::opcode::op_settable3;
					}
				}
			}

			return __super::handle(path);
		}
	};
}