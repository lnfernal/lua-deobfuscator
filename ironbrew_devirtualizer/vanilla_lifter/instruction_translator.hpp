#pragma once

#include <memory>
#include <unordered_map>

#include "vm_arch/vanilla_instruction.hpp"
#include "vm_arch/instruction.hpp"
#include "single_opcode_mapping.hpp"

namespace deobf::ironbrew_devirtualizer::vanilla_lifter {
	class instruction_translator {
		[[deprecated]] static inline vm_arch::instruction_type get_op_mode(vm_arch::instruction_type old) {
			switch (old) {
				case vm_arch::instruction_type::asbxc: {
					return vm_arch::instruction_type::abc; // usualy AC too but uh
				}
				default:
					break;
			}

			return old;
		}

		static constexpr auto encode_kst_index(std::size_t idx) {
			return (idx | 0xff);
		}

		// converts a opcode from our ISA back to vanilla if possible
		// couldve got normalized opcode with aux funcs -> convert to vanilla from enum (altho there are side effects aswell)
		static inline vm_arch::vanilla_opcode read_single_data_opcode_to_vanilla(vm_arch::opcode old) {
			if (auto result = vanilla_opcode_mapping.find(old); result != vanilla_opcode_mapping.cend())
				return result->second;

			throw std::runtime_error("[ironbrew_deobfuscator/final_step/vanilla_lifter/instruction_translator]: failed to read single data opcode to vanilla");
		}

	public:
		static std::unique_ptr<vm_arch::vanilla_instruction> convert_instruction(vm_arch::instruction*, std::uint32_t);
	};
}