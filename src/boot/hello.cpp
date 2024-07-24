#include "ulib/singleton.hpp"
#include <efi.h>

class EfiSystemTableWrapper : public hls::Singleton<EfiSystemTableWrapper>
{
    EFI_SYSTEM_TABLE *m_system_table;

  public:
    EfiSystemTableWrapper(EFI_SYSTEM_TABLE *system_table) : m_system_table(system_table) {};

    void console_out(const CHAR16 *str)
    {
        m_system_table->ConOut->OutputString(m_system_table->ConOut, const_cast<CHAR16 *>(str));
    }
};

void printstr(const wchar_t *str)
{
    EfiSystemTableWrapper::get_global_instance().console_out(reinterpret_cast<const CHAR16 *>(str));
}

extern "C" EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    (void)ImageHandle;
    EfiSystemTableWrapper::initialize_global_instance(SystemTable);
    printstr(L"Hello world\r\n");
    // SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16*)(L"Hello world\r\n"));

    for (;;)
        ;
}
