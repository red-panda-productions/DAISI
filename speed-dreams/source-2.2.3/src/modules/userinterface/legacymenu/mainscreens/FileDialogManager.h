/*
 * This program has been developed by students from the bachelor Computer Science at
 * Utrecht University within the Software Project course.
 * © Copyright Utrecht University (Department of Information and Computing Sciences)
 */

#pragma once
#define MAX_PATH_SIZE 260
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

bool SelectFile(char* p_buf, char* p_err, bool p_folder, const wchar_t** p_names = nullptr, const wchar_t** p_exts = nullptr, int p_extCount = 0, bool p_linuxDirectories = false);
