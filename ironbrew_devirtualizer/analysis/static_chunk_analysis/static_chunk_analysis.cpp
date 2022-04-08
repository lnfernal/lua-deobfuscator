#include "static_chunk_analysis.hpp"
#include "vm_arch/control_flow_graph.hpp"
#include "optimizations/common.hpp"
#include "constant_decryption/constant_decryption_recongizer.hpp"

namespace deobf::ironbrew_devirtualizer::static_chunk_analysis {

	void static_chunk_analysis::populate_constant_owners() {
		for (auto& instruction : chunk->instructions) {
			if (!(instruction->is_ka || instruction->is_kb || instruction->is_kc))
				continue;

			std::unordered_set<std::uint16_t> set_targets(3ul); // to handle duplicates

			if (instruction->is_ka) {
				set_targets.emplace(instruction->a - 1);
				const auto& constant = chunk->constants.at(instruction->a - 1).get();
				constant->owners.emplace_back(std::cref(*instruction));
			}

			if (instruction->is_kb) {
				std::size_t constant_number = 0;
				if (instruction->b > 0) {
					constant_number = instruction->b - 1;
				}
				else {
					constant_number = instruction->bx - 1;
				}

				if (!set_targets.count(constant_number)) {
					set_targets.emplace(constant_number);
					const auto& constant = chunk->constants.at(constant_number).get();
					std::cout << "kst:" << constant->to_string() << std::endl;
					constant->owners.emplace_back(std::cref(*instruction));
				}
			}

			if (instruction->is_kc) {
				if (!set_targets.count(instruction->c - 1)) {
					const auto& constant = chunk->constants.at(instruction->c - 1).get();
					std::cout << "kst:" << constant->to_string() << std::endl;

					constant->owners.emplace_back(std::cref(*instruction));
				}
			}
		}
	}
	
	void static_chunk_analysis::remove_dead_constants() {
		// no STL because sucks
		std::unordered_map<std::size_t, std::size_t> shifted_constants(chunk->constants.size());

		// Fixed-Point Iteration For Shifting Constants And Removing Dead Ones
		auto constant_num = 0ul, num_dead_constants = 0ul;
		for (auto current_constant = 0ul; current_constant < chunk->constants.size(); ++current_constant, ++constant_num) {
			bool is_collectable = true;
			for (auto& owner : chunk->constants.at(current_constant)->owners) {
				if (!(owner.get().flags & instruction::instruction_mark_flag::to_collect)) {
					is_collectable = false;
					break;
				}
			}

			if (chunk->constants.at(current_constant)->owners.size() == 0 || is_collectable) {
				chunk->constants.erase(chunk->constants.begin() + current_constant);
				--current_constant;
				++num_dead_constants;
				continue;
			}

			shifted_constants.emplace(constant_num, current_constant);
		}
		std::cout << "deadkst:" << num_dead_constants << std::endl;

		// ... fix kst targets
		auto get_shifted_target = [&](std::size_t target) {
			if (auto result = shifted_constants.find(target - 1); result != shifted_constants.cend())
				return result->second + 1;

			return target;
		};

		for (auto& instruction : chunk->instructions) {
			if (instruction->is_ka) {
				instruction->a = get_shifted_target(instruction->a);
			}

			if (instruction->is_kb) {
				if (instruction->b > 0) {
					instruction->b = get_shifted_target(instruction->b);
				}
				else {
					instruction->bx = get_shifted_target(instruction->bx);
				}
			}

			if (instruction->is_kc) {
				instruction->c = get_shifted_target(instruction->c);
			}
		}
	}
	
	void static_chunk_analysis::propagate_instructions_pc() {
		std::size_t current_pc = 0;

		std::weak_ptr<vm_arch::basic_block> current_block{ cfg_result };
		while (!current_block.expired()) {
			auto result_block = current_block.lock();
			for (auto& instruction : result_block->instructions) {
				instruction_pc_mapping.emplace(&instruction.get(), current_pc);
				++current_pc;
			}

			current_block = result_block->next_block;
		}
	}

	void static_chunk_analysis::fix_branch_targets() {
		std::weak_ptr<vm_arch::basic_block> current_block{ cfg_result };
		while (!current_block.expired()) {
			auto result_block = current_block.lock();
			
			if (!result_block->instructions.empty()) {
				auto last_instruction = result_block->instructions.back();
				auto last_opcode = last_instruction.get().op;
				if (last_opcode == vm_arch::opcode::op_jmp || last_opcode == vm_arch::opcode::op_forprep || last_opcode == vm_arch::opcode::op_forloop) {
					auto target_instruction = result_block->target_block->instructions.at(0);
					if (auto pc_result = instruction_pc_mapping.find(&target_instruction.get()); pc_result != instruction_pc_mapping.end()) {
						last_instruction.get().sbx = pc_result->second;
					}
				}
			}

			current_block = result_block->next_block;
		}
	}

