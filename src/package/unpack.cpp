#include <archive.h>
#include <archive_entry.h>
#include <string>

#include "package.h"

bool unpack_package_archive(std::string source, std::string destination) {
struct archive* a = archive_read_new();
struct archive_entry* entry;

archive_read_support_filter_xz(a);
archive_read_support_format_tar(a);

        if(archive_read_open_filename(a, source.c_str(), 10240) != 0) {
                archive_read_free(a);
                errormsg(("Can't extract " + ((std::filesystem::path)source).filename().string() + ": It's a broken package.").c_str());
                return false;
        }

        while(archive_read_next_header(a, &entry) == 0) {
                // "Распаковываем" символическую ссылку.
                if(archive_entry_filetype(entry) == AE_IFLNK) {
                        if (symlink(archive_entry_symlink(entry), (destination + "/" + archive_entry_pathname(entry)).c_str()) != 0) {
                                // Ну или нет, если есть какая-то проблема
                                archive_read_close(a);
                                archive_read_free(a);
                                return false;
                        }
                        continue;
                }
                // "Распаковываем" папку (тупо создаём эту папку).
                if(archive_entry_filetype(entry) == AE_IFDIR) {
                        if (mkdir((destination + "/" + archive_entry_pathname(entry)).c_str(), S_IRWXU | S_IRWXG | S_IRWXO) != 0) {
                                // Ну или нет, если есть какая-то проблема
                                archive_read_close(a);
                                archive_read_free(a);
                                return false;
                        }
                                continue;
                        }

                        FILE* file = fopen((destination + "/" + archive_entry_pathname(entry)).c_str(), "wb");

                        if(!file) {
                                archive_read_close(a);
                                archive_read_free(a);
                                return false;
                        }

                        const void* buff;
                        size_t size;
                        la_int64_t offset;

                        while(archive_read_data_block(a, &buff, &size, &offset) == 0)
                                fwrite(buff, 1, size, file);

                        fclose(file);

                        // Сохраняем права доступа файла.
                        mode_t mode = archive_entry_perm(entry);
                        chmod((destination + "/" + archive_entry_pathname(entry)).c_str(), mode);
                }

        archive_read_close(a);
        archive_read_free(a);
        return true;

}
