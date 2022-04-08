#pragma once
#include "vm_arch/vanilla_proto.hpp"
#include "utilities/bytecode_buffer/bytecode_buffer.hpp"

namespace deobf::vanilla_serializer {
	// serializes a lifted vanilla proto into lua 5.1.4 format
	using utilities::bytecode_buffer::bytecode_buffer;

	struct vanilla_serializer final {
		void serialize();

		const bytecode_buffer& get_stream() const {
			return *bytecode;
		}

		explicit vanilla_serializer(vm_arch::vanilla_proto& proto) : proto(proto), bytecode(new bytecode_buffer) { };
	private:
		void serialize_header();
		void serialize_proto(vm_arch::vanilla_proto& proto, bool is_top);

		static constexpr std::size_t magic_signature = 0x61754C1B; // precompiled code header (as big endian Aka huge indian)

		std::unique_ptr<bytecode_buffer> bytecode;
		vm_arch::vanilla_proto& proto;
	};
}