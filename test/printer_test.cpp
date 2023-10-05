//
// Created by pilot on 10/2/2023.
//

#include <gtest/gtest.h>
#include "printer.h"

TEST(printer_test, try_output) {
    Err("Hello World\n");
    Err("1: %d", 1);
}