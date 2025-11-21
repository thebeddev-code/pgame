#!/bin/bash
clang-format --style=file:.clang-format main.c

if [[ $1 == "ignore-style-errors" ]]; then
  errors=$(gcc -std=gnu11 -Wall -Wextra main.c -o main.exe 2>&1)
else
  errors=$(gcc -std=gnu11 -Wall -Werror -Wextra main.c -o main.exe 2>&1)
fi

if [ $? -ne 0 ]; then
  echo "Compilation errors:"
  echo "$errors"
  exit 0
fi
./main.exe
