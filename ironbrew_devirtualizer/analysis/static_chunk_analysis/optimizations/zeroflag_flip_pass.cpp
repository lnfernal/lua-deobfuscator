#include "zeroflag_flip_pass.hpp"
#include "vm_arch/auxiliaries.hpp"

namespace deobf::ironbrew_devirtualizer::static_chunk_analysis::optimizations::zeroflag_flip_pass {
	std::size_t zeroflag_flip_pass::run(vm_arch::basic_block* first_block) {
		std::size_t num_optimizations = 0;

		auto current_block = first_block;
		while (current_block) {
			auto last_instruction = current_block->instructions.back();

			switch (auto last_opcode = last_instruction.get().op) {
				case vm_arch::opcode::op_eq:
				case vm_arch::opcode::op_eq1:
				case vm_arch::opcode::op_eq2:
				case vm_arch::opcode::op_eq3:

				case vm_arch::opcode::op_le:
				case vm_arch::opcode::op_le1:
				case vm_arch::opcode::op_le2:
				case vm_arch::opcode::op_le3:

				case vm_arch::opcode::op_lt:
				case vm_arch::opcode::op_lt1:
				case vm_arch::opcode::op_lt2:
				case vm_arch::opcode::op_lt3:

				case vm_arch::opcode::op_gt:
				case vm_arch::opcode::op_gt1:
				case vm_arch::opcode::op_gt2:
				case vm_arch::opcode::op_gt3:
				case vm_arch::opcode::op_ge:
				case vm_arch::opcode::op_ge1:
				case vm_arch::opcode::op_ge2:
				case vm_arch::opcode::op_ge3:
				case vm_arch::opcode::op_ne:
				case vm_arch::opcode::op_ne1:
				case vm_arch::opcode::op_ne2:
				case vm_arch::opcode::op_ne3:

				case vm_arch::opcode::op_test:
				case vm_arch::opcode::op_testset:

				case vm_arch::opcode::op_test1:
				case vm_arch::opcode::op_testset1: {
					// todo might fuck up with while loops with no body, check jump dests
					auto& next_block = current_block->next_block;
					auto& target_block = current_block->target_block;

					if (!next_block->instructions.size() == 1) {
						break;
					}

					if (!target_block->instructions.size() == 1) {
						/*auto is_preserved_sub = (target_block->instructions.size() == 3 && target_block->instructions.at(0).get().op == vm_arch::opcode::op_move && target_block->instructions.at(1).get().op == vm_arch::opcode::op_loadbool);

						if (!is_preserved_sub) {
							break;
						}*/
						break;
					}

					if (next_block->instructions.back().get().op != vm_arch::opcode::op_jmp)
						break;

					if (target_block->instructions.back().get().op != vm_arch::opcode::op_jmp)
						break;

					if (next_block->target_block != target_block->next_block)
						break;

					if (target_block->target_block.get() == current_block)
						break;

					last_instruction.get().print();
					
					auto new_operator = aux::get_inverse_kst_optimized_logic_opcode(last_opcode);
					
					last_instruction.get().op = new_operator;

					last_instruction.get().print();

					//current_block->target_block->next_block = next_block->target_block->next_block;

					//next_block->target_block->next_block = current_block->target_block;

					//current_block->next_block = next_block->target_block;
					
					//current_block = next_block->target_block.get();

					{
						const auto old_next = current_block->next_block;

						current_block->next_block = current_block->target_block;

						current_block->target_block = old_next->target_block;

						//std::cout << old_next.use_count() << std::endl;
					}
					++num_optimizations;
					

					continue;
				}
			}

			for (auto& insn : current_block->instructions) {
				insn.get().print();
			}
			std::cout << "SEND\n";
			continue_search:
			current_block = current_block->next_block.get();
		}

		return num_optimizations;
	}
}
