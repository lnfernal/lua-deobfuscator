#include "cmp_spam_pass.hpp"
#include <iostream>
#include <queue>

#include "vm_arch/auxiliaries.hpp"

namespace deobf::ironbrew_devirtualizer::static_chunk_analysis::optimizations::cmp_spam_pass {

	std::weak_ptr<vm_arch::basic_block> identify_final_target_successor(vm_arch::instruction& last_instruction, std::weak_ptr<vm_arch::basic_block> start) {
		auto current_block{ start };

		auto verify_block_sucessor = [&](vm_arch::basic_block* result) -> vm_arch::block_iterate_direction {
			auto first_instruction = result->instructions.front();

			if (auto instr_op = first_instruction.get().op; instr_op == vm_arch::opcode::op_jmp) {
				return vm_arch::block_iterate_direction::target;
			}
			else if (instr_op == last_instruction.op && first_instruction.get().a == last_instruction.a) {
				return vm_arch::block_iterate_direction::next;
			}
			else {
				auto normalized_opcode = aux::get_normalized_opcode(instr_op);
				if (normalized_opcode == vm_arch::opcode::op_ge || normalized_opcode == vm_arch::opcode::op_lt) {
					return vm_arch::block_iterate_direction::target;
				}
			}

			return { };
		};

		while (!current_block.expired()) {
			auto current_block_result = current_block.lock();

			// too lazy to optimize this false-generated shit on eq mutate and its probably redudrant
			{
				bool do_double_skip = false;

				auto normalized_opcode = aux::get_normalized_opcode(current_block_result->instructions.front().get().op);

				if (normalized_opcode == vm_arch::opcode::op_ge || normalized_opcode == vm_arch::opcode::op_lt) {
					auto target = current_block_result->target_block;
					if (target->instructions.size() == 1) {
						auto normalized_opcode_2 = aux::get_normalized_opcode(target->instructions.front().get().op);
						if (normalized_opcode_2 == vm_arch::opcode::op_gt || normalized_opcode_2 == vm_arch::opcode::op_le) {
							do_double_skip = true;
						}
					}
				}

				if (do_double_skip) {
					current_block = current_block_result->target_block->next_block; //: current_block_result->target_block->target_block;
					continue;
				}
			}

			switch (verify_block_sucessor(current_block_result.get())) {
				case vm_arch::block_iterate_direction::next: {
					current_block = current_block_result->next_block;
					break;
				}
				case vm_arch::block_iterate_direction::target: {
					if (verify_block_sucessor(current_block_result->target_block.get()) == vm_arch::block_iterate_direction::none) {
						current_block_result->instructions.front().get().print();
						goto return_result;
					}

					current_block = current_block_result->target_block;
					break;
				}
				default:
					goto return_result;
			}
		}

		//current_block.lock()->instructions.front().get().print();
		return_result:
		return current_block;
	}


	std::size_t cmp_spam_pass::run(vm_arch::basic_block* first_block) {
		std::size_t num_optimizations = 0;

		auto current_block = first_block;
		while (current_block) {
			auto last_instruction = current_block->instructions.back();

			auto last_opcode = last_instruction.get().op;

			if (last_opcode >= vm_arch::opcode::op_eq && last_opcode <= vm_arch::opcode::op_ne3) {
				auto& first_jmp = current_block->next_block; // jump to target block
				auto& second_jmp = current_block->target_block; // jump to target's next block

				if (!(first_jmp->instructions.size() == 1 && first_jmp->instructions.at(0).get().op == vm_arch::opcode::op_jmp))
					goto continue_execution;

				if (!(second_jmp->instructions.size() == 1 && second_jmp->instructions.at(0).get().op == vm_arch::opcode::op_jmp))
					goto continue_execution;

				auto new_target_block = identify_final_target_successor(last_instruction.get(), first_jmp);
				auto target_result = new_target_block.lock();
				

				// terminator block (return) wont continue, usually appears on the first spam.
				// will be optimized on topological sorting aswell

				/*if (auto terminator_block = first_jmp->target_block->block_predecessors.front().lock(); terminator_block->is_terminator) {
					std::cout << "aaa\n";
					terminator_block->next_block = nullptr;
					terminator_block->instructions.back().get().print();
				}*/

				for (auto& predecessor : first_jmp->target_block->block_predecessors) {
					auto result = predecessor.lock();
					if (result->is_terminator) {
						result->next_block = nullptr;
					}
				}

				current_block->target_block->instructions.at(0).get().print();
				//std::cout << "t:" << first_jmp->target_block->block_predecessors.front().lock()->instructions.back().get().print() << std::endl;
				std::cout << target_result->instructions.size() << std::endl;
				target_result->instructions.at(0).get().print();

				// link basic blocks
				auto old_target = current_block->target_block;
				old_target->next_block->instructions.back().get().print();
				current_block->target_block = old_target->next_block;

				target_result->next_block = current_block->target_block;//current_block->target_block;

				current_block->next_block = target_result;

				++num_optimizations;
				//current_block->next_block->next_block->next_block->instructions.back().get().print();
				
				//first_jmp->block_sucessors.clear();
				//second_jmp->block_sucessors.clear();

				//current_block->next_block->next_block->next_block->instructions.at(0).get().print();


				// todo unlink terminator (first_jmp's preceddor) from those

				continue;
			}
			continue_execution:
			current_block = current_block->next_block.get();
		}

		return num_optimizations;
	}
}
