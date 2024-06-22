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
#include "misc/macros.hpp"
#include "misc/types.hpp"
#include "sys/bootdata.hpp"
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

        FrameData(FrameKB *f_ptr, size_t frame_count, size_t flags)
            : m_frame_pointer(f_ptr), m_frame_count(frame_count),  m_use_count(1) ,m_flags(flags)
        {
        }

        template <typename U>
        FrameData(FrameKB *f_ptr, size_t frame_count, size_t flags, U &&data) : FrameData(f_ptr, frame_count, flags)
        {
            m_userdata = hls::move(data);
        }

        ~FrameData() = default;

        template <typename U>
        void set_userdata(U &&newdata)
        {
            m_userdata = hls::move(newdata);
        }

        UserData &get_userdata()
        {
            const auto &as_const = *this;
            return const_cast<UserData &>(as_const.get_userdata());
        }

        const UserData &get_userdata() const
        {
            return m_userdata;
        }

        void set_flags(uint64_t flags)
        {
            m_flags = flags;
        }

        size_t get_flags() const
        {
            return m_flags;
        }

        size_t get_frame_count() const
        {
            return m_frame_count;
        }

        size_t size() const
        {
            return m_frame_count * FrameKB::s_size;
        }
        
        const FrameKB* get_frame_pointer() const
        {
            return m_frame_pointer;
        }
    };

    struct frame_info
    {
        FrameKB *frame_pointer = nullptr;
        size_t frame_count = 0;
        size_t use_count = 0;
        uint64_t flags = 0;

        size_t size()
        {
            return FrameKB::s_size * frame_count;
        }
    };

    const frame_info *framealloc(size_t count, uint64_t flags);
    const frame_info *framealloc(uint64_t flags);
    void framefree(void *);

    PageTable *init_initfalloc(size_t used, PageTable *tables);
    void *initfalloc();
    void initffree(void *);

    void *get_frame_management_begin_vaddress();
    void *get_frame_management_end_vaddress();

    void initialize_frame_manager(void *fdt, bootinfo *b_info);

    // TODO: REMOVE
    void *kmmap_frame_src();

} // namespace hls
