#pragma once
#include <array>
//#include <forward_list>

#include "./ast/ir/statement.hpp"

#include "deserializer_enums.hpp"

namespace deobf::ironbrew_devirtualizer::symbolic_execution::deserializer {
	using namespace ast;

	struct deserializer_context final {
		ir::statement::block* const root;

		//std::array<process_order, max_process_order> chunk_order{ };
		std::vector<deserializer_enums::process_order> chunk_order{ };

		std::map<const std::uint8_t, deserializer_enums::constant_type> constant_order_mapping{ };

		/*struct {
			constant_type string_order, boolean_order, number_order;
		};*/

		explicit deserializer_context(ir::statement::block* const root) :
			root(root)
		{
			chunk_order.reserve(deserializer_enums::max_process_order);
		}
	};
}