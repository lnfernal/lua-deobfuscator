#pragma once
#include "vm_arch/control_flow_graph.hpp"
#include "vm_arch/basic_block.hpp"

#include "general_pass_interface.hpp"
#include <queue>

namespace deobf::ironbrew_devirtualizer::static_chunk_analysis::optimizations {
	using namespace vm_arch;

	namespace regkst_propagation_pass {
		struct regkst_propagation_pass final : general_pass_interface {
			std::size_t run(vm_arch::basic_block* first_block) override;
		};
	}
}