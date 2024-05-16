#include "Files.h"

#if defined(SYSTEM_LINUX) || defined(SYSTEM_MACOS)



File open_file(const char* in_path) {
	return open(in_path, O_RDONLY);
}

void close_file(File in_file) {
	close(in_file);
}

const size_t get_fileSize(File in_file) {
	struct stat Status = { 0 };
	fstat(in_file, &Status);
	return Status.st_size;
}

FileMap map_file(File in_file) {
	return in_file;
}

void unmap_file(FileMap in_map) { return; }

void* view_fileMap(FileMap in_map, const size_t in_size) {
	return mmap(NULL, in_size, PROT_READ, MAP_PRIVATE, in_map, 0);
}

void unview_fileMap(void* in_view, const size_t in_size) {
	munmap(in_view, in_size);
}



#endif
