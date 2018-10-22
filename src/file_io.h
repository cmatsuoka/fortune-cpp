#ifndef SRC_FILE_IO_H_
#define SRC_FILE_IO_H_

#include <string>


class File {
    std::string path;
    std::fstream file;
 public:
    explicit File(std::string);
    File(std::string, std::ios_base::openmode);
    uint32_t read32b();
    File& read(char *, int);
    File& seekg(off_t);
};


class InputFile : public File {
 public:
    explicit InputFile(std::string);
};

#endif  // SRC_FILE_IO_H_
