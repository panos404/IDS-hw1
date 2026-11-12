#include <string.h>
#include <stdio.h>

#include "block_records.h"

int blockSetRecord(BF_Block *block, int slot, Record record) {
  if (slot < 0 || slot >= RECORDS_PER_BLOCK)
    return -1;

  char *data = BF_Block_GetData(block);

  /* Αντιγραφή της εγγραφής στη θέση slot του block */
  printf("In blockSetRecord...writing in slot %d", slot);
  printRecord(record);
  memcpy(data + slot * sizeof(Record), &record, sizeof(Record));

  /* Τα περιεχόμενα του block άλλαξαν, οπότε πρέπει κάποια στιγμή να γραφτούν
     στον δίσκο */
  BF_Block_SetDirty(block);
  return 0;
}

int blockGetRecord(BF_Block *block, int slot, Record *record) {
  if (slot < 0 || slot >= RECORDS_PER_BLOCK)
    return -1;

  char *data = BF_Block_GetData(block);

  /* Αντιγραφή της εγγραφής από τη θέση slot του block */
  memcpy(record, data + slot * sizeof(Record), sizeof(Record));
  return 0;
}


// 1.
// συναρτηση που παιρνει το πρωτο available slot του block
int blockGetSlot(BF_Block *block, int *slot){
  Record rp;
  
  int i = 0;
  // iterate through the 
  while (i < RECORDS_PER_BLOCK){;
    blockGetRecord(block, i, &rp);
    
    if (rp.id == -1) break;
    i++;
  }
  // η περιπτωση που ειναι γεματο
  if (i == RECORDS_PER_BLOCK)
    return -1;

  *slot = i;
  return 0;
}

// puts dummy records in the whole block
void blockInitSlots(BF_Block *block){
  int i;
  Record dummyrec;
  dummyrec.id = -1;
  for (i = 0; i < RECORDS_PER_BLOCK; i++){
    blockSetRecord(block, i, dummyrec);
  }
}

// 2.
// παιρνει το metadata block που δειχνει
// το πρωτο διαθεσιμο record
void getHeader(BF_Block* block, int *slot){
  char *data = BF_Block_GetData(block);

  memcpy(slot, data, sizeof(int));
}

// ανανεωνει το header metadata του block
// με την τιμη slot
void setHeader(BF_Block *block, int slot){
  char *data = BF_Block_GetData(block);

  memcpy(data, &slot, sizeof(int));
}