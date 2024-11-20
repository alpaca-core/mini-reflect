// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <mini-reflect.hpp>
#include <iostream>
#include <functional>

void indent(int depth) {
    for (int i = 0; i < depth; ++i) {
        std::cout << "  ";
    }
}
struct my_node;

struct my_reflection_info : public reflect::reflection_info {
    const char* type_name = nullptr;
    const char* field_name = nullptr;

    my_reflection_info() = default;
    my_reflection_info(my_node& owner, const char* field_name);

    void print_schema(int depth = 0) const {
        indent(depth);
        if (field_name) {
            std::cout << field_name << ": ";
        }
        std::cout << type_name << '\n';

        for (auto& c : child_infos()) {
            static_cast<const my_reflection_info*>(c)->print_schema(depth + 1);
        }
    }

    void for_each_child_of(const my_node& n, std::function<void(my_node&)> func);
};

using iptr = std::unique_ptr<my_reflection_info>;

iptr set_type(iptr& info, const char* type_name) {
    info->type_name = type_name;
    return std::move(info);
}

struct my_node : public reflect::node {
    using reflect::node::node;

    virtual void do_print_value(int depth) const = 0;

    void print_value() const {
        do_print_value(0);
    }

    void print_schema() const {
        static_cast<const my_reflection_info*>(m_base_info.get())->print_schema();
    }

    my_reflection_info* info() const {
        return static_cast<my_reflection_info*>(m_info);
    }
};


my_reflection_info::my_reflection_info(my_node& owner, const char* field_name)
    : reflect::reflection_info(owner)
    , type_name("unknown")
    , field_name(field_name)
{}

void my_reflection_info::for_each_child_of(const my_node& n, std::function<void(my_node&)> func) {
    const auto n_addr = reinterpret_cast<intptr_t>(static_cast<const reflect::node*>(&n));
    for (auto& c : child_infos()) {
        const auto c_offset = c->offset_in_owner();
        const auto c_addr = n_addr + c_offset;
        auto c_node = static_cast<my_node*>(reinterpret_cast<reflect::node*>(c_addr));
        func(*c_node);
    }
}

struct object : public my_node {
    object(iptr info) : my_node(set_type(info, "object")) {}
    object(const object& other) = default;
    void do_print_value(int depth) const override final {
        info()->for_each_child_of(*this, [&](my_node& n) {
            indent(depth);
            std::cout << n.info()->field_name << ":\n";
            n.do_print_value(depth + 1);
            std::cout << '\n';
        });
    }
protected:
    ~object() = default;
};

iptr i(object& owner, const char* name) {
    return iptr{new my_reflection_info(owner, name)};
}

struct integer final : public my_node {
    integer(iptr info) : my_node(set_type(info, "integer")) {}
    integer(const integer& other) = default;
    int value = 0;
    void do_print_value(int depth) const override {
        indent(depth);
        std::cout << value;
    }
};

struct string : public my_node {
    string(iptr info) : my_node(set_type(info, "string")) {}
    string(const string& other) = default;
    std::string value;
    void do_print_value(int depth) const override {
        indent(depth);
        std::cout << value;
    }
};

struct addr final : public object {
    using object::object;
    string street{i(*this, "street")};
    integer number{i(*this, "number")};
};

struct person final : public object {
    using object::object;
    string name{i(*this, "name")};
    integer age{i(*this, "age")};
    addr address{i(*this, "address")};
};

int main() {
    const person schema(iptr{new my_reflection_info});
    schema.print_schema();

    auto value = schema;
    value.name.value = "John";
    value.age.value = 42;
    value.address.street.value = "Elm";
    value.address.number.value = 123;

    value.print_value();

    return 0;
}
