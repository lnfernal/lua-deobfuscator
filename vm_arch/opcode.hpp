#pragma once

#include <cstdint>
#include <unordered_map>
#include <string_view>

/**
	~ IRONBREW DEOBFUSCATOR VM ARCHITECTURE ~
**/

// operand field size
#define SIZE_C          9
#define SIZE_B          9
#define SIZE_Bx         (SIZE_C + SIZE_B)
#define SIZE_A          8

#define SIZE_OP         6

// operand field positions
#define POS_OP          0
#define POS_A           (POS_OP + SIZE_OP)
#define POS_C           (POS_A + SIZE_A)
#define POS_B           (POS_C + SIZE_C)
#define POS_Bx          POS_C

#define MAXARG_A        ((1<<SIZE_A)-1)
#define MAXARG_B        ((1<<SIZE_B)-1)
#define MAXARG_C        ((1<<SIZE_C)-1)

/* creates a mask with `n' 1 bits at position `p' */
#define MASK1(n,p)      ((~((~(unsigned long)0)<<n))<<p)

#define cast(t, exp)    (static_cast<t>(exp))

#define GETARG_A(i)     (cast(int, ((i)>>POS_A) & MASK1(SIZE_A,0)))

#define GETARG_B(i)     (cast(int, ((i)>>POS_B) & MASK1(SIZE_B,0)))

#define GETARG_C(i)     (cast(int, ((i)>>POS_C) & MASK1(SIZE_C,0)))

#define GETARG_Bx(i)    (cast(int, ((i)>>POS_Bx) & MASK1(SIZE_Bx,0)))

#define MAXARG_sBx 131071

#define GETARG_sBx(i)   (GETARG_Bx(i)-MAXARG_sBx)

#define ENCODE_OPERAND(operand, size, pos) (operand & ((1 << size) - 1)) << pos

namespace deobf::vm_arch {
	enum class opcode : std::uint8_t { // part of the ironbrew ISA, vanilla : https://www.lua.org/source/5.1/lopcodes.h.html
		op_invalid,

		op_move,
		op_loadk,

		op_loadbool,
		op_loadbool1, // enable on branching loadbool (C != 0 -> PC++)

		op_loadnil,
		op_getupval,
		op_getglobal,

		op_gettable,
		op_gettable1, // gettable kst (R[A] := STK[B][RK(C)]) ( ENABLE IF ISK(C) )

		op_setglobal,
		op_setupval,

		// pseudo-instructions

		op_settable,
		op_settable1, // enable if isK(B)
		op_settable2, // enable if isK(C)
		op_settable3, // enable if isK(BC)

		op_newtable,

		op_self,
		op_self1, // enable if isK(C)

		// 3 cases because different generation for basic artimethic instructions (RK(B), RK(C) and stack)  

		op_add,
		op_add1, // enable if isK(B)
		op_add2, // enable if isK(C)
		op_add3, // enable if isK(BC)

		op_sub,
		op_sub1, // enable if isK(B)
		op_sub2, // enable if isK(C)
		op_sub3, // enable if isK(BC)

		op_mul,
		op_mul1, // enable if isK(B)
		op_mul2, // enable if isK(C)
		op_mul3, // enable if isK(BC)

		op_div,
		op_div1, // enable if isK(B)
		op_div2, // enable if isK(C)
		op_div3, // enable if isK(BC)

		op_mod,
		op_mod1, // enable if isK(B)
		op_mod2, // enable if isK(C)
		op_mod3, // enable if isK(BC)

		op_pow,
		op_pow1, // enable if isK(B)
		op_pow2, // enable if isK(C)
		op_pow3, // enable if isK(BC)

		op_unm,
		op_not,
		op_len,
		op_concat,
		op_jmp,

		// eq/lt/le (generated in control structures, nonlogical unlike test/set)
		// have their operand A as RK(B) (because if boolean A is false then we use eq and A=1 has a different opcode)


		op_eq,
		op_eq1, // enable if isK(B)
		op_eq2, // enable if isK(C)
		op_eq3, // enable if isK(BC)

		op_lt,
		op_lt1, // enable if isK(B)
		op_lt2, // enable if isK(C)
		op_lt3, // enable if isK(BC)

		op_le,
		op_le1, // enable if isK(B)
		op_le2, // enable if isK(C)
		op_le3, // enable if isK(BC)
		
		op_test,

		op_testset,

		op_test1, // inverse test (C == 1)
		op_testset1, // inverse testset (C == 1)

		// lt (generated when A==1)
		op_ge,
		op_ge1, // enable if isK(B)
		op_ge2, // enable if isK(C)
		op_ge3, // enable if isK(BC)

