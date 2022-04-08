#pragma once

#include "../identifier_handler.hpp"

namespace deobf::ironbrew_devirtualizer::vm_handler_identifiers {
	using namespace ast;

	struct vararg_identifier final : public basic_opcode_handler {
		vm_arch::opcode handle(ir::statement::block* path) override {
			auto for_step = path->find_first_of<ir::statement::for_step>();
			if (for_step.has_value()) {
				auto variable_assign = for_step->get().body->find_first_of<ir::statement::variable_assign>();
				if (variable_assign.has_value() && variable_assign->get().expressions.size() == 1) {
					if (auto assign_right = variable_assign->get().expressions.at(0)->as<ir::expression::variable>()) {
						const auto vararg_name = assign_right->name->to_string();

						if (vararg_name == "varargs") {
							std::cout << "vararg!\n";
							return vm_arch::opcode::op_vararg;
						}
						else if (auto symbol = for_step->get().body->find_symbol(vararg_name)) {
							if (auto variable_value = symbol->symbol_value->as<ir::expression::variable>()) {
								if (variable_value->name->to_string() == "varargs") {
									std::cout << "vararg!2\n";
									return vm_arch::opcode::op_vararg1;
								}
							}
						}
					}
				}
			}

			return __super::handle(path);
		}
	};
}