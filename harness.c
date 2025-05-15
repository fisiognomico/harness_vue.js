#include "utils.h"



// Not main, this should be called fuzzer loop smt
int main(int argc, char **argv) {

    // Register signal handlers and exit function to clean the test folders
    // Set up signal handlers for clean exit
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    atexit(cleanup_folder);

    srand(time(NULL));
    // The input is taken from the stated file, which contains the data
    // necessary. In a future release I could also mutate other parameters.
    if(argc != 2) {
        fprintf(stderr, "Usage %s <file name> \n", argv[0]);
        return -1;
    }

// TODO can i perform more steps with a single mutation?
#ifdef  __AFL_COMPILER 
    while (__AFL_LOOP(1000)) {
#endif
        int fd = open(argv[1], O_RDONLY);
        if (fd < 0 ) {
            perror("Error opening file\n");
            return -2;
        }
        struct stat st;
        stat(argv[1], &st);
        int len = st.st_size;
        if (len < 20)
            return -3;
        // Cut the fuzzer input
        int user_len = rand() % (len / 2);
        int event_len = rand() % (len / 2);
        char *user = malloc(user_len);
        char *event = malloc(event_len);
        read(fd, user, user_len);
        read(fd, event, event_len);

        // Launch the actual tests, use an environment variable to pass the
        // seeded values
        setenv("AFL_USERNAME", user, 1);
        setenv("AFL_EVENT_TITLE", event, 1);
        char command[255];
        char *tests = getenv("FUZZER_TEST");
        sprintf(command, "node %s", tests);
        execv("/usr/bin/env", &command);


        // Wait until a lock file (written finished tests) is present
        bool lock_file_present = false;
        while(!lock_file_present)
            lock_file_present = file_exists(LOCK_FILE);

        DIR *dir;
        struct dirent *entry;

        if ((dir = opendir(TEST_ARTIFACTS_DIR)) == NULL) {
            perror("Error opening directory");
            return -4;
        }

        // Parse the HTML, the instrumentation takes care of finding the eg XSS
        // or SSTI
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type == DT_REG) { // Regular file
                const char *name = entry->d_name;
                size_t len = strlen(name);

                // Check for HTML files (ends with .html or .htm)
                if ((len > 5 && strcmp(name + len - 5, ".html") == 0) ||
                    (len > 4 && strcmp(name + len - 4, ".htm") == 0)) {
                    char path[256];
                    snprintf(path, sizeof(path), "%s/%s", TEST_ARTIFACTS_DIR, name);
                    parse_html_file(path);
                }
            }
        }

#ifdef __AFL_COMPILER
    }
#endif


    closedir(dir);
    return 0;

}


