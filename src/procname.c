#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>

extern char **environ;

static int usage(const char *arg0) {
    fprintf(
            stderr,
            "procname - Set process name of command\n\n"
            "Usage: %s [NAME] exe [args]\n",
            arg0);
    return EXIT_FAILURE;
}

static __always_inline size_t str_has_prefix(const char *str, const char *prefix)
{
    size_t len = strlen(prefix);
    return strncmp(str, prefix, len) == 0 ? len : 0;
}

int main(int argc, char *argv[]) {
    const char *arg0 = argv[0];
    if (argc < 3) {
        return usage(arg0);
    }

    char *procname = argv[1];
    char* child_exe_path = argv[2];

    char** child_argv = calloc(argc + 1, sizeof(char *));
    if (!child_argv) {
        fprintf(stderr, "failed to alloc mem for child argv\n");
        return EXIT_FAILURE;
    }
    child_argv[0] = procname;
    for (int i = 3; i < argc; ++i) {
        child_argv[i - 2] = argv[i];
    }

    const char* libprocname = "libprocname.so";

    int environ_item_count = 2;
    int needed_environ_size = strlen("LD_PRELOAD=:") + strlen(libprocname) +
        strlen("PROCNAME=") + strlen(procname);
    for (char **ptr = environ; *ptr; ptr += 1) {
        needed_environ_size += strlen(*ptr) + 1;
        environ_item_count += 1;
    }
    int ptr_table_size = (environ_item_count + 1) * sizeof(char *);
    needed_environ_size += ptr_table_size;

    char *new_environ_bytes = malloc(needed_environ_size);
    if (!new_environ_bytes) {
        fprintf(stderr, "failed to alloc mem for environment\n");
        return EXIT_FAILURE;
    }

    char **new_environ_table = (char **)new_environ_bytes;
    char *new_environ_data = new_environ_bytes + ptr_table_size;
    bool found_item = false;
    for (char **ptr = environ; *ptr; ptr += 1) {
        char *old_item = *ptr;
        int old_item_len = strlen(old_item);

        *new_environ_table = new_environ_data;
        new_environ_table += 1;

        if (str_has_prefix(old_item, "LD_PRELOAD=")) {
            fprintf(stderr, "item: %s\n", old_item);
            found_item = true;

            new_environ_data += sprintf(
                    new_environ_data,
                    "LD_PRELOAD=%s:%s",
                    libprocname,
                    old_item + strlen("LD_PRELOAD=")
                    ) + 1;
        } else {
            memcpy(new_environ_data, old_item, old_item_len + 1);
            new_environ_data += old_item_len + 1;
        }
    }
    if (!found_item) {
        *new_environ_table = new_environ_data;
        new_environ_table += 1;
        new_environ_data += sprintf(new_environ_data, "LD_PRELOAD=%s", libprocname) + 1;
    }
    {
        *new_environ_table = new_environ_data;
        new_environ_table += 1;
        new_environ_data += sprintf(new_environ_data, "PROCNAME=%s", procname) + 1;
    }
    *new_environ_table = NULL;

    execvpe(child_exe_path, child_argv, (char**)new_environ_bytes);

    free(new_environ_bytes);
    free(child_argv);

    return EXIT_SUCCESS;
}

