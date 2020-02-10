//Musat Mihai-Robert 313CB

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp_header.h"

//declaram pixelul ca fiind format din cele trei culori : albastru(b), verde(g), rosu(r)

typedef struct Pixel {
  unsigned char b, g, r;
} Pixel;

//ce are un BMP ? -> header, info si o matrice de pixeli map

typedef struct BMP {
  struct bmp_fileheader header;
  struct bmp_infoheader info;
  Pixel **map;
} BMP;

//pentru fiecare cifra consider ca la pozitia din matrice (line,column) am valoarea value

typedef struct Digit {
  int value;
  int line;
  int column;
} Digit;

//reprezint cifrele intr-un vector de matrici formate din 0 si 1 

const int MAX_LENGTH = 256;
const Pixel WHITE_PIXEL = {255, 255, 255};

const int digitMasks[10][7][7] = 
{   
  // 0
  {{0, 0, 0, 0, 0, 0, 0},
   {0, 1, 1, 1, 1, 1, 0},
   {0, 1, 0, 0, 0, 1, 0},
   {0, 1, 0, 0, 0, 1, 0},
   {0, 1, 0, 0, 0, 1, 0},
   {0, 1, 1, 1, 1, 1, 0},
   {0, 0, 0, 0, 0, 0, 0}
  },
  // 1
  {{0, 0, 0, 0, 0, 0, 0}, 
   {0, 0, 0, 0, 0, 1, 0},
   {0, 0, 0, 0, 0, 1, 0},
   {0, 0, 0, 0, 0, 1, 0},
   {0, 0, 0, 0, 0, 1, 0},
   {0, 0, 0, 0, 0, 1, 0},
   {0, 0, 0, 0, 0, 0, 0}
  },
  // 2
  {{0, 0, 0, 0, 0, 0, 0},
   {0, 1, 1, 1, 1, 1, 0},
   {0, 0, 0, 0, 0, 1, 0},
   {0, 1, 1, 1, 1, 1, 0},
   {0, 1, 0, 0, 0, 0, 0},
   {0, 1, 1, 1, 1, 1, 0},
   {0, 0, 0, 0, 0, 0, 0}
  },
  // 3
  {{0, 0, 0, 0, 0, 0, 0},
   {0, 1, 1, 1, 1, 1, 0},
   {0, 0, 0, 0, 0, 1, 0},
   {0, 1, 1, 1, 1, 1, 0},
   {0, 0, 0, 0, 0, 1, 0},
   {0, 1, 1, 1, 1, 1, 0},
   {0, 0, 0, 0, 0, 0, 0}
  },
  // 4
  {{0, 0, 0, 0, 0, 0, 0},
   {0, 1, 0, 0, 0, 1, 0},
   {0, 1, 0, 0, 0, 1, 0},
   {0, 1, 1, 1, 1, 1, 0},
   {0, 0, 0, 0, 0, 1, 0},
   {0, 0, 0, 0, 0, 1, 0},
   {0, 0, 0, 0, 0, 0, 0}
  },
  // 5
  {{0, 0, 0, 0, 0, 0, 0},
   {0, 1, 1, 1, 1, 1, 0},
   {0, 1, 0, 0, 0, 0, 0},
   {0, 1, 1, 1, 1, 1, 0},
   {0, 0, 0, 0, 0, 1, 0},
   {0, 1, 1, 1, 1, 1, 0},
   {0, 0, 0, 0, 0, 0, 0}
  },
  // 6
  {{0, 0, 0, 0, 0, 0, 0},
   {0, 1, 1, 1, 1, 1, 0},
   {0, 1, 0, 0, 0, 0, 0},
   {0, 1, 1, 1, 1, 1, 0},
   {0, 1, 0, 0, 0, 1, 0},
   {0, 1, 1, 1, 1, 1, 0},
   {0, 0, 0, 0, 0, 0, 0}
  }, 
  // 7
  {{0, 0, 0, 0, 0, 0, 0},
   {0, 1, 1, 1, 1, 1, 0},
   {0, 0, 0, 0, 0, 1, 0},
   {0, 0, 0, 0, 0, 1, 0},
   {0, 0, 0, 0, 0, 1, 0},
   {0, 0, 0, 0, 0, 1, 0},
   {0, 0, 0, 0, 0, 0, 0}
  }, 
  // 8
  {{0, 0, 0, 0, 0, 0, 0},
   {0, 1, 1, 1, 1, 1, 0},
   {0, 1, 0, 0, 0, 1, 0},
   {0, 1, 1, 1, 1, 1, 0},
   {0, 1, 0, 0, 0, 1, 0},
   {0, 1, 1, 1, 1, 1, 0},
   {0, 0, 0, 0, 0, 0, 0}
  }, 
  // 9
  {{0, 0, 0, 0, 0, 0, 0},
   {0, 1, 1, 1, 1, 1, 0},
   {0, 1, 0, 0, 0, 1, 0},
   {0, 1, 1, 1, 1, 1, 0},
   {0, 0, 0, 0, 0, 1, 0},
   {0, 1, 1, 1, 1, 1, 0},
   {0, 0, 0, 0, 0, 0, 0}
  }
};

