#!/usr/bin/env python3

import shutil
import os
import re
import time


# INPUT_PATH = "/dev/shm/memtracker"
INPUT_PATH = "/tmp"
OUTPUT_PATH = "/home/pilot/memtracker"

FILE_PREFIX = "memtracker-"


def safe_copy(file_path, out_dir, dst = None):
    """Safely copy a file to the specified directory. If a file with the same name already 
    exists, the copied file name is altered to preserve both.

    :param str file_path: Path to the file to copy.
    :param str out_dir: Directory to copy the file into.
    :param str dst: New name for the copied file. If None, use the name of the original
        file.
    """
    name = dst or os.path.basename(file_path)
    if not os.path.exists(os.path.join(out_dir, name)):
        shutil.copy(file_path, os.path.join(out_dir, name))
    else:
        base, extension = os.path.splitext(name)
        i = 1
        while os.path.exists(os.path.join(out_dir, '{}_{}{}'.format(base, i, extension))):
            i += 1
        shutil.copy(file_path, os.path.join(out_dir, '{}_{}{}'.format(base, i, extension)))

def get_thread_log():
    thread_log = {}

    for f in os.listdir(INPUT_PATH):
        f_path = os.path.join(INPUT_PATH, f)
        
        if not os.path.isfile(f_path):
            continue

        if not re.match("^{}/memtracker-[0-9]+-[0-9]+$".format(INPUT_PATH), f_path):
            continue

        thread_id = int(f.split('-')[-2])
        log_id = int(f.split('-')[-1])

        if thread_id not in thread_log:
            thread_log[thread_id] = []

        thread_log[thread_id].append(log_id)
    
    return thread_log


def run_once():
    thread_log = get_thread_log()

    for k in thread_log:
        v:list = thread_log[k]
        v.sort()
        latest_file_path = "{}/memtracker-{}-{}".format(INPUT_PATH, k, v[-1])
        
        modify_ts = os.path.getmtime(latest_file_path)
        current_ts = time.time()
        
        if current_ts - modify_ts < 60:
            v.pop() # pop larget(latest) log_id

        for log_id in v:
            file_path = "{}/memtracker-{}-{}".format(INPUT_PATH, k, log_id)
            assert os.path.isfile(file_path)
            safe_copy(file_path, OUTPUT_PATH)
            shutil.rmtree(file_path)
            print("move {} to {} done.".format(file_path, OUTPUT_PATH))

run_once()

