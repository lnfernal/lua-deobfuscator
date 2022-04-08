#include "visitor_include.hpp"

#include <algorithm>

namespace deobf::ast {
	using namespace ir;

	antlrcpp::Any cst_visitor::visitTableconstructor(LuaParser::TableconstructorContext* ctx) {
		std::vector<std::pair<std::shared_ptr<expression_t>, std::shared_ptr<expression_t>>> entries;
		auto array_part = static_cast<std::size_t>(0u); // MAX_INT is max for array size in lua

		if (auto field_list = ctx->fieldlist()) {
			for (auto field : field_list->field()) {
				// (field->LSB() exp field->RSB()) OR (ast_string EQL exp) OR exp
				if (field->LSB() != nullptr) {
					auto key = visitExp(field->exp().front()).as<std::shared_ptr<expression_t>>();
					auto value = visitExp(field->exp().back()).as<std::shared_ptr<expression_t>>();

					entries.emplace_back(std::move(key), std::move(value)); // try_emplace for collisions
				}
				else if (auto field_name = field->NAME()) {
					auto key = std::make_shared<expression::string_literal>(field_name->getText());
					auto value = visitExp(field->exp().back()).as<std::shared_ptr<expression_t>>();

					entries.emplace_back(std::move(key), std::move(value));
				}
				else {
					auto array_key = std::make_shared<expression::array_index_literal>(++array_part);
					auto array_value = visitExp(field->exp().back()).as<std::shared_ptr<expression_t>>();

					entries.emplace_back(std::move(array_key), std::move(array_value));
				}
			}
		}

		auto table_node = std::make_shared<expression::table>(std::move(entries), array_part);

		return std::static_pointer_cast<expression_t>(table_node);
	}

	antlrcpp::Any cst_visitor::visitExplist(LuaParser::ExplistContext* ctx) {
		expression::expression_list_t expression_list;

		for (auto expression : ctx->exp()) {
			auto expression_node = visitExp(expression).as<std::shared_ptr<expression_t>>();
			expression_list.push_back(std::move(expression_node));
		}
		
		return expression_list;
	}

	antlrcpp::Any cst_visitor::visitArgs(LuaParser::ArgsContext* ctx) {
		// LP explist? RP | tableconstructor | string
		
		/* if (ctx->LP()) */

		auto arguments = expression::expression_list_t{ };

		if (auto expression_list = ctx->explist()) {
			arguments = visitExplist(ctx->explist()).as<expression::expression_list_t>();
		}
		else if (auto string_argument = ctx->string()) {
			auto string_literal = std::make_shared<expression::string_literal>(string_argument->getText());
			arguments.push_back(std::move(string_literal));
		}
		else if (auto table_constructor = ctx->tableconstructor()) {
			auto table_argument = visitTableconstructor(table_constructor).as<std::shared_ptr<expression_t>>();
			arguments.push_back(std::move(table_argument));
		}

		return arguments;
	}

	antlrcpp::Any cst_visitor::visitNameAndArgs(LuaParser::NameAndArgsContext* ctx) {
		std::string name;
		if (auto raw_name = ctx->NAME())
			name = raw_name->getText();

		auto arguments = visitArgs(ctx->args()).as<expression::expression_list_t>(); //std::variant<expression::expression_list_t, std::shared_ptr<expression_t>>

		auto name_and_args_node = std::make_shared<expression::name_and_args>(name, std::move(arguments));

		return name_and_args_node;
	}
	
	antlrcpp::Any cst_visitor::visitString(LuaParser::StringContext* ctx) {
		const auto string_value = ctx->getText();
		
		std::string init_value; 
		switch (string_value.front()) {
			case '[': {
				const auto start_delimeter = string_value.find('[', 1) + 1;
				const auto end_delimeter = string_value.find_last_of(']', string_value.size() - 2);
				init_value = string_value.substr(start_delimeter, end_delimeter - start_delimeter);
				break;
			}
			case '\'': // unused fallthough fornow
				init_value = string_value.substr(1, string_value.size() - 2);
				break;
			case '"':
				init_value = string_value.substr(1, string_value.size() - 2);
				break;
			default:
				throw std::logic_error("string_literal error");
		}

		auto string_literal = std::make_shared<expression::string_literal>(init_value);

		return std::static_pointer_cast<expression_t>(string_literal);
	}