BMP readBMP (char *fileName)
{
  BMP bmp;
  int column, line;

  FILE *file = fopen (fileName, "rb");
  fseek (file, 2, SEEK_SET); //pozitionez cursorul la inceputul fisierului
  fread (&bmp.header, sizeof (bmp.header), 1, file);
  fread (&bmp.info, sizeof (bmp.info), 1, file);
  
  //aloc dinamic matricea de o dimensiune mai mare cu 2 pentru bordare 
  bmp.map = (Pixel **) malloc ((2 + bmp.info.height) * sizeof (Pixel *));
  bmp.map++;
  
  int padding = (4 - (sizeof (Pixel) * bmp.info.width % 4)) % 4;

  bmp.map[-1] = (Pixel *) malloc ((bmp.info.width + 2) * sizeof (Pixel)); //aloc dinamic prima linie de bordare
  bmp.map[-1]++;
 
  for (column = -1; column <= bmp.info.width; column++)
    bmp.map[-1][column] = WHITE_PIXEL; //fac albe toate coloanele care inconjoara cifra

  for (line = bmp.info.height - 1; line >= 0; line--) { //traversez invers matricea pentru ca imaginea este rasturnata
    bmp.map[line] = (Pixel *) malloc ((bmp.info.width + 2) * sizeof (Pixel)); //aloc dinamic prima linie din matrice 
    bmp.map[line]++;
    bmp.map[line][-1] = WHITE_PIXEL; //analog dupa ce bordez toate liniile din jur, le fac albe
    bmp.map[line][bmp.info.width] = WHITE_PIXEL; 
    fread (bmp.map[line], sizeof (Pixel), bmp.info.width, file);
    fseek (file, padding, SEEK_CUR); //pozitionez cursorul la pozitia curenta in fisier utilizand padding-ul
  }
  
  bmp.map[bmp.info.height] = (Pixel *) malloc (bmp.info.width * sizeof (Pixel)); //aloc toate coloanele din matrice
  bmp.map[bmp.info.height]++;

  for (column = -1; column <= bmp.info.width; column++)
    bmp.map[bmp.info.height][column] = WHITE_PIXEL; //apoi le fac albe

  fclose (file);

  return bmp;
}


int isWhite(Pixel p) {
  return p.r == 255 && p.g == 255 && p.b == 255;
}

void writeBMP (BMP * bmp, char *fileName)
{
  FILE *file = fopen (fileName, "wb");
  //headerul pare sa fie gresit pentru implementarea mea deoarece imi da eroare la rularea checker-ului
  //de aceea, am preferat sa comentez primele 2 declaratii din structura bmp_fileheader
  //si sa afisez eu literele B si M
  fprintf (file, "BM"); 
  fwrite (&bmp->header, sizeof (bmp->header), 1, file);
  fwrite (&bmp->info, sizeof (bmp->info), 1, file);

  int padding = (4 - (sizeof (Pixel) * bmp->info.width % 4)) % 4;

  int line, i;
  for (line = bmp->info.height - 1; line >= 0; line--)
    {
      fwrite (bmp->map[line], sizeof (Pixel), bmp->info.width, file);
      for (i = 0; i < padding; i++)
	fprintf (file, "%c", 0); //pun 0 pe octetii de padding
    }

  fclose (file);
}


BMP task1 (BMP * picture, Pixel pixel)
{
  BMP output = *picture;
  int i, j;
  for (i = 0; i < output.info.height; i++)
    for (j = 0; j < output.info.width; j++)
      if (!isWhite(output.map[i][j])) //verific ca pixelul de la pozitia (i,j) sa nu fie alb
	output.map[i][j] = pixel; //daca da, ii pun valoarea de la pixel
  return output;
}

