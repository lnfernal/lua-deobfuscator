#include "mark_wrapper.hpp"
#include "marker_decorator_interface.hpp"
#include <mutex>

namespace deobf::ironbrew_devirtualizer::devirtualizer_ast_markers {

	// marks constants, instr, protos, parameters, instruction point, vararg size, stack, upvalues, top.

	struct mark_wrapper_visitor final : devirtualizer_ast_markers::marker_decorator_interface {
		using marker_decorator_interface::marker_decorator_interface;
		
		std::once_flag has_visited_for;

		static inline std::unordered_map<std::string_view, std::string_view> result_indice_mapping {
			{ "deserialize_result[1]", "instructions" },
			{ "deserialize_result[2]", "constants" },
			{ "deserialize_result[3]", "protos" }, // will that be different when theres no deserialize_result[4] ?
			{ "deserialize_result[4]", "parameters" },
			{ "1", "instruction_pointer" },
			{ "-( 1 )", "stack_top" },
		};

		bool accept(ir::statement::local_declaration* statement) override {
			if (statement->names.size() == 1 && statement->body.size() == 1) {
				//statement->body.at(0)->accept(this);

				auto variable_name = statement->names.at(0)->to_string();
				auto body_string = statement->body.at(0)->to_string();

				if (const auto symbol = current_block->find_symbol(body_string)) {
					std::cout << symbol->symbol_value->to_string() << std::endl;
					if (const auto result = result_indice_mapping.find(symbol->symbol_value->to_string()); result != result_indice_mapping.cend()) {
						current_block->find_symbol(variable_name)->resolve_identifier = result->second;
						result_indice_mapping.erase(body_string);
					}
					else if (auto binary_expression = statement->body.at(0)->as<ir::expression::binary_expression>()) {
						using operation_t = typename ir::expression::binary_expression::operation_t;

						if (binary_expression->operation == operation_t::add && binary_expression->right->to_string() == "1") {
							if (auto left_branch = binary_expression->left->as<ir::expression::binary_expression>()) {
								if (left_branch->right->to_string() == "protos" || left_branch->right->to_string() == "parameters") {
									current_block->find_symbol(variable_name)->resolve_identifier = "vararg_size";
								}
							}
						}
					}
				}
				// other stuff
				else if (body_string == "pack_return") {
					current_block->find_symbol(variable_name)->resolve_identifier = body_string;
				}
				else if (const auto result = result_indice_mapping.find(body_string); result != result_indice_mapping.cend()) {
					current_block->find_symbol(variable_name)->resolve_identifier = result->second;
					result_indice_mapping.erase(body_string);
				}
			}

			return true;
		}

		bool accept(ir::statement::for_step* statement) override {
			if (statement->init->to_string() == "0") {
				if (auto symbol = current_block->find_symbol(statement->end->to_string())) {
					if (symbol->symbol_value->to_string().find("select") != -1) {
						// proccess if statement
						std::call_once(has_visited_for, [&]() { // some stuff produce same for loop
							auto if_statement = statement->body->find_first_of<ir::statement::if_statement>();
							if (if_statement.has_value()) {
								auto& body = if_statement.value().get().body;
								auto& else_body = if_statement.value().get().else_body.value();
								const auto vararg_identifier = body->find_first_of<ir::expression::variable>().value().get().name->to_string();
								const auto stack_identifier = else_body->find_first_of<ir::expression::variable>().value().get().name->to_string();

								current_block->find_symbol(vararg_identifier)->resolve_identifier = "varargs";
								current_block->find_symbol(stack_identifier)->resolve_identifier = "stack";
							}
						});
					}
				}
			}

			return true;
		}

		bool accept(ir::statement::while_statement* statement) override {
			// todo
			{
				auto result = statement->body->find_first_of<ir::statement::variable_assign>();
				if (result.has_value()) {
					auto& result_pointer = result.value().get();

					auto& assign_variable = result_pointer.variables.at(0);
					current_block->find_symbol(assign_variable->to_string())->resolve_identifier = "current_instruction";

					const auto instruction_pointer = result_pointer.find_first_of<ir::expression::variable_suffix>().value().get().name;
					current_block->find_symbol(instruction_pointer->to_string())->resolve_identifier = "instruction_pointer";
				}
			}

			{
				auto result = statement->find_first_of<ir::statement::if_statement>();
				if (result.has_value()) {
					auto& result_reference = result->get();
		
					current_block->insert_symbol<true>("__vm_entry__", result_reference.as<ir::statement::if_statement>());

					auto value = result_reference.condition;
					if (auto condition = value->as<ir::expression::binary_expression>()) {
						if (condition->right->is<ir::expression::numeral_literal>()) {
							current_block->find_symbol(condition->left->to_string())->resolve_identifier = "virtual_opcode";
							result_reference.accept(this);
							//const_cast<ir::statement::if_statement*>(&result_reference)->accept(this); // for renaming purposes
						}
					}
				}
			}

			return false;
		}
	};

	void mark_wrapper::optimize()
	{
		const auto block = static_cast<const ir::statement::block*>(root);
		std::cout << "optimizer(mark_wrapper)\n";
		mark_wrapper_visitor optimizer{ "wrap", "pack_return", "environment", "upvalues", "current_instruction", "stack", "varargs", "instructions", "constants", "protos", "parameters", "instruction_pointer", "stack_top", "vararg_size", "virtual_opcode", "deserialize_result" };
		if (block->ret.has_value()) {
			const auto ret = block->ret.value()->body.at(0)->as<ir::expression::function_call>();
			if (const auto symbol = block->find_symbol(ret->name.value()->to_string())) {
				const auto functional_value = symbol->symbol_value->as<ir::expression::function>();


				functional_value->body->find_symbol(functional_value->function_name.value())->resolve_identifier = "wrap";

				// parameter 3 is always environment
				functional_value->body->find_symbol(functional_value->parameters.at(2)->to_string())->resolve_identifier = "environment";
				functional_value->body->find_symbol(functional_value->parameters.at(1)->to_string())->resolve_identifier = "upvalues";
				functional_value->body->find_symbol(functional_value->parameters.at(0)->to_string())->resolve_identifier = "deserialize_result";

				// aztupbrew
				/*const auto deserialize_result = functional_value->find_first_of<ir::statement::variable_assign>();
				if (deserialize_result.has_value()) {
					functional_value->body->find_symbol(deserialize_result.value().get().variables.at(0)->to_string())->resolve_identifier = "deserialize_result";
				}*/

				functional_value->body->accept(&optimizer);
			
				/*
				const auto vm_call = functional_value->find_first_of<ir::expression::function>();
				if (vm_call.has_value()) {
					vm_call.value().get().body->accept(&optimizer);
				}*/
			}
		}
	}
}