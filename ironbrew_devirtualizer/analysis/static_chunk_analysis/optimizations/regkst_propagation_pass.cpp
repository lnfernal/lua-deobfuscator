#include "regkst_propagation_pass.hpp"
#include "vm_arch/auxiliaries.hpp"

namespace deobf::ironbrew_devirtualizer::static_chunk_analysis::optimizations::regkst_propagation_pass {
	std::size_t regkst_propagation_pass::run(vm_arch::basic_block* first_block) {
		std::size_t num_optimizations = 0;

		auto current_block = first_block;
		while (current_block->next_block) {
			auto last_instruction = current_block->instructions.back();

			const auto last_opcode = last_instruction.get().op;

			if (current_block->instructions.size() < 3)
				goto continue_execution;
				
			if (last_opcode == vm_arch::opcode::op_le ||
				last_opcode == vm_arch::opcode::op_gt ||
				last_opcode == vm_arch::opcode::op_lt ||
				last_opcode == vm_arch::opcode::op_ge ||
				last_opcode == vm_arch::opcode::op_ne ||
			last_opcode == vm_arch::opcode::op_eq) {
				
				constexpr std::uint16_t reg_target_a = 250;
				constexpr std::uint16_t reg_target_c = 251;

				const auto reg_a = last_instruction.get().a;
				const auto reg_c = last_instruction.get().c;

				if (reg_a != reg_target_a && reg_c != reg_target_c)
					goto continue_execution;

				const auto reg_a_reference = current_block->instructions.at(current_block->instructions.size() - 3);
				const auto reg_c_reference = current_block->instructions.at(current_block->instructions.size() - 2);
				reg_a_reference.get().print();
				reg_c_reference.get().print();

				const auto is_ka = (reg_a_reference.get().op == vm_arch::opcode::op_loadk);
				const auto is_kc = (reg_c_reference.get().op == vm_arch::opcode::op_loadk);

				const auto original_reg_a = is_ka ? reg_a_reference.get().bx : reg_a_reference.get().b;
				const auto original_reg_c = is_kc ? reg_c_reference.get().bx : reg_c_reference.get().b;

				std::cout << (reg_a_reference.get().op != vm_arch::opcode::op_move) << std::endl;

				if (reg_a_reference.get().op != vm_arch::opcode::op_move && !is_ka)
					goto continue_execution;

				if (reg_c_reference.get().op != vm_arch::opcode::op_move && !is_kc)
					goto continue_execution;


				// might memory leak if we capture shared_ptr? capture weak_ptr instead?
				auto is_preserved_block = [=](vm_arch::basic_block* block) -> bool {
					if (block->instructions.size() < 3) {
						return false;
					}

					auto& first_instruction = block->instructions.at(0).get();
					auto& second_instruction = block->instructions.at(1).get();

					const auto is_true_first = (first_instruction.op == vm_arch::opcode::op_loadbool && first_instruction.a == reg_target_a && first_instruction.b == 0);
					const auto is_true_second = (second_instruction.op == vm_arch::opcode::op_loadbool && second_instruction.a == reg_target_c && second_instruction.b == 0);

					return (is_true_first && is_true_second);
				};

				const auto& next_branch_block = current_block->target_block;
				const auto& target_branch_block = current_block->next_block->target_block;

				if (!is_preserved_block(next_branch_block.get()) && !is_preserved_block(target_branch_block.get()))
					goto continue_execution;

				last_instruction.get().a = original_reg_a;
				last_instruction.get().c = original_reg_c;
				last_instruction.get().op = vm_arch::aux::get_kst_optimized_logic_opcode(last_instruction.get().op, is_ka, is_kc);
				
				last_instruction.get().print();
				// erase loadk/move shit
				current_block->instructions.erase(current_block->instructions.end() - 3, current_block->instructions.end() - 1);

				// erase resulting loadbools from logical op's successors
				next_branch_block->instructions.erase(next_branch_block->instructions.begin(), next_branch_block->instructions.begin() + 2);
				target_branch_block->instructions.erase(target_branch_block->instructions.begin(), target_branch_block->instructions.begin() + 2);

				++num_optimizations;
			}
			// handle relational operators

			else if (last_opcode == vm_arch::opcode::op_test ||
				last_opcode == vm_arch::opcode::op_test1 ||
				last_opcode == vm_arch::opcode::op_testset ||
			last_opcode == vm_arch::opcode::op_testset1) {

				// random junk registers
				const auto junk_reg_1 = current_block->instructions.at(current_block->instructions.size() - 3);
				const auto junk_reg_2 = current_block->instructions.at(current_block->instructions.size() - 2);

				if (!(junk_reg_1.get().op == vm_arch::opcode::op_move && junk_reg_2.get().op == vm_arch::opcode::op_move))
					goto continue_execution;

				if (junk_reg_1.get().b != junk_reg_2.get().a || junk_reg_2.get().b != last_instruction.get().a)
					goto continue_execution;

				const auto random_slot1 = junk_reg_1.get().a;
				const auto random_slot2 = junk_reg_1.get().b;

				auto is_preserved_block = [=](vm_arch::basic_block* block) -> bool {
					if (block->instructions.size() < 3) {
						return false;
					}

					auto& first_instruction = block->instructions.at(0).get();
					auto& second_instruction = block->instructions.at(1).get();

					first_instruction.print();
					second_instruction.print();

					// todo check first.b ?
					const auto is_true_first = (first_instruction.op == vm_arch::opcode::op_move && first_instruction.a == random_slot2);
					const auto is_true_second = (second_instruction.op == vm_arch::opcode::op_loadbool && second_instruction.a == random_slot1 && second_instruction.b == 0);

					return (is_true_first && is_true_second);
				};

				const auto& next_branch_block = current_block->target_block;
				const auto& target_branch_block = current_block->next_block->target_block;

				if (!is_preserved_block(next_branch_block.get()) && !is_preserved_block(target_branch_block.get()))
					goto continue_execution;

				// omg exception safety when

				//std::cout << "wew\n";
				// erase shitty junk code
				current_block->instructions.erase(current_block->instructions.end() - 3, current_block->instructions.end() - 1);
				//std::cout << "c\n";
				target_branch_block->instructions.at(0).get().print();

				next_branch_block->instructions.erase(next_branch_block->instructions.begin(), next_branch_block->instructions.begin() + 2);
				//std::cout << "d\n";
				target_branch_block->instructions.at(0).get().print();

				target_branch_block->instructions.erase(target_branch_block->instructions.begin(), target_branch_block->instructions.begin() + 2);
				
				++num_optimizations;
			}

			/*switch (last_instruction.get().op) {
				// register manipulation operators only
				case vm_arch::opcode::op_le:
				case vm_arch::opcode::op_gt:
				case vm_arch::opcode::op_ne:
				case vm_arch::opcode::op_eq: {

				}
			}*/

			continue_execution:
			current_block = current_block->next_block.get();
		}

		return num_optimizations;
	}
}
