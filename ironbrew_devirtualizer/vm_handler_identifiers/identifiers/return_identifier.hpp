#pragma once

#include "../identifier_handler.hpp"

namespace deobf::ironbrew_devirtualizer::vm_handler_identifiers {
	using namespace ast;

	struct return_identifier final : public basic_opcode_handler {
		vm_arch::opcode handle(ir::statement::block* path) override {
			if (path->body.size() == 1) {
				if (auto scope = path->body.at(0)->as<ir::statement::do_block>()) {
					if (scope->body->body.size() == 0 && scope->body->ret.has_value()) {
						if (auto func_call = scope->body->ret.value()->find_first_of<ir::expression::function_call>()) {
							if (func_call->get().name.has_value() && func_call->get().name.value()->to_string() == "unpack") {
								if (func_call->get().arguments.at(0)->body.back()->is<ir::expression::binary_expression>()) {
									return vm_arch::opcode::op_return;
								}
								else {
									return vm_arch::opcode::op_return3;
								}
							}
						}
						else {
							const auto num_returns = scope->body->ret.value()->body.size();

							switch (num_returns) {
								case 0:
									return vm_arch::opcode::op_return4;
								case 1:
									return vm_arch::opcode::op_return1;
								case 2:
									return vm_arch::opcode::op_return2;
							}
						}
					}
				}
			}

			return __super::handle(path);
		}
	};
}