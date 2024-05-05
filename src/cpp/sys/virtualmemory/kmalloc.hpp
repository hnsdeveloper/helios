#ifndef _LIBALLOC_H
#define _LIBALLOC_H

#include "include/macros.hpp"
#include "include/types.hpp"
#include "include/utilities.hpp"
#include "misc/new.hpp"

// This lets you prefix malloc and friends
#define PREFIX(func) k##func

#ifdef __cplusplus
extern "C" {
#endif

/** This function is supposed to lock the memory data structures. It
 * could be as simple as disabling interrupts or acquiring a spinlock.
 * It's up to you to decide.
 *
 * \return 0 if the lock was acquired successfully. Anything else is
 * failure.
 */
extern int liballoc_lock();

/** This function unlocks what was previously locked by the liballoc_lock
 * function.  If it disabled interrupts, it enables interrupts. If it
 * had acquiried a spinlock, it releases the spinlock. etc.
 *
 * \return 0 if the lock was successfully released.
 */
extern int liballoc_unlock();

/** This is the hook into the local system which allocates pages. It
 * accepts an integer parameter which is the number of pages
 * required.  The page size was set up in the liballoc_init function.
 *
 * \return NULL if the pages were not allocated.
 * \return A pointer to the allocated memory.
 */
extern void *liballoc_alloc(size_t);

/** This frees previously allocated memory. The void* parameter passed
 * to the function is the exact same value returned from a previous
 * liballoc_alloc call.
 *
 * The integer value is the number of pages to free.
 *
 * \return 0 if the memory was successfully freed.
 */
extern int liballoc_free(void *, size_t);

extern void *PREFIX(malloc)(size_t);          ///< The standard function.
extern void *PREFIX(realloc)(void *, size_t); ///< The standard function.
extern void *PREFIX(calloc)(size_t, size_t);  ///< The standard function.
extern void PREFIX(free)(void *);             ///< The standard function.

#ifdef __cplusplus
}
#endif

namespace hls {

template <typename T> class KMAllocator {
    SET_USING_CLASS(T, type);

  public:
    KMAllocator() {
        m_i = 0;
    }

    size_t m_i;

    template <typename... Args> type_ptr create(Args... args) {
        type_ptr v = allocate();
        if (v != nullptr) {
            new (v) type(hls::forward<Args>(args)...);
        }

        return v;
    }

    void destroy(type_const_ptr p) {
        if (p == nullptr)
            return;

        type_ptr p_nc = const_cast<type_ptr>(p);
        (*p_nc).~type();
        deallocate(p_nc);
    }

    type_ptr allocate() {
        return reinterpret_cast<type_ptr>(kmalloc(sizeof(type)));
    }

    void deallocate(type_const_ptr p) {
        if (p == nullptr)
            return;
        type_ptr p_nc = const_cast<type_ptr>(p);
        kfree(p_nc);
    }
};

void initialize_kmalloc();

} // namespace hls

#endif
