//
//  logger.c
//  cs_327
//
//  Created by Srinivas Dhanwada on 9/3/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "logger.h"

#define DEFAULT_LOG_NAME "default.log";
#define LINE_SPACING 32

#define LOG_T 0
#define LOG_D 1
#define LOG_I 2
#define LOG_W 3
#define LOG_E 4
#define LOG_F 5

static int initialized = 0;
static char* log_name = DEFAULT_LOG_NAME;

static char* get_date_string();
static int   file_exists(char* file_name);
static void  create_bak(char* prev, char* dir_path);
static void  create_bak_r(char* prev, char* dir_path, int idx);
static void  get_full_path(char* path);
static void  wrap_lines(char* msg, int msg_size);
static void  write_log(int log_lvl, const char* str, va_list args);

// Create function.  This function creates
// a log file as well as moves an existing log
// file to a backup state.
void create(const char* name) {
    char cwd[256];
    char log_file[256];
    FILE* log;
    
    if(initialized) {
        logger.w("Create called after logger has already been initialized!");
        return;
    }
    
    if(name != NULL) {
        log_name = (char*)name;
    }
    
    getcwd(cwd, sizeof(cwd));
    sprintf(cwd + strlen(cwd), "/logs/");
    int e = mkdir(cwd, 0755);
    // clear the error if there is one.
    if(e == -1) {
        errno = 0;
    }
    
    // Check if ./logs/{name} exists, create .bak file
    sprintf(log_file, "%s%s", cwd, log_name);
    if(file_exists(log_file)) {
        create_bak(log_file, cwd);
    }
    
    // Open file so it gets created. (Is this necessary?)
    log = fopen(log_file, "w");
    fclose(log);
    
    initialized = 1;
}

void t(const char* str, ...) {
    va_list args;
    va_start(args, str);
    write_log(LOG_T, str, args);
    va_end(args);
}

void d(const char* str, ...) {
    va_list args;
    va_start(args, str);
    write_log(LOG_D, str, args);
    va_end(args);
}

void i(const char* str, ...) {
    va_list args;
    va_start(args, str);
    write_log(LOG_I, str, args);
    va_end(args);
}

void w(const char* str, ...) {
    va_list args;
    va_start(args, str);
    write_log(LOG_W, str, args);
    va_end(args);
}

void e(const char* str, ...) {
    va_list args;
    va_start(args, str);
    write_log(LOG_E, str, args);
    va_end(args);
}

void f(const char* str, ...) {
    va_list args;
    va_start(args, str);
    write_log(LOG_F, str, args);
    va_end(args);
}

// Get the current date and
// create a formated string from it.
static char* get_date_string() {
    time_t t = time(NULL);
    char* date = (char*) malloc(100 * sizeof(char));
    strftime(date, 100 * sizeof(char), "[%F %T]", localtime(&t));
    return date;
}

// Private function to check if file exists.
static int file_exists(char* file_name) {
    struct stat buffer;
    int exist = stat(file_name, &buffer);
    if(exist == 0) {
        return 1;
    }
    return 0;
}

// Private function to start the process of creating a backup.
// This consist of moving the current {log_name} to {log_name}.back
// Then making a recursive call through create_bak_r to cascade this
// to the rest of the backup files.  A maximum of 10 backup files will
// be created (.bak - .bak9) and after that .bak9 will be deleted,
// so the cascade can take effect.
static void create_bak(char* prev, char* dir_path) {
    char log_file[256];
    sprintf(log_file, "%s%s.bak", dir_path, log_name);
    if(file_exists(log_file)) {
        create_bak_r(log_file, dir_path, 1);
    }
    rename(prev, log_file);
}

// The recursive call of create_bak that moves the file names properly
// so .bak is always the most recent previous log file.
static void create_bak_r(char* prev, char* dir_path, int idx) {
    char cur[256];
    sprintf(cur, "%s%s.bak%d", dir_path, log_name, idx);
    if(file_exists(cur)) {
        if(idx == 9) {
            remove(cur);
        } else {
            create_bak_r(cur, dir_path, idx+1);
        }
    }
    rename(prev, cur);
}

// Get the full path of the log_name
// This adds the path of current working directory,
// the /logs/ folder, then adds the log_name and
// stores it in path.
static void get_full_path(char* path) {
    char cwd[256];
    getcwd(cwd, sizeof(cwd));
    sprintf(cwd + strlen(cwd), "/logs/");
    sprintf(path, "%s%s", cwd, log_name);
}

