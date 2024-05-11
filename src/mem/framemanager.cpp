/*---------------------------------------------------------------------------------
MIT License

Copyright (c) 2024 Helio Nunes Santos

        Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
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

---------------------------------------------------------------------------------*/

#include "mem/framemanager.hpp"
#include "mem/mmap.hpp"
#include "misc/libfdt/libfdt.h"
#include "sys/print.hpp"

#define FRAMEMANAGEMENT_BEGIN 0x1000

namespace hls {

enum Color {
    BLACK = 0,
    RED = 1
};

struct FrameNode : frame_info {
    FrameNode *left = nullptr;
    FrameNode *right = nullptr;
    FrameNode *parent = nullptr;
    size_t use_count;
    Color c;
};

class FrameNodeAllocator {
    byte *m_nodes_vaddress_begin = nullptr;
    byte *m_nodes_vaddress_current = nullptr;

    FrameNode *m_nodes_list;
    FrameNodeAllocator() = default;

  public:
    FrameNode *get() {
        if (m_nodes_list == nullptr) {
            return nullptr;
        }
        auto temp = m_nodes_list;
        m_nodes_list = m_nodes_list->left;
        return temp;
    }

    void release(FrameNode *f) {
        f->left = m_nodes_list;
        m_nodes_list = f;
    }

    void set_nodes_vaddress_begin(byte *vaddress) {
        if (m_nodes_vaddress_begin == nullptr) {
            m_nodes_vaddress_begin = vaddress;
            m_nodes_vaddress_current = vaddress;
        }
    }

    size_t map_node_frame(FrameKB *p_frame, FrameKB **frames, size_t count) {
        auto vaddress = m_nodes_vaddress_current;
        size_t usedframes = kmmap(p_frame, m_nodes_vaddress, get_kernel_pagetable(), FrameLevel::KB_VPN,
                                  READ | WRITE | ACCESS | DIRTY, frames, *count);
        m_nodes_vaddress += FrameInfo<FrameLevel::KB_VPN>::s_size;

        FrameNode *list_first = reinterpret_cast<FrameNode *>(vaddress);
        FrameNode *list_last = nullptr;
        for (; vaddress < m_nodes_vaddress; vaddress += sizeof(FrameNode)) {
            auto frame = reinterpret_cast<FrameNode *>(vaddress);
            if (vaddress + sizeof(FrameNode) >= m_nodes_vaddress) {
                list_last = frame;
                frame->left = nullptr;
            } else {
                frame->left = frame + 1;
            }
        }

        list_last->left = m_nodes_list;
        m_nodes_list = list_first;

        return usedframes;
    };

    static FrameNodeAllocator &instance() {
        static FrameNodeAllocator s_allocator;
        return s_allocator;
    }
};

class PageFrameManager {
    FrameNode *m_free = nullptr;
    FrameNode *m_used = nullptr;
    FrameNode *m_nodes_list = nullptr;
    FrameNode m_null;

    FrameNode *null() {
        return &m_null;
    }

    FrameNode *find_minimum(FrameNode *n) {
        FrameNode *ret_val = n;
        while (n != null()) {
            ret_val = n;
            n = n->left;
        }
        return ret_val;
    }

    void transplant(FrameNode *n, FrameNode *sn, FrameNode **tree) {
        if (n == null())
            return;

        FrameNode *p = n->parent;
        if (p != null() && p != nullptr) {
            if (p->left == n)
                p->left = (sn);
            if (p->right == n)
                p->right = (sn);
        } else {
            *tree = sn;
            sn->parent = n->parent;
        }

        if (sn) {
            if (sn != n->left)
                sn->left = (n->left);
            if (sn != n->right)
                sn->right = (n->right);
            if (sn->left)
                sn->left->parent = (sn);
            if (sn->right)
                sn->right->parent = (sn);
            sn->parent = (n->parent);
        }
    }

    bool is_left_child(FrameNode *n, FrameNode *p = nullptr) const {
        if (n != nullptr) {
            if (n->parent != nullptr) {
                return n->parent->left == n;
            }
        }

        if (p != nullptr) {
            if (p->left == n)
                return true;
        }
        return false;
    }

