#pragma once

#include <unordered_set>
#include <queue>
#include <memory>

#include "vm_arch/basic_block.hpp"

namespace deobf::ironbrew_devirtualizer::static_chunk_analysis::block_toposorter {

	// time complexity, same as the SCC algorithm one : O(v + e), ( we don't have negative edge/node's so O(|v| + |e|) is redudrant )
	// space complexity O(v) where v is the amount of vertex (Basic Blocks)

	// sorts the Control Flow DAG (if has any toposort) so we make sure that our blocks have a predecessor before their next block
	// and most important we eliminate unreachable blocks (unvisited in BFS)
	// so will never have unreachable blocks (no sucessors) in our vector aka syntactic garbage

	// populates bblock's ID aswell (important!!!)

	struct [[deprecated]] block_toposorter final {
		std::weak_ptr<vm_arch::basic_block> initial_block;

		//std::unordered_set<vm_arch::basic_block*> visited_blocks;
		//std::queue<std::weak_ptr<vm_arch::basic_block>> work_queue;

		//std::vector<std::shared_ptr<vm_arch::basic_block>> basic_blocks;
		std::vector<std::shared_ptr<vm_arch::basic_block>> sorted_cfg;
		
		//void collect_basic_blocks();
		std::size_t run_toposort(); // returns number of unreachable blocks

		explicit block_toposorter(std::weak_ptr<vm_arch::basic_block> initial_block) : initial_block(initial_block) { };
	};
}