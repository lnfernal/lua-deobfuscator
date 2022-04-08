#pragma once

#include "node.hpp"
#include "expression.hpp"
#include "statement.hpp"

namespace deobf::ast::ir {
	struct abstract_visitor_pattern { // abstract visitor pattern interface
#define MAKE_BASE_VISITOR(child, parent) virtual bool accept(child* subject) { return accept(static_cast<parent*>(subject)); } // todo return optinal node instead of a boolean to accept another stats? (for optimizations)

		// basic node
		virtual bool accept(node*) { return true; };

		// node types
		MAKE_BASE_VISITOR(expression_t, node);
		MAKE_BASE_VISITOR(statement::statement, node);

		// categorial types.

		// expression types
		MAKE_BASE_VISITOR(expression::binary_expression, expression_t);
		MAKE_BASE_VISITOR(expression::boolean_literal, expression_t);
		MAKE_BASE_VISITOR(expression::function_call, expression_t);
		MAKE_BASE_VISITOR(expression::literal_base, expression_t);
		MAKE_BASE_VISITOR(expression::name_and_args, expression_t);
		MAKE_BASE_VISITOR(expression::nil_literal, expression_t);
		MAKE_BASE_VISITOR(expression::numeral_literal, expression_t);
		MAKE_BASE_VISITOR(expression::string_literal, expression_t);
		MAKE_BASE_VISITOR(expression::table, expression_t);
		MAKE_BASE_VISITOR(expression::unary_expression, expression_t);
		MAKE_BASE_VISITOR(expression::variable, expression_t);
		MAKE_BASE_VISITOR(expression::variable_suffix, expression_t);

		// colliding types (statement & expression)
		MAKE_BASE_VISITOR(expression::array_index_literal, expression_t);
		MAKE_BASE_VISITOR(expression::function, statement::statement);

		// statement types
		MAKE_BASE_VISITOR(statement::block, statement::statement);
		MAKE_BASE_VISITOR(statement::break_statement, statement::statement);
		MAKE_BASE_VISITOR(statement::do_block, statement::statement);
		MAKE_BASE_VISITOR(statement::for_in, statement::statement);
		MAKE_BASE_VISITOR(statement::for_step, statement::statement);
		MAKE_BASE_VISITOR(statement::if_statement, statement::statement);
		MAKE_BASE_VISITOR(statement::local_declaration, statement::statement);
		MAKE_BASE_VISITOR(statement::repeat, statement::statement);
		MAKE_BASE_VISITOR(statement::return_statement, statement::statement);
		MAKE_BASE_VISITOR(statement::semicolon, statement::statement);
		MAKE_BASE_VISITOR(statement::variable_assign, statement::statement);
		MAKE_BASE_VISITOR(statement::while_statement, statement::statement);

		virtual ~abstract_visitor_pattern() noexcept(true) = default;

#undef MAKE_BASE_VISITOR
	};
}