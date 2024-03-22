/*------------------------------------------------------------------------------
Copyright (c) 2024 Helio Nunes Santos

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------*/

#include "sys/fdt.hpp"
#include "include/types.h"
#include "sys/mem.hpp"
#include "sys/string.hpp"

namespace hls {
const uint32_t *FdtEngine::get_aligned_after_offset(const uint32_t *ptr,
                                                    size_t offset) {
  return ptr +
         (offset / sizeof(uint32_t) + (offset % sizeof(uint32_t) ? 1 : 0));
}

const uint32_t *FdtEngine::move_to_next_token(const uint32_t *token_ptr) {
  uint32_t token = read_value(token_ptr);
  if (token == FDT_BEGIN_NODE) {
    ++token_ptr;
    const char *as_str = reinterpret_cast<const char *>(token_ptr);
    // If it is the root node, as it doesn't have a name, we just skip one token
    // which is just padding.
    if (auto str_size = strlen(as_str))
      token_ptr = get_aligned_after_offset(token_ptr, str_size + 1);
    else
      ++token_ptr;
  } else if (token == FDT_END_NODE) {
    ++token_ptr;
  } else if (token == FDT_PROP) {
    ++token_ptr;
    const fdt_prop_desc *descriptor =
        reinterpret_cast<const fdt_prop_desc *>(token_ptr);
    size_t prop_length = read_value(&descriptor->len);
    token_ptr = get_aligned_after_offset(token_ptr,
                                         sizeof(fdt_prop_desc) + prop_length);
  } else if (token == FDT_NOP) {
    ++token_ptr;
  } else if (token == FDT_END) {
    // Do nothing, there is nothing else to be read anymore
  }
  return token_ptr;
}

uint32_t FdtEngine::read_value(const uint32_t *ptr) {
  // Handles different endianess by reversing byte order if the target is little
  // endian, given that values in the fdt header are all big endian
  if constexpr (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) {
    return ((*ptr & 0xFF) << 24) | ((*ptr & 0xFF00) << 8) |
           ((*ptr & 0xFF0000) >> 8) | ((*ptr & 0xFF000000) >> 24);
  }
  return *ptr;
}

const uint32_t *FdtEngine::get_structure_block_ptr(const fdt_header *header) {
  uint32_t structure_offset = read_value(&header->off_dt_struct);
  auto as_char_ptr = reinterpret_cast<const char *>(header) + structure_offset;
  return reinterpret_cast<const uint32_t *>(as_char_ptr);
}

const char *FdtEngine::get_string_block_ptr(const fdt_header *header) {
  uint32_t offset = read_value(&header->off_dt_strings);
  return (reinterpret_cast<const char *>(header)) + offset;
}

int FdtEngine::traverse_node(const uint32_t *&token_ptr,
                             const fdt_header *header,
                             TraversalAction &action) {
  const uint32_t *start_token = token_ptr;
  uint32_t token = read_value(token_ptr);

  // Given a node, it will traverse all of it subnodes recursively

  // The first token HAS to be a FDT_BEGIN_NODE, given that the function
  // traverses a node to its end.
  if (token == FDT_BEGIN_NODE) {

    action.on_FDT_BEGIN_NODE(header, token_ptr);
    token_ptr = move_to_next_token(token_ptr);

    while (true) {
      // If the action is satisfied, we have no reason at all to keep checking
      // the remaing of the structure
      if (action.is_action_satisfied())
        return ALL_OK;
      token = read_value(token_ptr);
      switch (token) {
      case FDT_BEGIN_NODE:
        // Special case, we have found another node!
        {
          auto retval = traverse_node(token_ptr, header, action);
          if (retval != ALL_OK)
            return retval;
        }
        break;
      case FDT_END_NODE:
        action.on_FDT_END_NODE(header, token_ptr);
        token_ptr = move_to_next_token(token_ptr);
        // If we started with the root node, the FDT_END token has to come next,
        // so we check if this is the case in the next iteration of the loop.
        if (start_token != get_structure_block_ptr(header))
          return ALL_OK;
        break;
      case FDT_PROP:
        action.on_FDT_PROP_NODE(header, token_ptr);
        token_ptr = move_to_next_token(token_ptr);
        break;
      case FDT_NOP:
        action.on_FDT_NOP_NODE(header, token_ptr);
        token_ptr = move_to_next_token(token_ptr);
        break;
      case FDT_END:
        // Finding a FDT_END token is only valid if we started with the root
        // node, otherwise there is something wrong...
        if (start_token == get_structure_block_ptr(header))
          return ALL_OK;
        [[fallthrough]];
      default:
        return INVALID_STRUCTURE_BLOCK;
      }
    }
  }

  // If we have found anything else, we don't have a valid structure block or
  // something really weird happened
  return INVALID_STRUCTURE_BLOCK;
}

// Definitions for NodeFinder action

NodeFinder::NodeFinder(const char *node_name, const char *unit_address)
    : m_node_name(node_name), m_unit_address(unit_address){
                                  // Empty
                              };

// Refactor, this is ugly af
bool NodeFinder::is_same_as(const char *node_string) {
  if (node_string != nullptr && m_node_name != nullptr) {
    // If we care about unit address
    if (m_unit_address != nullptr) {
      auto at_loc = strchr(node_string, '@');
      if (at_loc != nullptr) {
        size_t node_name_length = at_loc - node_string;
        if (strncmp(node_string, m_node_name, node_name_length) == 0) {
          if (strcmp(at_loc + 1, m_unit_address) == 0) {
            return true;
          }
        }
      }
    } else {
      return strcmp(node_string, m_node_name) == 0;
    }
  }

  return false;
}

void NodeFinder::on_FDT_BEGIN_NODE(const fdt_header *header,
                                   const uint32_t *token) {
  if (m_node_name != nullptr) {
    const char *node_name = reinterpret_cast<const char *>(token + 1);
    if (is_same_as(node_name)) {
      m_result = FdtNode(token, header);
    }
  }
}

bool NodeFinder::is_action_satisfied() const { return m_result.is_valid(); }

FdtNode NodeFinder::result() const { return m_result; }

// Definitions for PropertyFinder

void PropertyFinder::on_FDT_PROP_NODE(const fdt_header *header,
                                      const uint32_t *token) {
  auto property_descriptor = reinterpret_cast<const fdt_prop_desc *>(token + 1);
  auto property_name = FdtEngine::get_string_block_ptr(header) +
                       FdtEngine::read_value(&property_descriptor->nameoff);

  if (strcmp(property_name, m_looked_property) == 0) {
    m_property_length = FdtEngine::read_value(&property_descriptor->len);
    if (m_property_length)
      m_result = token + sizeof(fdt_prop_desc) / sizeof(uint32_t) + 1;
    m_property_found = true;
  }
}

bool PropertyFinder::is_action_satisfied() const { return is_property_found(); }

const void *PropertyFinder::property_content() const { return m_result; };

uint32_t PropertyFinder::property_length() const { return m_property_length; }

bool PropertyFinder::is_property_found() const { return m_property_found; }

// Definitions for FdtNode

FdtNode::FdtNode(const uint32_t *first_token, const fdt_header *fdt_header)
    : m_node_token_start(first_token), m_fdt_header(fdt_header){
                                           // Empty
                                       };

FdtNode FdtNode::get_sub_node(const char *node_name,
                              const char *unit_address) const {

  NodeFinder action(node_name, unit_address);
  const uint32_t *token_ptr = m_node_token_start;
  FdtEngine::traverse_node(token_ptr, m_fdt_header, action);
  return action.result();
}

const void *FdtNode::get_property(const char *property_name) const {
  PropertyFinder action(property_name);
  const uint32_t *token_ptr = m_node_token_start;
  FdtEngine::traverse_node(token_ptr, m_fdt_header, action);
  return action.property_content();
}

bool FdtNode::has_property(const char *property_name) const {
  PropertyFinder action(property_name);
  const uint32_t *token_ptr = m_node_token_start;
  FdtEngine::traverse_node(token_ptr, m_fdt_header, action);
  return action.is_property_found();
}

bool FdtNode::is_valid() const {
  return m_fdt_header != nullptr && m_node_token_start != nullptr;
}

} // namespace hls