	antlrcpp::Any cst_visitor::visitFunctioncall(LuaParser::FunctioncallContext* ctx) {
		expression::name_and_args_t arguments;
		for (const auto name : ctx->nameAndArgs()) {
			auto result = visitNameAndArgs(name).as<std::shared_ptr<expression::name_and_args>>();
			arguments.push_back(std::move(result));
		}

		std::shared_ptr<expression_t> expression_name = nullptr;

		auto var_or_exp = ctx->varOrExp();
		if (auto prefix_expression = var_or_exp->exp()) {
			expression_name = visitExp(prefix_expression).as<std::shared_ptr<expression_t>>();
		}
		else if (auto prefix_variable = var_or_exp->var()) {
			auto name = visitVar(prefix_variable).as<std::shared_ptr<expression_t>>();
			expression_name = std::move(name);
		}

		auto call_expression = std::make_shared<expression::function_call>(std::move(expression_name), std::move(arguments));

		return std::static_pointer_cast<statement::statement>(call_expression);
	}

	antlrcpp::Any cst_visitor::visitVarDecl(LuaParser::VarDeclContext* ctx) {
		auto variables = visitVarlist(ctx->varlist()).as<ir::expression::variable_list_t>();

		ir::expression::expression_list_t expressions;
		if (auto expression_list = ctx->explist())
			expressions = visitExplist(expression_list).as<ir::expression::expression_list_t>();

		auto variable_node = std::make_shared<ir::statement::variable_assign>(std::move(variables), std::move(expressions)); // statement

		return std::static_pointer_cast<statement::statement>(variable_node);
	}
	
	antlrcpp::Any cst_visitor::visitNamelist(LuaParser::NamelistContext* ctx) {
		expression::name_list_t name_list;

		for (auto name : ctx->NAME()) {
			auto name_node = std::make_shared<expression::string_literal>(name->getText());
			name_list.push_back(std::move(name_node));
		}

		return name_list;
	}

	antlrcpp::Any cst_visitor::visitFuncbody(LuaParser::FuncbodyContext* ctx) {

		ir::expression::name_list_t parameter_list;
		
		auto function_body = enter_scope(ctx->block());

		if (auto ctx_parameters = ctx->parlist()) {
			if (auto names = ctx_parameters->namelist()) {
				for (const auto name : names->NAME()) {
					//auto parameter = std::make_shared<expression::string_literal>(name->getText());
					
					std::string parameter_name = "p" + std::to_string(++parameter_counter);
					auto parameter_value = std::make_shared<ir::expression::string_literal>(parameter_name);


					auto symbol = function_body->insert_symbol<false>(parameter_name, parameter_value);

					auto new_weak_symbol = function_body->insert_symbol(name->getText());
					new_weak_symbol.first->second->resolve_identifier = parameter_name;
					
					parameter_list.push_back(parameter_value);
				}
			}
			else { // varargs might be handled after parameters aswell?
				auto variable_arguments = std::make_shared<expression::string_literal>("...");
				parameter_list.push_back(std::move(variable_arguments));
			}
		}

		//auto function_body = visitBlock(ctx->block()).as<std::shared_ptr<statement::block>>(); (removed because we need to insert symbols before visiting)

		const auto block = ctx->block();


		exit_scope(block);
		/*{
			const auto temp_old_block = current_parse_block;
			current_parse_block = function_body.get();

			ir::statement::managed_statement_list statements_body;
			for (const auto statement : block->stat()) {
				auto statement_node = visitStat(statement).as<std::shared_ptr<ir::statement::statement>>();
				statements_body.push_back(std::move(statement_node));
			}

			function_body->body = std::move(statements_body);

			if (auto return_statement = block->retstat()) {
				auto return_node = visitRetstat(return_statement).as<std::shared_ptr<ir::statement::return_statement>>();
				function_body->ret = std::move(return_node);
			}

			current_parse_block = temp_old_block;
		}*/
		
		auto function_node = std::make_shared<expression::function>(expression::function::function_type::anonymous_t,
			std::move(parameter_list),
			std::move(function_body)); // derives from expression, in statements for a reason.
		
		return function_node;
	}

