#pragma once

#include "./ast/ir/abstract_visitor_pattern.hpp"
#include "./ast/ir/node.hpp"
#include "deserializer_helper.hpp"
#include "deserializer_context.hpp"

namespace deobf::ironbrew_devirtualizer::symbolic_execution::deserializer {
	using namespace ast;

	struct deserializer_visitor;

	struct symoblic_deserializer final {
		void run();

		explicit symoblic_deserializer(deserializer_context* const deserializer_ctx) :
			deserializer_ctx(deserializer_ctx)
		{ }

	private:

		friend struct deserializer_visitor;

		deserializer_context* const deserializer_ctx;
	};
}