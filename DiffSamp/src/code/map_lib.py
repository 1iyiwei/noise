import sys
import os
import shutil, errno
import subprocess
import re
import glob
import ntpath
import shlex
import warnings

debug = True

def path_leaf(path):
    head, tail = ntpath.split(path)
    return tail or ntpath.basename(head)

def copy(source, target):
    if os.path.isdir(source):
        warnings.warn(source + " is a folder and not copied")
    else:
        shutil.copy(source, target)

def copy_and_add(source_folder, file_name, target_folder):
    source = os.path.join(source_folder, file_name)
    target = os.path.join(target_folder, file_name)
    
    command = "svn add " + target
    if debug:
        print(command)
    else:
        copy(source, target)
        os.system(command)

def move_and_copy(source_folder, template_folder, file_name, target_folder):
    source = os.path.join(source_folder, file_name)
    template = os.path.join(template_folder, file_name)
    target = os.path.join(target_folder, file_name)

    command = "svn mv " + source + " " + target
    if debug:
        print(command)
    else:
        os.system(command)
        copy(template, target)

argc = len(sys.argv)

if argc < 4:
    error_message = "python source_folder template_folder target_folder";
    print(error_message)
    raise Exception(error_message)

source_folder = sys.argv[1]
template_folder = sys.argv[2]
target_folder = sys.argv[3]

folder_list = glob.glob(os.path.join(template_folder, "*"))

for full_folder in folder_list:
    leaf_folder = path_leaf(full_folder)
    #print(leaf_folder)

    if (leaf_folder in set(["Main", "Scripts"])):
        continue

    if not os.path.isdir(full_folder):
        copy_and_add(template_folder, leaf_folder, target_folder)
    else:
        full_template_folder = os.path.join(template_folder, leaf_folder)
        full_target_folder = os.path.join(target_folder, leaf_folder)
        if not os.path.exists(full_target_folder):

            command = "svn add " + full_target_folder
            if debug:
                print(command)
            else:
                os.makedirs(full_target_folder)
                os.system(command)

        file_list = glob.glob(os.path.join(full_folder, "*"))

        for full_file in file_list:
            leaf_file = path_leaf(full_file)
            if os.path.exists(os.path.join(source_folder, leaf_file)):
                move_and_copy(source_folder, full_template_folder, leaf_file, full_target_folder)
            else:
                copy_and_add(full_template_folder, leaf_file, full_target_folder)
