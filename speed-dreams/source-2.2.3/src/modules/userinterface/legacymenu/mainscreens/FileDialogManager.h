#pragma once
#define MAX_PATH_SIZE 260
#include <cstdio>

bool SelectFile(char* p_buf, char* p_err, bool p_folder, const wchar_t** p_names = nullptr, const wchar_t** p_exts = nullptr, int p_extCount = 0);

#ifdef WIN32
// includes that are necessary for windows
#include <shobjidl.h>  // For Windows COM interface
#include <locale>
#include <codecvt>

void Release(IFileDialog* p_fileDialog, COMDLG_FILTERSPEC* p_filterSpec);

void Release(IShellItem* p_shellItem, IFileDialog* p_fileDialog, COMDLG_FILTERSPEC* p_filterSpec);



void Release(IFileDialog* p_fileDialog);

void Release(IShellItem* p_shellItem, IFileDialog* p_fileDialog);
#else
#define Release(...)
#endif