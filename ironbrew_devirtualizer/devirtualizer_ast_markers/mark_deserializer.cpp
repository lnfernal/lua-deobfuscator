#include "mark_deserializer.hpp"
#include "marker_decorator_interface.hpp"

namespace deobf::ironbrew_devirtualizer::devirtualizer_ast_markers {
	using namespace ast;

	struct deserializer_marker_visitor final : marker_decorator_interface {
		using marker_decorator_interface::marker_decorator_interface;

		/*bool accept(ir::expression::function_call* expression) override {
			if (expression->name.has_value()) {
				if (expression->name.value()->to_string() == "string.char") {
					if (const auto xor_call = expression->arguments.at(0)->body.at(0)->as<ir::expression::function_call>()) {
						const auto& call_arguments = xor_call->arguments.at(0)->body;
						if (call_arguments.size() == 2) {
							if (const auto vm_xor_key = call_arguments.back()->as<ir::expression::numeral_literal>()) {
								
								std::cout << "vm_xor_key:" << vm_xor_key->to_string() << std::endl;

								//current_block->insert_global_symbol("vm_string", vm_string);
								//current_block->insert_symbol<true>("vm_xor_key", vm_xor_key);
							}
						}
					}

				}
			}
			return true;
		}*/

		bool accept(ir::statement::variable_assign* statement) override {
			if (statement->variables.size() == 1 && statement->expressions.size() == 1) {
				if (auto get_call = statement->expressions.at(0)->as<ir::expression::function_call>()) {
					const auto& call_arguments = get_call->arguments.at(0)->body;
					if (call_arguments.size() == 3) {
						std::cout << get_call->name.value()->to_string() << std::endl;
						if (get_call->name.has_value(); get_call->name.value()->to_string() == "string.sub") {
							std::cout << call_arguments.at(0)->to_string() << std::endl;
							if (const auto symbol = current_block->find_symbol(call_arguments.at(0)->to_string())) {
								if (auto call_result = symbol->symbol_value->as<ir::expression::function_call>()) { // bytecode compression on
									const auto vm_string = call_result->arguments.at(0)->body.at(0);

									std::cout << "vm_string:" << vm_string->to_string() << std::endl;

									current_block->insert_symbol<true>("vm_string", vm_string);
								}
								else if (auto string_result = symbol->symbol_value->as<ir::expression::string_literal>()) { // bytecode compression off (edge case)
									std::cout << "vm_string:" << string_result->value << std::endl;

									current_block->insert_symbol<true>("vm_string", string_result);
								}
								return false;
							}
						}
					}
				}
				else if (statement->variables.at(0)->to_string() == "ByteString") { // kinda lazy ik
					current_block->insert_symbol<true>("vm_string", statement->expressions.at(0));
				}
			}
			return true;
		}

		// aztupbrew
		/*bool accept(ir::expression::function* expression) override {
			if (expression->body->ret.has_value() && expression->body->ret.value()->to_string() == "return function(...)") {
				const auto& get_call = expression->body->find_first_of<ir::expression::function_call>();
				if (auto symbol = current_block->find_symbol(get_call.value().get().name.value()->to_string())) {
					symbol->resolve_identifier = "bytecode_deserializer";
					
					const auto& function_body = symbol->symbol_value->as<ir::expression::function>();

					function_body->accept(this); // we must apply those changes since we are marking a function that may get visited after deserializer

					current_block->insert_global_symbol("bytecode_deserializer", function_body->body);

					return false;
				}
			}

			return true;
		}*/
	};

	void mark_deserializer::optimize() {
		deserializer_marker_visitor optimizer{ "bytecode_deserializer", "deserialize_return" };

		// ironbrew
		if (auto block = root->as<ir::statement::block>(); block->ret.has_value()) {
			std::cout << block->ret.value()->to_string() << std::endl;
			const auto function_call = block->ret.value()->body.at(0)->as<ir::expression::function_call>();
			const auto call_first_variable = function_call->arguments.at(0)->body.at(0)->find_first_of<ir::expression::variable>();
			if (call_first_variable.has_value()) {
				const auto deserialize_symbol = call_first_variable.value().get().to_string();
				if (const auto result = block->find_symbol(deserialize_symbol)) {
					const auto& deseriaize_function = result->symbol_value->as<ir::expression::function>();
					result->resolve_identifier = "bytecode_deserializer";
					block->insert_symbol<true>("bytecode_deserializer", result->symbol_value->as<ir::expression::function>()->body);
					
					if (deseriaize_function->body->ret.has_value() && deseriaize_function->body->ret.value()->body.size() == 1) {
						const auto return_symbol = deseriaize_function->body->ret.value()->body.at(0)->to_string();
						if (auto symbol_result = deseriaize_function->body->find_symbol(return_symbol)) {
							symbol_result->resolve_identifier = "deserialize_return";
						}
					}

				}
			}
		}

		root->accept(&optimizer);
	}
}