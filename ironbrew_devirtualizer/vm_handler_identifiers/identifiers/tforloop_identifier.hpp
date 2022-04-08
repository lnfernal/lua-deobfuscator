#pragma once

#include "../identifier_handler.hpp"

namespace deobf::ironbrew_devirtualizer::vm_handler_identifiers {
	using namespace ast;

	struct tforloop_identifier final : public basic_opcode_handler {
		vm_arch::opcode handle(ir::statement::block* path) override {
			auto function_call = path->find_first_of<ir::expression::function_call>();
			if (function_call.has_value() && function_call.value().get().arguments.at(0)->body.size() == 2) {
				if (auto function_name = function_call->get().name; function_name.has_value()) {
					const auto string_name = function_name.value()->as<ir::expression::variable>();

					if (string_name->name->to_string() == "stack" && string_name->suffixes.size() == 1 && string_name->suffixes.front()->name->to_string() == "instruction_opcode_a") {
						return vm_arch::opcode::op_tforloop;
					}
				}
			}

			return __super::handle(path);
		}
	};
}