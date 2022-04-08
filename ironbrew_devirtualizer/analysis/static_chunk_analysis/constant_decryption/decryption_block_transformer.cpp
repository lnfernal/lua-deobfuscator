#pragma once
#include "decryption_block_transformer.hpp"

#include <queue>

// todo optimize this code
namespace deobf::ironbrew_devirtualizer::static_chunk_analysis::constant_decryption {
	bool decryption_block_transformer::optimize() {
		// todo fix branch targets (might be unnecessary?)
		// todo handle debug lines?
		// todo fix duplicates

		// set encrypted constant to decrypted constant ref in proto

		auto begin_block = begin.lock();
		if (!begin_block)
			return false;


		auto final_block = end.lock();
		if (!final_block)
			return false;


		auto& constant_result = analyzer.chunk->constants.at(parameter_reference);
		constant_result->value = decrypted_constant;
		//constant_result->flags |= constant::constant_mark_flag::decrypted_constant;


		// purify begin_block & flag
		for (;; begin_block->instructions.pop_back()) {
			auto current_instruction = begin_block->instructions.back();

			// todo replace loadk with real load constant instruction (optimization pass)
			if (current_instruction.get().op == vm_arch::opcode::op_loadk && current_instruction.get().bx - 1 == parameter_reference) {
				const auto move_instruction = final_block->instructions.at(0);
				if (move_instruction.get().op != vm_arch::opcode::op_move)
					return false;

				current_instruction.get().a = move_instruction.get().a; // swap register values

				//current_instruction.get().print();

				break;
			}

			current_instruction.get().flags |= instruction::instruction_mark_flag::to_collect;
		}

		// flag all ranging instructions ranging from [begin_block, final_block] as collectable
		{
			std::weak_ptr<vm_arch::basic_block> current_block{ begin_block->next_block };
			while (!current_block.expired()) {
				auto result_block = current_block.lock();
				if (result_block == final_block)
					break;

				for (auto& instruction : result_block->instructions) {
					instruction.get().flags |= instruction::instruction_mark_flag::to_collect;
				}

				current_block = result_block->next_block;
			}
		}

		// add special IR instruction as bblock leader for later optimizations
		{
			auto result = new vm_arch::instruction{ vm_arch::opcode::op_decryptedkstflag };

			result->type = vm_arch::instruction_type::abx;
			result->a = final_block->instructions.at(0).get().a;
			result->bx = parameter_reference;
			result->is_kb = true;
			
			begin_block->instructions.emplace_back(*result);
		}

		// todo propagate bblocks?
		begin_block->next_block = final_block->target_block;

		analyzer.remove_dead_constants();

		return true;
	}
}