	antlrcpp::Any cst_visitor::visitFunctiondef(LuaParser::FunctiondefContext* ctx) {
		// local x = function() end
		auto function_node = visitFuncbody(ctx->funcbody()).as<std::shared_ptr<ir::expression::function>>();
		//function_node->type = expression::function::function_type::anonymous_t;

		return std::static_pointer_cast<expression_t>(function_node);
	}

	antlrcpp::Any cst_visitor::visitVarlist(LuaParser::VarlistContext* ctx) {
		expression::variable_list_t variable_list;

		for (auto variable : ctx->var()) {
			auto variable_node = visitVar(variable).as<std::shared_ptr<expression_t>>();
			variable_list.push_back(std::move(variable_node));
		}

		return variable_list;
	}

	antlrcpp::Any cst_visitor::visitVarSuffix(LuaParser::VarSuffixContext* ctx) {
		std::shared_ptr<expression_t> expression_name = nullptr;
		if (auto prefix_expression = ctx->exp())
			expression_name = visitExp(prefix_expression).as<std::shared_ptr<expression_t>>();
		else {
			auto string_literal = std::make_shared<expression::string_literal>(ctx->NAME()->getText());
			expression_name = std::static_pointer_cast<expression_t>(string_literal);
		}

		expression::name_and_args_t arguments;
		for (auto argument : ctx->nameAndArgs()) {
			auto result = visitNameAndArgs(argument).as<std::shared_ptr<expression::name_and_args>>();
			arguments.push_back(std::move(result));
		}

		auto variable_suffix = std::make_shared<expression::variable_suffix>(std::move(expression_name), std::move(arguments));

		return variable_suffix;
	}

	antlrcpp::Any cst_visitor::visitVar(LuaParser::VarContext* ctx) {
		std::shared_ptr<expression::expression> variable_name = nullptr;
		if (auto name = ctx->NAME()) {
			variable_name = std::make_shared<expression::string_literal>(name->getText());
		}
		else { // expression
			variable_name = visitExp(ctx->exp()).as<std::shared_ptr<expression_t>>();
		}

		if (auto symbol = current_parse_block->find_symbol(variable_name->to_string())) { // best way to do that instead of making a rename visitor? ( O(n * k) time complexity btw )
		
			variable_name = std::make_shared<expression::string_literal>(symbol->resolve_identifier);
		}


		std::vector<std::shared_ptr<expression::variable_suffix>> suffix_list;
		for (auto suffix : ctx->varSuffix()) {
			auto suffix_expresion = visitVarSuffix(suffix).as<std::shared_ptr<expression::variable_suffix>>();
			suffix_list.push_back(std::move(suffix_expresion));
		}

		const auto symbol_name = variable_name->to_string();

		auto variable_node = std::make_shared<expression::variable>(std::move(variable_name), std::move(suffix_list));
		
		return std::static_pointer_cast<expression_t>(variable_node);
	}

	antlrcpp::Any cst_visitor::visitPrefixexp(LuaParser::PrefixexpContext* ctx) {
		expression::name_and_args_t call_parameters;
		for (auto parameter : ctx->nameAndArgs()) {
			auto parameter_value = visitNameAndArgs(parameter).as<std::shared_ptr<expression::name_and_args>>();
			call_parameters.push_back(std::move(parameter_value));
		}

		std::shared_ptr<expression_t> expression_name = nullptr;

		auto var_or_exp = ctx->varOrExp();
		if (auto prefix_expression = var_or_exp->exp())
			expression_name = visitExp(prefix_expression).as<std::shared_ptr<expression_t>>();
		else if (auto prefix_variable = var_or_exp->var())
			expression_name = visitVar(prefix_variable).as<std::shared_ptr<expression_t>>();

		if (call_parameters.empty())
			return expression_name;

		auto call_expression = std::make_shared<expression::function_call>(std::move(expression_name), std::move(call_parameters));

		return std::static_pointer_cast<expression_t>(call_expression);
	}

