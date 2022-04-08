#include "single_ref_jmp_pass.hpp"

namespace deobf::ironbrew_devirtualizer::static_chunk_analysis::optimizations::single_ref_jmp_pass {
	
	std::size_t single_ref_jmp_pass::run(vm_arch::basic_block* first_block) {
		std::size_t num_optimizations = 0;

		auto current_block = first_block;
		while (current_block) {
			if (current_block->instructions.back().get().op == vm_arch::opcode::op_jmp) {
				auto target = current_block->target_block;
				if (target->instructions.size() == 1 && target->instructions.back().get().op == vm_arch::opcode::op_jmp) {
					auto next_target = target->next_block;
					/*if (next_target != nullptr && next_target->instructions.size() == 1 && next_target->instructions.back().get().op == vm_arch::opcode::op_jmp) {
						// check if next target is unreachable to verify
						// deprecated, might there be junk jumps?

						//bool is_next_unreachable = false;
						//if (auto num_successor = next_target->block_sucessors.size(); num_successor == 1) {
						//	if (auto result = next_target->block_sucessors.back().lock()) {
						//		is_next_unreachable = result->is_terminator;
						//	}
						//}
						//else {
						//	is_next_unreachable = (num_successor == 0);
						//}

						// if (is_next_unreachable) {
						current_block->instructions.at(0).get().print();
						current_block->target_block = target->target_block;
						target->next_block = nullptr;

						for (auto& succ : next_target->block_sucessors) {
							std::cout << "SETA\n";
							for (auto& ins : succ.lock()->instructions)
								ins.get().print();

							std::cout << "SETA\n";

						}
						std::cout << "unreachable:" << next_target->block_sucessors.size() << std::endl;


					}*/
					//current_block->instructions.back().get().print();
					current_block->target_block = target->target_block;
					++num_optimizations;
				}
			}

			current_block = current_block->next_block.get();
		}

		return num_optimizations;
	}
}
