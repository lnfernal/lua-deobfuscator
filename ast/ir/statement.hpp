// author ~ cyclops

#pragma once

#include <type_traits>
#include <unordered_set>
#include <stdexcept>
#include <map>
#include <variant>
#include <unordered_map>

#include "node.hpp"
#include <iostream>

namespace deobf::ast::ir {
    namespace expression { // forward declare, to avoid circular deps
        struct expression;
        struct string_literal;
        struct name_and_args;
        struct variable;

        using expression_list_t = std::vector<std::shared_ptr<expression>>;
        using name_and_args_t = std::vector<std::shared_ptr<name_and_args>>;
        using variable_list_t = std::vector<std::shared_ptr<expression>>;  // variables are expressions aswell, but for more context make variable_list_t
        using name_list_t = std::vector<std::shared_ptr<string_literal>>;
    }

    using expression_t = typename expression::expression;


    /*
        stat
        : ';'
        | varlist '=' explist
        | functioncall (in expression for a reason)
        | 'break'
        | 'do' block 'end'
        | 'while' exp 'do' block 'end'
        | 'repeat' block 'until' exp
        | 'if' exp 'then' block ('elseif' exp 'then' block)* ('else' block)? 'end'
        | 'for' NAME '=' exp ',' exp (',' exp)? 'do' block 'end'
        | 'for' namelist 'in' explist 'do' block 'end'
        | 'function' funcname funcbody (in expression for a reason)
        | 'local' 'function' NAME funcbody
        | 'local' attnamelist ('=' explist)?
    */

    /*
        while
        return
        repeat
        if
        forstep
        forin
        assign
        local
    */


    namespace statement {
        struct statement : public node {
            virtual ~statement() = default;
        };

        using managed_statement_list = std::vector<std::shared_ptr<statement>>;

        struct return_statement final : public statement {
            ir::expression::expression_list_t body{ };

            std::string to_string() const override; // wanted to use std::copy on stringstream but lazy to define a custom type for node

            [[nodiscard]] bool equals(const node* other_node) const override;

            std::vector<std::shared_ptr<node>> get_children() const override;

            explicit return_statement(ir::expression::expression_list_t body) :
                body(std::move(body))
            { }

            void accept(abstract_visitor_pattern* visitor) override;
        };

        static const std::unordered_set<std::string> known_symbol_names{
            "string.byte",
            "string.char",
            "string.sub",
            "table.concat",
            "math.ldexp",
            "table.insert",
            "getfenv",
            "select",
            "unpack",
            "table.unpack",
            "tonumber",
            "setmetatable",
            "bit",
            "bit.bxor",
            "pcall",
        };

        // todo weak_symbol_info struct?

        struct symbol_info final { // no need a symbol table for each class, function/loops composite a block.
            enum class symbol_kind {
                global,
                local,
            };

            std::string symbol_name; // todo string_view since symbol name is seperate?

            std::string resolve_identifier; // symbol identifier for renaming purposes

            std::shared_ptr<statement> symbol_value;

            explicit symbol_info(std::string symbol_name) :
                symbol_name(symbol_name)
            { };

            explicit symbol_info(std::string symbol_name, std::shared_ptr<statement> symbol_value) :
                symbol_name(symbol_name),
                symbol_value(std::move(symbol_value))
            { };

            symbol_info& operator=(symbol_info& other) { // copy constructor (may confused here smth? idk)
                symbol_name = other.symbol_name;
                symbol_value = std::move(other.symbol_value);
                return *this;
            };

            [[nodiscard]] bool is_known_symbol() const noexcept;
        };

        //struct vm_handler_block final : public block

        struct block final : public statement {
            managed_statement_list body;
            std::optional<std::shared_ptr<return_statement>> ret; // return stat

            block* parent = nullptr;

            template <const bool is_global, typename symbol_type, typename = std::enable_if_t<std::is_base_of<statement, symbol_type>::value>>
            auto insert_symbol(const std::string& symbol_name, std::shared_ptr<symbol_type> symbol_value) {
                auto head_scope = this;
                if constexpr (is_global) // not being optimized?
                    for (; head_scope->parent != nullptr; head_scope = head_scope->parent);

                return head_scope->symbol_table.insert_or_assign(symbol_name, std::make_unique<symbol_info>(symbol_name, symbol_value)); // handles redeclarations
            }

