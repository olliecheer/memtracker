//
// Created by pilot on 10/2/2023.
//

#ifndef UNTITLED_PRINTER_H
#define UNTITLED_PRINTER_H

#include <unistd.h>
#include <stdio.h>
#include <unistd.h>

const size_t PRINTER_BUFFER_SIZE = 128;
extern thread_local char printer_buffer[PRINTER_BUFFER_SIZE];

#define Err(...) \
    {            \
        int _len = snprintf(printer_buffer, sizeof(printer_buffer), __VA_ARGS__); \
        ::write(STDERR_FILENO, printer_buffer, _len); \
    }

#endif //UNTITLED_PRINTER_H
