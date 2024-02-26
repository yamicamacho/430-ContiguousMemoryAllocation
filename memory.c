/*
By: Yamileth Camacho
Project 4: Contiguous Memory Allocation
CSS 430 Operating Systems
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMSIZE 80 // max size

char memoryPool[MEMSIZE];

// Functions needed
void initializeMemory();
void showMemory();
void freeMemory(char name);
int executeFile(char *fileName);
void allocateMemory(char name, int size, char algo);
void compactMemory();

void initializeMemory() {
  for (int i = 0; i < MEMSIZE; i++) {
    memoryPool[i] = '.';
  }
}

void showMemory() {
  for (int i = 0; i < MEMSIZE; i++) {
    printf("%c", memoryPool[i]);
  }
  printf("\n");
}

void freeMemory(char name) {
  for (int i = 0; i < MEMSIZE; i++) {
    if (memoryPool[i] == name) {
      memoryPool[i] = '.';
    }
  }
  printf("Freed all allocations owned by process %c\n", name);
}

void allocateMemory(char name, int size, char algo) {
  int startPosition = -1;
  int maxBlockSize = 0;
  int tempBlockSize = 0;
  int found = 0;

  if (algo == 'F') {
    for (int i = 0; i < MEMSIZE; i++) {
      if (memoryPool[i] == '.' && !found) {
        startPosition = i;
        tempBlockSize = 1;
        found = 1;
      } else if (memoryPool[i] == '.') {
        tempBlockSize++;
      } else {
        tempBlockSize = 0;
        found = 0;
      }

      if (tempBlockSize >= size) {
        for (int j = startPosition; j < startPosition + size; j++) {
          memoryPool[j] = name;
        }
        printf("Allocated %d bytes for process %c using First-fit\n", size, name);
        return;
      }
    }
  } else if (algo == 'W') {
    for (int i = 0; i < MEMSIZE; i++) {
      if (memoryPool[i] == '.') {
        tempBlockSize++;
      } else {
        if (tempBlockSize > maxBlockSize) {
          maxBlockSize = tempBlockSize;
          startPosition = i - tempBlockSize;
        }
        tempBlockSize = 0;
      }
    }
    if (tempBlockSize > maxBlockSize) {
      maxBlockSize = tempBlockSize;
      startPosition = MEMSIZE - tempBlockSize;
    }

    if (maxBlockSize >= size) {
      for (int i = startPosition; i < startPosition + size; i++) {
        memoryPool[i] = name;
      }
      printf("Allocated %d bytes for process %c using Worst-fit\n", size, name);
      return;
    }
  } else if (algo == 'B') {
    startPosition = -1;
    maxBlockSize = 0;
    tempBlockSize = 0;
    for (int i = 0; i < MEMSIZE; i++) {
      if (memoryPool[i] == '.') {
        tempBlockSize++;
      } else {
        if ((startPosition == -1 || tempBlockSize < maxBlockSize) &&
            tempBlockSize >= size) {
          maxBlockSize = tempBlockSize;
          startPosition = i - tempBlockSize;
        }
        tempBlockSize = 0;
      }
    }
    if ((startPosition == -1 || tempBlockSize < maxBlockSize) &&
        tempBlockSize >= size) {
      maxBlockSize = tempBlockSize;
      startPosition = MEMSIZE - tempBlockSize;
    }

    if (startPosition != -1) {
      for (int i = startPosition; i < startPosition + size; i++) {
        memoryPool[i] = name;
      }
      printf("Allocated %d bytes for process %c using Best-fit\n", size, name);
      return;
    }

  }
  printf("Not enough contiguous memory for process %c\n", name);
}

void compactMemory() {
  int emptyPosition = 0;
  for (int i = 0; i < MEMSIZE; ++i) {
    if (memoryPool[i] == '.') {
      int nextNonEmpty = i + 1;
      while (nextNonEmpty < MEMSIZE && memoryPool[nextNonEmpty] == '.')
        nextNonEmpty++;

      if (nextNonEmpty < MEMSIZE) {
        // Move data to the empty position
        memoryPool[emptyPosition] = memoryPool[nextNonEmpty];
        memoryPool[nextNonEmpty] = '.';
        emptyPosition++;
      }
    } else {
      emptyPosition++;
    }
  }
}

int executeFile(char *fileName) {
  FILE *file = fopen(fileName, "r");
  if (file == NULL) {
    printf("Error opening file %s\n", fileName);
    return 1;
  }

  char cmd;
  char name;
  char algo;
  char fileNameTwo[100];
  int size;
  while (fscanf(file, " %c", &cmd) == 1) {
    switch (cmd) {
    case 'A':
      if (fscanf(file, " %c %d %c", &name, &size, &algo) != 3) {
        printf("Invalid input for allocation command\n");
        break;
      }
      if (algo != 'F' && algo != 'W' && algo != 'B') {
        printf("Invalid algorithm type\n");
        break;
      }
      allocateMemory(name, size, algo);
      break;
    case 'F':
      if (fscanf(file, " %c", &name) != 1) {
        printf("Invalid input for free command\n");
        break;
      }
      freeMemory(name);
      break;
    case 'C':
      compactMemory();
      printf("Memory Compacted\n");
      break;
    case 'R':
      if (fscanf(file, "%s", fileNameTwo) != 1) {
        printf("Invalid input for file execution command\n");
        break;
      }
      int exitOrNot = executeFile(fileNameTwo);
      printf("Commands executed from file: %s\n", fileNameTwo);
      if (exitOrNot == 1) {
        return 1;
      }
      break;
    case 'S':
      showMemory();
      break;
    case 'E':
      return 1;
      break;
    default:
      printf("Invalid command in file\n");
      break;
    }
  }
  fclose(file);
  return 0;
}

int main() {
  initializeMemory();
  // Read commands from terminal
  char cmd;
  char name;
  char algo;
  int size;
  char fileName[100];
  printf("Enter commands!\n");
  while (scanf(" %c", &cmd) == 1 && cmd != 'E') {
    switch (cmd) {
    case 'A':
      if (scanf(" %c %d %c", &name, &size, &algo) != 3) {
        printf("Invalid input for allocation command\n");
        break;
      }
      if (algo != 'F' && algo != 'W' && algo != 'B') {
        printf("Invalid algorithm type\n");
        break;
      }
      allocateMemory(name, size, algo);
      break;
    case 'F':
      if (scanf(" %c", &name) != 1) {
        printf("Invalid input for free command\n");
        break;
      }
      freeMemory(name);
      break;
    case 'C':
      compactMemory();
      printf("Memory Compacted\n");
      break;
    case 'S':
      showMemory();
      break;
    case 'R':
      if (scanf("%s", fileName) != 1) {
        printf("Invalid input for file execution command\n");
        break;
      }
      int exitOrNot = executeFile(fileName);
      printf("Commands executed from file: %s\n", fileName);
      if (exitOrNot == 1) {
        return 0;
      }
      break;
    default:

      printf("COMMAND NOT VALID, TRY AGAIN!\n");
      break;
    }
  }

  return 0;
}