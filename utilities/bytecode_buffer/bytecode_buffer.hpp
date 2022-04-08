#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <stdexcept>
#include <iostream>
namespace deobf::utilities::bytecode_buffer {
	// custom epic bytecode I/O stream buffer O(1) impelementation fuck ostringstream

	// todo use in deserializer
	class bytecode_buffer final {
		mutable std::vector<std::uint8_t> bytecode;
		
		std::size_t position_;
		std::size_t size_;

		//void seek(std::int32_t);

		bool is_avail() const;

		std::size_t in_avail() const;

	public:
		bytecode_buffer(); // initial max : 0xFFFFFFF

		bytecode_buffer(std::vector<std::uint8_t>& bytecode);
		
		bytecode_buffer(bytecode_buffer&&) = delete;
		bytecode_buffer(const bytecode_buffer&) = delete;

		~bytecode_buffer();

		std::size_t get_position() const noexcept;

		// R/W (READ/WRITE) functions
		template <typename T>
		T read() {
			if (position_ >= size_)
				throw std::overflow_error("bytecode_buffer overflow in read, info?");

			auto current = reinterpret_cast<const T*>(bytecode.data() + position_);
			
			position_ += sizeof T;
			
			return *current;
		}
		
		template <typename T>
		T* write(const T data) {
			auto current = reinterpret_cast<T*>(bytecode.data() + position_);
			
			position_ += sizeof T;

			std::memcpy(current, &data, sizeof T);
			
			return current;
		}

		void clear();

		std::string read_string();
		void write_string(const std::string&);

		std::string to_string() const;

		const std::vector<std::uint8_t>& get_data() const&;
	};
}