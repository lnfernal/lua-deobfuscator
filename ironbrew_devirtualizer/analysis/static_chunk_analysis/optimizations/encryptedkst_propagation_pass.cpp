#include "encryptedkst_propagation_pass.hpp"
#include "vm_arch/auxiliaries.hpp"

namespace deobf::ironbrew_devirtualizer::static_chunk_analysis::optimizations::encryptedkst_propagation_pass {

	std::size_t encryptedkst_propagation_pass::run(vm_arch::basic_block* first_block) {
		std::size_t num_optimizations = 0;

		auto current_block = first_block;
		while (current_block->next_block) {
			auto& last_instruction = current_block->instructions.back().get();
			if (last_instruction.op == vm_arch::opcode::op_decryptedkstflag) {
				const auto register_used = last_instruction.a;
				const auto constant_reference = last_instruction.bx;

				current_block->instructions.pop_back();
				// NOTE : register values are not shifted on inlining, they can be reused.

				auto& next_instruction = current_block->instructions.at(current_block->instructions.size() - 2).get(); //current_block->next_block->instructions.front().get();

				// supports ABC instructions only
				if (next_instruction.type == vm_arch::instruction_type::abc) {
					auto referenced_constant = chunk->constants.at(constant_reference).get();

					// fix constant owners
					{
						auto result = std::remove_if(referenced_constant->owners.begin(), referenced_constant->owners.end(), [&last_instruction](const vm_arch::instruction& owner) {
							return (owner == last_instruction);
						});

						// erase final owner
						referenced_constant->owners.erase(result, referenced_constant->owners.end());
					}

					current_block->instructions.pop_back(); // pop LOADK

					// propagate register -> constant

					if (!next_instruction.is_kb && next_instruction.b == register_used) {
						next_instruction.is_kb = true;
						next_instruction.b = constant_reference + 1;
					}

					if (!next_instruction.is_kc && next_instruction.c == register_used) {
						next_instruction.is_kc = true;
						next_instruction.c = constant_reference + 1;
					}

					referenced_constant->owners.emplace_back(std::cref(next_instruction));

					next_instruction.print();
				}

				// todo propagate basicblocks?

				++num_optimizations;
			}
			

			current_block = current_block->next_block.get();
		}

		return num_optimizations;
	}
}