// Wrap lines of msg and adds spaces to align
// text on the new line with the previous line
static void wrap_lines(char* msg, int msg_size) {
    // iterator
    int i;
    
    // used to keep track of how much is read from msg after each line
    int line_feed_size = 80;
    
    // character we need to replace with a '\n'
    char* return_pos;
    
    // Temp buffer to store our new message with wraps as we go.
    char temp_buffer[msg_size];
    memset(temp_buffer, 0, msg_size);
    
    // str to hold the spaces that will align the wrapped lines.
    char* indented_line_spacing = (char*)malloc(LINE_SPACING * sizeof(char));
    memset(indented_line_spacing, ' ', LINE_SPACING * sizeof(char));
    indented_line_spacing[LINE_SPACING - 2] = '\t';
    indented_line_spacing[LINE_SPACING - 1] = 0;
    
    // Start by copying the first 80 characters from message
    strncpy(temp_buffer, msg, line_feed_size * sizeof(char));
    
    // Iterate through the message to wrap lines every [line_feed_size]
    for(i = line_feed_size; i < strlen(msg); i+= line_feed_size) {
        char wrap_text[255];
        
        // find last space in temp buffer and replace it with newline and line_spacing
        return_pos = strrchr(temp_buffer, ' ');
        *return_pos = '\n';
        strcpy(wrap_text, return_pos + 1);
        sprintf(return_pos + 1, "%s%s", indented_line_spacing, wrap_text);
        
        // calculate what the next line_feed_size needs to be so we add up to 80 characters
        line_feed_size = 80 - (int)(strlen(wrap_text) + strlen(indented_line_spacing));
        
        // copy either the length of line_feed_size, or the rest of the message,
        // which ever is smaller.
        strncpy(return_pos + strlen(return_pos),
                msg + i,
                (strlen(msg + i) < line_feed_size) ? strlen(msg + i) : line_feed_size);
    }
    
    // copy the contents of msg back into temp_buffer
    strncpy(msg, temp_buffer, msg_size);
    
    // Add a newline to the end if it's not there already
    if(msg[strlen(msg) - 1] != '\n') {
        sprintf(msg + strlen(msg), "\n");
    }
    
    // free up memory
    free(indented_line_spacing);
}

static void write_log(int log_lvl, const char* str, va_list args) {
    // Set the correct header based on the log level.
    const char* header = log_lvl == LOG_T ? "TRACE : " :
    log_lvl == LOG_D ? "DEBUG : " :
    log_lvl == LOG_I ? "INFO  : " :
    log_lvl == LOG_W ? "WARN  : " :
    log_lvl == LOG_E ? "ERROR : " : "FATAL : ";
    
    // Max size of [str] and a stack trace if there is one.
    int max_va_list_size = 4146;
    
    // Create the message from the variable argument list.
    char* va_msg = (char*) malloc(strlen(str) + max_va_list_size);
    int va_string_size = vsnprintf(va_msg, strlen(str) + max_va_list_size, str, args);
    
    char* date = get_date_string();
    
    // Various sizes of pieces of data
    // string_size - size of the unwrapped message
    // line_wrap_size - size of the spacers when message is wrapped
    // msg_size - total size of the wrapped message including the stack trace (if there is one).
    int string_size = (int)strlen(date) + (int)strlen(va_msg) + 10 * sizeof(char);
    int line_wrap_size = 32 * (string_size / 80);
    int msg_size = string_size + line_wrap_size + (int)strlen(strerror(errno)) + (50 * sizeof(char));
    
    // Create the message string and start adding parts to it.
    char* msg = (char*) malloc(msg_size * sizeof(char));
    sprintf(msg, "%s\t%s", date, header);
    sprintf(msg + strlen(msg), "%s\n", va_msg);
    if(strlen(msg) > 80) {
        wrap_lines(msg, msg_size);
    }
    
    // if the log level was error or fatal level and an errno was set,
    // print the error as well.
    if(log_lvl >= LOG_E && errno) {
        char date_length_spacing[strlen(date) + 1];
        memset(date_length_spacing, ' ', strlen(date) + 1);
        sprintf(msg + strlen(msg), "%s\terrno : %s\n", date_length_spacing, strerror(errno));
    }
    
    // If we aren't initialized, call the create function.
    if(!initialized) {
        logger.create(NULL);
    }
    
    // Perform file writing
    char log_file[256];
    get_full_path(log_file);
    FILE* log = fopen(log_file, "a");
    fprintf(log, "%s", msg);
    fclose(log);
    
    // Free up memory
    free(date);
    free(msg);
    free(va_msg);
    
    // if the message was truncated, print a log message to notify the user.
    if(va_string_size > (strlen(str) + max_va_list_size)) {
        int truncated_size = va_string_size - ((int)strlen(str) + max_va_list_size);
        logger.i("Previous message truncated by %d bytes to fit into buffer", truncated_size);
    }
}

logger_namespace const logger = { t, d, i, w, e, f, create };
