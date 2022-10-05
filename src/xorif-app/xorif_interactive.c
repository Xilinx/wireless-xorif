/*
 * Copyright 2020 - 2022 Advanced Micro Devices, Inc.
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
 * @brief Source file for the xorif-app "interactive mode" functions.
 * @addtogroup xorif-app
 * @{
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <signal.h>
#include "xorif_app.h"

// TODO: save/load history to/from file

// Maximum history size
#define HISTORY_SIZE 50

// Local variables
static char buffer[LINE_BUFF_SIZE];
static char *history[HISTORY_SIZE];
static int history_head = 0;
static int history_tail = 0;

// Codes used for key operations
enum key_codes
{
    NONE = 0,
    PRINTABLE,
    RETURN,
    BACKSPACE,
    DELETE,
    CURSOR_LEFT,
    CURSOR_RIGHT,
    CURSOR_UP,
    CURSOR_DOWN,
    CURSOR_HOME,
    CURSOR_END,
};

static struct termios saved_tty_attr;

static void exit_handler(void)
{
    // Restore TTY attributes on exit
    tcsetattr(STDIN_FILENO, TCSANOW, &saved_tty_attr);
}

static void sigint_handler(int signal)
{
    // Restore TTY attributes on interrupt signal
    tcsetattr(STDIN_FILENO, TCSANOW, &saved_tty_attr);
    exit(EXIT_FAILURE);
}

/**
 * @brief Read line into buffer
 * @param prompt Prompt string to use
 * @param fp File pointer (normally stdin)
 * @return
 *      Pointer to buffer
 */
static char *read_line_file(const char *prompt, FILE *fp)
{
    printf("%s", prompt);
    return fgets(buffer, LINE_BUFF_SIZE, fp);
}

/**
 * @brief Insert a printable character into the buffer.
 * @param length Current length of the buffer
 * @param pos Current cursor position in the buffer
 * @param ch Character to insert
 * @note
 * The "length" and "pos" parameters are passed by pointer since they are
 * updated by the function.
 */
void insert_character(int *length, int *pos, char ch)
{
    if (*length < (LINE_BUFF_SIZE - 1))
    {
        for (int i = *length; i > *pos; --i)
        {
            buffer[i] = buffer[i-1];
        }
        buffer[(*pos)++] = ch;
        ++(*length);
    }
}
/**
 * @brief Read line into buffer
 * @param prompt Prompt string to use
 * @param fp File pointer (normally stdin)
 * @return
 *      Pointer to buffer
 * @note
 * The terminal is configured for non-canonical mode, which means that
 * characters are returned immediately, without waiting for "Enter" to be
 * pressed, this allows the processing of escape codes (such as cursor movement)
 * and other special charactors (such as back-space and delete).
 */