            auto insert_symbol(const std::string& symbol_name) {
                return symbol_table.insert_or_assign(symbol_name, std::make_unique<symbol_info>(symbol_name));
            }

            symbol_info* find_symbol(const std::string& symbol_name) const {
                for (auto current_scope = this; current_scope != nullptr; current_scope = current_scope->parent)
                    if (const auto result = current_scope->symbol_table.find(symbol_name); result != current_scope->symbol_table.cend())
                        return result->second.get();

                return nullptr;
            }

            std::string to_string() const override {
                return "(to_string on block)";
            }

            [[nodiscard]] bool equals(const node* other_node) const override;

            std::vector<std::shared_ptr<node>> get_children() const override;


            // extra performance boost :
            // iterator invalidation, forces 200 rehashings before adding any element to the symbol table (max per scope in lua 200 so is the symbol table limit)
            // bucket_count must be 200 each symbol table no matter what

            explicit block()
            {
                symbol_table.reserve(200);
            };

            explicit block(block* parent) :
                parent(parent)
            {
                symbol_table.reserve(200);
            }

            explicit block(block* parent, managed_statement_list body, std::optional<std::shared_ptr<return_statement>> ret) :
                parent(parent),
                body(std::move(body)),
                ret(std::move(ret))
            {
                symbol_table.reserve(200);
            }

            void accept(abstract_visitor_pattern* visitor) override;

        private:
            std::unordered_map<std::string, std::unique_ptr<symbol_info>> symbol_table;
        };

        struct do_block final : public statement {
            std::shared_ptr<block> body;

            std::string to_string() const override {
                return "do " + body->to_string() + " end";
            }

            [[nodiscard]] bool equals(const node* other_node) const override;

            std::vector<std::shared_ptr<node>> get_children() const override;

            explicit do_block(std::shared_ptr<block> body) :
                body(std::move(body))
            { };

            void accept(abstract_visitor_pattern* visitor) override;
        };

        struct break_statement final : public statement {

            std::string to_string() const override {
                return "break";
            }

            [[nodiscard]] bool equals(const node* other_node) const override {
                return dynamic_cast<const break_statement*>(other_node) != nullptr;
            }

            void accept(abstract_visitor_pattern* visitor) override;
        };

        struct semicolon final : public statement {

            std::string to_string() const override {
                return ";";
            }

            [[nodiscard]] bool equals(const node* other_node) const override {
                return dynamic_cast<const semicolon*>(other_node) != nullptr;
            }

            void accept(abstract_visitor_pattern* visitor) override;
        };


        struct while_statement final : public statement {
            // WHILE condition DO body END


            std::shared_ptr<expression_t> condition;
            std::shared_ptr<block> body;

            std::string to_string() const override;

            [[nodiscard]] bool equals(const node* other_node) const override;

            std::vector<std::shared_ptr<node>> get_children() const override;

            explicit while_statement(std::shared_ptr<expression_t> condition, std::shared_ptr<block> body) :
                body(std::move(body)),
                condition(std::move(condition))
            { }

            void accept(abstract_visitor_pattern* visitor) override;
        };

        struct local_declaration final : public statement { // i.e. local a, b, c = 1, 2, 3 (not really a decl but standard calls it.)
            //std::string static_symbol;

            ir::expression::name_list_t names{ };
            ir::expression::expression_list_t body{ };

            std::string to_string() const override;

            [[nodiscard]] bool equals(const node* other_node) const override;

            std::vector<std::shared_ptr<node>> get_children() const override;

            explicit local_declaration(ir::expression::name_list_t names, ir::expression::expression_list_t body) :
                names(std::move(names)),
                body(std::move(body))
            { };

            void accept(abstract_visitor_pattern* visitor) override;
        };

        struct variable_assign final : public statement { // (variables) = (expressions)
            ir::expression::variable_list_t variables{ };
            ir::expression::expression_list_t expressions{ }; // shouldv'e named it body?

            std::string to_string() const override;

            [[nodiscard]] bool equals(const node* other_node) const override;

            std::vector<std::shared_ptr<node>> get_children() const override;

