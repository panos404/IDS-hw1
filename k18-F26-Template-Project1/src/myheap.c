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
  int file,file2;

  // BF_Init(LRU);
  BF_PrintError( HP_CreateFile("DBs/poutsa.txt"));
  BF_PrintError( HP_CreateFile("DBs/piss.txt"));
  
  BF_PrintError( HP_CreateFile("DBs/idk.txt"));
  BF_PrintError( HP_CreateFile("DBs/idk2.txt"));
  BF_PrintError( HP_CreateFile("DBs/idk3.txt"));
  // BF_Close();

  // BF_Init(LRU);
  BF_PrintError(HP_OpenFile("DBs/piss.txt",&file2));
  HP_OpenFile("DBs/poutsa.txt",&file);
  Record rec = randomRecord();
  printf("\nLOOKING FOR:fds in main %d\n", file);
  
  HP_InsertEntry(file, rec);
  // printf("files %d,%d\n",file,file2);
  BF_PrintError(HP_CloseFile(file));
  // BF_PrintError(HP_CloseFile(file2));
  // BF_Close();

  return EXIT_SUCCESS;
}