static char *read_line_tty(const char *prompt, FILE *fp)
{
    char esc[4];
    int length = 0;
    int pos = 0;
    int history_pos = history_head;
    int ch = 0;

    // Clear buffer
    buffer[0] = '\0';

    // Print prompt
    printf("%s", prompt);

    while ((ch = getc(stdin)) != EOF)
    {
        // Get key code
        enum key_codes key_code = NONE;
        switch (ch)
        {
        case '\n':
        case '\r':
            key_code = RETURN;
            break;

        case '\b':
        case 127:
            key_code = BACKSPACE;
            break;

        case '\e':
            // Read escape sequence
            esc[0] = getc(stdin);
            esc[1] = getc(stdin);
            esc[2] = '\0';
            if (esc[0] == '[')
            {
                if (isalpha(esc[1]))
                {
                    if (esc[1] == 'D')
                    {
                        key_code = CURSOR_LEFT;
                    }
                    else if (esc[1] == 'C')
                    {
                        key_code = CURSOR_RIGHT;
                    }
                    else if (esc[1] == 'A')
                    {
                        key_code = CURSOR_UP;
                    }
                    else if (esc[1] == 'B')
                    {
                        key_code = CURSOR_DOWN;
                    }
                    else if (esc[1] == 'H')
                    {
                        key_code = CURSOR_HOME;
                    }
                    else if (esc[1] == 'F')
                    {
                        key_code = CURSOR_END;
                    }
                }
                else if (isdigit(esc[1]))
                {
                    esc[2] = getc(stdin);
                    esc[3] = '\0';
                    if (esc[2] == '~')
                    {
                        if (esc[1] == '3')
                        {
                            key_code = DELETE;
                        }
                        else if ((esc[1] == '1') || (esc[1] == '7'))
                        {
                            key_code = CURSOR_HOME;
                        }
                        else if ((esc[1] == '4') || (esc[1] == '8'))
                        {
                            key_code = CURSOR_END;
                        }
                    }
                }
#if 0 // For debug
                if (key_code == NONE)
                {
                    char *p = esc;
                    insert_character(&length, &pos, '^');
                    while (*p)
                    {
                        insert_character(&length, &pos, *p);
                        ++p;
                    }
                }
#endif
            }
            break;

        default:
            if (isprint(ch))
            {
                key_code = PRINTABLE;
            }
            break;
        }

        // Process key code
        switch (key_code)
        {
        case PRINTABLE:
            // Insert printable characters at cursor
            /*
            if (length < (LINE_BUFF_SIZE - 1))
            {
                for (int i = length; i > pos; --i)
                {
                    buffer[i] = buffer[i-1];
                }
                buffer[pos++] = ch;
                ++length;
            }
            */
            insert_character(&length, &pos, ch);
            break;

        case BACKSPACE:
            // Backspace
            if (pos > 0)
            {
                // Delete character to left and move cursor left
                for (int i = pos - 1; i < length; ++i)
                {
                    buffer[i] = buffer[i+1];
                }
                --pos;
                --length;
            }
            break;

        case DELETE:
            // Delete
            if (pos < length)
            {
                // Delete character to right
                for (int i = pos; i < length; ++i)
                {
                    buffer[i] = buffer[i+1];
                }
                --length;
            }
            break;

        case CURSOR_LEFT:
            // Cursor left
            if (pos > 0)
            {
                --pos;
            }
            break;

        case CURSOR_RIGHT:
            // Cursor right
            if (pos < length)
            {
                ++pos;
            }
            break;

        case CURSOR_UP:
            // Cursor up (i.e. older entries)
            if (history_pos != history_tail)
            {
                if (history_pos == history_head)
                {
                    // Save copy of working buffer
                    char *p = malloc(length + 1);
                    if (p)
                    {
                        strcpy(p, buffer);
                        if (history[history_head])
                        {
                            free(history[history_head]);
                        }
                        history[history_head] = p;
                    }
                }

                // Move history cursor
                history_pos = (history_pos - 1 + HISTORY_SIZE) % HISTORY_SIZE;

                // Copy history entry to working buffer
                strcpy(buffer, history[history_pos]);
                length = strlen(buffer);
                pos = length;
            }
            break;

        case CURSOR_DOWN:
            // Cursor down (i.e. newer entries)
            if (history_pos != history_head)
            {
                // Move cursor
                history_pos = (history_pos + 1) % HISTORY_SIZE;

                // Copy history entry to working buffer
                strcpy(buffer, history[history_pos]);
                length = strlen(buffer);
                pos = length;
            }
            break;

        case CURSOR_HOME:
            // Cursor to neginning of line
            pos = 0;
            break;

        case CURSOR_END:
            // Cursor to end of line
            pos = length;
            break;

        default:
            // Do nothing
            break;
        }

        // Ensure buffered string is terminated correctly
        buffer[length] = '\0';

        if (key_code == RETURN)
        {
            if (length > 0)
            {
                // Copy entry into history buffer
                char *p = malloc(length + 1);
                if (p)
                {
                    strcpy(p, buffer);
                    if (history[history_head])
                    {
                        free(history[history_head]);
                    }
                    history[history_head] = p;
                    history_head = (history_head + 1) % HISTORY_SIZE;
                    if (history_head == history_tail)
                    {
                        history_tail = (history_tail + 1) % HISTORY_SIZE;
                    }
                }
            }
            break;
        }

        // Print prompt, buffer and position cursor
        printf("\r%s%s", prompt, buffer);
        printf("\e[K");
        if (length > pos)
        {
            printf("\e[%dD", length - pos);
        }
    }
    printf("\n");

    return buffer;
}

int do_interactive(const char *prompt)
{
    char *ptr;
    int return_val = SUCCESS;
    char *(*read_line)(const char *prompt, FILE *fp);

    if (isatty(STDIN_FILENO))
    {
        // Save the TTY attributes so we can restore them later
        tcgetattr(STDIN_FILENO, &saved_tty_attr);

        // Register call-backs to restore TTY attributes on exit, Ctrl-C, etc.
        atexit(exit_handler);
        signal(SIGINT, sigint_handler);

        // Support line editing for TTY
        struct termios tattr;
        tcgetattr (STDIN_FILENO, &tattr);
        tattr.c_lflag &= ~(ICANON|ECHO); // non-canonical, no echo
        tattr.c_cc[VMIN] = 1;
        tattr.c_cc[VTIME] = 0;
        tcsetattr (STDIN_FILENO, TCSAFLUSH, &tattr);
        read_line = &read_line_tty;
    }
    else
    {
        // Use fgets for non-TTY
        read_line = &read_line_file;
    }

    // Read lines
    while ((ptr = read_line(prompt, stdin)) != NULL)
    {
        // Trim leading white space
        while(isspace(*ptr)) ptr++;

        // Trim trailing white space
        char *end = ptr + strlen(ptr) - 1;
        while((end > ptr) && (isspace(*end))) end--;
        end[1] = '\0';

        // Parse line
        int result = do_command(ptr);
        if (result < 0)
        {
            PERROR("(%d) whilst processing interactive command\n", result);
            PERROR("'%s'\n", ptr);
        }
        else if (result == TERMINATE)
        {
            return_val = SUCCESS;
            break;
        }
    }

    return return_val;
}

/** @} */