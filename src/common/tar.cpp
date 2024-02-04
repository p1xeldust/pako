/**
 * Written by Paul Goldstein, Dec 2023.
 */

#include <filesystem>
#include <string>

#include <archive_entry.h>
#include <archive.h>

#include "output.h"

extern Output output;

using std::string, std::filesystem::create_directories, std::filesystem::path;

int extractTar(std::filesystem::path source, std::filesystem::path destination)
{
    struct archive* a;
    struct archive_entry* entry;

    a = archive_read_new();
    archive_read_support_filter_xz(a);
    archive_read_support_format_tar(a);

    if (archive_read_open_filename(a, source.c_str(), 10240) != ARCHIVE_OK)
    {
        archive_read_free(a);
        output.error("tar.cpp: can't extract " + source.string() + ": It's a broken archive.");
        return -1;
    }
    create_directories(destination);

    while (archive_read_next_header(a, &entry) == ARCHIVE_OK)
    {
        if (archive_entry_filetype(entry) == AE_IFLNK)
        {
            if (symlink(archive_entry_symlink(entry), (destination / archive_entry_pathname(entry)).c_str()) != 0)
            {
                archive_read_close(a);
                archive_read_free(a);
                return -1;
            }
            continue;
        }

        if (archive_entry_filetype(entry) == AE_IFDIR)
        {
            if (!create_directories((destination / archive_entry_pathname(entry)).c_str()))
            {
                archive_read_close(a);
                archive_read_free(a);
                return 0;
            }
            continue;
        }

        FILE* file = fopen((destination / archive_entry_pathname(entry)).c_str(), "wb");
        if (!file)
        {
            archive_read_close(a);
            archive_read_free(a);
            return -1;
        }

        const void* buff;
        size_t size;
        la_int64_t offset;

        while (archive_read_data_block(a, &buff, &size, &offset) == ARCHIVE_OK)
            fwrite(buff, 1, size, file);

        fclose(file);

        mode_t mode = archive_entry_mode(entry);
        chmod((destination / archive_entry_pathname(entry)).c_str(), mode);
    }

    archive_read_close(a);
    archive_read_free(a);
    return 0;
}
