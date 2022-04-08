#include "statement.hpp"
#include <sstream>
#include <algorithm>
#include <iterator>
#include "expression.hpp"

#include "abstract_visitor_pattern.hpp"

#include <iostream>

namespace deobf::ast::ir::statement {

	// return statement

	std::string return_statement::to_string() const {
		std::ostringstream stream;

		stream << "return ";
		
		if (!body.empty()) {
			if (body.size() > 1)
				std::transform(body.cbegin(), body.cend() - 1, std::ostream_iterator<std::string>(stream, ","), [&stream](const auto& value) {
					return value->to_string();
				});

			stream << body.back()->to_string();
		}

		return stream.str();
	}

	bool return_statement::equals(const node* other_node) const {
		return false;
	}

	std::vector<std::shared_ptr<node>> return_statement::get_children() const {
		auto children_vector = std::vector<std::shared_ptr<node>>{ };

		for (auto& name_and_arg : body)
			children_vector.emplace_back(name_and_arg);

		return children_vector;
	}

	void return_statement::accept(abstract_visitor_pattern* visitor) {
		if (visitor->accept(this))
			for (auto& statement : body)
				statement->accept(visitor);
	}
	// block

	bool block::equals(const node* other_node) const {
		return false;
	}

	std::vector<std::shared_ptr<node>> block::get_children() const {
		auto children_vector = std::vector<std::shared_ptr<node>>{ }; // (body.begin(), body.end());
		
		for (auto& statement : body)
			children_vector.push_back(statement);

		if (ret.has_value())
			children_vector.push_back(ret.value());

		return children_vector;
	}
	
	void block::accept(abstract_visitor_pattern* visitor) {
		if (visitor->accept(this)) {
			for (auto& stat : body) {
				stat->accept(visitor);
			}

			if (ret.has_value()) {
				ret.value()->accept(visitor);
			}
		}
	}
	// do block

	bool do_block::equals(const node* other_node) const {
		return false;
	}

	std::vector<std::shared_ptr<node>> do_block::get_children() const {
		return { body };
	}

	void do_block::accept(abstract_visitor_pattern* visitor) {
		if (visitor->accept(this))
			body->accept(visitor);
	}

	// while statement

	std::string while_statement::to_string() const {
		return "while " + condition->to_string() + " do";
	}

	bool while_statement::equals(const node* other_node) const {
		return false;
	}

	std::vector<std::shared_ptr<node>> while_statement::get_children() const {
		return { body, condition };
	}

	void while_statement::accept(abstract_visitor_pattern* visitor) {
		if (visitor->accept(this)) {
			condition->accept(visitor);
			body->accept(visitor);
		}
	}

	// local declare

	std::string local_declaration::to_string() const {
		std::ostringstream stream;

		stream << "local ";

		if (!names.empty()) {
			std::transform(names.cbegin(), names.cend() - 1, std::ostream_iterator<std::string>(stream, ","), [&stream](const auto& value) {
				return value->to_string();
			});

			stream << names.back()->to_string();
		}

		
		if (!body.empty()) {
			stream << " = ";

			std::transform(body.cbegin(), body.cend() - 1, std::ostream_iterator<std::string>(stream, ","), [&stream](const auto& value) {
				return value->to_string();
			});

			stream << body.back()->to_string();
		}
		

		return stream.str();
	}

	bool local_declaration::equals(const node* other_node) const {
		return false;
	}

	std::vector<std::shared_ptr<node>> local_declaration::get_children() const {
		auto children_vector = std::vector<std::shared_ptr<node>>{ };

		std::copy(names.begin(), names.end(), std::back_inserter(children_vector));
		std::copy(body.begin(), body.end(), std::back_inserter(children_vector));

		return children_vector;
	}

	void local_declaration::accept(abstract_visitor_pattern* visitor) {
		if (visitor->accept(this)) {
			for (auto& name : names)
				name->accept(visitor);
			for (auto& expression : body)
				expression->accept(visitor);
		}
	}

	// variable assign

	std::string variable_assign::to_string() const {
		std::ostringstream stream;

		if (!variables.empty()) {
			std::transform(variables.cbegin(), variables.cend() - 1, std::ostream_iterator<std::string>(stream, ","), [&stream](const auto& value) {
				return value->to_string();
			});

			stream << variables.back()->to_string();
		}

		if (!expressions.empty()) {
			stream << " = ";

			std::transform(expressions.cbegin(), expressions.cend() - 1, std::ostream_iterator<std::string>(stream, ","), [&stream](const auto& value) {
				return value->to_string();
			});

			stream << expressions.back()->to_string();
		}


		return stream.str();
	}