    bool is_right_child(FrameNode *n, FrameNode *p = nullptr) const {
        if (n != nullptr) {
            if (n->parent != nullptr) {
                return n->parent->right == n;
            }
        }
        if (p) {
            if (n == p->right)
                return true;
        }
        return false;
    }

    void rotate_left(FrameNode *n, FrameNode **tree) {
        if (n == nullptr || n == null())
            return;

        FrameNode *nr = n->right;

        if (nr) {
            n->right = nr->left;

            if (n->right)
                n->right->parent = n;

            nr->left = (n);
            nr->parent = (n->parent);
            if (is_left_child(n))
                n->parent->left = (nr);
            else if (is_right_child(n))
                n->parent->right = (nr);
            else
                *tree = nr;

            n->parent = (nr);
        }
    }

    void rotate_right(FrameNode *n, FrameNode **tree) {
        if (n == nullptr || n == null())
            return;

        FrameNode *nl = n->left;
        if (nl) {
            n->left = (nl->right);
            if (n->left)
                n->left->parent = (n);

            nl->right = (n);
            nl->parent = (n->parent);

            if (is_left_child(n))
                n->parent->left = (nl);
            else if (is_right_child(n))
                n->parent->right = (nl);
            else
                *tree = nl;

            n->parent = (nl);
        }
    }

    bool is_black(FrameNode *n) {
        return !is_red(n);
    }

    bool is_red(FrameNode *n) {
        if (n == nullptr || n->c == Color::BLACK)
            return false;
        return true;
    }

    FrameNode *find_helper(FrameNode *node, FrameNode **parent_save, FrameNode **tree) {
        FrameNode *current = *tree;
        *parent_save = null();
        while (current != null()) {
            if (node->frame_pointer < current->frame_pointer) {
                *parent_save = current;
                current = current->left;
                continue;
            }

            if (node->frame_pointer > current->frame_pointer) {
                *parent_save = current;
                current = current->right;
                continue;
            }
            break;
        }
        return current;
    }

    void insert(FrameNode *n, FrameNode **tree) {
        n->c = Color::RED;
        n->left = null();
        n->right = null();
        n->parent = null();

        if (*tree == null() || *tree == nullptr) {
            n->c = BLACK;
            n->parent = nullptr;
            *tree = n;
            return;
        }

        FrameNode *p = nullptr;
        find_helper(n, &p, tree);
        if (n->frame_pointer < p->frame_pointer) {
            p->left = n;
        } else {
            p->right = n;
        }
        n->parent = p;
        insert_fix(n, tree);

        (*tree)->parent = nullptr;
    }

    void insert_fix(FrameNode *n, FrameNode **tree) {
        FrameNode *p = nullptr;
        FrameNode *u = nullptr;
        FrameNode *gp = nullptr;

        if (n == *tree) {
            n->c = Color::BLACK;
            return;
        }

        p = n->parent;

        if (!is_red(n) || !is_red(p))
            return;

        if (p)
            gp = p->parent;
        if (gp)
            u = gp->left == p ? gp->right : gp->left;

        if (is_red(u)) {
            gp->c = (Color::RED);
            u->c = (Color::BLACK);
            p->c = (Color::BLACK);
            insert_fix(gp, tree);
        } else if (is_black(u)) {
            if (is_right_child(p)) {
                if (is_right_child(n)) {
                    rotate_left(gp, tree);
                    p->c = (Color::BLACK);
                    gp->c = (Color::RED);
                } else if (is_left_child(n)) {
                    n->c = (Color::BLACK);
                    p->c = (Color::RED);
                    gp->c = (Color::RED);
                    rotate_right(n, tree);
                    rotate_left(p, tree);
                }
            } else if (is_left_child(p)) {
                if (is_left_child(n)) {
                    rotate_right(gp, tree);
                    p->c = (Color::BLACK);
                    gp->c = (Color::RED);
                } else if (is_right_child(n)) {
                    n->c = (Color::BLACK);
                    p->c = (Color::RED);
                    gp->c = (Color::RED);
                    rotate_left(n, tree);
                    rotate_right(p, tree);
                }
            }
        }
    }