	antlrcpp::Any cst_visitor::visitExp(LuaParser::ExpContext* ctx) {
		if (auto link_operator = ctx->linkOperator()) {
			using binary_expression_t = typename expression::binary_expression;

			// remember left > right
			auto binary_lhs = visitExp(ctx->exp().front()).as<std::shared_ptr<expression_t>>();
			auto binary_rhs = visitExp(ctx->exp().back()).as<std::shared_ptr<expression_t>>();

			binary_expression_t::operation_t operation;
			
			const auto operation_text = link_operator->getText();

			// make bidict?
			if (const auto result = std::find_if(binary_expression_t::operation_map.cbegin(),
				binary_expression_t::operation_map.cend(),
				[operation_text](const auto& entry) {
					return entry.second == operation_text;
				}); result != binary_expression_t::operation_map.cend()) {
				operation = result->first;
			}

			auto binary_expression = std::make_shared<binary_expression_t>(operation, std::move(binary_lhs), std::move(binary_rhs));

			return std::static_pointer_cast<expression_t>(binary_expression);
		}
		else if (ctx->TRUE()) {
			auto boolean_literal = std::make_shared<expression::boolean_literal>(true);
			return std::static_pointer_cast<expression_t>(boolean_literal);
		}
		else if (ctx->FALSE()) {
			auto boolean_literal = std::make_shared<expression::boolean_literal>(false);
			return std::static_pointer_cast<expression_t>(boolean_literal);
		}
		else if (ctx->NIL()) {
			auto nil_literal = std::make_shared<expression::nil_literal>();
			return std::static_pointer_cast<expression_t>(nil_literal);
		}
		else if (auto number_value = ctx->number()) {
			auto numeral_literal = std::make_shared<expression::numeral_literal>(number_value->getText());
			return std::static_pointer_cast<expression_t>(numeral_literal);
		}
		else if (auto string_value = ctx->string()) {
			//auto string_literal = std::make_shared<expression::string_literal>(string_value->getText());
			return visitString(string_value).as<std::shared_ptr<expression_t>>();
		}
		else if (auto function_define = ctx->functiondef()) {
			auto res = visitFunctiondef(function_define).as<std::shared_ptr<expression_t>>();
			return res;
		}
		else if (auto table_constructor = ctx->tableconstructor()) {
			return visitTableconstructor(table_constructor).as<std::shared_ptr<expression_t>>();
		}
		else if (auto unary_expression = ctx->unaryOperator()) {
			using unary_expression_t = typename expression::unary_expression;

			auto to_unary = visitExp(ctx->exp().front()).as<std::shared_ptr<expression_t>>();

			unary_expression_t::operation_t operation;
			
			const auto operation_text = unary_expression->getText();

			if (const auto result = std::find_if(unary_expression_t::operation_map.cbegin(),
				unary_expression_t::operation_map.cend(),
				[operation_text](const auto& entry) {
					return entry.second == operation_text;
				}); result != unary_expression_t::operation_map.cend()) {
				operation = result->first;
			}

			auto unary_node = std::make_shared<unary_expression_t>(operation, std::move(to_unary));

			return std::static_pointer_cast<expression_t>(unary_node);
		}
		else if (auto prefix_expression = ctx->prefixexp()) {
			return visitPrefixexp(prefix_expression).as<std::shared_ptr<expression_t>>();
		}
		else if (auto ellipsis = ctx->ELLIPSIS()) {
			auto string_literal = std::make_shared<expression::string_literal>("...");
			return std::static_pointer_cast<expression_t>(string_literal);
		}

		return { };
	}
}