#include <iostream>
#include <string>
#include <archive.h>
#include <archive_entry.h>
#include <sys/stat.h>

int unpack(std::string filePath, std::string destinationPath) {
    struct archive* a;
    struct archive_entry* entry;
    int result;

    a = archive_read_new();
    archive_read_support_filter_xz(a);
    archive_read_support_format_tar(a);

    result = archive_read_open_filename(a, filePath.c_str(), 10240);
    if (result != ARCHIVE_OK) {
        archive_read_free(a);
        return false;
    }
	std::cout << filePath << " " << destinationPath << std::endl;
    while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        const char* entryPath = archive_entry_pathname(entry);
        std::string extractPath = destinationPath + "/" + entryPath;

        if (archive_entry_filetype(entry) == AE_IFLNK) {
            const char* symlinkTarget = archive_entry_symlink(entry);
            symlink(symlinkTarget, extractPath.c_str());
            continue;
        }

        if (archive_entry_filetype(entry) == AE_IFDIR) {
            int mkdirResult = mkdir(extractPath.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
            if (mkdirResult != 0) {
                archive_read_close(a);
                archive_read_free(a);
                return false;
            }
            continue;
        }

        FILE* file = fopen(extractPath.c_str(), "wb");
        if (!file) {
            archive_read_close(a);
            archive_read_free(a);
            return false;
        }
        const void* buff;
        size_t size;
        la_int64_t offset;
        while (archive_read_data_block(a, &buff, &size, &offset) == ARCHIVE_OK) {
            fwrite(buff, 1, size, file);
        }

        fclose(file);

        mode_t mode = archive_entry_perm(entry);
        chmod(extractPath.c_str(), mode);
    }

    archive_read_close(a);
    archive_read_free(a);
    return true;
}

