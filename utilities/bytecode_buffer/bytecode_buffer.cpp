#include "bytecode_buffer.hpp"
#include <iostream>

namespace deobf::utilities::bytecode_buffer {
	bytecode_buffer::bytecode_buffer() :
		position_(0),
		size_(0xFFFFFF)
	{
		bytecode.resize(0xFFFFFF);

		//auto bytecode_allocator = bytecode.get_allocator();
		clear();
	}

	bytecode_buffer::bytecode_buffer(std::vector<std::uint8_t>& bytecode) :
		bytecode(bytecode),
		size_(bytecode.size()),
		position_(0)
	{ }

	bytecode_buffer::~bytecode_buffer() {
		size_ = 0;

		bytecode.clear();
	}

	void bytecode_buffer::clear() {
		std::fill(bytecode.begin(), bytecode.end(), 0);
	}

	//void bytecode_buffer::seek(std::int32_t position) {
	//	position_ += position;
	//}

	std::size_t bytecode_buffer::in_avail() const {
		return bytecode.size() - position_;
	}

	bool bytecode_buffer::is_avail() const {
		return (in_avail() > 0);
	}

	std::string bytecode_buffer::read_string() {
		auto current = reinterpret_cast<char*>(bytecode.data() + position_);

		auto result = std::string{ current };

		position_ += result.size() + 1;

		return result;
	}

	void bytecode_buffer::write_string(const std::string& value) {
		//if (value.size() == 0) {
		//	write<std::uint32_t>(1);
		//	write<std::uint8_t>(0);
		//	return;
		//}

		write<std::uint32_t>(value.size() + 1);

		std::strcpy(reinterpret_cast<char*>(bytecode.data() + position_), value.data());

		position_ += value.size() + 1; // including NULL terminator (strcpy)
	}

	std::size_t bytecode_buffer::get_position() const noexcept {
		return position_;
	}

	std::string bytecode_buffer::to_string() const {
		return { bytecode.cbegin(), bytecode.cbegin() + position_ };
	}

	const std::vector<std::uint8_t>& bytecode_buffer::get_data() const& {
		return bytecode;
	}
}