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
  int num_lines=0;
  
  if(!(argc == 3 && !strcmp(argv[1],"-i"))){
    printf("Change arguements\n");
    return 1;
  }
  

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

    while(
      fscanf(file,"%d %s %s %d %s",&recs[reco_nums].id,recs[reco_nums].firstname,recs[reco_nums].lastname,&recs[reco_nums].postcode,recs[reco_nums].city)
      !=EOF){
        printRecord(recs[reco_nums]);
        reco_nums++;

    }
  


  fclose(file);

  BF_PrintError( HP_CreateFile(DB_FILE));
  
  HP_OpenFile(DB_FILE,&fileDesc);

  for(int i=0;i<num_lines;i++){
    HP_InsertEntry(fileDesc,recs[i]);
  }
  Record rec;
  
  HP_GetEntry(fileDesc,1,0,&rec);
  printRecord(rec);
  HP_GetEntry(fileDesc,2,3,&rec);
  printRecord(rec);

  int post=26221;
  HP_PrintAllEntries(fileDesc,"firstname","Petros");
  HP_PrintAllEntries(fileDesc,"id",NULL);
  HP_PrintAllEntries(fileDesc,"postcode",&post);

  HP_CloseFile(fileDesc);

  return EXIT_SUCCESS;
}