            explicit variable_assign(ir::expression::variable_list_t variables, ir::expression::expression_list_t expressions) :
                variables(std::move(variables)),
                expressions(std::move(expressions))
            { }

            void accept(abstract_visitor_pattern* visitor) override;
        };

        struct for_step final : public statement {
            std::shared_ptr<ir::expression::string_literal> name;
            std::shared_ptr<expression_t> init, end; // i.e. for name = 1, 3, 1 (init, end, step) do BLOCk end
            std::optional<std::shared_ptr<expression_t>> step;
            std::shared_ptr<block> body;

            std::string to_string() const override;

            [[nodiscard]] bool equals(const node* other_node) const override;

            std::vector<std::shared_ptr<node>> get_children() const override;

            explicit for_step(std::shared_ptr<ir::expression::string_literal> name,
                std::shared_ptr<expression_t> init,
                std::shared_ptr<expression_t> end,
                std::shared_ptr<block> body) :
                name(std::move(name)),
                init(std::move(init)),
                end(std::move(end)),
                body(std::move(body))
            { };

            explicit for_step(std::shared_ptr<ir::expression::string_literal> name,
                std::shared_ptr<expression_t> init,
                std::shared_ptr<expression_t> end,
                std::shared_ptr<expression_t> step,
                std::shared_ptr<block> body) :
                name(std::move(name)),
                init(std::move(init)),
                end(std::move(end)),
                step(std::move(step)),
                body(std::move(body))
            { };

            void accept(abstract_visitor_pattern* visitor) override;
        };

        struct for_in final : public statement {
            enum class iterator_type {
                stateless,
                stateful
            };

            ir::expression::name_list_t names{ }; // for (...) in
            ir::expression::expression_list_t expressions{ }; // in a,b,c,d (everything)
            std::shared_ptr<block> body;

            [[nodiscard]] bool equals(const node* other_node) const override;

            std::string to_string() const override;

            std::vector<std::shared_ptr<node>> get_children() const override;

            explicit for_in(ir::expression::name_list_t names, ir::expression::expression_list_t expressions, std::shared_ptr<block> body) :
                names(std::move(names)),
                expressions(std::move(expressions)),
                body(std::move(body))
            { };

            void accept(abstract_visitor_pattern* visitor) override;
        };

        struct repeat final : public statement { // repeat BODY until (CONDITION)
            std::shared_ptr<block> body;
            std::shared_ptr<expression_t> condition;

            [[nodiscard]] bool equals(const node* other_node) const override;

            std::string to_string() const override {
                return "repeat";
            }

            std::vector<std::shared_ptr<node>> get_children() const override;

            explicit repeat(std::shared_ptr<block> body, std::shared_ptr<expression_t> condition) :
                body(std::move(body)),
                condition(std::move(condition))
            { }

            void accept(abstract_visitor_pattern* visitor) override;
        };

        struct if_statement final : public statement {
            using multi_statements = std::vector<std::pair<std::shared_ptr<expression_t>, std::shared_ptr<block>>>;

            std::shared_ptr<expression_t> condition;
            std::shared_ptr<block> body;

            [[nodiscard]] bool equals(const node* other_node) const override;

            multi_statements else_if_statements;

            std::optional<std::shared_ptr<block>> else_body; // thought to use optional on this, just incase use empty else case

            std::string to_string() const override;

            std::vector<std::shared_ptr<node>> get_children() const override;

            explicit if_statement(std::shared_ptr<expression_t> condition, std::shared_ptr<block> body) :
                condition(std::move(condition)),
                body(std::move(body))
            { }

            explicit if_statement(std::shared_ptr<expression_t> condition, std::shared_ptr<block> body, multi_statements else_if_statements) :
                condition(std::move(condition)),
                body(std::move(body)),
                else_if_statements(std::move(else_if_statements))
            { }

            explicit if_statement(std::shared_ptr<expression_t> condition, std::shared_ptr<block> body, multi_statements else_if_statements, std::shared_ptr<block> else_body) :
                condition(std::move(condition)),
                body(std::move(body)),
                else_if_statements(std::move(else_if_statements)),
                else_body(std::move(else_body))
            { }

            void accept(abstract_visitor_pattern* visitor) override;
        };
    }
}