#pragma once

#include <deque>

#include "./ast/ir/abstract_visitor_pattern.hpp"
#include "./ast/ir/node.hpp"
#include "../../../vm_arch/instruction.hpp"

#include <mutex>
#include <functional>


namespace deobf::ironbrew_devirtualizer {
	namespace vm_handler_identifiers {
		struct opcode_identifier_handler;
	}
}

namespace deobf::ironbrew_devirtualizer::symbolic_execution::vm {
	using namespace ast;

	/*
		aztupbrew and few more obfuscators use a loop unrolled, inlined version (divide and conquer) version of binary search in their VM Loop for better performance (though it causes branch mispredictions).
		meaning mapping proccess will be for us O(k * log n) where K is deque size and n is number of branches, this problem is NP hard.
		this one is better than feeding all accepted conditions into a SAT Solver and finding the ONLY `N` that can be evaluted.
		notice that we also can boost the search speed by a little bit via constructing a BST off the visitor pattern.

		algorithm Analysis:

		- Best case complexity : O(k) search time complexity
		- Worst case complexity : O(k * log n) search time complexity
		
		(notice that in lua, the cyclomatic complexity is the same as the time complexity we have)

		- Space complexity is O(1) in both cases.
	*/

	//template <typename handler_t>
	struct opcode_handler_computer final : ir::abstract_visitor_pattern {
		std::function<vm_arch::opcode(vm_arch::instruction&, ir::statement::block*)> callback_functor;
		std::deque<std::reference_wrapper<vm_arch::instruction>> back_track;
		
		mutable std::recursive_mutex handler_mutex;

		// opcode handler routine
		void handle(vm_arch::instruction&, ir::statement::block*);

		// visitor
		bool accept(ir::expression::binary_expression* expression) override;
		bool accept(ir::statement::block* body) override;
		bool accept(ir::statement::if_statement* if_stat) override;
		
		
		/*template <typename callback_t>
		void set_callback(callback_t&& callback) {
			callback_functor = std::move(callback);
		}*/

		void run_cycle() {
			//std::cout << "running.\n";
			//handler_mutex.lock();
			root->accept(this);
			//handler_mutex.unlock();
		}

		explicit opcode_handler_computer(ir::statement::if_statement* const root, std::deque<std::reference_wrapper<vm_arch::instruction>>& back_track) :
			root(root),
			back_track(std::move(back_track))
		{ }

		explicit opcode_handler_computer(ir::statement::if_statement* const root) :
			root(root)
		{ }

		std::map<std::uint8_t, std::vector<vm_arch::opcode>> memoized_virtuals{ };
	private:

		//static decltype(callback_functor) callback_handler;
		ir::statement::if_statement* const root;
	};
}