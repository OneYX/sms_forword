#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>

// Note: This function returns a pointer to a substring of the original string.
// If the given string was allocated dynamically, the caller must not overwrite
// that pointer with the returned value, since the original pointer must be
// deallocated using the same allocator with which it was allocated.  The return
// value must NOT be deallocated using free() etc.
char *trimwhitespace(char *str) {
    char *end;

    // Trim leading space
    while (isspace((unsigned char) *str)) str++;

    if (*str == 0)  // All spaces?
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char) *end)) end--;

    // Write new null terminator character
    end[1] = '\0';

    return str;
}

int sms_watch(char *optarg) {
    FILE *fp = NULL;
    char buff[101];
    fp = popen("sms_web get 1001 1", "r");
    while (NULL != fgets(buff, 101, fp)) {
        char status[2];
        memcpy(status, buff + 4, 1);
        status[1] = '\0';
        if (status[0] != '1') {
            continue;
        }

        char index[5];
        memcpy(index, buff, 4);
        index[4] = '\0';
        trimwhitespace(index);

        char phone[41];
        memcpy(phone, buff + 5, 40);
        phone[40] = '\0';
        trimwhitespace(phone);

        char exec[100] = {0};
        snprintf(exec, sizeof exec, "sms_web set 1008 \"%s\" \"`sms_web get 1004 %s`\r\n来自(%s)\"", optarg, index,
                 phone);
        system(exec);
        printf("%s\n", exec);
    }
    pclose(fp);
    return 0;
}

int main(int argc, char *argv[]) {
    int opt;
    char *phone = NULL;
    int time = 30;
    while ((opt = getopt(argc, argv, "p:t:")) != -1) {
        switch (opt) {
            case 'p':
                if (optarg == NULL) {
                    printf("%s\n", "not set phone use -p option");
                    return 0;
                } else {
                    phone = optarg;
                }
                break;
            case 't':
                if (optarg == NULL) {
                    printf("%s\n", "not set time use -t option");
                    return 0;
                } else {
                    time = atoi(optarg);
                }
                break;
            case '?': // 输入未定义的选项, 都会将该选项的值变为 ?
                printf("unknown option \n");
                break;
            default:
                printf("default \n");
        }

    }

    if (phone == NULL) {
        printf("Please specify the forwarding mobile phone number, Use -p option\n");
        return 0;
    }

    while (1) {
        sms_watch(phone);
        sleep(time);
    }
}
