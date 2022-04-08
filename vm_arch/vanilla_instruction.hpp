#pragma once
#include <cstdint>
#include "opcode.hpp"
#include <iostream>
#include <unordered_set>

namespace deobf::vm_arch {

	struct vanilla_instruction final {

		// wanted to use this before but it will get messy to handle on transpilation
		// https://i.imgur.com/rOC1qSo.png
/*#pragma pack(push, 1)
		union {
			struct {
				vm_arch::vanilla_opcode opcode;
				
				struct {
					std::uint8_t a;
					union {
						struct {
							std::uint8_t b;
							std::uint8_t c;
						};
						std::int16_t sbx;
						std::uint16_t bx;
					};
				};
			};
			std::uint32_t value; // is big endian
		};
#pragma pack(pop)*/

		std::size_t line_defined;
		vm_arch::vanilla_opcode opcode;
		vm_arch::vanilla_instruction_type type;

		//std::uint8_t a;
		//std::uint8_t b;
		//std::uint8_t c;
		//std::int16_t sbx;
		//std::uint16_t bx;

		std::int32_t a;
		std::int32_t b;
		std::int32_t c;
		std::int32_t sbx;
		std::int32_t bx;

		explicit vanilla_instruction() : a(0), b(0), c(0), sbx(0), bx(0), line_defined(0){ };
		
		std::uint32_t get_value() const;
		explicit vanilla_instruction(const vanilla_instruction& other) :
			opcode(other.opcode),
			a(other.a),
			b(other.b),
			c(other.c),
			sbx(other.sbx),
			bx(other.bx),
			type(other.type),
			line_defined(other.line_defined)
		{ };


		vanilla_instruction& operator=(const vanilla_instruction&) = delete;

		virtual ~vanilla_instruction() = default;
	};
}