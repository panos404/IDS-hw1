# IDS-hw1

### Panagiotis Konstantinidis sdi2400103 + Emmanouela Mavrommati sdi2400123

### (1) HP_CreateFile
It creates an hp file through bf_createfile making sure it's not attempting to create an existing file.
After it's created we must initialize the first block and insert the necessary metadata.
The way that's done is that after we open the newly created file we add to a global counter (i) thats initialized with -1 so that we can use the i-th hp_info struct from the metadata[BF_MAX_OPEN_FILES] array that will now be filled with the fields:

- the filename

- the filedesc (this is useless for now since once the file closes it wont be used anymore but we will use it again after open file)

- a last_bid field that represent the id of the last available(free slots) and allocated block.At create its initialized with -1 so we know theres no allocated free blocks

- file type which in create is filled with a flag with the string "heap_file"

- recs_last that for the last_bid saves the next slot to be filled/number of slots already filled if its full

- an iterator hashtable used for the last function(not used here not initialized)

Then this hp_info struct is memcpy-ed through a function set_metadata similar to set_record into the block 0 slot 0 of the file.
The file is closed and if no other file is using the bf we close it too.

### (2) HP_OpenFile

The function initializes the bf level if no other files have already done that (check global counter of open files). 
If the file does not exist the function returns HP_ERROR .
Else we get the metadata of the open file we the help of the counter of open files (i++) and the getmetadata function (similar to get record also works w/ a memcpy but from the first rec of the first block ).
Then it gets saved to the global metadata array so every function can access it. 
Before we continue we have to check if the file is a heap file which we do with a strcmp and if its not we close it, remove it from the counter of opne files and if noone else is using it we remove the bf level.
If its a heap file we also add to the newly saved globally metadata the new fileDesc.

### (3) HP_CloseFile

First we have to find the metadata for the open file using the find_hp_info function.
If the file does not exist it return a negative position and the close file function knows to exit w/ error.
If the file IS open the helper function returns its metadata position in the global array so we can access them.
We replace them with the last metadata  of the array (no empty space in array) and decrease the open file counter by one.
If this is the finale file thats open we close the BF level to save everything up until this point.

### (4) HP_InsertEntry

Pulling corresponding metadata for the fileDesc, checking if it is open. 
The function only writes in the last block, since the file is a heap file.


The allocation of a new block happens only under two conditions:
1. The file is empty (has only metadata block, hence last_bid = -1)
2. The insertion block is full and need to allocate another

In both cases the last_bid will need to be updated. The update of the last_bid happens through the BF_GetBlockCounter function and not my standard addition.


If the file isn't empty the last block taken. If the records in the last block match the given maximum, a new must be allocated. To do that safely, the previous block that we pulled must get unpinned and the pointer must get destroyed and reinitialized. Also the last_bid and recs_last metadata must be updated at this step respectively.


When the setting of the record is done the metadata go the first block again with the updated values (set_metadata), unpinning of the block, destruction of block pointer end exit.



### (5) HP_PrintAllEntries

We find the metadata of the heap file (find_hp_info) if it exists.
Then we itirate each block and the ammount of records they each have which is RECORDS_PER_BLOCK for all available blocks (till last_bid) EXCEPT the last that could have <= RECORDS_PER_BLOCK so if we're itirating the last block we loop till < last_recs from the metadata.
Inside the loop we check what field the user is asking for and if the record's value matches the given value and we print records that match the condition.
If the value is NULL we print the record no matter what.

### (6) HP_GetEntry

The function pulls one record within the range of the whole file. For safety, we need to find the number of records in the block we are looking to extract data from. That is either recs_last or RECORDS_PER_BLOCK if we are extracting from the last block or from any other, respectively.


blockGetRecord gets called, without checking the return value, because it is guaranteed from the previous check that the position/slot is within range.

### (7) HP_GetNextRecordWithID

For this function an iterator is needed, as said in the assignment. The implementation I went with uses a simple (semi-working) hashtable, where the values given (ids) are hashed to keys for easy access. It doesn't have a replacement policy or rehashing, when a collision happens it just overwrites and reinitializes the iterator. That guarantees trivial access and maintenance, but makes the hashtable kinda faulty, meaning two entries might collide and one overwrites the other, while many slots in the table are empty. Nontheless, for such small records bulk as the one provided, I think it is sufficient.


##### Quick look in hashEntry and iterator structs
**hashEntryL** holds the unhashed value that is provided. It is used for detecting wheter an update on the slot of the table is a collision with another value or an update to the saved one.
**iterator:** the iterator is nothing more than two ints pointing to a block_id and a slot :P.


The checking mentioned happens, startBid gets initialized at 1 (cause 0 is metadata block) and then if the value has already been searched, the starting points are updated.


Starting point for the iterator of records is either the next of where it was left of (to not pull the same previous record again) if we are still in the block we left off previously or 0 (iterating new block). The ending point is either recs_last if we are iterating the last block, or the maximum if iterating the full previous ones.


A boolean flag is also used in the loop, which is also terminating condition, to check if the record is found after full walk. 


Also, the blockid field of the iterator for the value is updated to the iterator - 1, because it gets increased one last time before reaching a terminating condition.