#pragma once

// includes that are necessary for windows
#include <shobjidl.h>  // For Windows COM interface
#include <locale>
#include <codecvt>

#define MAX_PATH_SIZE 260

/// @brief              Releases a file dialog, deletes the filterSpec array and uninitializes the COM interface
/// @param p_fileDialog The dialog to release
/// @param p_filterSpec The filterSpec array to delete
inline void Release(IFileDialog* p_fileDialog, COMDLG_FILTERSPEC* p_filterSpec)
{
    delete[] p_filterSpec;
    p_fileDialog->Release();
    CoUninitialize();
}

/// @brief              Releases a shell item and call Release with the remaining parameters
/// @param p_shellItem  The shell item to release
/// @param p_fileDialog The dialog to release
/// @param p_filterSpec The filterSpec array to delete
inline void Release(IShellItem* p_shellItem, IFileDialog* p_fileDialog, COMDLG_FILTERSPEC* p_filterSpec)
{
    p_shellItem->Release();
    Release(p_fileDialog, p_filterSpec);
}

/// @brief            Opens a file dialog for the user to select a file, limiting the files shown to the provided parameters
/// @param p_buf      A buffer to write the filename to
/// @param p_names    The names of the types of file to select
/// @param p_exts     The extension filters of the types of file to select
///	@param p_extCount The amount of extensions/names provided
/// @param p_err      A buffer to write an error to (if applicable)
/// @note             See SelectBlackBox in ResearcherMenu.cpp for an example on how to call this function
inline bool SelectFile(char* p_buf, const wchar_t** p_names, const wchar_t** p_exts, int p_extCount, char* p_err)
{
    // Opens a file dialog on Windows

    // Initialize COM interface
    HRESULT hresult = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(hresult))
    {
        // Could fail if some other method on this thread, before this call used the COM interface and failed to uninitialize it
        // https://docs.microsoft.com/en-us/windows/win32/api/combaseapi/nf-combaseapi-coinitializeex#return-value
        strcpy_s(p_err, MAX_PATH_SIZE, "Could not initialize COM interface");
        return false;
    }

    // Create a file dialog
    IFileDialog* fileDialog;
    hresult = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_IFileDialog, reinterpret_cast<void**>(&fileDialog));
    if (FAILED(hresult))
    {
        CoUninitialize();
        // I believe this is unreachable with the current parameters:
        // https://docs.microsoft.com/en-us/windows/win32/api/combaseapi/nf-combaseapi-cocreateinstance#return-value
        strcpy_s(p_err, MAX_PATH_SIZE, "Could not create file dialog");
        return false;
    }

    // Set file types for file dialog, then open it
    COMDLG_FILTERSPEC* filterSpec = new COMDLG_FILTERSPEC[p_extCount];
    for (int i = 0; i < p_extCount; i++)
    {
        filterSpec[i] = {p_names[i], p_exts[i]};
    }

    hresult = fileDialog->SetFileTypes(p_extCount, filterSpec);
    if (FAILED(hresult))
    {
        Release(fileDialog, filterSpec);
        // In this method, should be unreachable: we do none of the things that would fail according to the docs
        // https://docs.microsoft.com/en-us/windows/win32/api/shobjidl_core/nf-shobjidl_core-ifiledialog-setfiletypes#return-value
        strcpy_s(p_err, MAX_PATH_SIZE, "Could not set file types for file dialog");
        return false;
    }
    hresult = fileDialog->Show(nullptr);
    if (FAILED(hresult))
    {
        Release(fileDialog, filterSpec);
        strcpy_s(p_err, MAX_PATH_SIZE, "Closed the file dialog");
        return false;
    }

    // Get filename
    IShellItem* shellItem;
    hresult = fileDialog->GetResult(&shellItem);
    if (FAILED(hresult))
    {
        Release(fileDialog, filterSpec);
        // If the user does not choose multiple items, this should be unreachable as we enforce fileDialog->Show to succeed
        // https://docs.microsoft.com/en-us/windows/win32/api/shobjidl_core/nf-shobjidl_core-ifiledialog-getresult#return-value
        strcpy_s(p_err, MAX_PATH_SIZE, "Could not retrieve Shell Item (item for Windows Folder Hierarchy identification) from file dialog");
        return false;
    }
    PWSTR filePath;
    hresult = shellItem->GetDisplayName(SIGDN_FILESYSPATH, &filePath);
    if (FAILED(hresult))
    {
        Release(shellItem, fileDialog, filterSpec);
        // No clue if this can actually fail (the docs say it can, but not when):
        // https://docs.microsoft.com/en-us/windows/win32/api/shobjidl_core/nf-shobjidl_core-ishellitem-getdisplayname#return-value
        strcpy_s(p_err, MAX_PATH_SIZE, "Could not retrieve filepath from Shell Item (item for Windows Folder Hierarchy identification)");
        return false;
    }

    // Convert PWSTR to std::string
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    std::string fileName = converter.to_bytes(filePath);
    // Over max file length
    if (fileName.size() >= MAX_PATH_SIZE - 1)  // std::string isn't null terminated, while Windows paths/char* are
    {
        // Sanity check: This should be dead code: either your system is so old it does not support paths > 260 chars,
        // or it has a system where paths of those lengths get aliased to an 8.3 file name that is <= 260 chars
        Release(shellItem, fileDialog, filterSpec);
        strcpy_s(p_err, MAX_PATH_SIZE, "File is too long, would overflow the buffer");
        return false;
    }

    // Copy file to the buffer
    strcpy_s(p_buf, MAX_PATH_SIZE, fileName.c_str());

    // Release variables
    CoTaskMemFree(filePath);
    Release(shellItem, fileDialog, filterSpec);

    return true;
}

