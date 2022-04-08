#pragma once

#include "vm_arch/basic_block.hpp"

namespace deobf::ironbrew_devirtualizer::static_chunk_analysis::optimizations {
	// credit to VTIL for the idea

	struct general_pass_interface {
		// returns number of applied optimizations
		virtual std::size_t run(vm_arch::basic_block*) = 0;
		
		std::size_t operator()(vm_arch::basic_block* block) { return run(block); };
	};
	
	template <typename T, typename... U>
	struct multiple_general_passes : general_pass_interface {
		std::size_t run(vm_arch::basic_block* first_block) override {
			T{ }.run(first_block);
			multiple_general_passes<U...>{ }.run(first_block);

			return 0;
		}
	};
}