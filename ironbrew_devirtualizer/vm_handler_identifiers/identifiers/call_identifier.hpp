#pragma once

#include "../identifier_handler.hpp"

namespace deobf::ironbrew_devirtualizer::vm_handler_identifiers {
	using namespace ast;

	struct call_identifier final : public basic_opcode_handler {
		vm_arch::opcode handle(ir::statement::block* path) override {
			auto binary_exp = path->find_first_of<ir::expression::binary_expression>();
			if (binary_exp.has_value() && binary_exp->get().right->to_string() == "2") {
				return __super::handle(path);
			}

			if (!path->find_first_of<ir::statement::do_block>().has_value()) {
				auto function_call = path->find_first_of<ir::expression::function_call>();
				if (function_call.has_value()) {
					if (auto function_name = function_call->get().name; function_name.has_value()) {
						const auto string_name = function_name.value()->as<ir::expression::variable>();
						
						//call 5 & 2 todo

						if (string_name->to_string() == "pack_return") {
							if (auto func_call_pack = function_call->get().arguments.at(0)->body.at(0)->as<ir::expression::function_call>()) {
								if (func_call_pack->arguments.size() == 1) {
									auto& body = func_call_pack->arguments.at(0)->body;
									if (body.size() == 1) {
										if (!body.at(0)->is<ir::expression::function_call>()) {
											return vm_arch::opcode::op_call5;
										}
										else {
											auto func_call = static_cast<ir::expression::function_call*>(body.at(0).get());
											auto& fbody = func_call->arguments.at(0)->body;
											if (fbody.size() == 3) {
												if (fbody.at(2)->to_string() == "instruction_opcode_b") {
													return vm_arch::opcode::op_call4;
												}
												else {
													return vm_arch::opcode::op_call8;
												}
											}
										}
									}
									else {
										return vm_arch::opcode::op_call10;
									}
								}
							}
						}
						else if (string_name->name->to_string() == "stack" && string_name->suffixes.size() == 1 && string_name->suffixes.front()->name->to_string() == "instruction_opcode_a") { // A (call succesor)
							// WIP TODO 1-15
							const auto has_table = path->find_first_of<ir::expression::table>().has_value();
							const auto has_assign = path->find_first_of<ir::statement::variable_assign>().has_value();

							if (function_call->get().arguments.size() == 1) {
								
								auto& body = function_call->get().arguments.at(0)->body;
								if (body.size() == 1) {
									if (!body.at(0)->is<ir::expression::function_call>()) {
										
										if (has_table) {
											return vm_arch::opcode::op_call1;
										}
										else if (has_assign) {
											return vm_arch::opcode::op_call13;
										}

										return vm_arch::opcode::op_call7;
									}
									else {
										auto func_call = static_cast<ir::expression::function_call*>(body.at(0).get());
										auto& fbody = func_call->arguments.at(0)->body;
										if (fbody.size() == 3) {
											if (fbody.at(2)->to_string() == "instruction_opcode_b") {
												if (has_table) {
													return vm_arch::opcode::op_call;
												}
												else if (has_assign) {
													return vm_arch::opcode::op_call12;
												}

												return vm_arch::opcode::op_call6;
											}
											else {
												
												if (has_table) {
													return vm_arch::opcode::op_call2;
												}
												else if (has_assign) {
													return vm_arch::opcode::op_call14;
												}

												return vm_arch::opcode::op_call9;
											}
										}
									}
								}
								else {
									if (has_table) {
										return vm_arch::opcode::op_call3;
									}
									else if (has_assign) {
										return vm_arch::opcode::op_call15;
									}

									return vm_arch::opcode::op_call11;
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