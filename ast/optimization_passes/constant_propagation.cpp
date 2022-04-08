#include "constant_propagation.hpp"
#include "ast/ir/abstract_visitor_pattern.hpp"
#include "ast/ir/node.hpp"

#include <type_traits>
#include <thread>
#include <mutex>


namespace deobf::ast::optimization_passes {
	struct constant_propagation_visitor final : ir::abstract_visitor_pattern {

		std::optional<std::shared_ptr<ir::expression::expression>> optimize(ir::expression::variable* expression) { // constant propagation
			if (current_visitor_block == nullptr)
				return std::nullopt;

			if (const auto symbol = current_visitor_block->find_symbol(expression->to_string())) {
				if (symbol->is_known_symbol()) {
					return symbol->symbol_value->as<ir::expression_t>();
				}
				else if (symbol->symbol_value->to_string() == "( unpack or table.unpack )") { // some addition ig
					return std::make_shared<ir::expression::string_literal>("unpack");
				}
			}

			return std::nullopt;
		}

		bool accept(ir::expression::function_call* expression) override {
			if (expression->name.has_value()) {
				auto& name_value = expression->name.value();
				if (auto variable_node = dynamic_cast<ir::expression::variable*>(name_value.get())) {
					if (auto optimized_node = optimize(variable_node); optimized_node.has_value()) {
						expression->name = std::move(optimized_node.value());
					}
				}
			}

			return true;
		}

		bool accept(ir::statement::block* expression) override {
			const auto temp_block = current_visitor_block;
			current_visitor_block = expression; // could also pass as argument?

			for (auto& statement : expression->body) { // todo a function to optimize function calls, this is a literal mess
				statement->accept(this);
			}

			if (expression->ret.has_value()) {
				const auto ret_value = expression->ret.value();

				// first ret got some shitty call, populate the symbol table with the pushed parameters (this is a part of the aztupbrew deobfuscator)
				/*std::call_once(has_visited_return, [this, &ret_value]() {	
					auto& function_call = ret_value->find_first_of<ir::expression::function_call>().value().get();

					if (auto function_expression = function_call.name.value()->as<ir::expression::function>()) {
						if (function_call.arguments.at(0)->body.size() == function_expression->parameters.size()) {
							for (auto i = 0ul; i < function_call.arguments.size(); ++i) { // zip iterate
								const auto argument_value = function_call.arguments.at(0)->body.at(i)->to_string();
								const auto parameter_value = function_expression->parameters.at(i)->to_string();
								std::cout << "ins:::" << argument_value << std::endl;
								function_expression->body->insert_symbol(parameter_value, std::make_shared<ir::expression::string_literal>(argument_value));
							}
						}
					}
				});*/

				ret_value->accept(this);
			}

			current_visitor_block = temp_block;

			return false;
		}

	private:
		const ir::statement::block* current_visitor_block = nullptr;
		//std::once_flag has_visited_return;
	};

	void constant_propagation::optimize() {
		constant_propagation_visitor base;
		root->accept(&base);

	}
}