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
  int fileDesc;
  int fileDesc2;
  int num_lines=0;
  
  if(!(argc == 3 && !strcmp(argv[1],"-i"))){
    printf("Change arguements\n");
    return 1;
  }
  
  printf("Opening insertion file\n");
  FILE *file=fopen(argv[2],"r");
  if(file == NULL){
    return 1;
  }
  int c;
  while(( c = getc(file) )!= EOF){
    if (c == '\n'){
      num_lines++;
    }
  }
  rewind(file);
  Record recs[num_lines];

  int reco_nums=0;
  printf("Pulling records...\n");
    while(
      fscanf(file,"%d %s %s %d %s",&recs[reco_nums].id,recs[reco_nums].firstname,recs[reco_nums].lastname,&recs[reco_nums].postcode,recs[reco_nums].city)
      !=EOF){
        // printRecord(recs[reco_nums]);
        reco_nums++;
    }
  
  printf("Closing insertion file, pulled %d records\n", reco_nums);
  fclose(file);
  
  //CreateFile
  printf("\n======== DEMO-CreateFile ========\n");
  HP_CreateFile(DB_FILE);
  HP_CreateFile("DBs/newfile.db");
  printf("======== ENDDEMO-CreateFile ========\n");

  // OpenFile
  printf("\n======== DEMO-OpenFile ========\n");
  HP_OpenFile(DB_FILE, &fileDesc);
  HP_OpenFile("DBs/newfile.db", &fileDesc2);
  printf("======== ENDDEMO-OpenFile ========\n");

  printf("\n======== DEMO-InsertEntry ========\n");
  for(int i=0;i<num_lines;i++){
    HP_InsertEntry(fileDesc,recs[i]);
  }
  printf("======== ENDDEMO_InsertEntry ========\n");

  printf("\n======== DEMO PrintAllEntries id NULL ========\n");
  HP_PrintAllEntries(fileDesc,"id",NULL);
  printf("======== ENDDEMO-PrintAllEntries ========\n");

  printf("\n======== DEMO-PrintAllEntries post=26221 ========\n");
  int post=26221;
  printf("Entries w/ postcode: 26221: \n");
  HP_PrintAllEntries(fileDesc,"postcode",&post);
  printf("\t==ENDDEMO-PrintAllEntries==\n");
  
  printf("\n======== DEMO-PrintAllEntries name=petros ========\n");
  HP_PrintAllEntries(fileDesc,"firstname","Petros");
  printf("======== ENDDEMO-PrintAllEntries ========\n");
  

  printf("\n======== DEMO-GetEnry ========\n");
  Record rec;
  HP_GetEntry(fileDesc,2,4,&rec);
  printf("...Pulled record ");
  printRecord(rec);
  printf("======== ENDDEMO-GetEnry ========\n");

  printf("\n======== DEMO-GetNextRecordWithId ========\n");
  int id1 = 203;
  int id2 = 151;
  printf("...Calling function for id:%d\n", id1);
  HP_GetNextRecordWithID(fileDesc, &rec, &id1);
  printRecord(rec);
  printf("...Calling again for id:%d\n", id1);
  HP_GetNextRecordWithID(fileDesc, &rec, &id1);
  printRecord(rec);
  printf("...Calling function for id:%d\n", id2);
  HP_GetNextRecordWithID(fileDesc, &rec, &id2);
  printRecord(rec);
  printf("...Calling again for id: %d\n", id1);
  HP_GetNextRecordWithID(fileDesc, &rec, &id1);
  printRecord(rec);
  printf("======== ENDDEMO-GetNextRecordWithId ========\n");
  

  HP_CloseFile(fileDesc);
  HP_CloseFile(fileDesc2);
  return EXIT_SUCCESS;
}
