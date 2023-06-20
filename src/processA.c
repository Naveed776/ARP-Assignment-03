/*
	Naveed Manzoor Afridi	| 5149575 |
	Abdul Rauf		| 5226212 |
	Group Name		| NR007   |
*/

#include "./../include/processA_utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <bmpfile.h>
#include <math.h>
#include <time.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

// Define the size of the shared memory
#define WIDTH 1600
#define HEIGHT 600
#define DEPTH 4

// Define variable for socket messages
#define UP 38
#define DOWN 40
#define LEFT 37
#define RIGHT 38

// Define the struct of the shared memory
struct shared
{
    int x;
    int y;
    int m[WIDTH][HEIGHT];
};

// Set the color of the circle (0 - 255)
const u_int8_t RED = 0;
const u_int8_t GREEN = 0;
const u_int8_t BLUE = 255;
const u_int8_t ALPHA = 0;

// Delcare circle radius
const int RADIUS = 30;

// Define the semaphores
sem_t *S1;
sem_t *S2;

// Character buffer for the log file
char LogBuffer[100];

// File descriptor for the log file
int LogFileDescriptor;

// Function to draw a circle
void DrawCircle(int radius, int x, int y, bmpfile_t *bmp, rgb_pixel_t color) {
    // Define the center of the circle
    int centerX = x * 20;
    int centerY = y * 20;

    // Loop over the pixels of the circle
    for (int i = centerX - radius; i <= centerX + radius; i++) {
        for (int j = centerY - radius; j <= centerY + radius; j++) {
            if (pow(i - centerX, 2) + pow(j - centerY, 2) <= pow(radius, 2)) {
                // Color the pixel at the specified (x,y) position with the given pixel values
                bmp_set_pixel(bmp, i, j, color);
            }
        }
    }
}