    FrameNode *remove(FrameNode *n, FrameNode **tree) {
        FrameNode *x = null();
        Color original_color = n->c;

        if (n->left == null()) {
            x = n->right;
            transplant(n, x, tree);
        } else if (n->right == null()) {
            x = n->left;
            transplant(n, x, tree);
        } else {
            FrameNode *y = find_minimum(n->right);
            original_color = y->c;
            x = y->right;
            if (y == n->right) {
                x->parent = (y);
            } else {
                transplant(y, x, tree);
                x->parent = (y->parent);
            }
            transplant(n, y, tree);
            y->c = (original_color);
        }

        if (original_color == Color::BLACK)
            remove_fix(x, tree);

        (*tree)->parent = nullptr;

        n->left = null();
        n->right = null();
        n->parent = null();

        return n;
    }

    void remove_fix(FrameNode *n, FrameNode **tree) {
        while (n != *tree && is_black(n)) {
            FrameNode *p = n->parent;
            FrameNode *s;
            if (is_left_child(n, p)) {
                s = p->right;
                if (is_red(s)) {
                    s->c = (Color::BLACK);
                    p->c = (Color::RED);
                    rotate_left(p, tree);
                    s = p->right;
                }
                if (is_black(s->left) && is_black(s->right)) {
                    s->c = (Color::RED);
                    n = p;
                    p = p->parent;
                } else {
                    if (is_black(s->right)) {
                        s->left->c = (Color::BLACK);
                        s->c = (Color::RED);
                        rotate_right(s, tree);
                        s = p->right;
                    }
                    s->c = (p->c);
                    p->c = (Color::BLACK);
                    s->right->c = (Color::BLACK);
                    rotate_left(p, tree);
                    n = *tree;
                }
            } else {
                s = p->left;
                if (is_red(s)) {
                    s->c = (Color::BLACK);
                    p->c = (Color::RED);
                    rotate_right(p, tree);
                    s = p->left;
                }
                if (is_black(s->right) && is_black(s->left)) {
                    s->c = (Color::RED);
                    n = p;
                    p = p->parent;
                } else {
                    if (is_black(s->left)) {
                        s->right->c = (Color::BLACK);
                        s->c = (Color::RED);
                        rotate_left(s, tree);
                        s = p->left;
                    }
                    s->c = (p->c);
                    p->c = (Color::BLACK);
                    s->left->c = (Color::BLACK);
                    rotate_right(p, tree);
                    n = *tree;
                }
            }
        }
        if (n)
            n->c = (Color::BLACK);
    }

    FrameNode *get_in_order_successor(FrameNode *n) {
        if (n != null()) {
            if (n->right != null()) {
                return find_minimum(n->right);
            } else {
                while (n != nullptr && !is_left_child(n)) {
                    n = n->parent;
                }
                if (n != nullptr)
                    return n->parent;
            }
        }

        return null();
    }

    static PageFrameManager &__internal_instance(void *mem, size_t size, frame_fn f_alloc) {
        static PageFrameManager m(mem, size, f_alloc);
        return m;
    }

    PageFrameManager::PageFrameManager(void *mem, size_t size, frame_fn f_alloc) {
        m_null.frame_pointer = nullptr;
        m_null.c = Color::BLACK;
        m_null.left = nullptr;
        m_null.right = nullptr;
        m_null.parent = nullptr;

        m_free = null();
        m_used = null();

        void *p_frame_begin = mem;
        void *p_frame_end = align_back(apply_offset(p_frame_begin, size), FrameKB::s_alignment);

        FrameKB *p_begin = reinterpret_cast<FrameNode *>(align_forward(mem, FrameKB::s_alignment));
        FrameKB *p_end = reinterpret_cast<FrameKB *>(p_frame_end);

        auto &f_node_alloc = FrameNodeAllocator::instance();

        byte *vaddress_begin = nullptr;
        vaddress_begin = vaddress_begin + FRAMEMANAGEMENT_BEGIN;
        f_node_alloc.set_nodes_vaddress_begin(vaddress_begin);

        const size_t f_count = reinterpret_cast<size_t>(FrameLevel::LAST_VPN);
        FrameKB *frames[f_count];
        size_t used_frames = f_node_alloc.map_node_frame(p_begin, frames, f_count);
        p_begin += 1;
        p_begin += used_frames;

        size_t count = p_end - p_begin;

        auto node = f_node_alloc.get();

        node->frame_pointer = p_begin;
        node->size = count * FrameKB::s_size;
        node->use_count = 0;
        insert(node, &m_free);
    }

