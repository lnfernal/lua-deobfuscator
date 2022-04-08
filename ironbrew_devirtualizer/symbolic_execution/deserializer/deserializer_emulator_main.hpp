#pragma once

#include "./ast/ir/abstract_visitor_pattern.hpp"
#include "./ast/ir/node.hpp"
#include "deserializer_helper.hpp"
#include "symbolic_deserializer.hpp"
#include "deserializer_context.hpp"
#include "compression_utilities.hpp"

#include <mutex>

namespace deobf::ironbrew_devirtualizer::symbolic_execution::deserializer {
	using namespace ast;

	struct deserializer_emulator_main final {
		std::unique_ptr<vm_arch::proto> deserialize();

		explicit deserializer_emulator_main(ir::statement::statement* const root, std::string_view vm_string, const unsigned char xor_key) :
			deserializer_ctx(std::make_unique<deserializer_context>(static_cast<ir::statement::block* const>(root)))
		{
			const bool is_non_compressed = (vm_string.front() == '\\'); // non-compressed strings always start with an escape sequence

			deserializer_helper_object = std::make_unique<deserializer::deserializer_helper>(is_non_compressed ?
				compression_utilities::unescape_vm_string(vm_string) :
				compression_utilities::decompress_vm_string(vm_string), xor_key);

			symoblic_deserializer{ deserializer_ctx.get() }.run(); // run symbex deserializer to populate context with information
		}

	private:

		std::unique_ptr<deserializer::deserializer_helper> deserializer_helper_object; // composition over inheritance

		std::unique_ptr<deserializer_context> deserializer_ctx;
	};
}