int digitCount; 
Digit digits[400];

char *task2 (BMP * picture, char* output)
{
  int line, column, d, i, j;
  digitCount = 0;
  for (column = 0; column + 4 < picture->info.width; column++) {
    for (line = 0; line + 4 < picture->info.height; line++) {
      for (d = 0; d <= 9; d++) {
        int matches = 0; //numarul de potriviri
        for (i = -1; i < 6; i++) {
          for (j = -1; j < 6; j++) {
            matches += isWhite(picture->map[line + i][column + j]) != digitMasks[d][i + 1][j + 1];
          }
        }
        if (matches == 49) { //49 este numarul care se obtine cand fac potrivire pe toate cifrele
          digits[digitCount].value = d;
          digits[digitCount].line = line;
          digits[digitCount].column = column;
          output[digitCount] = '0' + d; //conversie din int in char corespunzator
          digitCount++;
        }
      }
    }
  }
  output[digitCount] = '\0'; //pun la final in vector '\0'
  return output;
}

BMP task3 (BMP * picture, int *isErased)
{
  BMP output = *picture;
  int source, i, j;
  int dest = 0;
  //copiez cifrele care imi trebuiesc de la sursa la destinatie
  for (source = 0; source < digitCount; source++) {
    if (!isErased[digits[source].value]) {
      for (i = -1; i < 6; i++) {
        for (j = -1; j < 6; j++) {
          output.map[digits[dest].line + i][digits[dest].column + j]
              = picture->map[digits[source].line + i][digits[source].column + j];
        }
      }
      dest++;
    }
  }
  //cifrele care nu imi trebuiesc le fac albe
  for(; dest < digitCount; dest++)
    for (i = -1; i < 6; i++)
      for (j = -1; j < 6; j++)
        output.map[digits[dest].line + i][digits[dest].column + j] = WHITE_PIXEL;
  return output;
}

int main ()
{
  char fileName[MAX_LENGTH + 1];
  char bonusFileName[MAX_LENGTH + 1];
  Pixel task1Pixel;
  int i, task3Erase[10];

  for (i = 0; i < 10; i++)
    task3Erase[i] = 0;

  FILE *inputFile = fopen ("input.txt", "r");
  fscanf (inputFile, "%s", fileName);
  fscanf (inputFile, "%hhu%hhu%hhu", &task1Pixel.b, &task1Pixel.g,&task1Pixel.r);
  
  char ch = '\0';
  do
    {
      fscanf (inputFile, "%c", &ch);
    }
  while (ch != '\n');
  do 
  {
      fscanf (inputFile, "%c", &ch);
      if ('0' <= ch && ch <= '9')
	task3Erase[ch - '0'] = 1;
    }
  while (ch != '\n');

  fscanf (inputFile, "%s", bonusFileName);
  fclose (inputFile);

  printf ("%s\n", fileName);
  printf ("%hhu %hhu %hhu\n", task1Pixel.r, task1Pixel.g, task1Pixel.b);

  for (i = 0; i < 10; i++)
    if (task3Erase[i])
      printf ("%d ", i);
  printf ("\n");
  printf ("%s\n", bonusFileName);

  BMP input = readBMP (fileName);
  BMP inputCopy = readBMP (fileName);
  fileName[strlen (fileName) - 4] = '\0'; //sterg .bmp de la final

  //imi fac fisierele pentru task-ul 1 
  BMP outputTask1 = task1 (&input, task1Pixel);
  char outputTask1FileName[MAX_LENGTH + 1];
  sprintf (outputTask1FileName, "%s_task1.bmp", fileName);
  writeBMP (&outputTask1, outputTask1FileName);

  //imi fac fisierele pentru task-ul 2 
  char outputTask2[MAX_LENGTH + 1];
  task2 (&input, outputTask2);
  char outputTask2FileName[MAX_LENGTH + 1];
  sprintf (outputTask2FileName, "%s_task2.txt", fileName);
  FILE *outputTask2File = fopen (outputTask2FileName, "wb");
  fprintf (outputTask2File, "%s\n", outputTask2);
  fclose (outputTask2File);

  //imi fac fisierele pentru task-ul 3 
  BMP outputTask3 = task3 (&inputCopy, task3Erase);
  char outputTask3FileName[MAX_LENGTH + 1];
  sprintf (outputTask3FileName, "%s_task3.bmp", fileName);
  writeBMP (&outputTask3, outputTask3FileName);

  return 0;
}
