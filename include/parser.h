#pragma once

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int is_empty(char *line);
void parse(char *line, char **argv);
