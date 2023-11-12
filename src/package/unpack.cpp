#include <archive.h>
#include <archive_entry.h>

#include <string>

#include "package.h"

bool Package::unpack(std::string filePath, std::string destinationPath) {
struct archive* a;
struct archive_entry* entry;

a = archive_read_new();
archive_read_support_filter_xz(a);
archive_read_support_format_tar(a);

        if(archive_read_open_filename(a, filePath.c_str(), 10240) != 0) {
                archive_read_free(a);
                out.errormsg("Not opening " + ((std::filesystem::path)filePath).filename().string());
                return false;
        }

        while(archive_read_next_header(a, &entry) == 0) {
                const char* entryPath = archive_entry_pathname(entry);
                std::string extractPath = destinationPath + "/" + entryPath;

                // "Распаковываем" символическую ссылку.
                if(archive_entry_filetype(entry) == AE_IFLNK) {
                        if (symlink(archive_entry_symlink(entry), extractPath.c_str()) != 0) {
                                // Ну или нет, если есть какая-то проблема
                                archive_read_close(a);
                                archive_read_free(a);
                                return false;
                        }
                        continue;
                }
                // "Распаковываем" папку (тупо создаём эту папку).
                if(archive_entry_filetype(entry) == AE_IFDIR) {
                        if (mkdir(extractPath.c_str(), S_IRWXU | S_IRWXG | S_IRWXO) != 0) {
                                // Ну или нет, если есть какая-то проблема
                                archive_read_close(a);
                                archive_read_free(a);
                                return false;
                        }
                                continue;
                        }

                        FILE* file = fopen(extractPath.c_str(), "wb");

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
                        chmod(extractPath.c_str(), mode);
                }

        archive_read_close(a);
        archive_read_free(a);
        return true;

}
