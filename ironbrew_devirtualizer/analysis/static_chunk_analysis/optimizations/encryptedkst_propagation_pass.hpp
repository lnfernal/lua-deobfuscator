#pragma once
#include "vm_arch/control_flow_graph.hpp"
#include "vm_arch/basic_block.hpp"
#include "general_pass_interface.hpp"
#include "vm_arch/proto.hpp"

#include <queue>

namespace deobf::ironbrew_devirtualizer::static_chunk_analysis::optimizations {
	using namespace vm_arch;

	namespace encryptedkst_propagation_pass {
		struct encryptedkst_propagation_pass final : general_pass_interface {
			vm_arch::proto* chunk;

			explicit encryptedkst_propagation_pass(vm_arch::proto* chunk) : chunk(chunk) { };
			std::size_t run(vm_arch::basic_block* first_block) override;
		};
	}
}