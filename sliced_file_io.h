#ifndef SLICED_FILE_IO_H
#define SLICED_FILE_IO_H

#include <string>

// act as a serializer and deserializer, allow sliced files
template<typename T>
class SliceFileIO {
public:
    SliceFileIO(std::string direcory, std::string prefix);
    ~SliceFileIO();
    T read_one();
    void write_one(T const&data);
    bool eof() const;
};

#endif

