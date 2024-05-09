#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <termios.h>

void writeToLocation(int x, int y, int letter);
int calculateNumber (int *numbers, int size);
void getScreenDimensions(int *dimensions);
void startAlternateScreen();
void endAlternateScreen();
void createPipe(int x, int y, int pipeGap, int upperPipeLength, int lowerPipeLength);
void gravity (int initCursorPosition, int fallingPosition);
void nextGameTick();
void disableCanonMode();

int main() {

    startAlternateScreen();

    disableCanonMode();

    int dimensions[2];
    getScreenDimensions(dimensions);

    int x = dimensions[1];
    int y = dimensions[0];
    int initCursorPosition = y / 2;
    int fallingPosition = 0;

    srand(time(NULL));

    int pipeGap = 12;
    int pipeLengthRange = y - pipeGap;
    int upperPipeLength = rand() % (pipeLengthRange - 20);
    int lowerPipeLength = pipeLengthRange - upperPipeLength;

    int pipePosition = x - pipeGap;

    while (1) {

        fd_set rfds;
        struct timeval tv;
        int retval;

        FD_ZERO(&rfds);
        FD_SET(0, &rfds);

        tv.tv_sec = 0;
        tv.tv_usec = 33333;

        retval = select(1, &rfds, NULL, NULL, &tv); 

        if (retval == -1) {
            perror("select()");
        } 
        else if (retval) {
            char pressedKeyBuffer[10];
            read(STDIN_FILENO, pressedKeyBuffer, 1);
            int pressedKey = pressedKeyBuffer[0];

            if (pressedKey == ' ') {

                printf("\033[A");
                fallingPosition -= 5;

            }
        }

        if (pipePosition > 0) {
            createPipe(pipePosition, y, pipeGap, upperPipeLength, lowerPipeLength);
            pipePosition -= 2;
        }

        if (pipePosition < 0) {
            pipePosition = x - pipeGap;
            upperPipeLength = rand() % (pipeLengthRange - 20);
            lowerPipeLength = pipeLengthRange - upperPipeLength;
        }

        gravity(initCursorPosition, fallingPosition);
        fallingPosition++;

        nextGameTick();

    }

    endAlternateScreen();

    return 0;

}

void disableCanonMode() {

    struct termios original_termios, modified_termios;

    if (tcgetattr(STDIN_FILENO, &original_termios) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    modified_termios = original_termios;
    modified_termios.c_lflag &= ~(ICANON | ECHO);

    if (tcsetattr(STDIN_FILENO, TCSANOW, &modified_termios) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }

}

void nextGameTick() {
    fflush(stdout);
    usleep(33333);
    printf("\033c");
}

void gravity (int initCursorPosition, int fallingPosition) {

    printf("\033[%d;%dH", initCursorPosition + fallingPosition, 30);

}

void createPipe(int x, int y, int pipeGap, int upperPipeLength, int lowerPipeLength) {

    int verticalLine = '|';
    int horizontalLine = '-';

    for(int i = 0; i < upperPipeLength; i++) {

        printf("\033[%d;%dH", i, x);
        printf("%c", verticalLine);
        printf("\033[%d;%dH", i, x + pipeGap);
        printf("%c", verticalLine);
        
    }

    for (int i = 0; i < lowerPipeLength; i++) {

        printf("\033[%d;%dH", i + (y - lowerPipeLength), x);
        printf("%c", verticalLine);
        printf("\033[%d;%dH", i + (y - lowerPipeLength), x + pipeGap);
        printf("%c", verticalLine);

    }
    
    printf("\033[%d;%dH", upperPipeLength, x);

    for(int i = 0; i < pipeGap; i++) {
        printf("%c", horizontalLine);
    }

    printf("\033[%d;%dH", y - lowerPipeLength, x);

    for(int i = 0; i < pipeGap; i++) {
        printf("%c", horizontalLine);
    }
    
}

void startAlternateScreen() {
    printf("\033[?1049h");
}

void endAlternateScreen() {
    printf("\33[?1049l");
}

void getScreenDimensions(int *dimensions) {

    char buffer[1024];
    FILE *fp;
    fp = popen("stty size", "r");
    fgets(buffer, sizeof(buffer), fp);

    int x = 0;
    int y = 0;

    int numberBuffer[3];
    int size = 0;

    int firstTime = 1;

    for (int i = 0; i < 7; i++) {

        if (buffer[i] != '\n' && buffer[i] != ' ') {
            numberBuffer[size] = buffer[i];
            size++;
        }

        if (buffer[i] == ' ' || buffer[i] == '\n') {
            if (firstTime) {
                x = calculateNumber(numberBuffer, size);
                firstTime = 0;
            } else {
                y = calculateNumber(numberBuffer, size);
            }

            numberBuffer[0] = '\0';
            size = 0;

        }
            
    }

    dimensions[0] = x;
    dimensions[1] = y;

}

int calculateNumber (int *numbers, int size) {

    int sum = 0;
    int multiplier = pow(10, size) / 10;

    for (int i = 0; i < size; i++) {

        sum += (numbers[i] - 48) * multiplier;
        multiplier = multiplier / 10;

    }

    return sum;

}

void writeToLocation(int x, int y, int letter) {

    printf("\033[%d;%dH", x, y);
    printf("%c", letter);

    fflush(stdout);
    usleep(33333);
    printf("\033[2J");

}