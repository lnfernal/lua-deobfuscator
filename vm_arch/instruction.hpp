#pragma once
#include <cstdint>
#include "opcode.hpp"
#include <iostream>
#include <unordered_set>
#include <bitset>

namespace deobf::vm_arch {
	struct instruction final {
		vm_arch::opcode op{ };
		vm_arch::instruction_type type{ }; // couldve made a mapping but lazy

		enum instruction_mark_flag {
			none = 1 << 0,
			to_collect = 1 << 1
		};

		std::size_t flags = instruction_mark_flag::none;

		// on mutated ops such eq/lt/le
		// couldve encoded all isK flags together but too lazy to change stuff so im going to keep this like that
		bool is_ka = false;
		bool is_kb = false;
		bool is_kc = false;
		
		std::size_t line_defined;
		double virtual_opcode;
		std::int16_t a;

		// optional parameters
		std::int16_t b{ };
		std::int16_t c{ };
		std::int32_t bx{ };
		std::int32_t sbx{ };

		explicit instruction(vm_arch::opcode op) : op(op), a(0), b(0), c(0), bx(0), sbx(0), type(vm_arch::instruction_type::abc) { }; // empty instruction constructor

		explicit instruction(std::uint16_t virtual_opcode, std::uint8_t a) :
			virtual_opcode(virtual_opcode),
			a(a)
		{ }

		bool operator==(const instruction&) const;

		// https://the-ravi-programming-language.readthedocs.io/en/latest/lua_bytecode_reference.html

		// tforloop and forloop and forprep and jmp : pcloc + sbx
		// eq, lt, le : pc++
		// test, testtest (conditional) : pc++


		std::size_t get_target_pc(std::size_t pc_loc) const;

		bool is_unconditional_jump() const;

		std::bitset<64> get_constant_targets() const;
		//std::size_t get_constant_targets() const;

		void print() const;

		explicit instruction(const instruction& other) :
			op(other.op),
			type(other.type),

			is_ka(other.is_ka),
			is_kb(other.is_kb),
			is_kc(other.is_kc),

			virtual_opcode(other.virtual_opcode),

			a(other.a),
			b(other.b),
			c(other.c),
			bx(other.bx),
			sbx(other.sbx)
		{ };

		instruction& operator=(const instruction&) = delete;

		virtual ~instruction() = default;
	};
}