		// le (generated when A==1)
		op_gt,
		op_gt1, // enable if isK(B) 
		op_gt2, // enable if isK(C)
		op_gt3, // enable if isK(BC)

		// eq (generated when A==1)
		op_ne,
		op_ne1, // enable if isK(B) 
		op_ne2, // enable if isK(C)
		op_ne3, // enable if isK(BC)

		// god damn it
		op_call, // b > 2, c > 2
		op_call1, // b == 2, c > 2 
		op_call2, // b == 0, c > 2
		op_call3, // b == 1, c > 2
		op_call4, // b > 2, c == 0
		op_call5, // b == 2, c == 0
		op_call6, // b > 2, c == 1
		op_call7, // b == 2, c == 1
		op_call8, // b == 0, c == 0
		op_call9, // b == 0, c == 1
		op_call10, // b == 1, c == 0
		op_call11, // b == 1, c == 1
		op_call12, // b > 2, c == 2
		op_call13, // b == 2, c == 2
		op_call14, // b == 0, c == 2
		op_call15, // b == 1, c == 2

		op_tailcall,
		op_tailcall1, // b == 0 (passed L->top amount of params to call stack)
		op_tailcall2, // b == 1 (0 parameters)

		op_return,
		op_return1, // b == 2, one stack return
		op_return2,  // b == 3, 2 stack returns
		op_return3, // b == 0, obviously L->top returns (could be checked if is preceded by call or vararg because top is set in both)
		op_return4, // b == 1, 0 returns.

		op_forloop,
		op_forprep,
		op_tforloop,

		op_setlist,
		op_setlist1, // B == 0 (insert STK[A][i] := STK[1 <= A+i <= top], aka UNK values.)
		op_setlist2, // C == 0, branching

		op_close,

		op_closure,
		op_closure1, // proto has no upvalues (no MOVEs followed by)

		op_vararg,
		op_vararg1, // enabled if B == 0, L->top amount of varargs are pushed to stack

		// IRONBREW SIMD INSTRUCTION SET
		op_newstack,
		op_settop,

		// IRONBREW DEOBFUSCATOR CUSTOM INSTRUCTION SET
		op_decryptedkstflag,
	};

	enum class vanilla_opcode : std::uint8_t {
		op_move,
		op_loadk,
		op_loadbool,
		op_loadnil,
		op_getupval,
		op_getglobal,
		op_gettable,
		op_setglobal,
		op_setupval,
		op_settable,
		op_newtable,
		op_self,
		op_add,
		op_sub,
		op_mul,
		op_div,
		op_mod,
		op_pow,
		op_unm,
		op_not,
		op_len,
		op_concat,
		op_jmp,
		op_eq,
		op_lt,
		op_le,
		op_test,
		op_testset,
		op_call,
		op_tailcall,
		op_return,
		op_forloop,
		op_forprep,
		op_tforloop,
		op_setlist,
		op_close,
		op_closure,
		op_vararg,
	};

