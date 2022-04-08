#include "./ast/ir/abstract_visitor_pattern.hpp"
#include "./ast/ir/node.hpp"
#include "opcode_handler_computer.hpp"
#include "./ironbrew_devirtualizer/vm_handler_identifiers/identifier_base.hpp"

#include <future>
#include <algorithm>
#include <numeric>

namespace deobf::ironbrew_devirtualizer::symbolic_execution::vm {
	using namespace ast;

	//decltype(loop_unrolled_bst::callback_functor) loop_unrolled_bst::callback_handler;

	bool opcode_handler_computer::accept(ir::expression::binary_expression* expression) {
		if (back_track.empty())
			return false;

		if (expression->left->is<ir::expression::variable>())
			if (expression->left->to_string() != "virtual_opcode")
				return false;

		if (expression->right->is<ir::expression::numeral_literal>()) {
			const auto right_value = static_cast<const ir::expression::numeral_literal*>(expression->right.get());

			using operation_t = typename ir::expression::binary_expression::operation_t;

			const auto virtual_opcode = back_track.front().get().virtual_opcode;

			switch (expression->operation) { // concerete execute for bound
			case operation_t::le:
				return virtual_opcode <= right_value->value; // this is a divide and conquer route, continue to next if statements
			case operation_t::gt:
				return virtual_opcode > right_value->value;
			case operation_t::eq:
				return virtual_opcode == right_value->value;
			}
		}

		return false;
	}

	bool opcode_handler_computer::accept(ir::statement::block* body) {
		if (body->body.size() == 1 && body->body.at(0)->is<ir::statement::if_statement>()) {
			auto if_statement = static_cast<ir::statement::if_statement*>(body->body.at(0).get());
			if (if_statement->condition->is<ir::expression::binary_expression>()) {
				const auto statment_lhs = static_cast<ir::expression::binary_expression*>(if_statement->condition.get())->left;

				if (statment_lhs->is<ir::expression::variable>() && statment_lhs->to_string() == "virtual_opcode") {
					if_statement->accept(this);
					return false;
				}
			}
		}

		if (back_track.empty()) {
			return false;
		}

		// we have entered the final block, map it

		if (auto memoized_result = memoized_virtuals.find(back_track.front().get().virtual_opcode); memoized_result != memoized_virtuals.cend()) {
			std::cout << "got top:" << static_cast<int>(memoized_result->first) << std::endl;
			for (auto& opcode : memoized_result->second) {
				if (back_track.empty())
					break;

				auto front_instruction = back_track.front();
				front_instruction.get().op = opcode;

				std::invoke(callback_functor, front_instruction, nullptr);
				back_track.pop_front();
			}
		}
		else {
			//std::cout << body->body.at(0)->to_string() << std::endl;
			// call super op handler
			std::cout << "front instr:" << back_track.front().get().virtual_opcode << std::endl;
			handle(back_track.front(), body);
		}
		std::cout << "done." << std::endl;

		//run_cycle();
		//std::async(std::launch::deferred, &opcode_handler_computer::run_cycle, this);

		return false;
	}

	bool opcode_handler_computer::accept(ir::statement::if_statement* if_stat) { // i couldve shorted this into 1 line, but changing the visitor pattern itself from the IR will affect other things.
		const auto root_condition = static_cast<ir::expression::binary_expression*>(if_stat->condition.get());

		if (accept(root_condition)) {
			if_stat->body->accept(this);
			return false;
		}

		for (const auto& [condition, body] : if_stat->else_if_statements) {
			const auto binary_condition = static_cast<ir::expression::binary_expression*>(condition.get());
			if (accept(binary_condition)) {
				body->accept(this);
				return false;
			}
		}

		if (if_stat->else_body.has_value()) {
			if (accept(if_stat->else_body.value().get())) {
				if_stat->body->accept(this);
				return false;
			}
		}

		return false;
	}

}
