#pragma once
#include <map>
#include "vm_arch/instruction.hpp"
#include "vm_arch/proto.hpp"
#include "vm_arch/basic_block.hpp"
#include "vm_arch/vanilla_proto.hpp"

namespace deobf::ironbrew_devirtualizer::static_chunk_analysis {
	// used for deobfuscator LASM-Level static analysis

	namespace constant_decryption {
		struct constant_decryption_recongizer;
		struct decryption_block_transformer;
	}

	class static_chunk_analysis final {
		friend struct constant_decryption::constant_decryption_recongizer;
		friend struct constant_decryption::decryption_block_transformer;

		vm_arch::proto* chunk = nullptr;
		std::shared_ptr<vm_arch::basic_block> cfg_result;
		std::vector<std::unique_ptr<vm_arch::instruction>> instructions_result;

		std::map<const vm_arch::instruction*, std::size_t> instruction_pc_mapping;
		std::map<const vm_arch::instruction*, const vm_arch::instruction*> instruction_sucessor_mapping; // use incase target basic block generation PC is stripped
		std::unordered_map<std::size_t, std::size_t> shifted_constant_mapping; // shift constant table after DCE

		void populate_constant_owners();
		void remove_dead_constants();
		void eliminate_duplicated_constants();

		void propagate_instructions_pc();
		void fix_branch_targets();
		void populate_instructions();
		void optimize_cflow_calls();
	
		static void chunk_header_vm_optimizations(vm_arch::vanilla_proto* proto);
	public:
		explicit static_chunk_analysis(vm_arch::proto* chunk) : chunk(chunk) { };
	
		static void later_analysis(vm_arch::vanilla_proto* proto);

		void run_analysis();
	};
}