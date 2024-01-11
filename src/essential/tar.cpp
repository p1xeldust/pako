/**
 * Written by Paul Goldstein, Dec 2023.
 */

#include <linux/limits.h>
#include <filesystem>
#include <string>

#include <archive_entry.h>
#include <archive.h>

#include "../essential/o.h"

extern Output output;

using std::string, std::filesystem::create_directories, std::filesystem::path;

int extract_tar(string source, string destination)
{
    struct archive* a;
    struct archive_entry* entry;

    a = archive_read_new();
    archive_read_support_filter_xz(a);
    archive_read_support_format_tar(a);

    if (archive_read_open_filename(a, source.c_str(), 10240) != ARCHIVE_OK)
    {
        archive_read_free(a);
        output.error("tar.cpp: can't extract " + source + ": It's a broken archive.");
        return -1;
    }
    create_directories(destination);

    while (archive_read_next_header(a, &entry) == ARCHIVE_OK)
    {
        // Распаковываем символическую ссылку
        if (archive_entry_filetype(entry) == AE_IFLNK)
        {
            if (symlink(archive_entry_symlink(entry), (destination + archive_entry_pathname(entry)).c_str()) != 0)
            {
                archive_read_close(a);
                archive_read_free(a);
                return -1;
            }
            continue;
        }

        // Распаковываем папку (создаем ее)
        if (archive_entry_filetype(entry) == AE_IFDIR)
        {
            if (!create_directories((destination + archive_entry_pathname(entry)).c_str()))
            {
                archive_read_close(a);
                archive_read_free(a);
                return 0;
            }
            continue;
        }

        FILE* file = fopen((destination + archive_entry_pathname(entry)).c_str(), "wb");
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

        // Сохраняем права доступа файла
        mode_t mode = archive_entry_perm(entry);
        chmod(archive_entry_pathname(entry), mode);
    }

    archive_read_close(a);
    archive_read_free(a);
    return 0;
}

#ifdef TEST_SUITE
int main()
{
    string filename = "archive.tar";
    string output_dir = "./output";
    int result = extract_tar(filename, output_dir);
    if (result != 1)
    {
        fprintf(stderr, "Extraction failed with code: %d\n", result);
        return 1;
    }
    printf("Extraction successful.\n");
    return 0;
}
#endif