    void validate_and_fix_node_allocator() {
        auto &allocator = FrameNodeAllocator::instance();
        auto ptr = allocator.get();
        if (ptr != nullptr) {
            allocator.release(ptr);
            return;
        }

        const size_t f_count = reinterpret_cast<size_t>(FrameLevel::LAST_VPN);
        FrameKB *frames[f_count + 1];

        for (size_t i = 0; i < f_count + 1;) {
            auto min = find_minimum(m_free);
            remove(min, &m_free);
            while (min->size && i < f_count + 1) {
                frames[i++] = min->frame_pointer;
                min->size -= FrameKB::s_size;
                min->frame_pointer += 1;
            }

            if (min->size) {
                insert(min, &m_free);
            } else {
                allocator.release(min);
            }
        }

        size_t used = allocator.map_node_frame(frames[0], frames + 1, f_count);

        while (used < f_count) {
            auto node = allocator.get();
            node->frame_pointer = frames[used];
            node->size = FrameKB::s_size;
            node->use_count = 1;
            insert(node, &m_used);
            // Let the releasing algorithm figure out joining frames.
            release_frame(frames[used]);
        }
    }

  public:
    static PageFrameManager &instance() {
        // TODO: PANIC IF UNITIALIZED
        return __internal_instance(nullptr, 0, nullptr);
    }

    static bool init(void *mem, size_t mem_size, frame_fn f_alloc) {
        __internal_instance(mem, mem_size, f_alloc);
        return false;
    }

    bool is_valid_frame(FrameNode *frame, size_t needed_size, size_t needed_alignment) {
    }

    frame_info *get_frames(size_t size, size_t alignment, uint64_t flags) {
        // Needed when getting a new frame, given that frames can be split.
        validate_and_fix_node_allocator();

        if (m_free == null())
            return nullptr;

        bool is_alignment_valid = alignment % FrameKB::s_alignment == 0;
        if (!is_alignment_valid)
            return nullptr;

        auto min = find_minimum(m_free);
    }

    void release_frames(void *frames) {
    }
};

bool is_memory_node(void *fdt, int node) {
    const char *memory_string = "memory@";
    int len = 0;
    const char *node_name = fdt_get_name(fdt, node, &len);

    if (len) {
        return strncmp(memory_string, node_name, strlen(memory_string)) == 0;
    }

    return false;
}

void initialize_frame_manager(void *fdt, bootinfo *b_info, frame_fn f_alloc) {
    byte *mem = b_info->p_kernel_physical_end;
    for (auto node = fdt_first_subnode(fdt, 0); node >= 0; node = fdt_next_subnode(fdt, node)) {
        if (is_memory_node(fdt, node)) {
            size_t address_cells = fdt_address_cells(fdt, 0);
            size_t size_cells = fdt_size_cells(fdt, 0);

            int len;
            const struct fdt_property *prop = fdt_get_property(fdt, node, "reg", &len);

            auto get_address = [&](size_t idx) {
                const char *d = reinterpret_cast<const char *>(prop->data);
                d += idx * (sizeof(uint32_t) * address_cells + sizeof(uint32_t) * size_cells);
                uintptr_t p = 0;

                if (address_cells == 1)
                    p = fdt32_ld(reinterpret_cast<const fdt32_t *>(d));
                else if (address_cells == 2)
                    p = fdt64_ld(reinterpret_cast<const fdt64_t *>(d));

                return to_ptr(p);
            };

            auto get_size = [&](size_t idx) {
                const char *d = reinterpret_cast<const char *>(prop->data);
                d += idx * (sizeof(uint32_t) * address_cells + sizeof(uint32_t) * size_cells);
                size_t p = 0;

                d += address_cells * sizeof(uint32_t);

                if (size_cells == 1)
                    p = fdt32_ld(reinterpret_cast<const fdt32_t *>(d));
                else if (size_cells == 2)
                    p = fdt64_ld(reinterpret_cast<const fdt64_t *>(d));

                return p;
            };

            byte *mem_temp = reinterpret_cast<byte *>(get_address(0));
            size_t mem_size = get_size(0);

            mem_size -= (mem - mem_temp);

            PageFrameManager::init(mem, mem_size, f_alloc);
            break;
        }
    }
}

} // namespace hls