	void static_chunk_analysis::populate_instructions() {
		std::weak_ptr<vm_arch::basic_block> current_block{ cfg_result };
		while (!current_block.expired()) {
			auto result_block = current_block.lock();
			for (auto& instruction : result_block->instructions) {
				instructions_result.emplace_back(std::make_unique<vm_arch::instruction>(instruction));
			}

			current_block = result_block->next_block;
		}
	}

	void static_chunk_analysis::optimize_cflow_calls() {
		if (!chunk->is_chunk_cflow())
			return;

		/*instructions_result.erase(instructions_result.cbegin()); // erase NEWSTACK

		for (auto it = instructions_result.begin(); it != instructions_result.end(); ++it) {
			auto& current_instruction = *it->get();
			if (current_instruction.op == vm_arch::opcode::op_move && current_instruction.type == vm_arch::instruction_type::abc) {
				if (current_instruction.a == 0 && current_instruction.b == 0 && current_instruction.c == 1) {
					// remove previous MOVE
					it--;

					it = instructions_result.erase(it, it + 2);
				}
			}
		}*/

		cfg_result->instructions.erase(cfg_result->instructions.begin()); // erase NEWSTACK

		// no STL because sucks
		std::weak_ptr<vm_arch::basic_block> current_block{ cfg_result };
		while (!current_block.expired()) {
			auto result_block = current_block.lock();
			for (auto it = result_block->instructions.begin(); it != result_block->instructions.end(); ) {
				auto& current_instruction = it->get();
				/*if (current_instruction.op == vm_arch::opcode::op_move && current_instruction.type == vm_arch::instruction_type::abc) {
					if (current_instruction.a == 0 && current_instruction.b == 0 && current_instruction.c == 1) {
						current_instruction.print();
						// remove previous MOVE
						//std::cout << std::distance(it, result_block->instructions.begin()) << std::endl;
						//result_block->next_block->instructions.at(0).get().print();
						
						it = result_block->instructions.erase(it);
						continue;
					}
					
				}*/
				if (current_instruction.op == vm_arch::opcode::op_move && current_instruction.a == 0 && current_instruction.b == 0) {
					it = result_block->instructions.erase(it);
					continue;
				}

				++it;
			}

			current_block = result_block->next_block;
		}
	}

	// populates chunk header for VM optimizations
	void static_chunk_analysis::chunk_header_vm_optimizations(vm_arch::vanilla_proto* proto) {
		/*const auto max_upvalue_instruction = std::max_element(current_proto->code.cbegin(), current_proto->code.cend(), [](const auto& maximum, const auto& current) {
			// B + 1 = upvalue size
			if (current->opcode == vm_arch::vanilla_opcode::op_setupval || current->opcode == vm_arch::vanilla_opcode::op_getupval) {
				return maximum->b < current->b;
			}

			return false;
		});

		if (max_upvalue_instruction->get()->op == vm_arch::opcode::op_setupval || max_upvalue_instruction->get()->opcode == vm_arch::opcode::op_getupval) {
			current_proto->num_upvalues = max_upvalue_instruction->get()->b + 1;
		}*/
		
		std::size_t num_upvalues = 0;
		for (auto& instruction : proto->code) {
			if (instruction->opcode == vm_arch::vanilla_opcode::op_getupval || instruction->opcode == vm_arch::vanilla_opcode::op_setupval) {
				if (num_upvalues < instruction->b + 1) {
					num_upvalues = instruction->b + 1;
				}
			}
		}

		proto->num_upvalues = num_upvalues;

		// populate vararg flag
		bool is_vararg = false;
		for (auto& instruction : proto->code) {
			if (instruction->opcode == vm_arch::vanilla_opcode::op_vararg) {
				is_vararg = true;
				break;
			}
		}

		// populate stack size
		std::size_t max_stack_size = 0;

		for (auto& instruction : proto->code) {
			if (instruction->a + 1 > max_stack_size) {
				max_stack_size = instruction->a + 1;
			}
		}
		proto->max_stack_size = max_stack_size;

		// parameters reserve registers in the stack aswell
		/*if (current_proto->num_params == 0) { // 1 free slot?
			current_proto->max_stack_size++;
		}*/

		// populate vararg flag

		proto->is_vararg = (is_vararg ? 3 : 2);
	}

	//struct duplicate_set_hash final {
	//	inline std::size_t operator()(const std::pair<std::string, std::size_t>& pair) const {
	//		return std::hash<std::string>{ }(pair.first) ^ pair.second;
	//	}
	//};

