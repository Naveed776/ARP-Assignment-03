/*
	Naveed Manzoor Afridi	| 5149575 |
	Abdul Rauf		| 5226212 |
	Group Name		| NR007   |
*/

#include "./../include/processB_utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <bmpfile.h>
#include <math.h>
#include <time.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#define WIDTH 1600
#define HEIGHT 600
#define DEPTH 4

struct shared {
    int x;
    int y;
    int m[WIDTH][HEIGHT];
};

size_t SHM_SIZE = sizeof(struct shared);
struct shared *SharedMem_Pointer;

const u_int8_t RED = 0;
const u_int8_t GREEN = 0;
const u_int8_t BLUE = 255;
const u_int8_t ALPHA = 0;

sem_t *S1;
sem_t *S2;

char LogBuffer[100];
int LogFileDescriptor;

void initialize_shared() {
    SharedMem_Pointer = calloc(1, sizeof(struct shared));
}

void DrawCircle(int radius, int x, int y, bmpfile_t *bmp, rgb_pixel_t color) {
    int centerX = x * 20;
    int centerY = y * 20;

    for (int i = centerX - radius; i <= centerX + radius; i++) {
        for (int j = centerY - radius; j <= centerY + radius; j++) {
            if (pow(i - centerX, 2) + pow(j - centerY, 2) <= pow(radius, 2)) {
                bmp_set_pixel(bmp, i, j, color);
            }
        }
    }
}

void ClearCircle(int radius, int x, int y, bmpfile_t *bmp) {
    int centerX = x * 20;
    int centerY = y * 20;
    rgb_pixel_t color = {255, 255, 255, 0};

    for (int i = centerX - radius; i <= centerX + radius; i++) {
        for (int j = centerY - radius; j <= centerY + radius; j++) {
            if (pow(i - centerX, 2) + pow(j - centerY, 2) <= pow(radius, 2)) {
                bmp_set_pixel(bmp, i, j, color);
            }
        }
    }
}

void open_shared_memory() {
    int SharedMem_fd = shm_open("my_shm", O_RDWR, 0666);
    if (SharedMem_fd == -1) {
        perror("Error in shm_open");
        exit(1);
    }

    ftruncate(SharedMem_fd, sizeof(struct shared));

    SharedMem_Pointer = mmap(NULL, sizeof(struct shared), PROT_READ | PROT_WRITE, MAP_SHARED, SharedMem_fd, 0);
    if (SharedMem_Pointer == MAP_FAILED) {
        perror("Error in mmap");
        exit(1);
    }

    close(SharedMem_fd);
}

void open_log_file() {
    LogFileDescriptor = open("log/processB.log", O_WRONLY | O_APPEND | O_CREAT, 0666);
    if (LogFileDescriptor == -1) {
        perror("Error opening log file");
        exit(1);
    }
}

void close_log_file() {
    close(LogFileDescriptor);
}

void initialize_semaphores() {
    S1 = sem_open("/my_sem1", 0);
    if (S1 == SEM_FAILED) {
        perror("sem_open failure");
        exit(1);
    }

    S2 = sem_open("/my_sem2", 0);
    if (S2 == SEM_FAILED) {
        perror("sem_open failure");
        exit(1);
    }
}

void close_semaphores() {
    sem_close(S1);
    sem_close(S2);
}

void write_to_log_file(const char* message) {
    time_t CurrentTime;
    struct tm *TimeInfo;

    time(&CurrentTime);
    TimeInfo = localtime(&CurrentTime);

    sprintf(LogBuffer, "<Process_B> %s (%s)\n", message, asctime(TimeInfo));
    if (write(LogFileDescriptor, LogBuffer, strlen(LogBuffer)) == -1) {
        perror("Error writing to log file");
        exit(1);
    }
}

int main(int argc, char const *argv[]) {
    initialize_shared();
    open_shared_memory();
    open_log_file();
    initialize_semaphores();

    init_console_ui();

    bmpfile_t *bmp;
    bmp = bmp_create(WIDTH, HEIGHT, DEPTH);
    if (bmp == NULL) {
        printf("Error: unable to create bitmap\n");
        exit(1);
    }

    int center_cord = 0;
    int x_cord[600] = {0};
    int y_cord[600] = {0};
    int cont = 0;
    int y_old;
    int x_old;
    int flag;

    while (1) {
        int cmd = getch();

        if (cmd == KEY_RESIZE) {
            reset_console_ui();
        } else {
            mvaddch(LINES / 2, COLS / 2, '0');
            refresh();

            sem_wait(S2);

            for (int i = 0; i < 600; i++) {
                x_cord[i] = 0;
                y_cord[i] = 0;
            }
            center_cord = 0;

            int i, j;
            cont = 0;
            flag = 0;

            for (i = 0; i < 1600; i++) {
                if (flag == 1) {
                    break;
                }
                for (j = 0; j < 600; j++) {
                    if (SharedMem_Pointer->m[i][j] == 1) {
                        x_cord[cont] = j;
                        y_cord[cont] = i;

                        if (x_cord[cont] > x_cord[cont - 1]) {
                            flag = 1;
                            break;
                        }

                        cont++;
                        break;
                    }
                }
            }

            center_cord = SharedMem_Pointer->y;
            y_cord[cont - 1] = SharedMem_Pointer->x;

            write_to_log_file("Position of center updated");

            mvaddch(center_cord, y_cord[cont - 1], '0');
            refresh();

            sem_post(S1);

            ClearCircle(30, y_old, x_old, bmp);
            rgb_pixel_t color = {RED, GREEN, BLUE, ALPHA};
            DrawCircle(30, y_cord[cont - 1], center_cord, bmp, color);
            y_old = y_cord[cont - 1];
            x_old = center_cord;
        }
    }

    close_semaphores();
    munmap(SharedMem_Pointer, sizeof(struct shared));
    close_log_file();

    endwin();

    return 0;
}
