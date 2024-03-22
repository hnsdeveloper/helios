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

#ifndef FDT_PARSER_HPP
#define FDT_PARSER_HPP

#include "include/types.h"
#include "ulib/result.hpp"

// ACCORDING TO DTS SPECIFICATION
#define FDT_MAGIC 0xD00DFEED
#define FDT_BEGIN_NODE 0x00000001
#define FDT_END_NODE 0x00000002
#define FDT_PROP 0x00000003
#define FDT_NOP 0x00000004
#define FDT_END 0x00000009

// RETURN VALUES FOR TRAVERSAL FUNCTION
#define ALL_OK 0
#define INVALID_STRUCTURE_BLOCK -1

namespace hls {

// Declarations
// ---------------------------------------------------------------------------------------------------------------

struct __attribute__((packed)) fdt_header {
  uint32_t magic;
  uint32_t totalsize;
  uint32_t off_dt_struct;
  uint32_t off_dt_strings;
  uint32_t off_mem_rsvmap;
  uint32_t version;
  uint32_t last_comp_version;
  uint32_t boot_cpuid_phys;
  uint32_t size_dt_strings;
  uint32_t size_dt_struct;
};

struct __attribute__((packed)) fdt_prop_desc {
  uint32_t len;
  uint32_t nameoff;
};

class TraversalAction {
protected:
  TraversalAction() = default;

public:
  virtual void on_FDT_BEGIN_NODE(const fdt_header *header,
                                 const uint32_t *token) {}
  virtual void on_FDT_END_NODE(const fdt_header *header,
                               const uint32_t *token) {}
  virtual void on_FDT_PROP_NODE(const fdt_header *header,
                                const uint32_t *token) {}
  virtual void on_FDT_NOP_NODE(const fdt_header *header,
                               const uint32_t *token) {}

  virtual bool is_action_satisfied() const { return false; }
};

class FdtEngine {
  static const uint32_t *get_aligned_after_offset(const uint32_t *ptr,
                                                  size_t offset);
  static const uint32_t *move_to_next_token(const uint32_t *token_ptr);

public:
  static uint32_t read_value(const uint32_t *ptr);
  static const uint32_t *get_structure_block_ptr(const fdt_header *header);
  static const char *get_string_block_ptr(const fdt_header *header);
  static int traverse_node(const uint32_t *&token_ptr, const fdt_header *header,
                           TraversalAction &action);
};

class FdtNode {

protected:
  const uint32_t *m_node_token_start = nullptr;
  const fdt_header *m_fdt_header = nullptr;

public:
  FdtNode(const uint32_t *first_token, const fdt_header *fdt_header);
  FdtNode() = default; // For invalid nodes
  ~FdtNode() = default;

  FdtNode get_sub_node(const char *node_name,
                       const char *unit_address = nullptr) const;
  const void *get_property(const char *property_name) const;
  bool has_property(const char *property_name) const;
  virtual bool is_valid() const;
};

class FDT : public FdtNode {

public:
  FDT(const fdt_header *fdt) {
    // This might not work as expected in some platforms
    uintptr_t ptr_as_uint = reinterpret_cast<uintptr_t>(fdt);

    // The FDT has to be aligned to a 8 byte boundary
    if (ptr_as_uint % 8 == 0) {
      auto magic_number = FdtEngine::read_value(&fdt->magic);
      if (magic_number == FDT_MAGIC) {
        m_fdt_header = fdt;
        m_node_token_start = FdtEngine::get_structure_block_ptr(fdt);
      }
    }
  }
};

class StructValidator : public TraversalAction {
public:
  virtual void on_FDT_BEGIN_NODE(const fdt_header *header,
                                 const uint32_t *token) {}
  virtual void on_FDT_END_NODE(const fdt_header *header,
                               const uint32_t *token) {}
  virtual void on_FDT_PROP_NODE(const fdt_header *header,
                                const uint32_t *token) {}
  virtual void on_FDT_NOP_NODE(const fdt_header *header,
                               const uint32_t *token) {}
};

class NodeFinder : public TraversalAction {
  FdtNode m_result;
  const char *m_node_name;
  const char *m_unit_address;

  bool is_same_as(const char *node_string);

public:
  NodeFinder(const char *node_name, const char *unit_address = nullptr);

  void on_FDT_BEGIN_NODE(const fdt_header *header,
                         const uint32_t *token) override;

  bool is_action_satisfied() const override;

  FdtNode result() const;
};

class PropertyFinder : public TraversalAction {
  const void *m_result = nullptr;
  const char *m_looked_property = nullptr;
  uint32_t m_property_length = 0;
  // Given that a property can be empty, we have to flag if it has been found,
  // as when retrieving the content the user might receive a nullptr
  bool m_property_found = false;

public:
  PropertyFinder(const char *to_look_for) { m_looked_property = to_look_for; };

  void on_FDT_PROP_NODE(const fdt_header *header,
                        const uint32_t *token) override;

  virtual bool is_action_satisfied() const;

  // The user has to be aware on how to interpret the property, given that the
  // data is specific to each property
  const void *property_content() const;
  uint32_t property_length() const;
  bool is_property_found() const;
};

} // namespace hls

#endif
