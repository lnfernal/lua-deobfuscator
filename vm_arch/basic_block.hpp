#pragma once

#include <variant>
#include <string>
#include <memory>
#include <functional>
#include <map>

#include "instruction.hpp"


namespace deobf::vm_arch {
	enum class block_iterate_direction {
		none,
		next,
		target,
	};

	// todo basic block iterator

	struct basic_block final {
		bool is_terminator = false; // ends with a return, terminator block

		std::vector<std::reference_wrapper<instruction>> instructions;

		std::map<std::size_t, std::shared_ptr<basic_block>> references;

		std::vector<std::weak_ptr<basic_block>> block_predecessors;
		std::vector<std::weak_ptr<basic_block>> block_sucessors;

		// todo dominator tree (easier detections for CFF)

		// basic block's successor/preccedor (assuming there is a single one)
		std::shared_ptr<basic_block> next_block;
		std::shared_ptr<basic_block> target_block;

		void remove_sucessor(std::size_t position = 0);

		std::size_t block_id = 0; // will be populated on toposort

		virtual ~basic_block() = default;
	};
}