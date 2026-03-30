#!/bin/bash
shell_bin="bin/main/main"
cmd_file="src/test/test$1.txt"

echo "Running my_shell with test$1.txt"
"$shell_bin" < "$cmd_file"

# test race condition
# for i in {1..100}
# do
#     echo "Run $i: Running my_shell with test$1.txt"
#     "$shell_bin" < "$cmd_file"
# done