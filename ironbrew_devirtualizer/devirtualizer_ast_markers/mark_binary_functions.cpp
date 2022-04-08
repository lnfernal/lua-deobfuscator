#include "mark_binary_functions.hpp"
#include "marker_decorator_interface.hpp"

namespace deobf::ironbrew_devirtualizer::devirtualizer_ast_markers {
	using namespace ast;

	struct binary_marker_visitor final : marker_decorator_interface { // marks functions that are being used inside the deserializer
		using marker_decorator_interface::marker_decorator_interface;
		
		// FIND LIST
		// to find get_bits & get_float : c(b, 21, 31);
		// to find get_8_bits : a = (h() ~= 0);
		// to find get_16_bits : a[4] = f();
		// to find get_32_bits : for a = 1, b() do
		// to find get_string : return '';


		bool accept(ir::expression::function_call* expression) override {
			auto& arguments = expression->arguments.at(0)->body;
			if (arguments.size() == 3) {
				if (arguments.at(1)->to_string() == "21" && arguments.at(2)->to_string() == "31") {
					if (auto symbol = current_block->find_symbol(expression->name.value()->to_string())) {
						symbol->resolve_identifier = "get_bits";
						current_block->find_symbol(current_function->function_name.value())->resolve_identifier = "get_float";
					}
				}
				else if (expression->name.value()->to_string() == "string.byte" && arguments.at(1)->to_string() == arguments.at(2)->to_string()) { // this is another reason why equals method is just another overhead lul
					current_block->find_symbol(current_function->function_name.value())->resolve_identifier = "get_8_bits";
					
					// moved vm_xor_key to here
					if (auto function_call = current_function->body->find_first_of<ir::expression::function_call>(); function_call.has_value()) {
						const auto& call_reference = function_call->get();
						const auto& call_arguments = call_reference.arguments.at(0)->body;

						if (call_reference.name.has_value() && call_arguments.size() == 2) {
							if (auto name_symbol = current_function->body->find_symbol(call_reference.name.value()->to_string())) {
								if (auto platform_check = name_symbol->symbol_value->find_first_of<ir::expression::binary_expression>(); platform_check.has_value()) {
									if (platform_check->get().to_string() == "( bit and bit.bxor )") {
										if (const auto vm_xor_key = call_arguments.back()->as<ir::expression::numeral_literal>()) {
											current_block->insert_symbol<true>("vm_xor_key", vm_xor_key);
										}
									}
								}
							}
						}
					}
				}
			}
			return true;
		}

		bool accept(ir::statement::return_statement* statement) override {
			if (statement->body.size() == 1) {
				auto& result = statement->body.at(0)->find_first_of<ir::expression::numeral_literal>();
				if (result.has_value()) {
					const auto value = result.value().get().to_string();
					if (value == "256") {
						current_block->find_symbol(current_function->function_name.value())->resolve_identifier = "get_16_bits";
					}
					else if (value == "1.67772e+07") {
						current_block->find_symbol(current_function->function_name.value())->resolve_identifier = "get_32_bits";
					}
					return true;
				}
				else if (auto result = statement->body.at(0)->as<ir::expression::string_literal>()) {
					const auto value = result->to_string();
					if (value.empty()) {
						current_block->find_symbol(current_function->function_name.value())->resolve_identifier = "get_string";
					}
				}
			}
			else if (statement->body.size() == 2) {
				auto& result = statement->find_first_of<ir::expression::function_call>();
				if (result.has_value() && result.value().get().name.value()->to_string() == "select" && statement->body.at(0)->is<ir::expression::table>()) {
					current_block->find_symbol(current_function->function_name.value())->resolve_identifier = "pack_return";
					return false;
				}
			}

			return true;
		}

		bool accept(ir::expression::function* expression) override {
			current_function = expression;
			return true;
		}

	private:
		ir::expression::function* current_function;
		using operation_t = typename ir::expression::binary_expression::operation_t;
	};

	void mark_binary_functions::optimize() {
		binary_marker_visitor optimizer{ "get_bits", "get_8_bits", "get_16_bits", "get_32_bits", "get_float", "get_string", "pack_return" };
		root->accept(&optimizer);
	}
}