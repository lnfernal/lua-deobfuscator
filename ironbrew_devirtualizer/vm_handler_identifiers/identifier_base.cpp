#include "identifier_base.hpp"

namespace deobf::ironbrew_devirtualizer::vm_handler_identifiers {
	opcode_identifier_client::opcode_identifier_client() :
		handler(new basic_opcode_handler) {
		std::vector<std::unique_ptr<basic_opcode_handler>> handler_dispatch_table;

		// add handlers
		handler_dispatch_table.emplace_back(new add_identifier);
		handler_dispatch_table.emplace_back(new call_identifier);
		handler_dispatch_table.emplace_back(new close_identifier);
		handler_dispatch_table.emplace_back(new closure_identifier);
		handler_dispatch_table.emplace_back(new concat_identifier);
		handler_dispatch_table.emplace_back(new div_identifier);
		handler_dispatch_table.emplace_back(new eq_identifier);
		handler_dispatch_table.emplace_back(new forloop_identifier);
		handler_dispatch_table.emplace_back(new forprep_identifier);
		handler_dispatch_table.emplace_back(new getglobal_identifier);
		handler_dispatch_table.emplace_back(new gettable_identifier);
		handler_dispatch_table.emplace_back(new getupval_identifier);
		handler_dispatch_table.emplace_back(new jmp_identifier);
		handler_dispatch_table.emplace_back(new le_identifier);
		handler_dispatch_table.emplace_back(new len_identifier);
		handler_dispatch_table.emplace_back(new loadbool_identifier);
		handler_dispatch_table.emplace_back(new loadk_identifier);
		handler_dispatch_table.emplace_back(new loadnil_identifier);
		handler_dispatch_table.emplace_back(new lt_identifier);
		handler_dispatch_table.emplace_back(new mod_identifier);
		handler_dispatch_table.emplace_back(new move_identifier);
		handler_dispatch_table.emplace_back(new mul_identifier);
		handler_dispatch_table.emplace_back(new newtable_identifier);
		handler_dispatch_table.emplace_back(new not_identifier);
		handler_dispatch_table.emplace_back(new pow_identifier);
		handler_dispatch_table.emplace_back(new return_identifier);
		handler_dispatch_table.emplace_back(new self_identifier);
		handler_dispatch_table.emplace_back(new setglobal_identifier);
		handler_dispatch_table.emplace_back(new setlist_identifier);
		handler_dispatch_table.emplace_back(new settable_identifier);
		handler_dispatch_table.emplace_back(new setupval_identifier);
		handler_dispatch_table.emplace_back(new sub_identifier);
		handler_dispatch_table.emplace_back(new tailcall_identifier);
		handler_dispatch_table.emplace_back(new test_identifier);
		handler_dispatch_table.emplace_back(new testset_identifier);
		handler_dispatch_table.emplace_back(new tforloop_identifier);
		handler_dispatch_table.emplace_back(new unm_identifier);
		handler_dispatch_table.emplace_back(new vararg_identifier);

		// custom vm identifiers
		handler_dispatch_table.emplace_back(new newstack_identifier);
		handler_dispatch_table.emplace_back(new settop_identifier);

		handler_interface* current_handler = handler.get();
		for (auto& opcode_handler : handler_dispatch_table) {
			current_handler = current_handler->set_next_handler(std::move(opcode_handler));
		}
	}

	vm_arch::opcode opcode_identifier_client::handle(ir::statement::block* path) {
		//if (const auto result = memoized_paths.find(path); result != memoized_paths.cend())
			//return result->second;

		return handler->handle(path);
	}
}