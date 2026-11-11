#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bf.h"
#include "hp_file.h"
#include "record.h"

/* Τα αρχεία βάσης δημιουργούνται στον κατάλογο DBs/ του project */
#define DB_FILE "DBs/myheap.db"

/*
 * O driver της ασκησης.
 *
 * Το εκτελέσιμο πρέπει να διαβάζει τις εγγραφές που αποθηκεύει από ένα αρχείο
 * κειμένου, το οποίο υποδεικνύεται με τη σημαία -i:
 *
 *     ./bin/myheap -i inputs/records.txt
 *
 * Κάθε γραμμή του αρχείου περιέχει μία εγγραφή, με τα πεδία χωρισμένα με κενά
 * και στη σειρά που έχουν στη δομή Record:
 *
 *     id firstname lastname postcode city
 *     151 Dimitra Stasinou 12376 Ioannina
 *
 * Ο driver θα πρέπει να δημιουργεί το αρχείο σωρού, να εισάγει σε αυτό τις
 * εγγραφές του αρχείου εισόδου, και να επιδεικνύει τη λειτουργία των
 * συναρτήσεων του API.
 */

int main(int argc, char *argv[]) {
  int file;

  // BF_Init(LRU);
  BF_PrintError( HP_CreateFile("DBs/poutsa.txt"));
  // BF_PrintError( HP_CreateFile("DBs/piss.txt"));
  
  // BF_PrintError( HP_CreateFile("DBs/idk.txt"));
  // BF_PrintError( HP_CreateFile("DBs/idk2.txt"));
  // BF_PrintError( HP_CreateFile("DBs/idk3.txt"));
  // BF_Close();

  // BF_Init(LRU);
  // BF_PrintError(HP_OpenFile("DBs/piss.txt",&file2));
  HP_OpenFile("DBs/poutsa.txt",&file);

  Record rec = randomRecord();
  Record rec2 = randomRecord();
  printf("I put Record with id %d and name %s\n", rec.id, rec.firstname);
  printf("\nLOOKING FOR:fds in main %d\n", file);
  int p=0;
  HP_InsertEntry(file, rec);
  HP_InsertEntry(file, rec2);

  // printf("files %d,%d\n",file,file2);
  // BF_PrintError(HP_CloseFile(file));
  Record getrec;
  Record getrec2;
  HP_GetEntry(file, 1, 0, &getrec);
  HP_GetEntry(file, 1, 1, &getrec2);
  // BF_PrintError(HP_CloseFile(file2));
  printf("-For first record\n--Expected:%d, %s Got:%d, %s\n", rec.id, rec.firstname, getrec.id, getrec.firstname);
  printf("-For second record\n--Expected:%d, %s Got:%d, %s\n", rec2.id, rec2.firstname, getrec2.id, getrec2.firstname);


  // BF_Close();
  

  return EXIT_SUCCESS;
}
