#include "Files.h"

#if defined(SYSTEM_WINDOWS)



File open_file(const char* in_path) {
	return CreateFileA(in_path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
}

void close_file(File in_file) {
	CloseHandle(in_file);
}

const size_t get_fileSize(File in_file) {
	return GetFileSize(in_file, NULL);
}

FileMap map_file(File in_file) {
	return CreateFileMappingA(in_file, NULL, PAGE_READONLY, 0, 0, NULL);
}

void unmap_file(FileMap in_map) {
	CloseHandle(in_map);
}

void* view_fileMap(FileMap in_map, const size_t in_size) {
	return MapViewOfFile(in_map, FILE_MAP_READ, 0, 0, in_size);
}

void unview_fileMap(void* in_view, const size_t in_size) {
	UnmapViewOfFile(in_view);
}



#endif
