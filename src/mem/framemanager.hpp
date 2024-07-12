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

#include "arch/riscv64gc/plat_def.hpp"
#include "mem/bumpallocator.hpp"
#include "mem/nodeallocator.hpp"
#include "misc/macros.hpp"
#include "misc/types.hpp"
#include "sys/bootdata.hpp"
#include "ulib/pair.hpp"
#include "ulib/rb_tree.hpp"
#include "ulib/singleton.hpp"
#include "ulib/variant.hpp"

#define FRAME_SWAPPABLE 1 << 0

namespace hls
{

    class FrameOwner
    {
    };

    struct placeholder_XXX
    {
    };

    class FrameData
    {
        using UserData = Variant<placeholder_XXX>;

        FrameKB *m_frame_pointer;
        size_t m_frame_count;
        size_t m_use_count;
        size_t m_flags;
        UserData m_userdata;

      public:
        FrameData() = default;
        FrameData(FrameKB *f_ptr, size_t frame_count, size_t flags);
        FrameData(FrameKB *f_ptr, size_t frame_count);
        template <typename U>
        FrameData(FrameKB *f_ptr, size_t frame_count, size_t flags, U &&data) : FrameData(f_ptr, frame_count, flags)
        {
            m_userdata = hls::move(data);
        }
        FrameData(const FrameData &other);
        FrameData(FrameData &&other);

        ~FrameData() = default;

        UserData &get_userdata();
        const UserData &get_userdata() const;

        void set_flags(uint64_t flags);
        size_t get_flags() const;
        size_t get_frame_count() const;
        size_t size() const;

        FrameKB *get_frame_pointer() const;
        FrameData &shrink_begin(size_t frames);
        FrameData &shrink_end(size_t frames);

        template <typename U>
        void set_userdata(U &&newdata)
        {
            m_userdata = hls::move(newdata);
        }
    };

    template <>
    class Hash<FrameData>
    {
        SET_USING_CLASS(FrameData, type);
        SET_USING_CLASS(uintptr_t, hash_result);

      public:
        hash_result operator()(type_const_reference v) const
        {
            return to_uintptr_t(v.get_frame_pointer());
        }
    };

    class FrameManager : public Singleton<FrameManager>
    {
        using tree = RedBlackTree<FrameData, Hash, LessComparator, NodeAllocator>;
        BumpAllocator m_bump_allocator;
        tree m_used_frames;
        tree m_free_frames;
        size_t m_frame_count;

        FrameManager();
        FrameManager(const FrameManager &) = delete;
        FrameManager(FrameManager &&) = delete;
        friend class Singleton<FrameManager>;

      public:
        void expand_memory(const Pair<void *, size_t> mem_info);
        FrameData *get_frames(size_t count, uint64_t flags);
        void release_frames(void *frame_pointer);
    };

    void initialize_frame_manager(void *fdt, bootinfo *b_info);

    PageTable *init_initfalloc(size_t used, PageTable *tables);
    void *initfalloc();
    void initffree(void *);
} // namespace hls
