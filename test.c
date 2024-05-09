#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

void createPipe(int x, int y, int pipeGap, int upperPipeLength, int lowerPipeLength);
void getScreenDimensions(int *dimensions);
int calculateNumber (int *numbers, int size);
void startAlternateScreen();
void endAlternateScreen();

int main() {

    startAlternateScreen();

    int dimensions[2];
    getScreenDimensions(dimensions);

    int x = dimensions[1];
    int y = dimensions[0];

    srand(time(NULL));

    int pipeGap = 12;
    int pipeLengthRange = y - pipeGap;
    int upperPipeLength = rand() % (pipeLengthRange - 20);
    int lowerPipeLength = pipeLengthRange - upperPipeLength;

    int pipePosition = x - pipeGap;

    while (pipePosition > 0) {

        createPipe(pipePosition, y, pipeGap, upperPipeLength, lowerPipeLength);
        pipePosition -= 2;
        fflush(stdout);
        usleep(33333);
        printf("\033c");

    }

    fflush(stdout);
    sleep(3);
    endAlternateScreen();

    return 0;

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

void startAlternateScreen() {
    printf("\033[?1049h");
}

void endAlternateScreen() {
    printf("\33[?1049l");
}