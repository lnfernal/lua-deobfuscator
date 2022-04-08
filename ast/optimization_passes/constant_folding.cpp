// rewrite this entire bs sadpepe

#include "constant_folding.hpp"
#include "ast/ir/abstract_visitor_pattern.hpp"
#include "ast/ir/node.hpp"

#include <iostream>
#include <type_traits>
#include <algorithm>

namespace deobf::ast::optimization_passes {
	struct folding_visitor final : ir::abstract_visitor_pattern {
		std::optional<std::shared_ptr<ir::expression::expression>> optimize(ir::expression::unary_expression* expression) { // will cause a shared_ptr duplicate?
			using operation_t = typename ir::expression::unary_expression::operation_t;

			// todo optimization visitor or any pattern so we dont have to make those kind of shit?
			// making a template for those stuff is useless and fucks up with ADL

			switch (expression->operation) {
				case operation_t::minus: {
					if (auto body_literal = expression->body->as<ir::expression::numeral_literal>())
						return std::make_shared<ir::expression::numeral_literal>(-body_literal->value);
				}
				case operation_t::len: {
					if (auto table = expression->body->as<ir::expression::table>()) {
						const auto has_initial_rehashing = std::any_of(table->entries.cbegin(), table->entries.cend(), [](const auto& pair) {
							return pair.first->is<ir::expression::array_index_literal>() && pair.second->is<ir::expression::nil_literal>();
						});

						// todo nil sequences

						if (!has_initial_rehashing) {
							return std::make_shared<ir::expression::numeral_literal>(static_cast<double>(table->array_size));
						}
					}
					else if (auto string_literal = std::dynamic_pointer_cast<ir::expression::string_literal>(expression->body)) {
						return std::make_shared<ir::expression::numeral_literal>(string_literal->value.length());
					}
					break;
				}
				case operation_t::not : {
					if (expression->body->is<ir::expression::nil_literal>()) {
						return std::make_shared<ir::expression::boolean_literal>(false);
					}
					else if (auto bool_literal = expression->body->as<ir::expression::boolean_literal>()) {
						return std::make_shared<ir::expression::boolean_literal>(!bool_literal->value);
					}
				}
			}

			return std::nullopt;
		}

		std::optional<std::shared_ptr<ir::expression::expression>> optimize(ir::expression::binary_expression* expression)  { // folds number expressions. (todo concat in future?)
			auto left = expression->left->as<ir::expression::numeral_literal>();
			auto right = expression->right->as<ir::expression::numeral_literal>();
			
			if (!(left && right)) {
				return std::nullopt;
			}

			using operation_t = typename ir::expression::binary_expression::operation_t;
			switch (expression->operation) {
				case operation_t::none:

				// basic artimethic, [+-*/^%]
				case operation_t::add : {
					return std::make_shared<ir::expression::numeral_literal>(left->value + right->value);
				}
				case operation_t::sub: {
					return std::make_shared<ir::expression::numeral_literal>(left->value - right->value);
				}
				case operation_t::div: {
					if (right->value == 0)
						return std::nullopt;

					return std::make_shared<ir::expression::numeral_literal>(left->value / right->value);
				}
				case operation_t::mul: {
					return std::make_shared<ir::expression::numeral_literal>(left->value * right->value);
				}
				case operation_t::pow: {
					return std::make_shared<ir::expression::numeral_literal>(std::pow(left->value, right->value));
				}
				case operation_t::mod: {
					if (right->value == 0)
						return std::nullopt;

					return std::make_shared<ir::expression::numeral_literal>(std::fmod(left->value, right->value));
				}
				
				// boolean expression (epsilon in lua is different?)
				case operation_t::gt: {
					return std::make_shared<ir::expression::boolean_literal>(left->value > right->value);
				}
				case operation_t::lt: {
					return std::make_shared<ir::expression::boolean_literal>(left->value < right->value);
				}
				case operation_t::ge: {
					return std::make_shared<ir::expression::boolean_literal>(left->value >= right->value);
				}
				case operation_t::le: {
					return std::make_shared<ir::expression::boolean_literal>(left->value <= right->value);
				}
				case operation_t::eq: {
					return std::make_shared<ir::expression::boolean_literal>(left->value == right->value);
				}
				case operation_t::neq: {
					return std::make_shared<ir::expression::boolean_literal>(left->value != right->value);
				}

				// logical (short-circurit evals)
				case operation_t::and: {
					return right;
				}
				case operation_t::or: {
					return left;
				}

				/*default:
					throw std::logic_error("encountered an unimpelemented type of binary_expression? (binary_expression optimizer).");*/
			}

			return std::nullopt;
		}

