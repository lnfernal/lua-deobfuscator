#pragma once
#include "instruction.hpp"
#include "constant.hpp"
#include "opcode.hpp"

#include <vector>
#include <memory>
#include <iostream>

namespace deobf::vm_arch {
	struct proto {
		std::vector<std::unique_ptr<instruction>> instructions;
		std::vector<std::unique_ptr<proto>> protos;
		std::vector<std::int32_t> lineinfo; // load vector we wont need to propagate them into each insn
		std::vector<std::shared_ptr<constant>> constants;

		std::int8_t num_params;
		std::int16_t nups;
		std::size_t max_stack_size;
		std::int8_t is_vararg;

		bool is_chunk_cflow() const {
			return (instructions.size() > 0 && instructions.at(0)->op == vm_arch::opcode::op_newstack);
		}

		std::string print_chunk() const;

		virtual ~proto() = default;
	};
}