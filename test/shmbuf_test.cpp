//
// Created by pilot on 10/2/2023.
//

#include <gtest/gtest.h>
#include <stdio.h>
#include "shmbuf.h"

TEST(ShmBufTest, FilenameNumerator) {
    char filename[] = "HelloWorld";
    FilenameNumerator instance(filename, sizeof(filename) - 1);
    printf("filename: %s ; total len: %ld\n", instance.get_name(), instance.get_len());
    instance.next();
    printf("filename: %s ; total len: %ld\n", instance.get_name(), instance.get_len());
    instance.next();
    printf("filename: %s ; total len: %ld\n", instance.get_name(), instance.get_len());
    instance.next();
    printf("filename: %s ; total len: %ld\n", instance.get_name(), instance.get_len());
    instance.next();
    instance.next();
    instance.next();
    instance.next();
    instance.next();
    instance.next();
    instance.next();
    instance.next();
    printf("filename: %s ; total len: %ld\n", instance.get_name(), instance.get_len());
    instance.next();
    printf("filename: %s ; total len: %ld\n", instance.get_name(), instance.get_len());
}

TEST(ShmBufTest, ShmBuf) {
    char filename[] = "/tmp/HelloWorld";
    char buffer[] = "close() closes a file descriptor, so that it no longer refers to any file and may be reused.  Any record locks (see fcntl(2)) held on the file  it  was  associated with,  and  owned  by the process, are removed (regardless of the file descriptor that was used to obtain the lock).";

    ShmBuf instance(filename, 30);
    instance.write(buffer, 10);
    instance.write(buffer, 10);
    instance.write(buffer, 10);
    instance.write(buffer, 10);
}