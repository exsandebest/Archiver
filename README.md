# Archiver
A simple archiving Qt application based on the Huffman algorithm
## Features
* Simple Qt interface
* Maximum length of file name - 256 symbols (with extension)
* Russian letters are **not supported** in the file name
* Maximum size of file -  1 GByte
* Archived file has `.xxx` extension 
## Structure
* 1 Byte - Length of file name (in Bytes) - **N**
* **N** Bytes - File name
* 2 Bytes - Number of different Bytes (symbols) in the file (0-256) - **K**
* **K** blocks (Huffman tree):
    * 1 Byte - Byte (symbol)
    * 1 Byte - Encoding length of this Byte (symbol) (in Bits) - **S**
    * ⌈**S**/8⌉ Bytes - Encoding of this Byte (symbol)
* 8 Bytes - Length of encoded data (in Bits) - **P**
* ⌈**P**/8⌉ Bytes - Encoded data
## Interface - example
![Interface](https://raw.githubusercontent.com/exsandebest/Archiver/master/examples/screenshots/encoding.png)
## Authors
* [exsandebest](https://github.com/exsandebest) ([Repository](https://github.com/exsandebest/Archiver))
* [klimkomx](https://github.com/klimkomx) ([Repository](https://github.com/klimkomx/HuffmanArchivator))
* [hensuto](https://gitlab.com/hensuto) ([Repository](https://gitlab.com/hensuto/huffman-archiver))
