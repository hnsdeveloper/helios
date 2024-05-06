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

#ifndef _PAGING_HPP_
#define _PAGING_HPP_

#include "include/arch/riscv/plat_def.hpp"
#include "include/symbols.hpp"
#include "misc/libfdt/libfdt.h"
#include "ulib/bit.hpp"
#include "ulib/reference.hpp"
#include "ulib/result.hpp"

const size_t BITSET_SIZE = sizeof(uint64_t) * 8;

namespace hls {

using BMap = Bit<BITSET_SIZE>;

/**
 * @brief PageFrameManager class. Manages 4096KB page frames, to be used either
 * as page frames or page tables.
 * @todo Implement different sized page frames.
 *
 * @remark Thread safety: ST.
 */
class PageFrameManager {
    PageKB *m_frames = nullptr; // Pointer to page frames array
    BMap *m_bitmap = nullptr;   // Pointer to bitmap array. Frames are know to be
                                // used/free throght this.

    size_t m_frame_count = 0;  // Total frame count.
    size_t m_bitmap_count = 0; // Count of BITSET_SIZE bitmaps.

    size_t m_free_frames = 0; // Total free frames.

    /**
     * @brief Finds the first free frame index.
     * @remark Thread safety: ST.
     * @return Result<size_t> Contains index on success or error on fail.
     * If no free frame is found, it returns Error::OUT_OF_MEMORY.
     */
    Result<size_t> find_free_frame();

    /**
     * @brief Finds the first free frame index.
     * @remark Thread safety: ST.
     * @return Result<size_t> Contains index on success or error on fail.
     * If no free frame is found, it returns Error::OUT_OF_MEMORY.
     */
    Result<size_t> find_free_frames(size_t frames);

    /**
     * @brief Set bit found at index idx to value (true or false).
     * @remark Thread safety: ST.
     * @param idx The bit index.
     * @param value True or false.
     */
    void set_bit(size_t idx, bool value);

    /**
     * @brief Returns instance to static PageFrameManager. On first call, it
     * initializes an instance.
     * @remark Thread safety: ST.
     * @param base_address The memory address to which PageFrameManager will start
     * working on.
     * @param mem_size The size of the available memory from base_address.
     * @return PageFrameManager& A static PageFrameManager instance.
     */
    static PageFrameManager &__internal_instance(void *base_address, size_t mem_size);

    /**
     * @brief Construct a new Page Frame Manager object
     * @param base_address The memory address to which PageFrameManager will start
     * working on.
     * @param mem_size The size of the available memory from base_address.
     */
    PageFrameManager(void *base_address, size_t mem_size);

  public:
    PageFrameManager(const PageFrameManager &) = delete;
    PageFrameManager(PageFrameManager &&) = delete;
    ~PageFrameManager() {
    }

    /**
     * @brief How many frames the PageFrameManager manages.
     * @remark Thread safety: MT.
     * @return size_t Count of how many frames it manages.
     */
    size_t frame_count() const;

    /**
     * @brief How many frames are still free.
     * @remark Thread safety: MT.
     * @return size_t Count of free frames.
     */
    size_t free_frames() const;

    /**
     * @brief How many frames are in use.
     * @remark Thread safety: MT.
     * @return size_t Count of used frames.
     */
    size_t used_frames() const;

    /**
     * @brief Get a free 4096KB page frame at a random address and marks it as
     * used.
     * @remark Thread safety: ST.
     * @return Result<PageKB *> Contains a pointer to a page frame on success or
     * an error value on fail.
     */
    Result<PageKB *> get_frame();

    /**
     * @brief Get a free 4096KB page frame at a random address and marks it as
     * used.
     * @remark Thread safety: ST.
     * @return Result<PageKB *> Contains a pointer to a page frame on success or
     * an error value on fail.
     */
    Result<PageKB *> get_frames(size_t frames);

    /**
     * @brief Release a page frame.
     * @remark Thread safety: ST.
     * @param frame Pointer to frame to be released.
     */
    void release_frame(void *frame);

    void release_frames(void *f, size_t frames);

    /**
     * @brief Instance to PageFrameManager singleton.
     * @remark Thread safety: MT.
     * @return PageFrameManager& Singleton instance of PageFrameManager or panics
     * on failure.
     */
    static PageFrameManager &instance();

    /**
     * @brief Initializes singleton instance of PageFrameManager.
     * @remark Thread safety: ST.
     * @param base_address The physical memory address from which PageFrameManager
     * will manage pageframes.
     * @param mem_size Memory size in bytes (end_address - base_address).
     * @return True if it is initialized successfully
     * @return False if it is not initialzied succesfully
     */
    static bool init(void *base_address, size_t mem_size);
};

void setup_page_frame_manager(void *fdt);

} // namespace hls

#endif