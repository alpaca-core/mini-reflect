// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <vector>
#include <memory>
#include <cstdint>
#include <cassert>

#if !defined(MINI_REFLECT_API)
    #if MINI_REFLECT_SHARED
        #if !defined(SYMBOL_IMPORT)
            #if defined(_WIN32)
                #define SYMBOL_EXPORT __declspec(dllexport)
                #define SYMBOL_IMPORT __declspec(dllimport)
            #else
                #define SYMBOL_EXPORT __attribute__((__visibility__("default")))
                #define SYMBOL_IMPORT
            #endif
        #endif

        #if MINI_REFLECT_IMPLEMENT
            #define MINI_REFLECT_API SYMBOL_EXPORT
        #else
            #define MINI_REFLECT_API SYMBOL_IMPORT
        #endif
    #else
        #define MINI_REFLECT_API
    #endif
#endif

namespace reflect {

class node;

class MINI_REFLECT_API reflection_info {
public:
    reflection_info() noexcept = default; // root info
    reflection_info(node& owner_node); // implemented below as it depends on node

    virtual ~reflection_info() noexcept;

    const node& reflection_node() const noexcept { return *m_reflection_node; }
    intptr_t offset_in_owner() const noexcept { return m_offset_in_owner; }

    const reflection_info* owner_info() const noexcept { return m_owner_info; }
    const std::vector<const reflection_info*>& child_infos() const noexcept { return m_child_infos; }

private:
    friend class node;

    void set_node(const node& n) noexcept {
        assert(!m_reflection_node); // only set once and only by the node constructor
        m_reflection_node = &n;
        if (!m_owner_info) return; // we're root
        auto owner_node = m_owner_info->m_reflection_node;
        assert(owner_node);
        // evil
        auto offset =
            reinterpret_cast<intptr_t>(m_reflection_node)
            - reinterpret_cast<intptr_t>(owner_node);
        assert(offset >= 0);
        m_offset_in_owner = offset;
    }

    const node* m_reflection_node = nullptr; // the node that this info reflects

    intptr_t m_offset_in_owner = 0; // offset of this field in the owner struct

    reflection_info* const m_owner_info = nullptr;
    std::vector<const reflection_info*> m_child_infos; // should be of reference_wrapper, but too much hassle
};

using reflection_info_ptr = std::unique_ptr<reflection_info>;

class node {
protected:
    // create a reflection node
    explicit node(reflection_info_ptr proto_info) noexcept
        : m_base_info(std::move(proto_info))
    {
        assert(m_base_info);
        m_base_info->set_node(*this);
    }

    // create a value node from a reflection node or another value node
    node(const node& other) noexcept
        : m_info(other.m_base_info ? other.m_base_info.get() : other.m_info)
    {
        assert(m_info);
    }

    // not yet clear what (if anything) to do here
    node& operator=(const node& other) = delete;

    node* owner() const noexcept {
        assert(m_info);
        if (!m_info->m_owner_info) return nullptr; // we're root
        return reinterpret_cast<node*>(
            reinterpret_cast<intptr_t>(this) - m_info->m_offset_in_owner
        );
    }

    friend class reflection_info;

    // one and only one of the following must be non-null
    const reflection_info_ptr m_base_info; // something here means a reflection node and a source of structured data
    reflection_info* const m_info = nullptr; // pointing to the info of the prototype reflection node
};

inline reflection_info::reflection_info(node& owner_node)
    : m_owner_info(owner_node.m_base_info.get())
{
    assert(m_owner_info); // must be a reflection node
    m_owner_info->m_child_infos.push_back(this);
}

} // namespace reflect