	static inline std::unordered_map<vm_arch::opcode, std::string_view> opcode_map{
		{ opcode::op_invalid, "invalid" },

		{ opcode::op_move, "move" },
		{ opcode::op_loadk, "loadk" },
		
		{ opcode::op_loadbool, "loadbool" },
		{ opcode::op_loadbool1, "loadbool1" },

		{ opcode::op_loadnil, "loadnil" },
		{ opcode::op_getupval, "getupval" },
		{ opcode::op_getglobal, "getglobal" },
		
		{ opcode::op_gettable, "gettable" },
		{ opcode::op_gettable1, "gettable1" },

		{ opcode::op_setglobal, "setglobal" },
		{ opcode::op_setupval, "setupval" },
		
		{ opcode::op_settable, "settable" },
		{ opcode::op_settable1, "settable1" },
		{ opcode::op_settable2, "settable2" },
		{ opcode::op_settable3, "settable3" },

		{ opcode::op_newtable, "newtable" },
		
		{ opcode::op_self, "self" },
		{ opcode::op_self1, "self1" },

		{ opcode::op_add, "add" },
		{ opcode::op_add1, "add1" },
{ opcode::op_add2, "add2" },
{ opcode::op_add3, "add3" },

		{ opcode::op_sub, "sub" },
		{ opcode::op_sub1, "sub1" },
		{ opcode::op_sub2, "sub2" },
		{ opcode::op_sub3, "sub3" },

		{ opcode::op_mul, "mul" },
		{ opcode::op_mul1, "mul1" },
		{ opcode::op_mul2, "mul2" },
		{ opcode::op_mul3, "mul3" },

		{ opcode::op_div, "div" },
		{ opcode::op_div1, "div1" },
		{ opcode::op_div2, "div2" },
		{ opcode::op_div3, "div3" },

		{ opcode::op_mod, "mod" },
		{ opcode::op_mod1, "mod1" },
		{ opcode::op_mod2, "mod2" },
		{ opcode::op_mod3, "mod3" },

		{ opcode::op_pow, "pow" },
		{ opcode::op_pow1, "pow1" },
		{ opcode::op_pow2, "pow2" },
		{ opcode::op_pow3, "pow3" },

		{ opcode::op_unm, "unm" },
		{ opcode::op_not, "not" },
		{ opcode::op_len, "len" },
		{ opcode::op_concat, "concat" },
		{ opcode::op_jmp, "jmp" },

		{ opcode::op_eq, "eq" },
		{ opcode::op_eq1, "eq1" },
		{ opcode::op_eq2, "eq2" },
		{ opcode::op_eq3, "eq3" },

		{ opcode::op_lt, "lt" },
		{ opcode::op_lt1, "lt1" },
		{ opcode::op_lt2, "lt2" },
		{ opcode::op_lt3, "lt3" },

		{ opcode::op_le, "le" },
		{ opcode::op_le1, "le1" },
		{ opcode::op_le2, "le2" },
		{ opcode::op_le3, "le3" },


		{ opcode::op_ne, "ne" },
		{ opcode::op_ne1, "ne1" },
		{ opcode::op_ne2, "ne2" },
		{ opcode::op_ne3, "ne3" },


		{ opcode::op_gt, "gt" },
		{ opcode::op_gt1, "gt1" },
		{ opcode::op_gt2, "gt2" },
		{ opcode::op_gt3, "gt3" },


		{ opcode::op_ge, "ge" },
		{ opcode::op_ge1, "ge1" },
		{ opcode::op_ge2, "ge2" },
		{ opcode::op_ge3, "ge3" },

		{ opcode::op_test, "test" },
		{ opcode::op_test1, "test1" },

		{ opcode::op_testset, "testset" },
		{ opcode::op_testset1, "testset1" },

		{ opcode::op_call, "call" },
		{ opcode::op_call1, "call1" },
		{ opcode::op_call2, "call2" },
		{ opcode::op_call3, "call3" },
		{ opcode::op_call4, "call4" },
		{ opcode::op_call5, "call5" },
		{ opcode::op_call6, "call6" },
		{ opcode::op_call7, "call7" },
		{ opcode::op_call8, "call8" },
		{ opcode::op_call9, "call9" },
		{ opcode::op_call10, "call10" },
		{ opcode::op_call11, "call11" },
		{ opcode::op_call12, "call12" },
		{ opcode::op_call13, "call13" },
		{ opcode::op_call14, "call14" },
		{ opcode::op_call15, "call15" },

		{ opcode::op_tailcall, "tailcall" },
		{ opcode::op_tailcall1, "tailcall1" },
		{ opcode::op_tailcall2, "tailcall2" },

		{ opcode::op_return, "return" },
		{ opcode::op_return1, "return1" },
		{ opcode::op_return2, "return2" },
		{ opcode::op_return3, "return3" },
		{ opcode::op_return4, "return4" },

		{ opcode::op_forloop, "forloop" },
		{ opcode::op_forprep, "forprep" },
		{ opcode::op_tforloop, "tforloop" },
		
		{ opcode::op_setlist, "setlist" },
		{ opcode::op_setlist1, "setlist1" },
		{ opcode::op_setlist2, "setlist2" },

		{ opcode::op_close, "close" },
		
		{ opcode::op_closure, "closure" },
		{ opcode::op_closure1, "closure1" },

		{ opcode::op_vararg, "vararg" },
		{ opcode::op_vararg1, "vararg1" },

		// custom vm opcodes
		{ opcode::op_newstack, "newstack" },
		{ opcode::op_settop, "settop" },

		// ironbrew deobfuscator special set
		{ opcode::op_decryptedkstflag, "decryptedkstflag" }
	};

	enum class instruction_mode {
		a, // 8 bits
		b, // 9 bits
		c, // 9 bits
		bx, // 18 bits (b+c)
		sbx, // signed bx
	};
	
	enum class vanilla_instruction_type {
		a,
		ab,
		ac,
		abc,
		sbx,
		abx,
		asbx,
	};

	enum class instruction_type {
		abc,
		abx,
		asbx,
		asbxc,
	};
}