// Function to clear the circle
void ClearCircle(int radius, int x, int y, bmpfile_t *bmp) {
    // Define the center of the circle
    int centerX = x * 20;
    int centerY = y * 20;
    // Define the color of the circle
    rgb_pixel_t color = {255, 255, 255, 0}; // White

    // Loop over the pixels of the circle
    for (int i = centerX - radius; i <= centerX + radius; i++) {
        for (int j = centerY - radius; j <= centerY + radius; j++) {
            // If the pixel is inside the circle..
            if (pow(i - centerX, 2) + pow(j - centerY, 2) <= pow(radius, 2)) {
                // Color the pixel at the specified (x,y) position with the given pixel values
                bmp_set_pixel(bmp, i, j, color);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    // Open the log file
    if ((LogFileDescriptor = open("log/processA.log",O_WRONLY|O_APPEND|O_CREAT, 0666)) == -1)
    {
        // If the file could not be opened, print an error message and exit
        perror("Error opening command file");
        exit(1);
    }

    int mode = atoi(argv[1]);
    int sock_fd, newsock_fd, PortNum, ClientAddLength;
    char address[256];
    char buffer[256];
    struct sockaddr_in ServAdd, cli_addr;
    //
    struct hostent *ServerInfo;
    int confirmInt = 0;
    int Confirmation2= 0;
    in_addr_t addr;

    if (mode == 2){
        printf("\n\n   - - - SERVER MODE - - -\n");
        while (confirmInt == 0){
            do {
                printf("\n\n Please provide the port number of the server application (2000 - 65535):  ");
                scanf("%d", &PortNum);
                printf("\n The port number of the client application is: %d \n Enter '1' to confirm.  ", PortNum);
                scanf("%d", &confirmInt);
            } while (PortNum < 2000 || PortNum > 65535);
            if (confirmInt != 1){
                confirmInt = 0;
            }
        }    
        sprintf(LogBuffer, " \n Waiting for the client connection.. \n");
        write(LogFileDescriptor, LogBuffer, strlen(LogBuffer));
        sock_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (sock_fd < 0) {
            sprintf(LogBuffer, "socket error\n");
            write(LogFileDescriptor, LogBuffer, strlen(LogBuffer));
        }
        bzero((char *) &ServAdd, sizeof(ServAdd));
        ServAdd.sin_family = AF_INET;
        ServAdd.sin_addr.s_addr = INADDR_ANY;
        ServAdd.sin_port = htons(PortNum);
        sprintf(LogBuffer, " \n\n %d \n %d \n %d \n", ServAdd.sin_family, ServAdd.sin_port, ServAdd.sin_addr.s_addr);
        write(LogFileDescriptor, LogBuffer, strlen(LogBuffer));
        if (bind(sock_fd, (struct sockaddr *) &ServAdd, sizeof(ServAdd)) < 0){
            sprintf(LogBuffer, "socket bind error\n");
            write(LogFileDescriptor, LogBuffer, strlen(LogBuffer));
        }
        listen(sock_fd,5);
        ClientAddLength = sizeof(cli_addr);
        newsock_fd = accept(sock_fd, (struct sockaddr *) &cli_addr, &ClientAddLength);
        if (newsock_fd < 0){
            sprintf(LogBuffer, "socket error\n");
            write(LogFileDescriptor, LogBuffer, strlen(LogBuffer));
        }
        sprintf(LogBuffer, "\n  The connection is successful! Now the two processes can communicate! ");
        write(LogFileDescriptor, LogBuffer, strlen(LogBuffer));
    }
    else if (mode == 3){
        printf("\n\n   - - - CLIENT MODE - - -\n");
        while (confirmInt == 0){
            do {
                printf("\n\n Please provide the port number of the server application (2000 - 65535):  ");
                scanf("%d", &PortNum);
                printf("\n The port number of the server application is: %d \n Enter '1' to confirm.  ", PortNum);
                scanf("%d", &confirmInt);
            } while (PortNum < 2000 || PortNum > 65535);
            if (confirmInt != 1){
                confirmInt = 0;
            }
        }
        while (Confirmation2== 0){
            printf("\n\n Please provide the address of the server application:  ");
            // old scanf("%s", &address);
            scanf("%s", address);
            printf("\n The address of the server application is: %s \n Enter '1' to confirm.  ", address);
            scanf("%d", &Confirmation2);
            if (Confirmation2!= 1){
                Confirmation2= 0;
            }
        }
        sprintf(LogBuffer, " \n Waiting for the server connection.. \n");
        write(LogFileDescriptor, LogBuffer, strlen(LogBuffer));
        sock_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (sock_fd < 0){
            sprintf(LogBuffer, "socket error\n");
            write(LogFileDescriptor, LogBuffer, strlen(LogBuffer));
        }
        ServerInfo = gethostbyname(address);
        //addr = inet_addr((char*)address);
        if (ServerInfo == NULL) {
            fprintf(stderr,"ERROR, no such host\n");
            exit(0);
        }
        bzero((char *) &ServAdd, sizeof(ServAdd));
        ServAdd.sin_family = AF_INET;
        // bcopy((char *)ServerInfo->h_addr_list[0], (char *)&ServAdd.sin_addr.s_addr, sizeof(ServAdd.sin_addr.s_addr));
        bcopy((char *)ServerInfo->h_addr, (char *)&ServAdd.sin_addr.s_addr, ServerInfo->h_length);
        // ServAdd.sin_addr.s_addr = addr;
        ServAdd.sin_port = htons(PortNum);
        if (connect(sock_fd,(struct sockaddr *)&ServAdd,sizeof(ServAdd)) < 0){
            sprintf(LogBuffer, "socket connection error\n");
            write(LogFileDescriptor, LogBuffer, strlen(LogBuffer));
        }
        sprintf(LogBuffer, "\n  The connection is successful! Now the two processes can communicate! ");
        write(LogFileDescriptor, LogBuffer, strlen(LogBuffer));
    }

    // File descriptor for the shared memory
    int SharedMem_fd;

    // Pointer to the struct of shared memory
    struct shared *SharedMem_Pointer;

    // Open the shared memory
    SharedMem_fd = shm_open("my_shm", O_RDWR | O_CREAT, 0666);
    if (SharedMem_fd == -1) {
        perror("Error in shm_open");
        exit(1);
    }

    // Set the size of the shared memory
    ftruncate(SharedMem_fd, sizeof(struct shared));

    // Map the shared memory to the memory space of the process
    SharedMem_Pointer = mmap(NULL, sizeof(struct shared), PROT_READ | PROT_WRITE, MAP_SHARED, SharedMem_fd, 0);
    if (SharedMem_Pointer == MAP_FAILED) {
        perror("Error in mmap");
        exit(1);
    }
    
    // Create the bitmap
    bmpfile_t *bmp;
    bmp = bmp_create(WIDTH, HEIGHT, DEPTH);
    if (bmp == NULL) {
        printf("Error: unable to create bitmap\n");
        return 1;
    }
    
    // Open the semaphores
    S1 = sem_open("/my_sem1", O_CREAT, 0666, 1);
    if (S1 == SEM_FAILED) {
        perror("Error in sem_open");
        exit(1);
    }
    S2 = sem_open("/my_sem2", O_CREAT, 0666, 1);
    if (S2 == SEM_FAILED) {
        perror("Error in sem_open");
        exit(1);
    }

    // Variable declaration in order to get the time
    time_t CurrentTime;
    struct tm *TimeInfo;

    // Utility variable to avoid trigger resize event on launch
    int first_resize = TRUE;

    // Initialize UI
    init_console_ui();

    while (TRUE) {

        int n;
        int i;
        int command;

        if (mode == 2){
            // Get current time
            time(&CurrentTime);
            TimeInfo = localtime(&CurrentTime);

            // Get the position of the circle
            int x = circle.x;
            int y = circle.y;

            // Waiting to receive the input from the client application..
            bzero(buffer,256);
            n = read(newsock_fd,buffer,255);
            if (n < 0){
                sprintf(LogBuffer, "socket read error\n");
                write(LogFileDescriptor, LogBuffer, strlen(LogBuffer));
            }
            int cmd = atoi(buffer);

            // If the user risezes the window..
            if (cmd == 1) {
                    mvprintw(LINES - 1, 1, "Print button pressed");

                    // Write to the log file
                    sprintf(LogBuffer, "<Process_A> Print button pressed: %s\n", asctime(TimeInfo));
                    if (write(LogFileDescriptor, LogBuffer, strlen(LogBuffer)) == -1)
                    {
                        // If the file could not be opened, print an error message and exit
                        perror("Error writing to log file");
                        exit(1);
                    }

                    // Save the bitmap
                    bmp_save(bmp, "out/image.bmp");

                    refresh();
                    sleep(1);
                    for(int j = 0; j < COLS - BTN_SIZE_X - 2; j++) {
                        mvaddch(LINES - 1, j, ' ');
                    }
                }

            // If input is an arrow key, move circle accordingly...
            if(cmd == KEY_UP || cmd == KEY_DOWN || cmd == KEY_LEFT || cmd == KEY_RIGHT || cmd == 1) 
            {
                // Wait for the semaphore
                sem_wait(S1);
            
                // Write to the log file
                sprintf(LogBuffer, "PROCESS_A: Keyboard button pressed: %d - %d (%s)\n", circle.x, circle.y, asctime(TimeInfo));
                if (write(LogFileDescriptor, LogBuffer, strlen(LogBuffer)) == -1)
                {
                    // If the file could not be opened, print an error message and exit
                    perror("Error writing to log file");
                    exit(1);
                }  
            
                // Move the circle
                move_circle(cmd);

                // Draw the circle
                draw_circle();

                // Cancel the circle
                ClearCircle(RADIUS,x,y, bmp);

                // Set the shared memory to 0 for the pixels of the circle
                memset(SharedMem_Pointer->m, 0, sizeof(SharedMem_Pointer->m));
            
                // Choose the circle color
                rgb_pixel_t color = {RED, GREEN, BLUE, ALPHA};

                // Draw the new circle position and update the shared memory
                DrawCircle(RADIUS,circle.x,circle.y, bmp, color);
                SharedMem_Pointer->x = circle.x;
                SharedMem_Pointer->y = circle.y;

                // Loop through the pixels of the bitmap
                for (int i = 0; i < WIDTH; i++) {
                    for (int j = 0; j < HEIGHT; j++) {
                        // Get the pixel
                        rgb_pixel_t *pixel = bmp_get_pixel(bmp, i, j);
                    
                        // Set the corresponding pixel in the shared memory to 1
                        if ((pixel->blue == BLUE) && (pixel->red == RED) && (pixel->green == GREEN) && (pixel->alpha == ALPHA)) {
                            SharedMem_Pointer->m[i][j] = 1; 
                        }                    
                    }
                }

                // Signal the semaphore 2
                sem_post(S2);
            }
            for (i=0;i<strlen(buffer);i++){
                buffer[i]= '\0';
            }
        }

        else {
            // Get current time
            time(&CurrentTime);
            TimeInfo = localtime(&CurrentTime);

            // Get the mouse event
            int cmd = getch();

            // Get the position of the circle
            int x = circle.x;
            int y = circle.y;

            // If the user resize the window..
            if(cmd == KEY_RESIZE) {
                if(first_resize) {
                    first_resize = FALSE;
                }
                else {
                    reset_console_ui();
                }
            }

            // Else, if user presses print button..
            else if(cmd == KEY_MOUSE) {
                if(getmouse(&event) == OK) {
                    if(check_button_pressed(print_btn, &event)) {
                        if (mode == 3){
                            cmd = 1;
                            bzero(buffer,256);
                            sprintf(buffer, "%d", cmd);
                            n = write(sock_fd,buffer,strlen(buffer));
                            if (n < 0){
                                sprintf(LogBuffer, "socket write error\n");
                                write(LogFileDescriptor, LogBuffer, strlen(LogBuffer));
                            }
                        }
                        mvprintw(LINES - 1, 1, "Print button pressed");

                        // Write to the log file
                        sprintf(LogBuffer, "<Process_A> Print button pressed: %s\n", asctime(TimeInfo));
                        if (write(LogFileDescriptor, LogBuffer, strlen(LogBuffer)) == -1)
                        {
                            // If the file could not be opened, print an error message and exit
                            perror("Error writing to log file");
                            exit(1);
                        }

                        // Save the bitmap
                        bmp_save(bmp, "out/image.bmp");

                        refresh();
                        sleep(1);
                        for(int j = 0; j < COLS - BTN_SIZE_X - 2; j++) {
                            mvaddch(LINES - 1, j, ' ');
                        }
                    }
                }
            }

            // If input is an arrow key, move circle accordingly...
            else if(cmd == KEY_LEFT || cmd == KEY_RIGHT || cmd == KEY_UP || cmd == KEY_DOWN) 
            {
                if (mode == 3){
                    bzero(buffer,256);
                    sprintf(buffer, "%d", cmd);
                    n = write(sock_fd,buffer,strlen(buffer));
                    if (n < 0){
                        sprintf(LogBuffer, "socket write error\n");
                        write(LogFileDescriptor, LogBuffer, strlen(LogBuffer));
                    }
                }
                // Wait for the semaphore
                sem_wait(S1);
                
                // Write to the log file
                sprintf(LogBuffer, "PROCESS_A: Keyboard button pressed: %d - %d (%s)\n", circle.x, circle.y, asctime(TimeInfo));
                if (write(LogFileDescriptor, LogBuffer, strlen(LogBuffer)) == -1)
                {
                    // If the file could not be opened, print an error message and exit
                    perror("Error writing to log file");
                    exit(1);
                }  
                
                // Move the circle
                move_circle(cmd);

                // Draw the circle
                draw_circle();
    
                // Cancel the circle
                ClearCircle(RADIUS,x,y, bmp);

                // Set the shared memory to 0 for the pixels of the circle
                memset(SharedMem_Pointer->m, 0, sizeof(SharedMem_Pointer->m));
                
                // Choose the circle color
                rgb_pixel_t color = {RED, GREEN, BLUE, ALPHA};

                // Draw the new circle position and update the shared memory
                DrawCircle(RADIUS,circle.x,circle.y, bmp, color);
                SharedMem_Pointer->x = circle.x;
                SharedMem_Pointer->y = circle.y;

                // Loop through the pixels of the bitmap
                for (int i = 0; i < WIDTH; i++) {
                    for (int j = 0; j < HEIGHT; j++) {
                        // Get the pixel
                        rgb_pixel_t *pixel = bmp_get_pixel(bmp, i, j);
                        
                        // Set the corresponding pixel in the shared memory to 1
                        if ((pixel->blue == BLUE) && (pixel->red == RED) && (pixel->green == GREEN) 
                                && (pixel->alpha == ALPHA)) {
                            SharedMem_Pointer->m[i][j] = 1; 
                        }                    
                    }
                }

                // Signal the semaphore 2
                sem_post(S2);
            }
        }
    }

    // Close the semaphores and unlink the shared memory
    // Close the bitmap
    bmp_destroy(bmp);
    sem_close(S1);
    sem_close(S2);
    sem_unlink("my_sem1");
    sem_unlink("my_sem2");
    munmap(SharedMem_Pointer, sizeof(struct shared));
    shm_unlink("my_shm");

    return 0;
}
