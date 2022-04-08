#pragma once

#include "../identifier_handler.hpp"

namespace deobf::ironbrew_devirtualizer::vm_handler_identifiers {
	using namespace ast;

	struct loadbool_identifier final : public basic_opcode_handler {
		vm_arch::opcode handle(ir::statement::block* path) override {
			const auto is_c0 = (path->body.size() == 1);
			const auto is_c1 = (path->body.size() == 2);

			if (is_c0 || is_c1) {
				if (auto assign = path->body.at(0)->as<ir::statement::variable_assign>()) {
					const auto assign_string = assign->to_string();
					if (assign_string == "stack[instruction_opcode_a] = ( instruction_opcode_b ~= 0 )") {
						return is_c1 ? vm_arch::opcode::op_loadbool1 : vm_arch::opcode::op_loadbool;
					}
				}
			}
			
			return __super::handle(path);
		}
	};
}