	bool variable_assign::equals(const node* other_node) const {
		return false;
	}

	std::vector<std::shared_ptr<node>> variable_assign::get_children() const {
		auto children_vector = std::vector<std::shared_ptr<node>>{ };

		std::copy(variables.begin(), variables.end(), std::back_inserter(children_vector));
		std::copy(expressions.begin(), expressions.end(), std::back_inserter(children_vector));

		return children_vector;
	}

	void variable_assign::accept(abstract_visitor_pattern* visitor) {
		if (visitor->accept(this)) {
			for (auto& variable : variables)
				variable->accept(visitor);
			for (auto& expression : expressions)
				expression->accept(visitor);
		}
	}
	
	// for step

	std::string for_step::to_string() const {
		std::ostringstream stream;
		
		stream << "for " << name->value << " = " << init->to_string() << ", " << end->to_string();

		if (step.has_value())
			stream << ", " << step.value();

		return stream.str();
	}

	bool for_step::equals(const node* other_node) const {
		return false;
	}

	std::vector<std::shared_ptr<node>> for_step::get_children() const {
		auto children_vector = std::vector<std::shared_ptr<node>>{ name, init, end };

		if (step.has_value())
			children_vector.push_back(step.value());

		children_vector.push_back(body);

		return children_vector;
	}

	void for_step::accept(abstract_visitor_pattern* visitor) {
		if (visitor->accept(this)) {
			init->accept(visitor);
			end->accept(visitor);

			if (step.has_value())
				step.value()->accept(visitor);

			body->accept(visitor);
		}
	}

	// for in

	bool for_in::equals(const node* other_node) const {
		return false;
	}

	std::string for_in::to_string() const {
		std::ostringstream stream;
		
		stream << "for ";

		if (!names.empty()) {
			std::transform(names.cbegin(), names.cend() - 1, std::ostream_iterator<std::string>(stream, ","), [&stream](const auto& value) {
				return value->to_string();
			});

			stream << names.back()->to_string();
		}

		if (!expressions.empty()) {
			stream << " in ";

			std::transform(expressions.cbegin(), expressions.cend() - 1, std::ostream_iterator<std::string>(stream, ","), [&stream](const auto& value) {
				return value->to_string();
			});

			stream << expressions.back()->to_string();
		}

		return stream.str();
	}

	std::vector<std::shared_ptr<node>> for_in::get_children() const {
		auto children_vector = std::vector<std::shared_ptr<node>>{ };

		std::copy(names.begin(), names.end(), std::back_inserter(children_vector));
		std::copy(expressions.begin(), expressions.end(), std::back_inserter(children_vector));

		children_vector.push_back(body);

		return children_vector;
	}

	void for_in::accept(abstract_visitor_pattern* visitor) {
		if (visitor->accept(this)) {
			for (auto& name : names)
				name->accept(visitor);

			for (auto& expression : expressions)
				expression->accept(visitor);

			body->accept(visitor);
		}
	}
	// repeat

	bool repeat::equals(const node* other_node) const {
		return false;
	}

	std::vector<std::shared_ptr<node>> repeat::get_children() const {
		return { body, condition };
	}

	void repeat::accept(abstract_visitor_pattern* visitor) {
		if (visitor->accept(this)) {
			condition->accept(visitor);
			body->accept(visitor);
		}
	}

	// if statement

	bool if_statement::equals(const node* other_node) const {
		return false;
	}

	std::string if_statement::to_string() const {
		return "if " + condition->to_string() + " then";
	}

	std::vector<std::shared_ptr<node>> if_statement::get_children() const {
		return { condition, body };
	}

	void if_statement::accept(abstract_visitor_pattern* visitor) { // todo : modify this so if accepting the condition evalutes false the visitor will stop
		if (visitor->accept(this)) {
			condition->accept(visitor);
			body->accept(visitor);
			for (auto& [condition, body] : else_if_statements) {
				condition->accept(visitor);
				body->accept(visitor);
			}

			if (else_body.has_value()) {
				else_body.value()->accept(visitor);
			}
		}
	}


	// break statement
	
	void break_statement::accept(abstract_visitor_pattern* visitor) {
		visitor->accept(this);
	}

	// semicolon

	void semicolon::accept(abstract_visitor_pattern* visitor) {
		visitor->accept(this);
	}


	// symbol table

	bool symbol_info::is_known_symbol() const noexcept {
		/*if (std::holds_alternative<std::shared_ptr<expression::string_literal>>(symbol_value)) {
			return known_symbol_names.count(std::get<0>(symbol_value)->to_string());
		}*/

		return known_symbol_names.count(symbol_value->to_string()); // is global
	}
}