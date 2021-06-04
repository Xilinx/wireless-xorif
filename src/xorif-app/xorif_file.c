/*
 * Copyright 2020 - 2021 Xilinx, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file xorif_file.c
 * @author Steven Dickinson
 * @brief Source file for the xorif-app "file mode" functions.
 * @addtogroup xorif-app
 * @{
 */

#include "xorif_app.h"

int do_file(const char *name)
{
    static char buff[MAX_BUFF_SIZE];
    int return_val = SUCCESS;

    // Open file for reading
    FILE *fp = fopen(name, "r");

    if (fp == NULL)
    {
        PERROR("Failed to open file '%s'\n", name);
        return FAILURE;
    }

    // Reset line count
    int num_lines = 0;

    // Read lines
    while (fgets(buff, MAX_BUFF_SIZE, fp) != NULL)
    {
        // Remove any trailing newline character
        if (buff[strlen(buff) - 1] == '\n')
        {
            buff[strlen(buff) - 1] = '\0';
        }

        // Count lines for debug / error reporting
        ++num_lines;

        // Parse line
        int result = do_command(buff);
        if (result < 0)
        {
            PERROR("(%d) whilst processing '%s', line %d\n", result, name, num_lines);
            PERROR("'%s'\n", buff);
            return_val = FAILURE;
            break;
        }
        else if (result == TERMINATE)
        {
            return_val = SUCCESS;
            break;
        }
    }

    // Close file
    fclose(fp);

    return return_val;
}

/** @} */