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

### (5) HP_PrintAllEntries

We find the metadata of the heap file (find_hp_info) if it exists.
Then we itirate each block and the ammount of records they each have which is RECORDS_PER_BLOCK for all available blocks (till last_bid) EXCEPT the last that could have <= RECORDS_PER_BLOCK so if we're itirating the last block we loop till < last_recs from the metadata.
Inside the loop we check what field the user is asking for and if the record's value matches the given value and we print records that match the condition.
If the value is NULL we print the record no matter what.

