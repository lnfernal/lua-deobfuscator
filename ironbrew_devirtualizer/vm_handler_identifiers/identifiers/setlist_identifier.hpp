#pragma once

#include "../identifier_handler.hpp"

namespace deobf::ironbrew_devirtualizer::vm_handler_identifiers {
	using namespace ast;

	struct setlist_identifier final : public basic_opcode_handler {
		vm_arch::opcode handle(ir::statement::block* path) override {
			if (auto for_step = path->find_first_of<ir::statement::for_step>(); for_step.has_value()) {
				auto function_call = for_step.value().get().body->find_first_of<ir::expression::function_call>();
				if (function_call.has_value()) {
					if (auto function_name = function_call->get().name; function_name.has_value()) {
						if (function_name.value()->to_string() == "table.insert") {
							if (auto result = path->find_first_of<ir::statement::variable_assign>(); result.has_value()) {
								if (result.value().get().to_string() == "instruction_pointer = ( instruction_pointer + 1 )") {
									return vm_arch::opcode::op_setlist2;
								}
							}

							if (auto variable = for_step.value().get().end->as<ir::expression::variable>()) {
								if (variable->to_string() != "instruction_opcode_b") { // top
									return vm_arch::opcode::op_setlist1;
								}
							}

							return vm_arch::opcode::op_setlist;
						}
					}
				}
			}

			return __super::handle(path);
		}
	};
}