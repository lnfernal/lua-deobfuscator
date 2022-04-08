#include "instruction_translator.hpp"

namespace deobf::ironbrew_devirtualizer::vanilla_lifter {
	std::unique_ptr<vm_arch::vanilla_instruction> instruction_translator::convert_instruction(vm_arch::instruction* original_instruction, std::uint32_t pc) {
		auto new_instruction = std::make_unique<vm_arch::vanilla_instruction>();
		
		//if (original_instruction->op == vm_arch::opcode::op_invalid)
		//	throw std::runtime_error("invalid opcode");

		auto new_opcode = read_single_data_opcode_to_vanilla(original_instruction->op);

		new_instruction->line_defined = original_instruction->line_defined;
		new_instruction->opcode = new_opcode;
		new_instruction->a = original_instruction->a;
		new_instruction->type = vm_arch::vanilla_instruction_type::a;

		switch (original_instruction->type) {
			/*case vm_arch::instruction_type::ab: {
				new_instruction->type = vm_arch::vanilla_instruction_type::ab;
				new_instruction->b = original_instruction->b;
				break;
			}*/
			case vm_arch::instruction_type::abc: { // SET ARG B & SET ARG C
				new_instruction->type = vm_arch::vanilla_instruction_type::abc;
				new_instruction->b = original_instruction->b;
				new_instruction->c = original_instruction->c;
				break;
			}
			case vm_arch::instruction_type::abx: { // SET ARG BX
				new_instruction->type = vm_arch::vanilla_instruction_type::abx;
				new_instruction->bx = original_instruction->bx;
				break;
			}
			case vm_arch::instruction_type::asbx: { // SET ARG SBX
				new_instruction->type = vm_arch::vanilla_instruction_type::asbx;
				new_instruction->sbx = original_instruction->sbx;
				break;
			}
			case vm_arch::instruction_type::asbxc: { // AC & SBX, SET ARG SBX & SET ARG C (WILL BE DISCARDED LATER)
				original_instruction->print();
				new_instruction->sbx = original_instruction->sbx;
				new_instruction->c = original_instruction->c;
				break;
			}
			default:
				throw std::runtime_error("[ironbrew_deobfuscator/final_step/vanilla_lifter/instruction_translator]: invalid instruction type");
		}

		// transpile instructions (could be done shorter and way better, but i prefer like that)

		// TODO : new_instruction->(b || c) += 0xff; could be done efficencfly with encode_kst_index(idx + 1)

		// kst_start = (1 << (MAX_B - 1)) - 1 = 256 - 1 = 0xff

		switch (original_instruction->op) {
			case vm_arch::opcode::op_setlist: {
				new_instruction->b -= original_instruction->a;
				break;
			}
			case vm_arch::opcode::op_setlist2: {
				// todo (rare case altho we might hit block 512 on big tables)
				new_instruction->c = 0;
				break;
			}
			case vm_arch::opcode::op_closure: { // upvalues are resolved by operand C at runtime in ironbrew, lua Vm already resolves it, null out C
				new_instruction->type = vm_arch::vanilla_instruction_type::abx;

				new_instruction->sbx = 0;
				new_instruction->c = 0;
				new_instruction->bx = original_instruction->bx;

				break;
			}
			case vm_arch::opcode::op_tforloop: {
				new_instruction->type = vm_arch::vanilla_instruction_type::ac;

				new_instruction->b = 0;
				break;
			}
			case vm_arch::opcode::op_settable1: {
				new_instruction->b += 0xff;
				break;
			}
			case vm_arch::opcode::op_settable2: {
				new_instruction->c += 0xff;
				break;
			}
			case vm_arch::opcode::op_settable3: {
				new_instruction->b += 0xff;
				new_instruction->c += 0xff;
				break;
			}
			// arthimethic
			case vm_arch::opcode::op_add1: {
				new_instruction->b += 0xff;
				break;
			}
			case vm_arch::opcode::op_add2: {
				new_instruction->c += 0xff;
				break;
			}
			case vm_arch::opcode::op_add3: {
				new_instruction->b += 0xff;
				new_instruction->c += 0xff;
				break;
			}

			case vm_arch::opcode::op_sub1: {
				new_instruction->b += 0xff;
				break;
			}
			case vm_arch::opcode::op_sub2: {
				new_instruction->c += 0xff;
				break;
			}
			case vm_arch::opcode::op_sub3: {
				new_instruction->b += 0xff;
				new_instruction->c += 0xff;
				break;
			}

			case vm_arch::opcode::op_mul1: {
				new_instruction->b += 0xff;
				break;
			}
			case vm_arch::opcode::op_mul2: {
				new_instruction->c += 0xff;
				break;
			}
			case vm_arch::opcode::op_mul3: {
				new_instruction->b += 0xff;
				new_instruction->c += 0xff;
				break;
			}

			case vm_arch::opcode::op_div1: {
				new_instruction->b += 0xff;
				break;
			}
			case vm_arch::opcode::op_div2: {
				new_instruction->c += 0xff;
				break;
			}
			case vm_arch::opcode::op_div3: {
				new_instruction->b += 0xff;
				new_instruction->c += 0xff;
				break;
			}

			case vm_arch::opcode::op_mod1: {
				new_instruction->b += 0xff;
				break;
			}
			case vm_arch::opcode::op_mod2: {
				new_instruction->c += 0xff;
				break;
			}
			case vm_arch::opcode::op_mod3: {
				new_instruction->b += 0xff;
				new_instruction->c += 0xff;
				break;
			}

			case vm_arch::opcode::op_pow1: {
				new_instruction->b += 0xff;
				break;
			}
			case vm_arch::opcode::op_pow2: {
				new_instruction->c += 0xff;
				break;
			}
			case vm_arch::opcode::op_pow3: {
				new_instruction->b += 0xff;
				new_instruction->c += 0xff;
				break;
			}

			// arthimethic end...

			case vm_arch::opcode::op_loadbool1: {
				// flip loadbool flag
				new_instruction->c = 1;
				break;
			}

			case vm_arch::opcode::op_gettable1: { // gettablekst
				new_instruction->c += 0xff;
				break;
			}

			// ironbrew constant table is rebased to 1, doing the same to all instructions that perform kst
			case vm_arch::opcode::op_getglobal: {
				new_instruction->bx--; //--;
				break;
			}
			case vm_arch::opcode::op_setglobal: {
				new_instruction->bx--; //--;
				break;
			}
			case vm_arch::opcode::op_loadk: {
				new_instruction->bx--; //--;
				break;
			}
			case vm_arch::opcode::op_return: { // R(A) ... R(A + B + 2)
				new_instruction->b += 2;
				break;
			}
			case vm_arch::opcode::op_vararg: { // R(A) ... R(A + B - 1)
				new_instruction->b -= original_instruction->a - 1;
				break;
			}
			case vm_arch::opcode::op_tailcall: {
				new_instruction->b -= original_instruction->a - 1;
				break;
			}
			case vm_arch::opcode::op_self1: {
				new_instruction->c += 0xff;
				break;
			}
			case vm_arch::opcode::op_forprep: {
				new_instruction->sbx -= pc + 2;
				break;
			}
			case vm_arch::opcode::op_forloop: {
				new_instruction->sbx -= pc + 1;
				break;
			}

			// logic/relational
			case vm_arch::opcode::op_eq: { // A B C
				new_instruction->type = vm_arch::vanilla_instruction_type::abc;

				new_instruction->sbx = 0;
				new_instruction->b = original_instruction->a;
				new_instruction->a = 0;
				break;
			}
			case vm_arch::opcode::op_eq1: { // A B C
				new_instruction->type = vm_arch::vanilla_instruction_type::abc;

				new_instruction->sbx = 0;
				new_instruction->b = original_instruction->a;
				new_instruction->a = 0;

				new_instruction->b += 0xff;
				break;
			}
			case vm_arch::opcode::op_eq2: { // A B C
				new_instruction->type = vm_arch::vanilla_instruction_type::abc;

				new_instruction->sbx = 0;
				new_instruction->b = original_instruction->a;
				new_instruction->a = 0;
				
				new_instruction->c += 0xff;
				break;
			}
			case vm_arch::opcode::op_eq3: { // A B C
				new_instruction->type = vm_arch::vanilla_instruction_type::abc;

				new_instruction->sbx = 0;
				new_instruction->b = original_instruction->a;
				new_instruction->a = 0;
				
				new_instruction->b += 0xff;
				new_instruction->c += 0xff;
				break;
			}

			case vm_arch::opcode::op_ne: { // A B C
				new_instruction->type = vm_arch::vanilla_instruction_type::abc;

				new_instruction->sbx = 0;
				new_instruction->b = original_instruction->a;
				new_instruction->a = 1;
				break;
			}
			case vm_arch::opcode::op_ne1: { // A B C
				new_instruction->type = vm_arch::vanilla_instruction_type::abc;

				new_instruction->sbx = 0;
				new_instruction->b = original_instruction->a;
				new_instruction->a = 1;

				new_instruction->b += 0xff;
				break;
			}
			case vm_arch::opcode::op_ne2: { // A B C
				new_instruction->type = vm_arch::vanilla_instruction_type::abc;
				new_instruction->sbx = 0;
				new_instruction->b = original_instruction->a;
				new_instruction->a = 1;

				new_instruction->c += 0xff;
				break;
			}
			case vm_arch::opcode::op_ne3: { // A B C
				new_instruction->type = vm_arch::vanilla_instruction_type::abc;

				new_instruction->sbx = 0;
				new_instruction->b = original_instruction->a;
				new_instruction->a = 1;

				new_instruction->b += 0xff;
				new_instruction->c += 0xff;
				break;
			}

			case vm_arch::opcode::op_lt: { // A B C
				new_instruction->type = vm_arch::vanilla_instruction_type::abc;

				new_instruction->sbx = 0;
				new_instruction->b = original_instruction->a;
				new_instruction->a = 0;
				break;
			}
			case vm_arch::opcode::op_lt1: { // A B C
				new_instruction->type = vm_arch::vanilla_instruction_type::abc;

				new_instruction->sbx = 0;
				new_instruction->b = original_instruction->a;
				new_instruction->a = 0;

				new_instruction->b += 0xff;
				break;
			}
			case vm_arch::opcode::op_lt2: { // A B C
				new_instruction->type = vm_arch::vanilla_instruction_type::abc;

				new_instruction->sbx = 0;
				new_instruction->b = original_instruction->a;
				new_instruction->a = 0;

				new_instruction->c += 0xff;
				break;
			}
			case vm_arch::opcode::op_lt3: { // A B C
				new_instruction->type = vm_arch::vanilla_instruction_type::abc;

				new_instruction->sbx = 0;
				new_instruction->b = original_instruction->a;
				new_instruction->a = 0;

				new_instruction->b += 0xff;
				new_instruction->c += 0xff;
				break;
			}

			case vm_arch::opcode::op_ge: { // A B C
				new_instruction->type = vm_arch::vanilla_instruction_type::abc;

				new_instruction->sbx = 0;
				new_instruction->b = original_instruction->a;
				new_instruction->a = 1;
				break;
			}
			case vm_arch::opcode::op_ge1: { // A B C
				new_instruction->type = vm_arch::vanilla_instruction_type::abc;

				new_instruction->sbx = 0;
				new_instruction->b = original_instruction->a;
				new_instruction->a = 1;

				new_instruction->b += 0xff;
				break;
			}
			case vm_arch::opcode::op_ge2: { // A B C
				new_instruction->type = vm_arch::vanilla_instruction_type::abc;

				new_instruction->sbx = 0;
				new_instruction->b = original_instruction->a;
				new_instruction->a = 1;

				new_instruction->c += 0xff;
				break;
			}
			case vm_arch::opcode::op_ge3: { // A B C
				new_instruction->type = vm_arch::vanilla_instruction_type::abc;

				new_instruction->sbx = 0;
				new_instruction->b = original_instruction->a;
				new_instruction->a = 1;

				new_instruction->b += 0xff;
				new_instruction->c += 0xff;
				break;
			}


			case vm_arch::opcode::op_le: { // A B C
				new_instruction->type = vm_arch::vanilla_instruction_type::abc;

				new_instruction->sbx = 0;
				new_instruction->b = original_instruction->a;
				new_instruction->a = 0;
				break;
			}
			case vm_arch::opcode::op_le1: { // A B C
				new_instruction->type = vm_arch::vanilla_instruction_type::abc;

				new_instruction->sbx = 0;
				new_instruction->b = original_instruction->a;
				new_instruction->a = 0;

				new_instruction->b += 0xff;
				break;
			}
			case vm_arch::opcode::op_le2: { // A B C
				new_instruction->type = vm_arch::vanilla_instruction_type::abc;

				new_instruction->sbx = 0;
				new_instruction->b = original_instruction->a;
				new_instruction->a = 0;

				new_instruction->c += 0xff;
				break;
			}
			case vm_arch::opcode::op_le3: { // A B C
				new_instruction->type = vm_arch::vanilla_instruction_type::abc;

				new_instruction->sbx = 0;
				new_instruction->b = original_instruction->a;
				new_instruction->a = 0;

				new_instruction->b += 0xff;
				new_instruction->c += 0xff;
				break;
			}

			case vm_arch::opcode::op_gt: { // A B C
				new_instruction->type = vm_arch::vanilla_instruction_type::abc;

				new_instruction->sbx = 0;
				new_instruction->b = original_instruction->a;
				new_instruction->a = 1;
				break;
			}
			case vm_arch::opcode::op_gt1: { // A B C
				new_instruction->type = vm_arch::vanilla_instruction_type::abc;

				new_instruction->sbx = 0;
				new_instruction->b = original_instruction->a;
				new_instruction->a = 1;

				new_instruction->b += 0xff;
				break;
			}
			case vm_arch::opcode::op_gt2: { // A B C
				new_instruction->type = vm_arch::vanilla_instruction_type::abc;

				new_instruction->sbx = 0;
				new_instruction->b = original_instruction->a;
				new_instruction->a = 1;

				new_instruction->c += 0xff;
				break;
			}
			case vm_arch::opcode::op_gt3: { // A B C
				new_instruction->type = vm_arch::vanilla_instruction_type::abc;

				new_instruction->sbx = 0;
				new_instruction->b = original_instruction->a;
				new_instruction->a = 1;

				new_instruction->b += 0xff;
				new_instruction->c += 0xff;
				break;
			}

			case vm_arch::opcode::op_test: {
				new_instruction->type = vm_arch::vanilla_instruction_type::ac;

				new_instruction->sbx = 0;
				new_instruction->b = 0;
				new_instruction->c = 0;
				break;
			}
			case vm_arch::opcode::op_test1: {
				new_instruction->type = vm_arch::vanilla_instruction_type::ac;

				new_instruction->sbx = 0;
				new_instruction->b = 0;
				new_instruction->c = 1;
				break;
			}

			case vm_arch::opcode::op_testset: {
				new_instruction->type = vm_arch::vanilla_instruction_type::abc;

				new_instruction->b = original_instruction->c;
				new_instruction->c = 0;
				break;
			}
			case vm_arch::opcode::op_testset1: {
				new_instruction->type = vm_arch::vanilla_instruction_type::abc;

				new_instruction->b = original_instruction->c;
				new_instruction->c = 1;
				break;
			}
			// target branches are patched on static chunk analysis, only encode 18 bit REL PC (relative pc) instead of BX
			case vm_arch::opcode::op_jmp: {
				// is bx on ironbrew
				new_instruction->a = 0;
				new_instruction->bx = 0;
				new_instruction->sbx = original_instruction->sbx - pc - 1;
				break;
			}
			
			// call modes...
			case vm_arch::opcode::op_call: {
				new_instruction->b -= original_instruction->a - 1;
				new_instruction->c -= original_instruction->a - 2;
				break;
			}
			case vm_arch::opcode::op_call1:
			case vm_arch::opcode::op_call2:
			case vm_arch::opcode::op_call3: {
				new_instruction->c -= original_instruction->a - 2;
				break;
			}
			case vm_arch::opcode::op_call4:
			case vm_arch::opcode::op_call5: 
			case vm_arch::opcode::op_call6: {
				new_instruction->b -= original_instruction->a - 1;
				break;
			}
			case vm_arch::opcode::op_call12: {
				new_instruction->b -= original_instruction->a - 1;
				break;
			}
			default:
				break;
		}

		return std::move(new_instruction);
	}
}