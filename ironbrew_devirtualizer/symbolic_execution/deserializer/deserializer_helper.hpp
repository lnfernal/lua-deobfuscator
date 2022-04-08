#pragma once

#include "../../../vm_arch/proto.hpp"

#include <string_view>
#include <cstdlib>
#include <unordered_set>
#include <iostream>
#include <unordered_map>
#include <algorithm>

#include <sstream>

#include "deserializer_context.hpp"

namespace deobf::ironbrew_devirtualizer::symbolic_execution::deserializer {
	struct deserializer_emulator_main;

	// pov reinterpret_cast<ironbrew_proto*)(stream) boom
	class deserializer_helper final {
	private:
		friend struct deserializer::deserializer_emulator_main;

		static constexpr std::size_t short_size = sizeof std::int16_t;
		static constexpr std::size_t sizet_size = sizeof std::int32_t;

		const unsigned char vm_xor_key;

		const inline bool get_bits(std::size_t number, std::size_t i) const;
		const inline std::size_t get_bits(std::size_t number, std::size_t i, std::size_t j) const;

		//static const std::string decompress_vm_string(std::string_view vm_string);

		std::istringstream managed_deserializer_string;
	public:
		deserializer_helper(deserializer_helper&&) = default;

		explicit deserializer_helper(const std::string& vm_string, const unsigned char xor_key) : managed_deserializer_string{ vm_string }, vm_xor_key(xor_key) { };


		// utilities
		template <class T>
		const T get_bits() {
			T result = 0;

			auto result_type = reinterpret_cast<char*>(&result);
			managed_deserializer_string.read(result_type, sizeof(T));

			for (auto i = 0; i < sizeof(T); ++i)
				result_type[i] ^= vm_xor_key;

			return result;
		}

		template <>
		const std::int8_t get_bits<std::int8_t>() {
			return managed_deserializer_string.get() ^ vm_xor_key;
		}

		// ...
		const double get_float();
		const std::string get_string(std::size_t length);
		const std::string get_string();

		//[[deprecated]] std::unique_ptr<vm_arch::proto> deserialize(deserializer_context* ctx);
	};
}