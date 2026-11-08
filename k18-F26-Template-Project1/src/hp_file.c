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

#define HP_FLAG "hp_flag=1"

void set_metadata(int fileDesc, int slot, struct HP_info info){
  if (slot < 0 || slot >= RECORDS_PER_BLOCK)
  return -1;
  // extract metadata block
  BF_Block *block;
  BF_Block_Init(&block);
  BF_GetBlock(fileDesc, 0, block);

  printf("The metadata is:\n %s, %d, %d", 
    info.fileName,
    info.num_id, info.max_recs);

  char *data = BF_Block_GetData(block);

  printf("%p\n", data);


  memcpy(data + slot * sizeof(HP_info), &info, sizeof(HP_info));

  printf("hi\n");

  //apodesmeusi
  BF_Block_SetDirty(block);
  CALL_BF(BF_UnpinBlock(block));
  BF_Block_Destroy(&block);
}

void get_metadata(int fileDesc, struct HP_info *info){
  //extract metadata block
  BF_Block *block;
  BF_GetBlock(fileDesc, 0, block);

  char *data = BF_Block_GetData(block);

  memcpy(info, data, sizeof(info));

  //apodesmeusi
  CALL_BF(BF_UnpinBlock(block));
}


HP_ErrorCode HP_CreateFile(const char *fileName) {
  /* TODO: δημιουργήστε το αρχείο, δεσμεύστε το πρώτο μπλοκ και αποθηκεύστε
     εκεί ένα στιγμιότυπο της HP_info */
  int file_desc;
  BF_Block *new_block;
  BF_Block_Init(&new_block);


  CALL_BF(BF_Init(LRU));
  printf("edw1\n");
  CALL_BF(BF_CreateFile(fileName));
  
  int counter;
  CALL_BF(BF_OpenFile(fileName, &file_desc));
  CALL_BF(BF_AllocateBlock(file_desc, new_block));
  CALL_BF(BF_UnpinBlock(new_block));
  
  // CALL_BF(BF_GetBlockCounter(&file_desc, &counter));
    i++;


  
  printf("edw4");
  // initiliaze metadata
  metadata[i].fileName = fileName;
  metadata[i].num_id = 0;
  metadata[i].first_free_block = 0;
  metadata[i].blockstaken = 0;
  metadata[i].max_recs = RECORDS_PER_BLOCK;

  printf("The metadata is:\n %s, %d, %d", 
    metadata[i].fileName,
    metadata[i].num_id, metadata[i].max_recs);

  // put metadata block
  set_metadata(file_desc, 0, metadata[i]);

  BF_Block_Destroy(&new_block);
  CALL_BF(BF_CloseFile(file_desc));
  i--;
  
  CALL_BF(BF_Close());
  //format of meta hp data block: #hp_flag=1 #record organization
  return HP_ERROR;
}

// HP_ErrorCode HP_OpenFile(const char *fileName, int *fileDesc) {
//   /* TODO: ανοίξτε το αρχείο, διαβάστε τα μεταδεδομένα από το πρώτο μπλοκ και
//      ελέγξτε ότι πρόκειται όντως για αρχείο σωρού */
//     BF_Block *meta;
//     BF_Init(LRU);
//     CALL_BF(BF_OpenFile(fileName,fileDesc));
//     BF_GetBlock(fileDesc,0,meta);

//     char* meta_data=BF_Block_GetData(meta);
//     char** data;
//     int count=0;
//     int length=0;

//     for(int i=0;i<strlen(meta_data);i++){//exctract meta data 
//       if(meta_data[i]!=';'){
//         data[count][length]=meta_data[i];
//         length++;
//       }else{
//         length=0;
//         count++;
//       }
//     }

//     if(data[0]==HP_FLAG){//check if heap file
      
//     }

//   return HP_ERROR;
// }

HP_ErrorCode HP_CloseFile(int fileDesc) {
  /* TODO: γράψτε στον δίσκο ό,τι εκκρεμεί και κλείστε το αρχείο */
  return HP_ERROR;
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
