#include "expression.hpp"
#include "abstract_visitor_pattern.hpp"

#include <sstream>
#include <algorithm>
#include <iterator>

// we dont have to deal with multirets here, hence why we won't use std::transform.
namespace deobf::ast::ir::expression {
	const std::unordered_map<binary_expression::operation_t, std::string>
		binary_expression::operation_map {
			// logical
			{ operation_t::and, "and" },
			{ operation_t::or, "or" },

			// basic math
			{ operation_t::add, "+" },
			{ operation_t::sub, "-" },
			{ operation_t::mul, "*" },
			{ operation_t::mod, "%" },
			{ operation_t::div, "/" },
			{ operation_t::pow, "^" },

			// string manip
			{ operation_t::concat, ".." },

			// comparsion
			{ operation_t::le, "<=" },
			{ operation_t::lt, "<" },
			{ operation_t::gt, ">" },
			{ operation_t::ge, ">=" },
			{ operation_t::eq, "==" },
			{ operation_t::neq, "~=" },
	};

	const std::unordered_map<unary_expression::operation_t, std::string>
		unary_expression::operation_map{
			{ operation_t::len, "#" },
			{ operation_t::minus, "-" },
			{ operation_t::not, "not" },
	};

	// basic literal types

	bool numeral_literal::equals(const node* other_node) const {
		return false;
	}

	void numeral_literal::accept(abstract_visitor_pattern* visitor) {
		visitor->accept(this);
	}

	bool boolean_literal::equals(const node* other_node) const {
		return false;
	}

	void boolean_literal::accept(abstract_visitor_pattern* visitor) {
		visitor->accept(this);
	}

	bool string_literal::equals(const node* other_node) const {
		return false;
	}

	void string_literal::accept(abstract_visitor_pattern* visitor) {
		visitor->accept(this);
	}

	void nil_literal::accept(abstract_visitor_pattern* visitor) {
		visitor->accept(this);
	}

	void literal_base::accept(abstract_visitor_pattern* visitor) { // removing would boost speed in optimizations?
		visitor->accept(this);
	}

	void array_index_literal::accept(abstract_visitor_pattern* visitor) { // removing would boost speed in optimizations?
		visitor->accept(this);
	}
	// binary expression

	std::string binary_expression::to_string() const {
		std::ostringstream stream;

		stream << "( ";
		
		if (const auto result = operation_map.find(operation); result != operation_map.cend())
			stream << left->to_string() << " " << result->second << " " << right->to_string();
		
		stream << " )";

		return stream.str();
	}

	std::vector<std::shared_ptr<node>> binary_expression::get_children() const {
		return { left, right };
	}

	void binary_expression::accept(abstract_visitor_pattern* visitor) {
		if (visitor->accept(this)) { // does it accept the expression?, if so tell expressions to accept the visitor
			left->accept(visitor);
			right->accept(visitor);
		}
	}
	// unary expression

	std::string unary_expression::to_string() const {
		if (const auto result = operation_map.find(operation); result != operation_map.cend())
			return result->second + "( " + body->to_string() + " )";

		return "( )";
	}

	bool unary_expression::equals(const node* other_node) const {
		return false;
	}

	std::vector<std::shared_ptr<node>> unary_expression::get_children() const {
		return { body };
	}

	void unary_expression::accept(abstract_visitor_pattern* visitor) {
		if (visitor->accept(this))
			body->accept(visitor);
	}

	// table

	std::string table::to_string() const {
		std::ostringstream stream;
		stream << "{ ";

		for (auto& [key, value] : entries) {
			stream << '[' << key->to_string() << "] = " << value->to_string() << ", ";
		}
		stream << "}";

	
		return stream.str();
	}

	bool table::equals(const node* other_node) const {
		return false;
	}

	std::vector<std::shared_ptr<node>> table::get_children() const
	{
		auto children_vector = std::vector<std::shared_ptr<node>>{ };

		for (auto& [key, value] : entries) {
			children_vector.push_back(key);
			children_vector.push_back(value);
		}

		return children_vector;
	}

	void table::accept(abstract_visitor_pattern* visitor) {
		if (visitor->accept(this)) {
			for (auto& [key, value] : entries) {
				key->accept(visitor);
				value->accept(visitor);
			}
		}
	}

	// nameAndArg? expression

