#include "vanilla_serializer.hpp"
#include <variant>

namespace deobf::vanilla_serializer {
	
	template<typename ... Ts>
	struct overloaded : Ts ... {
		using Ts::operator() ...;
	};
	template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

	void vanilla_serializer::serialize_proto(vm_arch::vanilla_proto& proto, bool is_top) {
		bytecode->write_string(is_top ? proto.chunk_name : "");

		//(is_top ? bytecode->write_string(proto.chunk_name) : bytecode->write<std::uint32_t>(0));

		// serialize proto header
		bytecode->write<std::uint32_t>(proto.line_defined);
		bytecode->write<std::uint32_t>(proto.last_line_defined);
		bytecode->write<std::uint8_t>(proto.num_upvalues);
		bytecode->write<std::uint8_t>(proto.num_parameters);
		bytecode->write<std::uint8_t>(proto.is_vararg);
		bytecode->write<std::uint8_t>(proto.max_stack_size);

		// encode and serialize instructions
		bytecode->write<std::uint32_t>(proto.code.size());

		for (auto& instruction : proto.code) {
			bytecode->write<std::uint32_t>(instruction->get_value());
		}

		// serialize constants
		bytecode->write<std::uint32_t>(proto.constants.size());
		for (auto& constant : proto.constants) {
			std::visit(overloaded {
				[this](std::nullptr_t) {
					bytecode->write<std::uint8_t>(0); // LUA_TNIL
				},
				[this](double value) {
					bytecode->write<std::uint8_t>(3); // LUA_TNUMBER

					bytecode->write<double>(value);
				},
				[this](const std::string& value) {
					bytecode->write<std::uint8_t>(4); // LUA_TSTRING
					
					bytecode->write_string(value);
				},
				[this](bool value) {
					bytecode->write<std::uint8_t>(1); // LUA_TBOOLEAN

					bytecode->write<std::uint8_t>(value ? 1 : 0);
				}
			}, constant->value);
		}

		// serialize protos
		std::cout << "NUM:" << proto.protos.size() << std::endl;
		bytecode->write<std::uint32_t>(proto.protos.size());
		for (auto& proto : proto.protos) {
			serialize_proto(*proto, false);
		}

		// - debug information -

		// serialize lineinfo (stripped unless lineinfo is preserved)
		/*bytecode->write<std::uint32_t>(proto.line_info.size());
		for (auto& line : proto.line_info) {
			bytecode->write<std::uint32_t>(line);
		}*/
		/*if (proto.line_info.size() != 0) { // has lineinfo?
			bytecode->write<std::uint32_t>(proto.code.size());
			for (auto& code : proto.code) {
				bytecode->write<std::uint32_t>(code->line_defined);
			}
		}*/

		bytecode->write<std::uint32_t>(0);

		// TODO FIX LINEINFO

		bytecode->write<std::uint32_t>(0);

		// serialize locvars (stripped)
		bytecode->write<std::uint32_t>(0);

		// seriaize upvals (stripped)
		bytecode->write<std::uint32_t>(0);
	}

	void vanilla_serializer::serialize_header() {
		// write precompiled chunk signature
		bytecode->write<std::uint32_t>(magic_signature);
		
		// lua version (5.1.4)
		bytecode->write<std::uint8_t>(0x51);

		// is format version ?
		bytecode->write<std::uint8_t>(0);

		// big endian
		bytecode->write<std::uint8_t>(1);

		// machine int size
		bytecode->write<std::uint8_t>(4);
		
		// machine size_t size
		bytecode->write<std::uint8_t>(4);
		
		// instruction size
		bytecode->write<std::uint8_t>(4);
		
		// lua_Number Size
		bytecode->write<std::uint8_t>(8);

		// integral flag
		bytecode->write<std::uint8_t>(0);
	}

	void vanilla_serializer::serialize() {
		serialize_header();

		serialize_proto(proto, true);
	}
}