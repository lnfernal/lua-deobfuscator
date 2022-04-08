#pragma once

#include "../identifier_handler.hpp"

namespace deobf::ironbrew_devirtualizer::vm_handler_identifiers {
	using namespace ast;

	struct lt_identifier final : public basic_opcode_handler {
		vm_arch::opcode handle(ir::statement::block* path) override {
			if (path->body.size() == 1) {
				if (auto if_stat = path->body.at(0)->as<ir::statement::if_statement>()) {
					const auto expression_string = if_stat->condition->to_string();
					if (if_stat->body->body.size() != 1) {
						return __super::handle(path);
					}

					bool is_ge = (if_stat->body->body.at(0)->to_string() != "instruction_pointer = instruction_opcode_b");

					if (expression_string == "( instruction_opcode_a < instruction_opcode_c )") {
						return is_ge ? vm_arch::opcode::op_ge3 : vm_arch::opcode::op_lt3;
					}

					if (expression_string == "( stack[instruction_opcode_a] < instruction_opcode_c )") {
						return is_ge ? vm_arch::opcode::op_ge2 : vm_arch::opcode::op_lt2;
					}

					if (expression_string == "( instruction_opcode_a < stack[instruction_opcode_c] )") {
						return is_ge ? vm_arch::opcode::op_ge1 : vm_arch::opcode::op_lt1;
					}

					if (expression_string == "( stack[instruction_opcode_a] < stack[instruction_opcode_c] )") {
						return is_ge ? vm_arch::opcode::op_ge : vm_arch::opcode::op_lt;
					}
				}
			}

			return __super::handle(path);
		}
	};
}