	void static_chunk_analysis::eliminate_duplicated_constants() {
		// no STL because sucks

		std::unordered_set<std::string> string_constants(chunk->constants.size());
		std::unordered_map<std::string, std::ptrdiff_t> constant_mapping(chunk->constants.size());
		auto constant_shift_factor = 0;

		auto shift_constant_value = [](vm_arch::instruction& owner, int old_index, int new_index) {
			if (owner.is_ka && owner.a == old_index) {
				owner.a = new_index;
			}

			if (owner.is_kb) {
				if (owner.b > 0 && owner.b == old_index) {
					owner.b = new_index;
				}
				else if (owner.bx == old_index) {
					owner.bx = new_index;
				}
			}

			if (owner.is_kc && owner.c == old_index) {
				owner.c = new_index;
			}
		};

		auto current_index = 0ul;

		for (auto it = chunk->constants.begin(); it != chunk->constants.end(); ++it) {
			++current_index;

			auto& current_constant = (*it);
			if (current_constant->get_constant_type() == constant::constant_type::string) {
				//const auto index_difference = std::distance(chunk->constants.begin(), it);

				const auto result = std::get<std::string>(current_constant->value);
				if (string_constants.count(result)) { // duplicate detected
					auto index = constant_mapping.find(result)->second;
					for (const auto& instruction : current_constant->owners) {
						auto& owner = const_cast<vm_arch::instruction&>(instruction.get());
						shift_constant_value(owner, current_index, index);
					}

					it = chunk->constants.erase(it);

					--constant_shift_factor, --it;
					continue;
				}

				if (constant_shift_factor != 0) {
					for (const auto& instruction : current_constant->owners) {
						auto& owner = const_cast<vm_arch::instruction&>(instruction.get());
						shift_constant_value(owner, current_index, current_index + constant_shift_factor);
					}
				}

				string_constants.emplace(result);
				constant_mapping.emplace(result, current_index);
			}
		}
	}

	void static_chunk_analysis::later_analysis(vm_arch::vanilla_proto* proto) {
		chunk_header_vm_optimizations(proto);
	}

	void static_chunk_analysis::run_analysis() {
		// populate constant references
		populate_constant_owners();

		// generate control flow graph
		cfg_result = vm_arch::control_flow_graph::generate_graph(chunk->instructions);

		// optimize branch flag flip trick
		const auto test_flip_optimizations = optimizations::zeroflag_flip_pass::zeroflag_flip_pass{ }(cfg_result.get());

		// todo rename control flow passes into better names (test spam to something else blah blah)

		// apply maxCFG optimizations
		if (chunk->is_chunk_cflow()) {

			// TODO FIX EQ MUTATE
			const auto eq_mutate_optimizations = optimizations::single_ref_jmp_pass::single_ref_jmp_pass{ }(cfg_result.get());

			const auto test_preserve_optimizations = optimizations::regkst_propagation_pass::regkst_propagation_pass{ }(cfg_result.get());

			const auto bounce_optimizations = optimizations::single_ref_jmp_pass::single_ref_jmp_pass{ }(cfg_result.get());

			const auto test_spam_optimizations = optimizations::cmp_spam_pass::cmp_spam_pass{ }(cfg_result.get());

			// todo calculate pass score and distrubtion for analysis.
		}
		
		std::cout << "instructions:" << std::endl;
		{
			auto curr = cfg_result;
			while (curr) {
				for (auto& ins : curr->instructions) {
					ins.get().print();
				}
				curr = curr->next_block;
			}
		}

		std::cout << "optimizedcflow\n";
		// OPTIMIZE CHUNK -> INSTRUCTION GENERATION
		
		// decrypt constants
		{
			auto recongizer = constant_decryption::constant_decryption_recongizer{ *this };
			recongizer.run();

			while (!recongizer.block_transformers.empty()) {
				auto& transformer = recongizer.block_transformers.front();
				transformer.optimize();
				recongizer.block_transformers.pop();
			}

			eliminate_duplicated_constants();
			optimizations::encryptedkst_propagation_pass::encryptedkst_propagation_pass{ chunk }(cfg_result.get());
		}

		// fix bx/target_pc for branch instructions

		// (DEPRECATED)
		// order code paths (SYNTACTIC GARBAGE/UNREACHABLE PATHS ARE DISCARDED) by topological sorting our control flow graph
		//auto block_toposorter = static_chunk_analysis::block_toposorter::block_toposorter{ cfg_result };
		//block_toposorter.run_toposort();

		/*for (auto& block : block_toposorter.sorted_cfg) {
			for (auto& instr : block->instructions) {
				instr.get().print();
			}
		}*/

		// remove dead constants (false generated by obfuscator) and shift constant table, relocate its references
		remove_dead_constants();


		// optimize CFLOW call locations, remove NEWSTACK
		optimize_cflow_calls();

		// set up instructions
		propagate_instructions_pc();

		// fix branch targets
		fix_branch_targets();

		// populate instructions
		populate_instructions();

		// todo fix big string constants

		chunk->instructions = std::move(instructions_result);
		
		std::cout << "lol\n";

		for (auto& instruction : chunk->instructions)
			instruction.get()->print();
		for (auto& kst : chunk->constants)
			std::cout << kst->to_string() << std::endl;



		std::cout << "gg\n";

		//for (auto& proto : chunk->protos)
		//	static_chunk_analysis{ proto.get() }.run_analysis();
	}
}