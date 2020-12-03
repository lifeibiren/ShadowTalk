#!/bin/bash
dir=(client server common)
clang-format -i `find $dir -regex '.*\.\(h\|\(cpp\)\|c\)'`
