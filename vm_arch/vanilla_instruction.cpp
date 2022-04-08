#include "vanilla_instruction.hpp"

namespace deobf::vm_arch {
	std::uint32_t vanilla_instruction::get_value() const {
		std::uint32_t result = 0;

		result |= static_cast<std::uint8_t>(opcode);

		switch (type) {
			case vm_arch::vanilla_instruction_type::a: {
				result |= ENCODE_OPERAND(a, SIZE_A, POS_A);
				break;
			}
			case vm_arch::vanilla_instruction_type::ab: {
				result |= ENCODE_OPERAND(a, SIZE_A, POS_A);
				result |= ENCODE_OPERAND(b, SIZE_B, POS_B);
				break;
			}
			case vm_arch::vanilla_instruction_type::ac: {
				result |= ENCODE_OPERAND(a, SIZE_A, POS_A);
				result |= ENCODE_OPERAND(c, SIZE_C, POS_C);
				break;
			}
			case vm_arch::vanilla_instruction_type::abc: {
				result |= ENCODE_OPERAND(a, SIZE_A, POS_A);
				result |= ENCODE_OPERAND(b, SIZE_B, POS_B);
				result |= ENCODE_OPERAND(c, SIZE_C, POS_C);
				break;
			}
			case vm_arch::vanilla_instruction_type::sbx: {
				result |= ENCODE_OPERAND(sbx + MAXARG_sBx, SIZE_Bx, POS_Bx);
				break;
			}
			case vm_arch::vanilla_instruction_type::abx: {
				result |= ENCODE_OPERAND(a, SIZE_A, POS_A);
				result |= ENCODE_OPERAND(bx, SIZE_Bx, POS_Bx);
				break;
			}
			case vm_arch::vanilla_instruction_type::asbx: {
				result |= ENCODE_OPERAND(a, SIZE_A, POS_A);
				result |= ENCODE_OPERAND(sbx + MAXARG_sBx, SIZE_Bx, POS_Bx);
				break;
			}
			default:
				throw std::runtime_error("unhandled instruction type in vanilla");
		}

		return result;
	}
}