	std::string name_and_args::to_string() const
	{
		std::ostringstream stream;

		if (!body.empty()) {
			if (body.size() > 1)
				std::transform(body.cbegin(), body.cend() - 1, std::ostream_iterator<std::string>(stream, ","), [&stream](const auto& value) {
					return value->to_string();
				});

			stream << body.back()->to_string();
		}

		return name + stream.str();
	}

	bool name_and_args::equals(const node* other_node) const {
		return false;
	}

	void name_and_args::accept(abstract_visitor_pattern* visitor) {
		if (visitor->accept(this)) {
			for (auto& argument : body)
				argument->accept(visitor);
		}
	}

	std::vector<std::shared_ptr<node>> name_and_args::get_children() const {
		return { };
	}

	// variable suffix expression

	std::string variable_suffix::to_string() const {
		std::ostringstream stream;

		/*for (auto& name_and_arg : name_and_args)
			stream << name_and_arg->to_string();
			*/
		if (name->is<ir::expression::string_literal>())
			stream << '.' << name->to_string();
		else
			stream << '[' << name->to_string() << ']';

		return stream.str();
	}

	bool variable_suffix::equals(const node* other_node) const {
		return false;
	}

	std::vector<std::shared_ptr<node>> variable_suffix::get_children() const {
		auto children_vector = std::vector<std::shared_ptr<node>>(name_and_args.begin(), name_and_args.end());

		children_vector.push_back(name);

		return children_vector;
	}

	void variable_suffix::accept(abstract_visitor_pattern* visitor) {
		if (visitor->accept(this)) {
			name->accept(visitor); // yes its either an expression
			for (const auto& argument : name_and_args)
				argument->accept(visitor);
		}
	}

	// variable expression

	std::string variable::to_string() const {
		std::ostringstream stream;

		if (name->is<ir::expression::string_literal>())
			stream << static_cast<ir::expression::string_literal*>(name.get())->value;
		else
			stream << '(' << name->to_string() << ')';

		for (auto& suffix : suffixes) // wouldve used stream iterator but we would need a custom type
			stream << suffix->to_string();

		return stream.str();
	}

	bool variable::equals(const node* other_node) const {
		return false;
	}

	std::vector<std::shared_ptr<node>> variable::get_children() const {
		auto children_vector = std::vector<std::shared_ptr<node>>{ };

		children_vector.emplace_back(name);
		
		std::copy(suffixes.begin(), suffixes.end(), std::back_inserter(children_vector));

		return children_vector;
	}

	void variable::accept(abstract_visitor_pattern* visitor) {
		if (visitor->accept(this)) {
			name->accept(visitor);
			for (const auto& suffix : suffixes)
				suffix->accept(visitor);
		}
	}
	// function call expression

	std::string function_call::to_string() const {
		std::ostringstream stream;

		if (name.has_value())
			stream << name.value()->to_string();

		stream << '(';

		for (auto& argument : arguments) {
			stream << argument->to_string();
		}
		stream << ')';
		
		return stream.str();
	}
	
	bool function_call::equals(const node* other_node) const {
		return false;
	}

	std::vector<std::shared_ptr<node>> function_call::get_children() const {
		auto children_vector = std::vector<std::shared_ptr<node>>{ };
		
		if (name.has_value())
			children_vector.push_back(name.value());

		std::copy(arguments.begin(), arguments.end(), std::back_inserter(children_vector));

		return children_vector;
	}

	void function_call::accept(abstract_visitor_pattern* visitor) {
		if (visitor->accept(this)) {
			
			if (name.has_value())
				name.value()->accept(visitor);

			for (auto& argument : arguments)
				argument->accept(visitor);
		}
	}

	// function

	std::string function::to_string() const {
		std::ostringstream stream;

		stream << (type == function_type::local_t ? "local function" : "function");

		if (function_name.has_value())
			stream << " " << function_name.value();

		stream << "(";

		if (!parameters.empty()) {
			if (parameters.size() > 1)
				std::transform(parameters.cbegin(), parameters.cend() - 1, std::ostream_iterator<std::string>(stream, ","), [&stream](const auto& value) {
					return value->to_string();
				});

			stream << parameters.back()->to_string();
		}

		stream << ")";

		return stream.str();
	}

	bool function::equals(const node* other_node) const {
		return false;
	}

	std::vector<std::shared_ptr<node>> function::get_children() const {
		return { body };
	}

	void function::accept(abstract_visitor_pattern* visitor) {
		if (visitor->accept(this)) {
			body->accept(visitor);
			for (auto& parameter : parameters)
				parameter->accept(visitor);
		}
	}
}