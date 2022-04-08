#pragma once
#include <variant>
#include <string>
#include <functional>
#include <type_traits>

#include "vm_arch/instruction.hpp"

// credits to da stack overflow post for the is_contained pattern

template <typename T, typename ...>
struct is_contained : std::false_type { };

template <typename T, typename Head, typename ...Tail>
struct is_contained<T, Head, Tail...> : std::integral_constant<bool, std::is_same<T, Head>::value || is_contained<T, Tail...>::value> { };

namespace deobf::vm_arch {

	struct constant {
		// todo references for DFA? not redudrant atm
		std::vector<std::reference_wrapper<const vm_arch::instruction>> owners;

		enum class constant_type : short {
			nil, // todo do we really have to hold NIL ?

			number,
			string,
			boolean,
		};

		// todo possibly use an union for all value types instead of variance?
		std::variant<std::nullptr_t, double, std::string, bool> value;

		template <typename kst_type,
			typename = std::enable_if<is_contained<typename std::decay<kst_type>::type, std::nullptr_t, double, std::string, bool>::value>>

		explicit constant(kst_type&& value) :
			value(value)
		{ }


		// todo operator==

		[[nodiscard]] inline auto get_constant_type() const noexcept {
			return static_cast<enum constant_type>(value.index());
		}

		bool operator==(const constant& other) const {
			return other.value == value;
		}

		std::string to_string() const;

		virtual ~constant() = default;
	};
}