/// @brief              Releases a file dialog and uninitializes the COM interface
/// @param p_fileDialog The dialog to release
inline void Release(IFileDialog* p_fileDialog)
{
    p_fileDialog->Release();
    CoUninitialize();
}

/// @brief              Releases a shell item and calls Release with the remaining parameter
/// @param p_shellItem  The shell item to release
/// @param p_fileDialog The dialog to release
inline void Release(IShellItem* p_shellItem, IFileDialog* p_fileDialog)
{
    p_shellItem->Release();
    Release(p_fileDialog);
}

/// @brief            Opens a file dialog for the user to select a file without limiting the files shown
/// @param p_buf      A buffer to write the filename to
/// @param p_err      A buffer to write an error to (if applicable)
/// @param p_folder   Whether to select files (true) or folders (false)
/// @note             See SelectReplayFile in DeveloperMenu.cpp for an example on how to call this function
inline bool SelectFile(char* p_buf, char* p_err, bool p_folder)
{
    // Opens a file dialog on Windows

    // Initialize COM interface
    HRESULT hresult = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(hresult))
    {
        // Could fail if some other method on this thread, before this call used the COM interface and failed to uninitialize it
        // https://docs.microsoft.com/en-us/windows/win32/api/combaseapi/nf-combaseapi-coinitializeex#return-value
        strcpy_s(p_err, MAX_PATH_SIZE, "Could not initialize COM interface");
        return false;
    }

    // Create a file dialog
    IFileDialog* fileDialog;
    hresult = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_IFileDialog, reinterpret_cast<void**>(&fileDialog));
    if (FAILED(hresult))
    {
        CoUninitialize();
        // I believe this is unreachable with the current parameters:
        // https://docs.microsoft.com/en-us/windows/win32/api/combaseapi/nf-combaseapi-cocreateinstance#return-value
        strcpy_s(p_err, MAX_PATH_SIZE, "Could not create file dialog");
        return false;
    }

    if (p_folder)
    {
        DWORD dwOptions;
        hresult = fileDialog->GetOptions(&dwOptions);
        if (FAILED(hresult))
        {
            Release(fileDialog);
            // No actual documentation on how this can fail
            // https://docs.microsoft.com/en-us/windows/win32/api/shobjidl_core/nf-shobjidl_core-ifiledialog-getoptions#return-values
            strcpy_s(p_err, MAX_PATH_SIZE, "Could not get file dialog options");
            return false;
        }
        hresult = fileDialog->SetOptions(dwOptions | FOS_PICKFOLDERS);
        if (FAILED(hresult))
        {
            Release(fileDialog);
            // No actual documentation on how this can fail
            // https://docs.microsoft.com/en-us/windows/win32/api/shobjidl_core/nf-shobjidl_core-ifiledialog-setoptions#return-value
            strcpy_s(p_err, MAX_PATH_SIZE, "Could not set file dialog options");
            return false;
        }
    }

    hresult = fileDialog->Show(nullptr);
    if (FAILED(hresult))
    {
        Release(fileDialog);
        strcpy_s(p_err, MAX_PATH_SIZE, "Closed the file dialog");
        return false;
    }

    // Get filename
    IShellItem* shellItem;
    hresult = fileDialog->GetResult(&shellItem);
    if (FAILED(hresult))
    {
        Release(fileDialog);
        // If the user does not choose multiple items, this should be unreachable as we enforce fileDialog->Show to succeed
        // https://docs.microsoft.com/en-us/windows/win32/api/shobjidl_core/nf-shobjidl_core-ifiledialog-getresult#return-value
        strcpy_s(p_err, MAX_PATH_SIZE, "Could not retrieve Shell Item (item for Windows Folder Hierarchy identification) from file dialog");
        return false;
    }
    PWSTR filePath;
    hresult = shellItem->GetDisplayName(SIGDN_FILESYSPATH, &filePath);
    if (FAILED(hresult))
    {
        Release(shellItem, fileDialog);
        // No clue if this can actually fail (the docs say it can, but not when):
        // https://docs.microsoft.com/en-us/windows/win32/api/shobjidl_core/nf-shobjidl_core-ishellitem-getdisplayname#return-value
        strcpy_s(p_err, MAX_PATH_SIZE, "Could not retrieve filepath from Shell Item (item for Windows Folder Hierarchy identification)");
        return false;
    }

    // Convert PWSTR to std::string
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    std::string fileName = converter.to_bytes(filePath);
    // Over max file length
    if (fileName.size() >= MAX_PATH_SIZE - 1)  // std::string isn't null terminated, while Windows paths/char* are
    {
        // Sanity check: This should be dead code: either your system is so old it does not support paths > 260 chars,
        // or it has a system where paths of those lengths get aliased to an 8.3 file name that is <= 260 chars
        Release(shellItem, fileDialog);
        strcpy_s(p_err, MAX_PATH_SIZE, "File is too long, would overflow the buffer");
        return false;
    }

    // Copy file to the buffer
    strcpy_s(p_buf, MAX_PATH_SIZE, fileName.c_str());

    // Release variables
    CoTaskMemFree(filePath);
    Release(shellItem, fileDialog);

    return true;
}