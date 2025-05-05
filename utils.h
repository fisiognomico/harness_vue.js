#include <stdio.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>


#define TEST_ARTIFACTS_DIR "test-artifacts"
#define LOCK_FILE "test-artifacts/tests.lock"

void parse_html_file(const char *filename) {
    FILE *scroll = fopen(filename, "rb");
    if (!scroll) {
        return;
    }

    fseek(scroll, 0, SEEK_END);
    long scroll_length = ftell(scroll);
    fseek(scroll, 0, SEEK_SET);

    char *html_input = (char *)malloc(scroll_length + 1);
    if (!html_input) {
        fclose(scroll);
        return;
    }

    // Perform the actual reading - the digital equivalent of unrolling papyrus
    size_t bytes_read = fread(html_input, 1, scroll_length, scroll);
    html_input[bytes_read] = '\0';
    fclose(scroll);

    htmlDocPtr doc = htmlReadMemory(html_input, bytes_read,
                                  filename, NULL,
                                  HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);

    if (!doc) {
        fprintf(stderr, "The HTML oracle remained silent - parsing failed\n");
        free(html_input);
        return;
    }

    // Free right after parsing
    xmlFreeDoc(doc);
    free(html_input);
}

void cleanup_folder() {
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(TEST_ARTIFACTS_DIR)) == NULL) {
        perror("Error opening directory for cleanup");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) { // Regular file
            const char *name = entry->d_name;

            char path[256];
            snprintf(path, sizeof(path), "%s/%s", TEST_ARTIFACTS_DIR, name);
            if (remove(path) != 0) {
                fprintf(stderr, "Failed to remove %s\n", path);
            } else {
                printf("Removed: %s\n", path);
            }
        }
    }

    closedir(dir);
}

void signal_handler(int sig) {
    printf("\nCleaning up before exit...\n");
    cleanup_folder();
    exit(sig);
}

bool file_exists(const char *filename) {
    return access(filename, F_OK) == 0;
}


