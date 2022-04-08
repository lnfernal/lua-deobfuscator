#pragma once

#include <cstdint>
#include <cstddef>


namespace deobf::ironbrew_devirtualizer::symbolic_execution::deserializer::deserializer_enums {
	enum class process_order {
		instructions,
		protos,
		lineinfo,
		constants,
		parameters,
	};

	constexpr std::size_t max_process_order = 5;

	enum class constant_type : const std::uint8_t {
		string,
		number,
		boolean,
	};

	constexpr std::size_t max_constant_order = 3;
}