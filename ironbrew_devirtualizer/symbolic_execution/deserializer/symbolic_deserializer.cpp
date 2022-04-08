#pragma once

#include "./ast/ir/abstract_visitor_pattern.hpp"
#include "./ast/ir/node.hpp"
#include "symbolic_deserializer.hpp"
#include "deserializer_context.hpp"

#include <stack>


namespace deobf::ironbrew_devirtualizer::symbolic_execution::deserializer {
	using namespace ast;
		
	using namespace deserializer_enums;

	struct deserializer_visitor final : ir::abstract_visitor_pattern { // process chunks via symbolic execution
		explicit deserializer_visitor(symoblic_deserializer& const object) :
			object(object)
		{ }

		const constant_type determine_constant_index(const ir::statement::block* path) { 
			constant_type type{ };
			
			if (auto assign = path->find_first_of<ir::statement::variable_assign>(); assign.has_value()) {
				const auto& assign_stat = assign->get();
				if (assign_stat.variables.size() == 1 && assign_stat.expressions.size() == 1) {
					const auto expression_string = assign_stat.expressions.at(0)->to_string();
					if (expression_string == "( get_8_bits() ~= 0 )")
						type = constant_type::boolean;
					else if (expression_string == "get_float()")
						type = constant_type::number;
					else if (expression_string == "get_string()")
						type = constant_type::string;
				}
			}

			return type;
		}

		//inline void populate_constant_order(ir::statement::block*& statement) {

		// todo pair might be slower?
		inline void populate_constant_order(const std::pair<const ir::expression::binary_expression*, const ir::statement::block*>& condition_pair) {
			const auto result = determine_constant_index(condition_pair.second);
			if (auto numeral_right = condition_pair.first->right->as<ir::expression::numeral_literal>()) {
				object.deserializer_ctx->constant_order_mapping.emplace(static_cast<std::uint8_t>(numeral_right->value), result);
			}
		}

		inline void populate_constant_order(const ir::statement::if_statement* statement) {
			const auto stat_condition = statement->condition.get()->as<ir::expression::binary_expression>();
			if (stat_condition) {
				populate_constant_order({ stat_condition.get(), statement->body.get() });
			}

			for (auto& stat : statement->else_if_statements) {
				const auto stat_condition = stat.first->as<ir::expression::binary_expression>();
				if (stat_condition) {
					populate_constant_order({ stat_condition.get(), stat.second.get() });
				}
			}
		}

		bool accept(ir::statement::variable_assign* statement) override {
			if (statement->to_string() == "deserialize_return[3] = get_8_bits()") {
				object.deserializer_ctx->chunk_order.push_back(process_order::parameters);
			}

			return false;
		}

		bool accept(ir::statement::for_step* statement) override {
			auto end_string = statement->end->to_string();

			if (auto symbol = object.deserializer_ctx->root->find_symbol(end_string)) {
				if (symbol->symbol_value->to_string() == "get_32_bits()") { // only constants do this. mark
					object.deserializer_ctx->chunk_order.push_back(deserializer::process_order::constants);

					auto constant_check = statement->body->find_first_of<ir::statement::if_statement>();
					if (constant_check.has_value()) {
						const auto& if_statement = constant_check->get();
						if (if_statement.else_if_statements.size() == 2) {
							populate_constant_order(&if_statement);
						}
					}
				}
			}
			else if (end_string == "get_32_bits()") {
				if (statement->body->body.size() == 1) {
					auto assign = statement->body->find_first_of<ir::statement::variable_assign>();
					if (assign.has_value()) {
						const auto& assign_statement = assign->get();
						if (assign_statement.expressions.size() == 1) {
							const auto result_string = assign_statement.expressions.at(0)->to_string();
							if (result_string == "bytecode_deserializer()") { // protos step?
								object.deserializer_ctx->chunk_order.push_back(deserializer::process_order::protos);
							}
							else if (result_string == "get_32_bits()") {
								object.deserializer_ctx->chunk_order.push_back(deserializer::process_order::lineinfo);
							}
						}
					}
				}
				else {
					auto if_statement = statement->body->find_first_of<ir::statement::if_statement>();
					if (if_statement.has_value()) {
						if (auto binary_condition = if_statement->get().condition->as<ir::expression::binary_expression>()) {
							if (binary_condition->right->to_string() == "0") {
								if (auto bit_call = binary_condition->left->as<ir::expression::function_call>()) {
									if (bit_call->name.has_value() && bit_call->name.value()->to_string() == "get_bits" && bit_call->arguments.at(0)->body.size() == 3) {
										// todo check parameters (UNK, 1, 1)? useless prob
										object.deserializer_ctx->chunk_order.push_back(deserializer::process_order::instructions);
									}
								}
							}
						}
					}
				}
			}

			return false;
		}
	private:
		//std::unordered_map<std::string, std::size_t> concerte_values; // mapping for resolved concerte values
		//std::stack<std::size_t> execution_track; // keeps tracks of concerte values like assigns etc
		//std::unique_ptr<vm_arch::proto> current_chunk; // current write chunk
		symoblic_deserializer& const object; // holds a reference to deserializer object
	};

    void symoblic_deserializer::run() {
		deserializer_visitor optimizer(*this);

		deserializer_ctx->root->accept(&optimizer);

		if (deserializer_ctx->constant_order_mapping.size() != max_constant_order) {
			throw std::runtime_error("constant_order does not sum up to max_constant_order");
		}

		deserializer_ctx->chunk_order.shrink_to_fit();
	}
}