#ifndef FILES_H
#define FILES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../Build-Info.h"



#if defined(SYSTEM_LINUX) || defined(SYSTEM_MACOS)
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/fcntl.h>
#include <sys/file.h>
#include <unistd.h>

typedef int File;
typedef int FileMap;
#endif

#if defined(SYSTEM_WINDOWS)
#include <Windows.h>
#include <WinBase.h>

typedef HANDLE File;
typedef HANDLE FileMap;
#endif

File open_file(const char* in_path);
void close_file(File in_file);
const size_t get_fileSize(File in_file);
FileMap map_file(File in_file);
void unmap_file(FileMap in_map);
void* view_fileMap(FileMap in_map, const size_t in_size);
void unview_fileMap(void* in_view, const size_t in_size);



#ifdef __cplusplus
}
#endif

#endif
