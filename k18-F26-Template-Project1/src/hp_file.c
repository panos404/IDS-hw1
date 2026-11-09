#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bf.h"
#include "block_records.h"
#include "hp_file.h"
#include "record.h"

HP_info metadata[BF_MAX_OPEN_FILES];  //array of metadata of open files
int i=-1; //counter to get metadata for open file

/* Βοηθητικό macro: καλεί μια συνάρτηση του επιπέδου BF και, σε περίπτωση
   σφάλματος, εκτυπώνει την περιγραφή του και επιστρέφει HP_ERROR */
#define CALL_BF(call)         \
  {                           \
    BF_ErrorCode code = call; \
    if (code != BF_OK) {      \
      BF_PrintError(code);    \
      return HP_ERROR;        \
    }                         \
  }

#define HP_FLAG "heap_file"

HP_ErrorCode set_metadata(int fileDesc, int slot, struct HP_info info){
  if (slot < 0 || slot >= RECORDS_PER_BLOCK)
  return HP_ERROR;
  // extract metadata block
  BF_Block *block;
  BF_Block_Init(&block);
  // printf("file: %d\n",fileDesc);
  CALL_BF(BF_GetBlock(fileDesc, 0, block));

  // printf("The metadata is:\n %s, %d, %d", 
  //   info.fileName,
  //   info.num_id, info.max_recs);

  char *data = BF_Block_GetData(block);

  memcpy(data + slot * sizeof(HP_info), &info, sizeof(HP_info));

  printf("hi\n");

  //apodesmeusi
  BF_Block_SetDirty(block);
  CALL_BF(BF_UnpinBlock(block));
  BF_Block_Destroy(&block);//doesnt get re written on disk ,issue
  return HP_OK;
}

HP_ErrorCode get_metadata(int fileDesc, struct HP_info *info){
  //extract metadata block
  BF_Block *block;
  BF_Block_Init(&block);
  CALL_BF(BF_GetBlock(fileDesc, 0, block));

  char *data = BF_Block_GetData(block);

  memcpy(info, data, sizeof(HP_info));

  //apodesmeusi
 // CALL_BF(BF_UnpinBlock(block));//need pinned the meta data block
  BF_Block_Destroy(&block);

  return HP_OK;
}

int find_hp_info(int fileDesc){//finds metadata hp info struct for a open file
  for (int j=0;j<=i;j++){
    printf("find %d,%d\n",metadata[j].fileDesc,fileDesc);
    if (metadata[j].fileDesc == fileDesc){
      return j;
    }
  }
  return -1;//file not found
}


HP_ErrorCode HP_CreateFile(const char *fileName) {
  /* TODO: δημιουργήστε το αρχείο, δεσμεύστε το πρώτο μπλοκ και αποθηκεύστε
     εκεί ένα στιγμιότυπο της HP_info */
  int file_desc;
  BF_Block *new_block;
  if(i<0)
    BF_Block_Init(&new_block);

  
  CALL_BF(BF_Init(LRU));
  // printf("edw1\n");

  CALL_BF(BF_CreateFile(fileName));
  CALL_BF(BF_OpenFile(fileName, &file_desc));
  i++;
  // printf("after open\n");
  CALL_BF(BF_AllocateBlock(file_desc, new_block));

  printf("after alloc %d, %s\n",i,fileName);

  // initiliaze metadata
  strcpy(metadata[i].fileName, fileName);
  // printf("after strcpy\n");
  metadata[i].fileDesc = file_desc;
  metadata[i].num_id = 0;
  metadata[i].first_free_block = 0;
  metadata[i].blockstaken = 0;
  strcpy(metadata[i].file_type, HP_FLAG);
  metadata[i].max_recs = RECORDS_PER_BLOCK;

  // printf("The metadata is:\n %s, %d, %d\n", 
  //   metadata[i].fileName,
  //   metadata[i].num_id, metadata[i].max_recs);

  // put metadata block
  set_metadata(file_desc, 0, metadata[i]);
  BF_UnpinBlock(new_block);
  BF_Block_Destroy(&new_block);
  CALL_BF(BF_CloseFile(file_desc));
  i--;
  if(i=0)
    CALL_BF(BF_Close());
  printf("end1\n");
 return HP_OK;
}

HP_ErrorCode HP_OpenFile(const char *fileName, int *fileDesc) {
  /* TODO: ανοίξτε το αρχείο, διαβάστε τα μεταδεδομένα από το πρώτο μπλοκ και
     ελέγξτε ότι πρόκειται όντως για αρχείο σωρού */
    if(i<0)
     CALL_BF(BF_Init(LRU));

    CALL_BF(BF_OpenFile(fileName,fileDesc));
    printf("file in open: %d",*fileDesc);
    i++;

    get_metadata(*fileDesc,&metadata[i]);
    printf("file in open: %s\n",metadata[i].fileName);
    if(strcmp(metadata[i].file_type ,HP_FLAG)){
      printf("error with strcmp\n");
      return HP_ERROR;
    }

    metadata[i].fileDesc = *fileDesc;
    printf("%d\n",*fileDesc);
    
    // CALL_BF(BF_Close());
  printf("end2\n");
  return HP_OK;
}

HP_ErrorCode HP_CloseFile(int fileDesc) {
  /* TODO: γράψτε στον δίσκο ό,τι εκκρεμεί και κλείστε το αρχείο */
  // CALL_BF(BF_Init(LRU));

  int j = find_hp_info(fileDesc);
  printf("bf md %d %d,%d,%d\n",i,j,fileDesc,metadata[j].fileDesc);
  // BF_Block *block;
  // BF_Block_Init(&block);
  // for(int z = 0;z < metadata[j].num_id; z++){
  //   CALL_BF(BF_GetBlock(fileDesc,z,block));

  // }

  set_metadata(fileDesc,0,metadata[j]);
  printf("af md\n");
  CALL_BF(BF_CloseFile(fileDesc));
  metadata[j]=metadata[i];
  i--;
  if(i<0)
    CALL_BF(BF_Close());
  printf("end3\n");
  return HP_OK;
}

HP_ErrorCode HP_InsertEntry(int fileDesc, Record record) {
  /* TODO: βρείτε ή δεσμεύστε το κατάλληλο μπλοκ και αντιγράψτε εκεί την
     εγγραφή με την blockSetRecord. Μην ξεχάσετε να ενημερώσετε τα
     μεταδεδομένα του αρχείου. */
  return HP_ERROR;
}

HP_ErrorCode HP_PrintAllEntries(int fileDesc, char *fieldName, void *value) {
  /* TODO: διατρέξτε τα μπλοκ του αρχείου και εκτυπώστε τις εγγραφές που στο
     πεδίο fieldName έχουν τιμή value. Αν value == NULL, εκτυπώστε τις όλες. */
  return HP_ERROR;
}

HP_ErrorCode HP_GetEntry(int fileDesc, int blockid, int position,
                         Record *record) {
  /* TODO: φέρτε το μπλοκ blockid και διαβάστε την εγγραφή της θέσης position
     με την blockGetRecord */
  return HP_ERROR;
}

HP_ErrorCode HP_GetNextRecordWithID(int fileDesc, Record *record, void *value) {
  /* TODO: υλοποιήστε τον iterator. Η κατάσταση του (πού σταμάτησε η
     προηγούμενη κλήση) πρέπει να επιβιώνει ανάμεσα στις κλήσεις. */
  return HP_ERROR;
}
