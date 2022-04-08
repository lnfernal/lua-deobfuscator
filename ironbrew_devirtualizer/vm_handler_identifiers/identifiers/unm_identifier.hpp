#pragma once

#include "../identifier_handler.hpp"

namespace deobf::ironbrew_devirtualizer::vm_handler_identifiers {
	using namespace ast;

	struct unm_identifier final : public basic_opcode_handler {
		vm_arch::opcode handle(ir::statement::block* path) override {
			if (path->body.size() == 1) {
				if (path->body.at(0)->to_string() == "stack[instruction_opcode_a] = -( stack[instruction_opcode_b] )") {
					return vm_arch::opcode::op_unm;
				}
			}

			return __super::handle(path);
		}
	};
}