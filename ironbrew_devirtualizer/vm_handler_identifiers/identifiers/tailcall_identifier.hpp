#pragma once

#include "../identifier_handler.hpp"

namespace deobf::ironbrew_devirtualizer::vm_handler_identifiers {
	using namespace ast;

	struct tailcall_identifier final : public basic_opcode_handler {
		vm_arch::opcode handle(ir::statement::block* path) override {
			auto do_statement = path->find_first_of<ir::statement::do_block>();
			if (do_statement.has_value()) {
				auto& body_wrapper = do_statement->get();
				std::cout << "dostat\n";
				if (body_wrapper.body->body.size() == 0) {
					auto call_statement = body_wrapper.find_first_of<ir::expression::function_call>();
					if (call_statement.has_value()) {
						auto& call_wrapper = call_statement->get();
						
						// .to_string() result -> "stack[instruction_opcode_a](unpack(stack,( instruction_opcode_a + 1 ),instruction_opcode_b))"
						
						if (auto variable_name = call_wrapper.name.value()->as<ir::expression::variable>()) {
							if (variable_name->to_string() == "stack[instruction_opcode_a]") {
								if (call_wrapper.arguments.size() == 0) {
									return vm_arch::opcode::op_tailcall2;
								}
								else if (auto argument = call_wrapper.arguments.at(0)->body.at(0)->as<ir::expression::function_call>()) {
									std::cout << argument->to_string() << std::endl;
									if (argument->arguments.at(0)->body.size() == 3) {
										if (auto last_argument = argument->arguments.at(0)->body.back()->as<ir::expression::variable>()) {
											if (last_argument->to_string() == "instruction_opcode_b") {
												return vm_arch::opcode::op_tailcall;
											}
											else {
												return vm_arch::opcode::op_tailcall2;
											};
										}
									}
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