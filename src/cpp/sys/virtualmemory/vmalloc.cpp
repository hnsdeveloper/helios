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

#include "include/arch/riscv/plat_def.hpp"
#include "sys/virtualmemory/paging.hpp"
#include "sys/virtualmemory/vmalloc.hpp"
#include "sys/virtualmemory/kmalloc.hpp"
#include "sys/virtualmemory/memmap.hpp"
#include "ulib/list.hpp"
#include "ulib/map.hpp"

#include "include/macros.hpp"

namespace hls {

class VirtualMemoryAllocator {
    template<typename K, typename V>
    using map = Map<K,V, KMAllocator>;
    using _amap = map<uintptr_t, size_t>;
    SET_USING_CLASS(_amap, address_map);

    address_map m_reserved_addresses;
    map<PageTable*, address_map> m_used_addresses_map;
    
    void* determine_vaddress(size_t b, PageTable* p) {
        

    }

    static VirtualMemoryAllocator& __internal_instance(address_map* reserved_addresses) {
        static VirtualMemoryAllocator vma {hls::move(*reserved_addresses)};
        return vma;
    }

    VirtualMemoryAllocator(address_map&& reserved_addresses) : m_reserved_addresses(hls::move(reserved_addresses)) {};

public:
    VirtualMemoryAllocator(const VirtualMemoryAllocator&) = delete;
    VirtualMemoryAllocator(VirtualMemoryAllocator&&) = delete;
    ~VirtualMemoryAllocator() = default;

    void* allocate(size_t b, PageTable* p) {
        using list = List<void*, KMAllocator>;
        void* vaddress = determine_vaddress(b, p);
        auto& used_addresses = m_used_addresses_map[p];
        
        size_t page_count = b / PAGE_FRAME_SIZE + (b % PAGE_FRAME_SIZE ? 1 : 0);
        used_addresses[to_uintptr_t(vaddress)] = page_count * PAGE_FRAME_SIZE;

        list pages;
        auto& frame_manager = PageFrameManager::instance();
        for(size_t i = 0; i < page_count; ++i) {
            auto result = frame_manager.get_frame();
            if(result.is_error()) {
                // TODO: HANDLE ERROR
                return nullptr;
            }
            pages.push_front(result.get_value());
        }

        list::iterator it = pages.begin();
        for(size_t i = 0; i < page_count; ++i) {
            byte* c = reinterpret_cast<byte*>(vaddress);
            c = c + i * PAGE_FRAME_SIZE;
            kmmap(*it, c, p, PageLevel::KB_VPN, true, false);
        }

        return vaddress;
    }

    void* reallocate(void* p, size_t b, PageTable* t) {
        using list = List<void*, KMAllocator>;
        
        if(m_used_addresses_map.find(t) == m_used_addresses_map.end())
            return nullptr;

        auto& used_addresses = m_used_addresses_map[t];
        auto it = used_addresses.find(to_uintptr_t(p));

        if(it != used_addresses.end()) {
            uintptr_t addr = it->first;
            size_t size = it->second;

            if(b < size) {
                return p;
            }

            size_t old_page_count = size/PAGE_FRAME_SIZE;
            size_t new_page_count = b/PAGE_FRAME_SIZE + (b % PAGE_FRAME_SIZE ? 1 : 0);

            list pages;
            auto& frame_manager = PageFrameManager::instance();

            if((++it != used_addresses.end()) && (addr + new_page_count * PAGE_FRAME_SIZE <= it->first)) {
                for(size_t i = 0; i < new_page_count - old_page_count; ++i) {
                    auto result = frame_manager.get_frame();
                    if(result.is_error()) {
                        //TODO: HANDLE ERROR

                        return nullptr;
                    }
                    pages.push_front(result.get_value());       
                }

                list::iterator it = pages.begin();
                for(size_t i = old_page_count; i < new_page_count; ++i) {
                    byte* c = reinterpret_cast<byte*>(p);
                    c = c + i * PAGE_FRAME_SIZE;
                    kmmap(*it, c, t, PageLevel::KB_VPN, true, false);
                }

                return p;
            }
        }

        return nullptr;
    }

    void free(void* ptr, PageTable* t) {
        if(m_used_addresses_map.find(t) == m_used_addresses_map.end())
            return;
        
        auto& used_addresses = m_used_addresses_map[t];

        if(used_addresses.find(to_uintptr_t(ptr)) != used_addresses.end()) {
            size_t blocks = used_addresses[to_uintptr_t(ptr)] / PAGE_FRAME_SIZE;
            for(size_t i = 0; i < blocks; ++i) {
                byte* c = reinterpret_cast<byte*>(ptr);
                c = c + i * PAGE_FRAME_SIZE;
                void* frame = get_physical_address(t, c).get_value();
                kmunmap(c, t);
                PageFrameManager::instance().release_frame(frame);
            }
            
            auto it = used_addresses.find(to_uintptr_t(ptr));
            used_addresses.erase(it);
        }

        if(used_addresses.size() == 0) {
            auto it = m_used_addresses_map.find(t);
            m_used_addresses_map.erase(it);
        }
    }

    static bool init(address_map reserved_addresses = {}) {
        static bool is_initialized = false;
        if (!is_initialized) {
            if (reserved_addresses.size() == 0)
                return is_initialized;
            else
                __internal_instance(&reserved_addresses);
            is_initialized = true;
        }

        return is_initialized;
    }

    static VirtualMemoryAllocator& instance() {
        if(init() == false)
            PANIC("VirtualMemoryAllocator must be initialized before use.");
        return __internal_instance(nullptr);
    }
};




void initialize_vmalloc() {
    
}


}