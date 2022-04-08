#pragma once

#include <vector>
#include <string>
#include <memory>
#include <optional>
#include <functional>

namespace deobf::ast::ir {
	struct abstract_visitor_pattern; // visitor dependency

	// polymorphic clone CRTP
	template <class derived>
	struct cloneable_node : node {
		virtual std::unique_ptr<derived> clone() const {
			return std::make_unique<derived>(static_cast<const derived&>(*this));
		}
	};

	struct node : public std::enable_shared_from_this<node> {
		virtual std::string to_string() const = 0; // just like in Lua.g4 antlr form
		
		[[nodiscard]] virtual bool equals(const node* other_node) const = 0; // useless, deprecated in our IR for now (todo operator==(const node& other_node) instead?)

		virtual std::vector<std::shared_ptr<node>> get_children() const {
			return { };
		};

		template <typename node_type>
		std::vector<std::reference_wrapper<node_type>> get_children_type() const { // somewhat static polymorphism doesn't apply to this template method
			std::vector<std::reference_wrapper<node_type>> results;

			const auto children = get_children();
			for (const auto& current_node : children) {
				if (auto result = dynamic_cast<node_type*>(current_node.get())) {
					results.emplace_back(*result);
				}

				/*const auto next_children = current_node.get()->get_children_type<node_type>();
				for (auto& children : next_children)
					results.emplace_back(children);*/
				//std::move(next_children.begin(), next_children.end(), std::back_inserter(results));
			}

			return results;
		}

		virtual void accept(abstract_visitor_pattern* visitor) = 0;

		template <typename node_type>
		std::optional<std::reference_wrapper<node_type>> find_first_of() const {
			const auto children = get_children();
			for (const auto& current_node : children) { // depth first traversal
				if (auto return_node = dynamic_cast<node_type*>(current_node.get())) { // dynamic_cast<node_type&> cant be casted into bool smh
					return *return_node;
				}
				else if (auto found_node = current_node.get()->find_first_of<node_type>()) {
					return found_node;
				}
			}

			return std::nullopt;
		}
		
		template <typename node_type>
		auto as() {
			return std::dynamic_pointer_cast<node_type>(shared_from_this());
		}

		template <typename node_type>
		const auto as() const { // support for const nodes instead of const_cast
			return std::dynamic_pointer_cast<const node_type>(shared_from_this());
		}

		template <typename node_type>
		auto is() const {
			return dynamic_cast<const node_type*>(this) != nullptr; // performance-wise this would be better than using a dynamic_pointer_cast since it won't create a temp shared_ptr on failure
		}
		
		// move ctor & etc rules
		node() = default;
		node(node&&) = default;
		node(const node&) = delete;

		node& operator=(node&&) = default;
		node& operator=(const node&) = delete;

		virtual ~node(void) noexcept = default; // abstract ctor (set default)
	};
}