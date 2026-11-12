#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "bf.h"
#include "block_records.h"
#include "hp_file.h"
#include "record.h"

HP_info metadata[BF_MAX_OPEN_FILES];  //array of metadata of open files
int i = -1; //counter to get metadata for open file

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


  //apodesmeusi
  BF_Block_SetDirty(block);
  CALL_BF(BF_UnpinBlock(block));
  BF_Block_Destroy(&block);//doesnt get re written on disk ,issue
  return HP_OK;
}

HP_ErrorCode get_metadata(int fileDesc, int slot, struct HP_info *info){
  //extract metadata block
  BF_Block *block;
  BF_Block_Init(&block);
  CALL_BF(BF_GetBlock(fileDesc, 0, block));

  char *data = BF_Block_GetData(block);

  if (!memcpy(info, data + slot, sizeof(HP_info)))
    return HP_EOF;

  //apodesmeusi
 // CALL_BF(BF_UnpinBlock(block));//need pinned the meta data block
  BF_Block_Destroy(&block);

  return HP_OK;
}

int find_hp_info(int fileDesc){//finds metadata hp info struct for a open file
  for (int j=0;j <= i;j++){
    // printf("There are %d files open\n", i);
    // printf("find %d,%d\n",metadata[j].fileDesc,fileDesc);
    if (metadata[j].fileDesc == fileDesc){
      // printf("returning %d\n", j);
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
  BF_Block_Init(&new_block);

  // if no other file is open, open the BF level
  if (i < 0)
    CALL_BF(BF_Init(LRU));

  if(BF_CreateFile(fileName) == BF_FILE_ALREADY_EXISTS){
    perror("Error");
    BF_Block_Destroy(&new_block);
    if(i < 0){
      // printf("close bf in already created file\n");
      BF_Close();
    }
    return HP_ERROR;
  }
  CALL_BF(BF_OpenFile(fileName, &file_desc));
  i++;
  
  CALL_BF(BF_AllocateBlock(file_desc, new_block));

  // initiliaze metadata
  strcpy(metadata[i].fileName, fileName);
  
  metadata[i].fileDesc = file_desc;
  metadata[i].last_bid = -1;
  strcpy(metadata[i].file_type, HP_FLAG);
  metadata[i].recs_last = 0;

  // initialize the hashtable
  for (int j=0; j < 25; j++){
    metadata[i].iterHTable[j].valueid = -1; //doesn't correspond to value
    metadata[i].iterHTable[j].iterator.bid = 1;
    metadata[i].iterHTable[j].iterator.slot = 0;
  }

  // put metadata block
  set_metadata(file_desc, 0, metadata[i]);
  i--;
  BF_UnpinBlock(new_block);
  BF_Block_Destroy(&new_block);
  CALL_BF(BF_CloseFile(file_desc));
  printf("Successfully create new file\n");
  if(i < 0)
    CALL_BF(BF_Close());
  return HP_OK;
}

HP_ErrorCode HP_OpenFile(const char *fileName, int *fileDesc) {
  /* TODO: ανοίξτε το αρχείο, διαβάστε τα μεταδεδομένα από το πρώτο μπλοκ και
     ελέγξτε ότι πρόκειται όντως για αρχείο σωρού */
    if(i<0)
     CALL_BF(BF_Init(LRU));

    if(BF_OpenFile(fileName,fileDesc) == BF_ERROR){
      printf("File does not exist.\n");
      if(i < 0)
       BF_Close();
      return HP_ERROR;
    }
    i++;

    get_metadata(*fileDesc, 0, &metadata[i]);
    if(strcmp(metadata[i].file_type ,HP_FLAG)){
      printf("Not a heap file.\n");
      return HP_ERROR;
    }
    
    printf("...Opened file %s with fds %d\n", fileName, *fileDesc);
    metadata[i].fileDesc = *fileDesc;
    
  return HP_OK;
}

HP_ErrorCode HP_CloseFile(int fileDesc) {
  /* TODO: γράψτε στον δίσκο ό,τι εκκρεμεί και κλείστε το αρχείο */
  // CALL_BF(BF_Init(LRU));

  int j = find_hp_info(fileDesc);
  if (j == -1){
    printf("You must open the file first\n");
    return HP_ERROR;
  }
  // BF_Block *block;
  // BF_Block_Init(&block);
  // for(int z = 0;z < metadata[j].num_id; z++){
  //   CALL_BF(BF_GetBlock(fileDesc,z,block));

  // }

  set_metadata(fileDesc,0,metadata[j]);

  CALL_BF(BF_CloseFile(fileDesc));
  metadata[j]=metadata[i];
  i--;
  if(i<0)
    CALL_BF(BF_Close());
  
  return HP_OK;
}

HP_ErrorCode HP_InsertEntry(int fileDesc, Record record) {
  /* TODO: βρείτε ή δεσμεύστε το κατάλληλο μπλοκ και αντιγράψτε εκεί την
     εγγραφή με την blockSetRecord. Μην ξεχάσετε να ενημερώσετε τα
     μεταδεδομένα του αρχείου. */

  // βαζει ενα record τη φορα

  // εχουμε ηδη ανοιχτο το αρχειο με HP_Open
  // και εχουμε το fileDesc
  // οποτε παιρνουμε τα metadata του file

  // get position for metadata on the array
  int mdpos = find_hp_info(fileDesc);

  if (mdpos == -1){
    printf("You must open the file first\n");
    return HP_ERROR;
  }

  // get pointer for the block
  BF_Block *block;
  BF_Block_Init(&block);

  // get block_id of the available block in the file
  int bid = metadata[mdpos].last_bid;

  // αν δεν υπαρχει free block(-1) φτιαξε ενα καινουργιο
  // και αποθηκευσε την τιμη του
  if (bid == -1){
    CALL_BF(BF_AllocateBlock(fileDesc, block));
    // fill me dummy records 
    blockInitSlots(block);
    
    // get the position of the new block (the new first free)
    BF_GetBlockCounter(fileDesc, &bid);
    bid--;

    // put it back in metadata table
    metadata[mdpos].last_bid = bid;
  }
  else{
    // αν υπαρχει ηδη μπλοκ παρτο 
    BF_GetBlock(fileDesc, bid, block);
  }


  if (metadata[mdpos].recs_last == RECORDS_PER_BLOCK){
    BF_UnpinBlock(block);
    BF_Block_Destroy(&block);

    BF_Block_Init(&block);
    BF_AllocateBlock(fileDesc, block);

    int lbid;
    BF_GetBlockCounter(fileDesc, &lbid);
    metadata[mdpos].last_bid = lbid - 1;
    metadata[mdpos].recs_last = 0;
  }

  int slot = metadata[mdpos].recs_last;
  printf("...Putting record in block %d | slot %d\n", metadata[mdpos].last_bid, slot);
  blockSetRecord(block, slot, record);
  metadata[mdpos].recs_last ++;
  // records in the blocks
  set_metadata(fileDesc, 0, metadata[mdpos]);

  BF_Block_SetDirty(block);
  BF_UnpinBlock(block);
  BF_Block_Destroy(&block);
  return HP_OK;
}

HP_ErrorCode HP_PrintAllEntries(int fileDesc, char *fieldName, void *value) {
  /* TODO: διατρέξτε τα μπλοκ του αρχείου και εκτυπώστε τις εγγραφές που στο
     πεδίο fieldName έχουν τιμή value. Αν value == NULL, εκτυπώστε τις όλες. */
    BF_Block *block;
    BF_Block_Init(&block);
    Record rec;

    //find metadata
    int j = find_hp_info(fileDesc);
    if (j == -1){
     printf("You must open the file first\n");
      BF_Block_Destroy(&block);
      return HP_ERROR;
    
    }
    int num_records=RECORDS_PER_BLOCK;

    //itirate till last active block except metadata block
    // printf("last block: %d\n",metadata[j].last_bid);
    for(int z = 1;z <= metadata[j].last_bid;z++){
      BF_GetBlock(fileDesc,z,block);
      if (z == metadata[j].last_bid){

        num_records = metadata[j].recs_last;
      }
      for(int slot = 0;slot < num_records; slot++){//max_recs of block z need to be defined in entry

        blockGetRecord(block,slot,&rec);//using metadata to know the way records are saveed in the block
        if(value == NULL){
          printf("Block: %d,Slot: %d",z,slot);
          printRecord(rec);
        } else if(!strcmp(fieldName,"city") && !strcmp(rec.city, (char*)value)){
          printRecord(rec);
        }else if (!strcmp(fieldName, "firstname") && !strcmp(rec.firstname, (char*)value)){
          printRecord(rec);
        }else if(!strcmp( fieldName, "lastname") && !strcmp(rec.lastname, (char*)value)){
          printRecord(rec);
        }else if(!strcmp(fieldName , "postcode") && rec.postcode == *(int*)value){
          printRecord(rec);
        }
        else if(value == NULL){
          printRecord(rec);
        };
      }
      BF_UnpinBlock(block);
    }
    BF_Block_Destroy(&block);
  return HP_OK;
}

HP_ErrorCode HP_GetEntry(int fileDesc, int blockid, int position, Record *record) {
  /* TODO: φέρτε το μπλοκ blockid και διαβάστε την εγγραφή της θέσης position
     με την blockGetRecord */

  // 1.παιρνει block
  // 2.παει στο position
  // 3.καλει get record

  // καλο ειναι να τσεκαρει και το position 
  // οτι ειναι μεσα στα γραμμενα records του block

  // get metadata
  int mdpos = find_hp_info(fileDesc);
  
  if (mdpos == -1){
    printf("You must open the file first\n");
    return HP_ERROR;
  }

  // get block
  BF_Block *block;
  BF_Block_Init(&block);

  // print error edw
  BF_GetBlock(fileDesc, blockid, block);

  // exw to block
  // check how many records the block has
  // if it is the last it has last_rec
  // else MAX_RECORDS

  // get records in block
  int rcsInBlock;
  if (blockid == metadata[mdpos].last_bid)
    rcsInBlock = metadata[mdpos].recs_last;
  else 
    rcsInBlock = RECORDS_PER_BLOCK;
  
  // check position
  if (position > rcsInBlock - 1){
    printf("Invalid position given\n");
    return HP_ERROR;
  }
  
  blockGetRecord(block, position, record);
  // if (record->id == -1){
  //   record not found
  // }
  BF_UnpinBlock(block);
  BF_Block_Destroy(&block);
  return HP_OK;
}

HP_ErrorCode HP_GetNextRecordWithID(int fileDesc, Record *record, void *value) {
  /* TODO: υλοποιήστε τον iterator. Η κατάσταση του (πού σταμάτησε η
     προηγούμενη κλήση) πρέπει να επιβιώνει ανάμεσα στις κλήσεις. */
  
  // αρχιζει απο εκει που ειναι ο iterator
  // εχει αρχικοποιηθει στο -1, -1 σε περιπτωση που δεν εχει χρησιμοποιηθει (?)

  // struct iterator με δυο ints σε block_id και slot στο id
  // για το record που ψαξαμε τελευταιο

  // Η HP_GetNextRecordWithID ανανεωνει τον iterator

  // κανει walk απο την αρχη του αρχειου (εκτος metadata block)
  // και double for loop αν φτασει στο τελος ενος block
  // παει στο επομενο κλπ. Αν φτασει στο τελευταιο block 
  // σε slot max_recs - 1 και δεν το εχει βρει σημαινει οτι δεν υπαρχει

  // Αν καλεσουμε τη συναρτηση για αλλο value ξεκιναει απο την αρχη
  
  // Για overlapping κλησεις με διαφορετικα values πώς δεν θα ξεκιναει 
  // απο την αρχη αν εχει κληθει για ιδιο value και πριν ;;;
  // -> καπου κραταει τη θεση του iterator για καθε value που εχει δοθει
  // -> array γινεται και να γραφει απο πανω
  // global μεταβλητη για καλεσμενες values
  // με hashtable δεν χρειαζεται αυτο
  // θα γραφει παντα απο πανω απο ο,τι υπαρχει

  // array με τα values στο metadata
  int mdpos = find_hp_info(fileDesc);
  
  // find the correct hashed position
  int val = *(int*)value;
  int key = val % 25;
  // extreact the value in the position in hashtable
  int idInTable = metadata[mdpos].iterHTable[key].valueid;


  // αν εχει αλλο value
  //-- γραψε απο πανω
  //-- αρχισε iterate απο την αρχη
  //-- αποθηκευσε το καινουργιο positon

  // αν εχει το ιδιο value
  //-- αρχισε iterate απο το αποθηκευεμενο
  //-- αποθηκευσε το καινουργιο position

  // αρα αρχιζει απο την αρχη παντα
  // εκτος αν εχει το ιδιο value στο hashtable
  int startBid = 1;
  int startRid = 0;

  if (idInTable == val){
    startBid = metadata[mdpos].iterHTable[key].iterator.bid;
    startRid = metadata[mdpos].iterHTable[key].iterator.slot;
  }
  else
    metadata[mdpos].iterHTable[key].valueid = val;

  // start iterating?
  int bIter;  // block iterator
  int rIter;  //record iterateor
  BF_Block *block;
  BF_Block_Init(&block);

  Record rec; //to pull the records
  bool found = false;

  // cleaned code

  // printf("StartBid is: %d\n", startBid);

  for (bIter = startBid; bIter <= metadata[mdpos].last_bid && !found; bIter++){
    
    int recStart = (bIter == startBid) ? startRid + 1 : 0; 
    int recEnd = (bIter == metadata[mdpos].last_bid) ? metadata[mdpos].recs_last : RECORDS_PER_BLOCK;

    for (rIter = recStart; rIter < recEnd; rIter++){
      // printf("In Loop...Getting data from block %d | record %d ", bIter, rIter);

      // HP_GetEntry(fileDesc, bIter, rIter, &rec);
      BF_GetBlock(fileDesc, bIter, block);
      blockGetRecord(block, rIter, &rec);
      // printRecord(rec);
      if (rec.id == val){
        found = true;
        *(record) = rec;
        break;
      }
    }
    BF_UnpinBlock(block);
  }
  // printf("Out of Loop...Stopped in b:%d r:%d\n", bIter - 1, rIter);

  if (!found){
    printf("Entry with id %d not found in records\n", val);
    BF_Block_Destroy(&block);
    return HP_ERROR;
  }

  BF_UnpinBlock(block);
  BF_Block_Destroy(&block);
  metadata[mdpos].iterHTable[key].iterator.bid = bIter - 1;
  metadata[mdpos].iterHTable[key].iterator.slot = rIter;
  return HP_OK;
}