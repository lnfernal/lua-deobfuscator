#include "eq_extension_pass.hpp"
#include "vm_arch/auxiliaries.hpp"

namespace deobf::ironbrew_devirtualizer::static_chunk_analysis::optimizations::eq_extension_pass {

	/*
	psuedocode:
		if (LHS > RHS) then
		  goto NEXT_LEADER
		else if (LHS < RHS) then
		  goto NEXT_LEADER
		else
		  goto SUCCESSOR
		end

		ge R[A] TARGET_LEADER R[B]
		jmp 0 NEXT_LEADER
		le R[A] TARGET_LEADER R[B]
		jmp 0 NEXT_LEADER
		jmp 0 SUCCESSOR

	*/

	/*static std::unordered_map<vm_arch::opcode, vm_arch::opcode> last_to_first_mapping { // could be used to check if exists aswell
		// A == 1
		{ vm_arch::opcode::op_ge, vm_arch::opcode::op_lt },
		{ vm_arch::opcode::op_ge1, vm_arch::opcode::op_lt1 },
		{ vm_arch::opcode::op_ge2, vm_arch::opcode::op_lt2 },
		{ vm_arch::opcode::op_ge3, vm_arch::opcode::op_lt3 },

		{ vm_arch::opcode::op_gt, vm_arch::opcode::op_le },
		{ vm_arch::opcode::op_gt1, vm_arch::opcode::op_le1 },
		{ vm_arch::opcode::op_gt2, vm_arch::opcode::op_le2 },
		{ vm_arch::opcode::op_gt3, vm_arch::opcode::op_le3 },

		// A == 0
		{ vm_arch::opcode::op_lt, vm_arch::opcode::op_ge },
		{ vm_arch::opcode::op_lt1, vm_arch::opcode::op_ge1 },
		{ vm_arch::opcode::op_lt2, vm_arch::opcode::op_ge2 },
		{ vm_arch::opcode::op_lt3, vm_arch::opcode::op_ge3 },

		{ vm_arch::opcode::op_le, vm_arch::opcode::op_gt },
		{ vm_arch::opcode::op_le1, vm_arch::opcode::op_gt1 },
		{ vm_arch::opcode::op_le2, vm_arch::opcode::op_gt2 },
		{ vm_arch::opcode::op_le3, vm_arch::opcode::op_gt3 },

	};*/

	static std::unordered_map<vm_arch::opcode, std::tuple<vm_arch::opcode, vm_arch::opcode>> last_to_first_mapping {
		// EQ A == 1 (should generate NE)
		{ vm_arch::opcode::op_ge, { vm_arch::opcode::op_le, vm_arch::opcode::op_ne} },
		{ vm_arch::opcode::op_ge1, { vm_arch::opcode::op_le1, vm_arch::opcode::op_ne1 } },
		{ vm_arch::opcode::op_ge2, { vm_arch::opcode::op_le2, vm_arch::opcode::op_ne2 } },
		{ vm_arch::opcode::op_ge3, { vm_arch::opcode::op_le3, vm_arch::opcode::op_ne3 } },

		// EQ A == 0
		{ vm_arch::opcode::op_lt, { vm_arch::opcode::op_gt, vm_arch::opcode::op_eq } },
		{ vm_arch::opcode::op_lt1, { vm_arch::opcode::op_gt1, vm_arch::opcode::op_eq1 } },
		{ vm_arch::opcode::op_lt2, { vm_arch::opcode::op_gt2, vm_arch::opcode::op_eq2 } },
		{ vm_arch::opcode::op_lt3, { vm_arch::opcode::op_gt3, vm_arch::opcode::op_eq3 } },
	};

	std::size_t eq_extension_pass::run(vm_arch::basic_block* first_block) {
		std::size_t num_optimizations = 0;

		auto current_block = first_block;
		while (current_block->next_block) {
			auto last_instruction = current_block->instructions.back();
			
			if (auto new_opcode = last_to_first_mapping.find(last_instruction.get().op); new_opcode != last_to_first_mapping.cend()) { // keep in mind ge's type is asbxc
				auto r_a = last_instruction.get().a;
				auto target_leader = last_instruction.get().sbx;
				auto r_c = last_instruction.get().c;

				// iterate successors instead todo?
				auto next_block = current_block->target_block;
				if (next_block != nullptr) {

					auto le_cmp = next_block->instructions.back();

					// support for this bullshit
					auto is_false_generated = (le_cmp.get().op == vm_arch::opcode::op_le || le_cmp.get().op == vm_arch::opcode::op_gt);

					if (le_cmp.get().op != std::get<0>(new_opcode->second) && !is_false_generated) {
						goto continue_search;
					}
					
					auto next_jmp_block_1 = current_block->next_block;
					auto next_jmp_block_2 = next_block->next_block;

					if (!(next_jmp_block_1->instructions.size() == 1 && next_jmp_block_2->instructions.size() == 1)) {
						goto continue_search;
					}

					if (next_jmp_block_1->instructions.at(0).get().op != vm_arch::opcode::op_jmp)
						goto continue_search;

					if (!(next_jmp_block_1->instructions.at(0).get() == next_jmp_block_2->instructions.at(0).get()))
						goto continue_search;

					if (le_cmp.get().a == r_a && le_cmp.get().c == r_c) {
						auto& le_target_block = next_block->target_block;
						le_target_block->instructions.at(0).get().print();


						if (le_target_block->instructions.back().get().op != vm_arch::opcode::op_jmp) {
							goto continue_search;
						}


						
						// no need to modify parameters
						last_instruction.get().op = std::get<1>(new_opcode->second);
						
						
						if (is_false_generated) {
							last_instruction.get().op = aux::get_inverse_kst_optimized_logic_opcode(std::get<1>(new_opcode->second));
							current_block->next_block = next_block->next_block;
							current_block->target_block = le_target_block;
						}
						else {
							current_block->next_block = le_target_block;
							current_block->target_block = le_target_block->next_block;
						}

						++num_optimizations;

						std::cout << "bsflag:" << is_false_generated << std::endl;
						last_instruction.get().print();
						//next_block->next_block->instructions.back().get().print();
						
						std::cout << "DAMN\n";
						//while (true) { }
						//next_block->target_block->instructions.back().get().print();
					}
				}
			}
			continue_search:
			current_block = current_block->next_block.get();
		}

		return num_optimizations;
	}
}
