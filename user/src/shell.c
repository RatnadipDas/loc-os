#include "shell.h"

#include "ecall.h"
#include "exit.h"
#include "lib.h"
#include "str.h"
#include "utils.h"

void main(void) {
    while (true) {
    prompt:
        printf("> ");
        char cmdline[128];
        for (size_t i = 0;; i++) {
            char ch = getchar();
            putchar(ch);
            if (i == sizeof(cmdline) - 1) {
                FAILED("Command line too long");
                goto prompt;
            } else if (ch == '\r') {
                printf("\n");
                cmdline[i] = '\0';
                break;
            } else {
                cmdline[i] = ch;
            }
        }

        if (strcmp(cmdline, "hello") == 0)
            printf("Hello world from shell!\n");
        else if (strcmp(cmdline, "readfile") == 0) {
            char buf[128];
            int32_t len = readfile("hello.txt", buf, sizeof(buf));
            buf[len] = '\0';
            printf("%s\n", buf);
        } else if (strcmp(cmdline, "writefile") == 0)
            writefile("hello.txt", "Hello from shell!\n", 19);
        else if (strcmp(cmdline, "shutdown") == 0)
            shutdown();
        else if (strcmp(cmdline, "exit") == 0)
            exit();
        else
            FAILED("Unknown command: %s", cmdline);
    }
}