		bool accept(ir::expression::table* expression) override { // optimize a table with unary expressions as indices
			for (auto& [expression, _] : expression->entries) { // todo a std::views::keys alternative? lazy to use cpp20
				if (auto unary = dynamic_cast<ir::expression::unary_expression*>(expression.get())) {
					unary->body->accept(this);

					if (auto optimization_result = optimize(unary); optimization_result.has_value()) {
						expression = std::move(optimization_result.value());
					}
				}
			}

			return true;
		}

		bool accept(ir::expression::unary_expression* expression) override {
			if (auto unary = dynamic_cast<ir::expression::unary_expression*>(expression->body.get())) {
				if (auto optimization_result = optimize(unary); optimization_result.has_value()) {
					expression->body = std::move(optimization_result.value());
				}
			}

			return true;
		}


		bool accept(ir::statement::local_declaration* declare) override {
			for (auto& statement : declare->body) {
				if (auto binary = dynamic_cast<ir::expression::binary_expression*>(statement.get())) {
					binary->accept(this);
					
					// optimiez final expression, attach into statement (todo a better approach?)
					if (auto optimized_result = optimize(binary); optimized_result.has_value())
						statement = std::move(optimized_result.value());
					std::cout << declare->to_string() << std::endl;
				}
			}

			return true;
		}

		bool accept(ir::expression::binary_expression* expression) override {
			return fold_basic_expression<ir::expression::binary_expression, ir::expression::unary_expression>(expression);
		}

		private:
			// the following code is garbage and needs to be changed soon (works good but can be improved more)
			template <class... ast_types>
			using ast_node_variance = std::variant<std::monostate, std::shared_ptr<ast_types>...>;

			template <typename ast_type, class fold_nodes_t>
			constexpr bool check_variance_node(fold_nodes_t&& to_visit, std::shared_ptr<ir::expression::expression> expression) noexcept {
				const auto result = expression->is<ast_type>();

				if (result)
					to_visit = std::static_pointer_cast<ast_type>(expression);

				return result;
			}

			template <typename... ast_types>
			constexpr bool fold_basic_expression(ir::expression::binary_expression* expression) {
				static_assert((... && std::is_base_of_v<typename ir::expression::expression, ast_types>), "[optimization_passes/constant_folding/fold_basic_expression]: invalid varardic specialization for ast_types");

				ast_node_variance<ast_types...> left, right;

				if (!(check_variance_node<ast_types>(left, expression->left) || ...))
					return false;

				if (!(check_variance_node<ast_types>(right, expression->right) || ...))
					return false;

				std::visit([this, &expression](auto&& left_expression, auto&& right_expression) {
					using left_expression_t = std::decay_t<decltype(left_expression)>;
					using right_expression_t = std::decay_t<decltype(right_expression)>;

					if constexpr (std::integral_constant<bool, (std::is_same_v<left_expression_t, std::shared_ptr<ast_types>> || ...)>{ }) {
						left_expression->accept(this);
						if (auto optimization_result = optimize(left_expression.get());  optimization_result.has_value())
							expression->left = optimization_result.value();
					}

					if constexpr (std::integral_constant<bool, (std::is_same_v<right_expression_t, std::shared_ptr<ast_types>> || ...)>{ }) {
						right_expression->accept(this);

						if (auto optimization_result = optimize(right_expression.get()); optimization_result.has_value())
							expression->right = optimization_result.value();
					}
				}, left, right);

				return true;
			}
	};

	void constant_folding::optimize() {
		folding_visitor base;
		root->accept(&base);
	}
}