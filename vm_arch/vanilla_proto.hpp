#pragma once
#include "vanilla_instruction.hpp"
#include "constant.hpp"

namespace deobf::vm_arch {
	struct vanilla_proto final {
		std::string chunk_name = "deobf_chunk";

		std::vector<std::unique_ptr<vanilla_instruction>> code;
		std::vector<std::shared_ptr<constant>> constants;
		std::vector<std::size_t> line_info;
		std::vector<std::unique_ptr<vanilla_proto>> protos;

		std::uint8_t is_vararg;
		std::size_t max_stack_size;
		std::size_t num_upvalues;
		std::size_t num_parameters;
		
		std::size_t line_defined;
		std::size_t last_line_defined;

		explicit vanilla_proto() :
			is_vararg(2),
			max_stack_size(250),
			num_upvalues(0),
			num_parameters(0),
			line_defined(1),
			last_line_defined(1)
		{ };

		virtual ~vanilla_proto() = default;
	};
};