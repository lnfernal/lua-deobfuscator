#pragma once

#include "../identifier_handler.hpp"

namespace deobf::ironbrew_devirtualizer::vm_handler_identifiers {
	using namespace ast;

	struct self_identifier final : public basic_opcode_handler {
		vm_arch::opcode handle(ir::statement::block* path) override {
			
			auto result = path->get_children_type<ir::statement::variable_assign>();

			std::cout << "sz:" << result.size() << std::endl;

			if (result.size() == 2) {
				bool is_self = false;

				auto first_variable = result.at(0).get().variables.at(0);
				if (auto variable = first_variable->as<ir::expression::variable>()) {
					std::cout << "abb:" << variable->to_string() << std::endl;
					is_self = (variable->to_string() == "stack[( instruction_opcode_a + 1 )]");
				}
				
				if (!is_self)
					goto handle_proc;

			
				if (auto variable = result.at(1).get().expressions.at(0)->as<ir::expression::variable>()) {
					bool is_kc = (variable->suffixes.at(0)->to_string() == "[instruction_opcode_c]");
					return (is_kc ? vm_arch::opcode::op_self1 : vm_arch::opcode::op_self);
				}
			}

			/*if (auto variable_assign = stat->as<ir::statement::variable_assign>()) {
				if (variable_assign->variables.size() == 1) {
					auto first_variable = variable_assign->variables.at(0);
					if (auto variable = first_variable->as<ir::expression::variable>()) {
						if (variable->to_string() == "stack[( instruction_opcode_a + 1 )]") {
							return vm_arch::opcode::op_self;
						}
					}
				}
			}*/

			handle_proc:
			return __super::handle(path);
		}
	};
}