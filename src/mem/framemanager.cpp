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
#include "arch/riscv64gc/plat_def.hpp"

namespace hls {

enum Color {
    BLACK = 0,
    RED = 1
};

struct FrameNode {
    FrameNode *left = nullptr;
    FrameNode *right = nullptr;
    FrameNode *parent = nullptr;
    PageKB *frame_pointer = nullptr;
    Color c;
};

LKERNELFUN char *as_char_p(void *p) {
    return reinterpret_cast<char *>(p);
}

class PageFrameManager {
    FrameNode *m_free = nullptr;
    FrameNode *m_used = nullptr;
    FrameNode m_null;

    LKERNELCLSSFUN FrameNode *null() {
        return &m_null;
    }

    LKERNELCLSSFUN FrameNode *find_minimum(FrameNode *n) {
        FrameNode *ret_val = n;
        while (n != null()) {
            ret_val = n;
            n = n->left;
        }
        return ret_val;
    }

    LKERNELCLSSFUN void transplant(FrameNode *n, FrameNode *sn, FrameNode **tree) {
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

    LKERNELCLSSFUN bool is_left_child(FrameNode *n, FrameNode *p = nullptr) const {
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

    LKERNELCLSSFUN bool is_right_child(FrameNode *n, FrameNode *p = nullptr) const {
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

    LKERNELCLSSFUN void rotate_left(FrameNode *n, FrameNode **tree) {
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

    LKERNELCLSSFUN void rotate_right(FrameNode *n, FrameNode **tree) {
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

    LKERNELCLSSFUN bool is_black(FrameNode *n) {
        return !is_red(n);
    }

    LKERNELCLSSFUN bool is_red(FrameNode *n) {
        if (n == nullptr || n->c == Color::BLACK)
            return false;
        return true;
    }

    LKERNELCLSSFUN FrameNode *find_helper(FrameNode *node, FrameNode **parent_save, FrameNode **tree) {
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

    LKERNELCLSSFUN void insert(FrameNode *n, FrameNode **tree) {
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

    LKERNELCLSSFUN void insert_fix(FrameNode *n, FrameNode **tree) {
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

    LKERNELCLSSFUN FrameNode *remove(FrameNode *n, FrameNode **tree) {
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

    LKERNELCLSSFUN void remove_fix(FrameNode *n, FrameNode **tree) {
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

    LKERNELCLSSFUN static PageFrameManager &__internal_instance(void *mem, size_t size) {
        LKERNELSDATA static PageFrameManager m(mem, size);
        return m;
    }

    LKERNELFUN PageFrameManager(void *mem, size_t size);

  public:
    LKERNELCLSSFUN PageKB *get_frame() {
        if (m_free == null())
            return nullptr;

        auto node = find_minimum(m_free);
        remove(node, &m_free);
        insert(node, &m_used);
        return node->frame_pointer;
    }

    LKERNELCLSSFUN void release_frame(void *frame) {
        if (m_used == null() || m_used == nullptr)
            return;

        FrameNode n;
        n.frame_pointer = reinterpret_cast<PageKB *>(frame);
        FrameNode *p = nullptr;
        auto node = find_helper(&n, &p, &m_used);

        if (node != null()) {
            node = remove(node, &m_used);
            insert(node, &m_free);
        }
    }

    LKERNELCLSSFUN static PageFrameManager &instance() {
        return __internal_instance(nullptr, 0);
    }

    LKERNELCLSSFUN static void init(void *mem, size_t mem_size) {
        __internal_instance(mem, mem_size);
    }
};

LKERNELFUN PageFrameManager::PageFrameManager(void *mem, size_t size) {
    m_null.frame_pointer = nullptr;
    m_null.c = Color::BLACK;
    m_null.left = nullptr;
    m_null.right = nullptr;
    m_null.parent = nullptr;

    m_free = null();
    m_used = null();

    uintptr_t p_begin = reinterpret_cast<uintptr_t>(mem);
    size_t align_begin = p_begin % sizeof(FrameNode);
    if (align_begin) {
        p_begin += align_begin;
    }

    uintptr_t p_end = reinterpret_cast<uintptr_t>(mem) + size;
    size_t align_end = p_end % 4096;
    if (align_end) {
        p_end = p_end - (4096 - (p_end % 4096));
    }

    FrameNode *nodes = reinterpret_cast<FrameNode *>(p_begin);
    PageKB *frames = reinterpret_cast<PageKB *>(p_end);

    size_t count = 0;

    while (as_char_p(nodes + 1) < as_char_p(frames - 1)) {
        ++nodes;
        --frames;
        ++count;
    }

    nodes = reinterpret_cast<FrameNode *>(p_begin);

    for (size_t i = 0; i < count; ++i) {
        nodes->frame_pointer = frames;
        insert(nodes, &m_free);
        ++nodes;
        ++frames;
    }
}

LKERNELFUN void *get_frame() {
    return PageFrameManager::instance().get_frame();
}
LKERNELFUN void release_frame(void *frame) {
    return PageFrameManager::instance().release_frame(frame);
}
LKERNELFUN void initialize_frame_manager(void *mem, size_t mem_size) {
    PageFrameManager::init(mem, mem_size);
}

} // namespace hls
