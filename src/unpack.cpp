#include <archive.h>
#include <archive_entry.h>
#include <iostream>
#include <string>


int unpack(const std::string& input_file, const std::string& output_folder) {
    struct archive* tar_archive = archive_read_new();
    archive_read_support_filter_bzip2(tar_archive);
    archive_read_support_format_tar(tar_archive);

    if (archive_read_open_filename(tar_archive, input_file.c_str(), 10240) != ARCHIVE_OK) {
        archive_read_free(tar_archive);
        return -1;
    }

    struct archive* tar_extract = archive_write_disk_new();
    archive_write_disk_set_options(tar_extract, ARCHIVE_EXTRACT_TIME);

    int result;
    struct archive_entry* entry;
    while (archive_read_next_header(tar_archive, &entry) == ARCHIVE_OK) {
        const char* entry_pathname = archive_entry_pathname(entry);
        std::string output_path = output_folder + "/" + entry_pathname;

        archive_entry_set_pathname(entry, output_path.c_str());

        result = archive_write_header(tar_extract, entry);
        if (result != ARCHIVE_OK) {
            return 2;
        }

        if (archive_entry_size(entry) > 0) {
            char buff[10240];
            ssize_t len;
            while ((len = archive_read_data(tar_archive, buff, sizeof(buff))) > 0) {
                result = archive_write_data(tar_extract, buff, len);
                if (result < ARCHIVE_OK) {
					return 3;
                }
            }

            if (result < ARCHIVE_OK) {
                return 3;
            }
        }

        result = archive_write_finish_entry(tar_extract);
        if (result != ARCHIVE_OK) {
            return 4;
        }
    }

    archive_read_close(tar_archive);
    archive_read_free(tar_archive);
    archive_write_close(tar_extract);
    archive_write_free(tar_extract);

    if (result == ARCHIVE_OK) {
        return 0;
    } else {
        return 1;
    }
}
