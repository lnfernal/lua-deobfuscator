#pragma once
#include <unordered_map>
#include "vm_arch/opcode.hpp"

namespace deobf::ironbrew_devirtualizer::vanilla_lifter {
	// opcode mapping
	static const std::unordered_map<vm_arch::opcode, vm_arch::vanilla_opcode> vanilla_opcode_mapping{
		{ vm_arch::opcode::op_move, vm_arch::vanilla_opcode::op_move },

		{ vm_arch::opcode::op_loadk, vm_arch::vanilla_opcode::op_loadk },

		{ vm_arch::opcode::op_loadbool, vm_arch::vanilla_opcode::op_loadbool },
		{ vm_arch::opcode::op_loadbool1, vm_arch::vanilla_opcode::op_loadbool },

		{ vm_arch::opcode::op_loadnil, vm_arch::vanilla_opcode::op_loadnil },

		{ vm_arch::opcode::op_getupval, vm_arch::vanilla_opcode::op_getupval },

		{ vm_arch::opcode::op_getglobal, vm_arch::vanilla_opcode::op_getglobal },

		{ vm_arch::opcode::op_gettable, vm_arch::vanilla_opcode::op_gettable },
		{ vm_arch::opcode::op_gettable1, vm_arch::vanilla_opcode::op_gettable },

		{ vm_arch::opcode::op_setglobal, vm_arch::vanilla_opcode::op_setglobal },

		{ vm_arch::opcode::op_setupval, vm_arch::vanilla_opcode::op_setupval },

		{ vm_arch::opcode::op_settable, vm_arch::vanilla_opcode::op_settable },
		{ vm_arch::opcode::op_settable1, vm_arch::vanilla_opcode::op_settable },
		{ vm_arch::opcode::op_settable2, vm_arch::vanilla_opcode::op_settable },
		{ vm_arch::opcode::op_settable3, vm_arch::vanilla_opcode::op_settable },

		{ vm_arch::opcode::op_newtable, vm_arch::vanilla_opcode::op_newtable },

		{ vm_arch::opcode::op_self, vm_arch::vanilla_opcode::op_self },
		{ vm_arch::opcode::op_self1, vm_arch::vanilla_opcode::op_self },

		{ vm_arch::opcode::op_add, vm_arch::vanilla_opcode::op_add },
		{ vm_arch::opcode::op_add1, vm_arch::vanilla_opcode::op_add },
		{ vm_arch::opcode::op_add2, vm_arch::vanilla_opcode::op_add },
		{ vm_arch::opcode::op_add3, vm_arch::vanilla_opcode::op_add },

		{ vm_arch::opcode::op_sub, vm_arch::vanilla_opcode::op_sub },
		{ vm_arch::opcode::op_sub1, vm_arch::vanilla_opcode::op_sub },
		{ vm_arch::opcode::op_sub2, vm_arch::vanilla_opcode::op_sub },
		{ vm_arch::opcode::op_sub3, vm_arch::vanilla_opcode::op_sub },

		{ vm_arch::opcode::op_mul, vm_arch::vanilla_opcode::op_mul },
		{ vm_arch::opcode::op_mul1, vm_arch::vanilla_opcode::op_mul },
		{ vm_arch::opcode::op_mul2, vm_arch::vanilla_opcode::op_mul },
		{ vm_arch::opcode::op_mul3, vm_arch::vanilla_opcode::op_mul },

		{ vm_arch::opcode::op_div, vm_arch::vanilla_opcode::op_div },
		{ vm_arch::opcode::op_div1, vm_arch::vanilla_opcode::op_div },
		{ vm_arch::opcode::op_div2, vm_arch::vanilla_opcode::op_div },
		{ vm_arch::opcode::op_div3, vm_arch::vanilla_opcode::op_div },

		{ vm_arch::opcode::op_mod, vm_arch::vanilla_opcode::op_mod },
		{ vm_arch::opcode::op_mod1, vm_arch::vanilla_opcode::op_mod },
		{ vm_arch::opcode::op_mod2, vm_arch::vanilla_opcode::op_mod },
		{ vm_arch::opcode::op_mod3, vm_arch::vanilla_opcode::op_mod },

		{ vm_arch::opcode::op_pow, vm_arch::vanilla_opcode::op_pow },
		{ vm_arch::opcode::op_pow1, vm_arch::vanilla_opcode::op_pow },
		{ vm_arch::opcode::op_pow2, vm_arch::vanilla_opcode::op_pow },
		{ vm_arch::opcode::op_pow3, vm_arch::vanilla_opcode::op_pow },

		{ vm_arch::opcode::op_unm, vm_arch::vanilla_opcode::op_unm },
		{ vm_arch::opcode::op_not, vm_arch::vanilla_opcode::op_not },
		{ vm_arch::opcode::op_len, vm_arch::vanilla_opcode::op_len },
		{ vm_arch::opcode::op_concat, vm_arch::vanilla_opcode::op_concat },
		{ vm_arch::opcode::op_jmp, vm_arch::vanilla_opcode::op_jmp },

			// eq & lt & le op ASBXC
		{ vm_arch::opcode::op_eq, vm_arch::vanilla_opcode::op_eq },
		{ vm_arch::opcode::op_eq1, vm_arch::vanilla_opcode::op_eq },
		{ vm_arch::opcode::op_eq2, vm_arch::vanilla_opcode::op_eq },
		{ vm_arch::opcode::op_eq3, vm_arch::vanilla_opcode::op_eq },

		{ vm_arch::opcode::op_lt, vm_arch::vanilla_opcode::op_lt },
		{ vm_arch::opcode::op_lt1, vm_arch::vanilla_opcode::op_lt },
		{ vm_arch::opcode::op_lt2, vm_arch::vanilla_opcode::op_lt },
		{ vm_arch::opcode::op_lt3, vm_arch::vanilla_opcode::op_lt },

		{ vm_arch::opcode::op_le, vm_arch::vanilla_opcode::op_le },
		{ vm_arch::opcode::op_le1, vm_arch::vanilla_opcode::op_le },
		{ vm_arch::opcode::op_le2, vm_arch::vanilla_opcode::op_le },
		{ vm_arch::opcode::op_le3, vm_arch::vanilla_opcode::op_le },

		{ vm_arch::opcode::op_test, vm_arch::vanilla_opcode::op_test },
		{ vm_arch::opcode::op_testset, vm_arch::vanilla_opcode::op_test },

			// A == 1
		{ vm_arch::opcode::op_ne, vm_arch::vanilla_opcode::op_eq },
		{ vm_arch::opcode::op_ne1, vm_arch::vanilla_opcode::op_eq },
		{ vm_arch::opcode::op_ne2, vm_arch::vanilla_opcode::op_eq },
		{ vm_arch::opcode::op_ne3, vm_arch::vanilla_opcode::op_eq },

		{ vm_arch::opcode::op_ge, vm_arch::vanilla_opcode::op_lt },
		{ vm_arch::opcode::op_ge1, vm_arch::vanilla_opcode::op_lt },
		{ vm_arch::opcode::op_ge2, vm_arch::vanilla_opcode::op_lt },
		{ vm_arch::opcode::op_ge3, vm_arch::vanilla_opcode::op_lt },

		{ vm_arch::opcode::op_gt, vm_arch::vanilla_opcode::op_le },
		{ vm_arch::opcode::op_gt1, vm_arch::vanilla_opcode::op_le },
		{ vm_arch::opcode::op_gt2, vm_arch::vanilla_opcode::op_le },
		{ vm_arch::opcode::op_gt3, vm_arch::vanilla_opcode::op_le },

		{ vm_arch::opcode::op_test1, vm_arch::vanilla_opcode::op_test },
		{ vm_arch::opcode::op_testset1, vm_arch::vanilla_opcode::op_test },

			// calls..
		{ vm_arch::opcode::op_call, vm_arch::vanilla_opcode::op_call },
		{ vm_arch::opcode::op_call1, vm_arch::vanilla_opcode::op_call },
		{ vm_arch::opcode::op_call2, vm_arch::vanilla_opcode::op_call },
		{ vm_arch::opcode::op_call3, vm_arch::vanilla_opcode::op_call },
		{ vm_arch::opcode::op_call4, vm_arch::vanilla_opcode::op_call },
		{ vm_arch::opcode::op_call5, vm_arch::vanilla_opcode::op_call },
		{ vm_arch::opcode::op_call6, vm_arch::vanilla_opcode::op_call },
		{ vm_arch::opcode::op_call7, vm_arch::vanilla_opcode::op_call },
		{ vm_arch::opcode::op_call8, vm_arch::vanilla_opcode::op_call },
		{ vm_arch::opcode::op_call9, vm_arch::vanilla_opcode::op_call },
		{ vm_arch::opcode::op_call10, vm_arch::vanilla_opcode::op_call },
		{ vm_arch::opcode::op_call11, vm_arch::vanilla_opcode::op_call },
		{ vm_arch::opcode::op_call12, vm_arch::vanilla_opcode::op_call },
		{ vm_arch::opcode::op_call13, vm_arch::vanilla_opcode::op_call },
		{ vm_arch::opcode::op_call14, vm_arch::vanilla_opcode::op_call },
		{ vm_arch::opcode::op_call15, vm_arch::vanilla_opcode::op_call },
			// ....

		{ vm_arch::opcode::op_tailcall, vm_arch::vanilla_opcode::op_tailcall },
		{ vm_arch::opcode::op_tailcall1, vm_arch::vanilla_opcode::op_tailcall },
		{ vm_arch::opcode::op_tailcall2, vm_arch::vanilla_opcode::op_tailcall },

		{ vm_arch::opcode::op_return, vm_arch::vanilla_opcode::op_return },
		{ vm_arch::opcode::op_return1, vm_arch::vanilla_opcode::op_return },
		{ vm_arch::opcode::op_return2, vm_arch::vanilla_opcode::op_return },
		{ vm_arch::opcode::op_return3, vm_arch::vanilla_opcode::op_return },
		{ vm_arch::opcode::op_return4, vm_arch::vanilla_opcode::op_return },

		{ vm_arch::opcode::op_forloop, vm_arch::vanilla_opcode::op_forloop },
		{ vm_arch::opcode::op_forprep, vm_arch::vanilla_opcode::op_forprep },
		{ vm_arch::opcode::op_tforloop, vm_arch::vanilla_opcode::op_tforloop },

		{ vm_arch::opcode::op_setlist, vm_arch::vanilla_opcode::op_setlist },
		{ vm_arch::opcode::op_setlist1, vm_arch::vanilla_opcode::op_setlist },
		{ vm_arch::opcode::op_setlist2, vm_arch::vanilla_opcode::op_setlist },

		{ vm_arch::opcode::op_close, vm_arch::vanilla_opcode::op_close },

		{ vm_arch::opcode::op_closure, vm_arch::vanilla_opcode::op_closure },
		{ vm_arch::opcode::op_closure1, vm_arch::vanilla_opcode::op_closure },

		{ vm_arch::opcode::op_vararg, vm_arch::vanilla_opcode::op_vararg },
		{ vm_arch::opcode::op_vararg1, vm_arch::vanilla_opcode::op_vararg },
	};
};