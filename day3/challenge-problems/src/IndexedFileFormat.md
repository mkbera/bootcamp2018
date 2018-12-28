# Indexed Blockchain Format

The format consists of:

1. Header
1. Chunk (array) of blocks
1. Balance map at the end of the above blocks
1. Chunk (array) of blocks
1. Balance map at the end of the above blocks
1. ...
1. ...
1. Chunk (array) of blocks
1. Balance map at the end of the above blocks

The array of blocks and balance map are repeated for each chunk in the file.

The header consists of the following information.

1. Number of chunks (type: uint32_t)
1. Array of uint64_t's with the start offset for each chunk of blocks.
1. Array of uint64_t's with the end offset for each chunk of blocks. This is also the start offset for the balance map.
1. Array of uint64_t's with the size of the balance map at the end of the corresponding chunk.

Each block is stored in the same format as before and can be read/written using the block_t::write functions.

Each balance map is stored as follows.
1. Number of entries in the map (uint32_t)
1. Entry
    * 32 byte address
    * uint64_t balance
1. Entry
    * 32 byte address
    * uint64_t balance
1. ...
1. Entry
    * 32 byte address
    * uint64_t balance

# Instructions for Running the Parallel Read Command

The command is: readindexed <filename>. Example:

    $ ./readindexed tests/t1.idx

