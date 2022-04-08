#include "basic_block.hpp"

namespace deobf::vm_arch {
	void basic_block::remove_sucessor(std::size_t position) {
		if (position > block_sucessors.size())
			throw std::out_of_range("position to remove basic_block's sucessor is out of range");

		if (auto result = block_sucessors.at(position).lock()) {
			// lazy to use stl for this
			for (auto it = result->block_predecessors.begin(); it != result->block_predecessors.end(); ++it) {
				if (it->lock().get() == this) {
					result->block_predecessors.erase(it);
					break;
				}
			}

			block_sucessors.erase(block_sucessors.begin() + position);
		}
	}
}