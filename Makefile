all:
	gcc -o StaticHuffman/huffman StaticHuffman/staticHuffman.c
	gcc -o FGK/fgk FGK/fgk.c
	gcc -o Golomb/golomb Golomb/bitfile.h Golomb/bitfile.c Golomb/optlist.h Golomb/optlist.c Golomb/rice.h Golomb/rice.c Golomb/sample.c
	g++ -o Tunstall/tunstall Tunstall/Tunstall.cpp
	gcc -o Arithmetic/arithmetic Arithmetic/header.h Arithmetic/header.c Arithmetic/coder.h Arithmetic/coder.c Arithmetic/main.c
	gcc -o LZ77/lz77 LZ77/include/bitio.h LZ77/include/header.h LZ77/include/lz77.h LZ77/include/treemanager.h LZ77/src/bitio.c LZ77/src/header.c LZ77/src/lz77comp.c LZ77/src/lz77dec.c LZ77/src/main.c LZ77/src/treemanager.c
	gcc -o LZ78/lz78 LZ78/bitio.h LZ78/bitio.c LZ78/lz78.h LZ78/lz78.c LZ78/wrapper.h LZ78/wrapper.c LZ78/main.c
	gcc -o LZW/lzw LZW/lzw.c
	gcc -o LZSS/lzss LZSS/LZSS.c

huffman:
	./StaticHuffman/huffman ${ARGS}

fgk:
	./FGK/fgk ${ARGS}

golomb:
	./Golomb/golomb ${ARGS}

tunstall:
	./Tunstall/tunstall ${ARGS}

arithmetic:
	./Arithmetic/arithmetic ${ARGS}

lz77:
	./LZ77/lz77 ${ARGS}

lz78:
	./LZ78/lz78 ${ARGS}

lzw:
	./LZW/lzw ${ARGS}

lzss:
	./LZSS/lzss ${ARGS}
