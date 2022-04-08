#pragma once

#include "../identifier_handler.hpp"

namespace deobf::ironbrew_devirtualizer::vm_handler_identifiers {
	using namespace ast;

	struct loadnil_identifier final : public basic_opcode_handler {
		vm_arch::opcode handle(ir::statement::block* path) override {
			if (auto result = path->find_first_of<ir::statement::variable_assign>(); result.has_value()) {
				if (result->get().variables.size() == 1 && result->get().expressions.size() == 1) {
					if (auto variable = result->get().variables.at(0)->as<ir::expression::variable>()) {
						if (variable->name->to_string() == "stack" && result->get().expressions.at(0)->is<ir::expression::nil_literal>()) {
							std::cout << "LOADNIL!\n";
							return vm_arch::opcode::op_loadnil;
						}
					}
				}
			}

			return __super::handle(path);
		}
	};
}