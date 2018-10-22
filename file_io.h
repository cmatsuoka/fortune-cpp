#ifndef FORTUNE_FILE_IO_H_
#define FORTUNE_FILE_IO_H_

class File {
    std::string path;
    std::fstream file;
public:
    File(std::string);
    File(std::string, std::ios_base::openmode);
    uint32_t read32b();
    File& read(char *, int);
    File& seekg(long);
};


class InputFile : public File {
public:
    InputFile(std::string);
};

#endif
