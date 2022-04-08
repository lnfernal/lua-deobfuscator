#include "block_toposorter.hpp"

namespace deobf::ironbrew_devirtualizer::static_chunk_analysis::block_toposorter {
	std::size_t block_toposorter::run_toposort() {
		std::size_t num_unreachable_blocks = 0;

		std::unordered_set<vm_arch::basic_block*> visited_blocks;
		std::queue<std::weak_ptr<vm_arch::basic_block>> work_queue;

		// collect basic blocks
		//collect_basic_blocks();

		// enqueue initial block
		work_queue.emplace(initial_block);
		visited_blocks.emplace(initial_block.lock().get());

		// run a BFS cycle, O(v + e)
		while (!work_queue.empty()) { // visit topmost element in queue
			auto result = work_queue.front().lock();
			work_queue.pop();

			sorted_cfg.emplace_back(result);

			// enqueue sucessors aswell
			for (auto& sucessor : result->block_sucessors) {
				if (visited_blocks.emplace(sucessor.lock().get()).second) {
					work_queue.emplace(sucessor);
				}
			}
		}

		{
			std::size_t current_block_id = 0;
			auto current_block{ initial_block };
			while (!current_block.expired()) {
				auto result = current_block.lock();
				if (!visited_blocks.count(result.get())) {
					std::cout << "UNREACHABLE\n";
					++num_unreachable_blocks;
				}
				else {
					result->block_id = ++current_block_id;
				}

				current_block = result->next_block;
			}
		}

		return num_unreachable_blocks;
	}
}
