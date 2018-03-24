#!/bin/bash
clang-format -i `find . -regex '.*\.\(h\|\(cpp\)\|c\)'`
