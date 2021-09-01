CC=g++

XXXCFLAGS=-std=c++11 -O2 -ICommonLibs -ICompressionLibrary

SRC_COMMON = $(shell find CommonLibs CompressionLibrary -name '*.cpp' -o -name '*.c')
SRC_ARCHIVER = $(shell find FF4Archiver -name '*.cpp' -o -name '*.c')
SRC_MINIARCHIVER = $(shell find FF4MiniArchiver -name '*.cpp' -o -name '*.c')
SRC_DECOMPRESSOR = $(shell find FF4Decompressor -name '*.cpp' -o -name '*.c')
SRC_SORTER = $(shell find MonstersSorter -name '*.cpp' -o -name '*.c')

LIBS=-lboost_filesystem -lboost_system -lboost_regex -lboost_locale

.PHONY: all FF4Archiver FF4MiniArchiver FF4Decompressor FF4MonstersSorter

all: FF4Archiver FF4MiniArchiver FF4Decompressor FF4MonstersSorter

clean: 
	rm -f ff4archiver ff4miniarchiver ff4decompressor ff4monsterssorter
	
FF4Archiver:
	$(CC) $(XXXCFLAGS) -IFF4Archiver $(SRC_COMMON) $(SRC_ARCHIVER) $(LIBS) -o ff4archiver

FF4MiniArchiver:
	$(CC) $(XXXCFLAGS) -IFF4MiniArchiver $(SRC_COMMON) $(SRC_MINIARCHIVER) $(LIBS) -o ff4miniarchiver
	
FF4Decompressor:
	$(CC) $(XXXCFLAGS) -IFF4Decompressor $(SRC_COMMON) $(SRC_DECOMPRESSOR) $(LIBS) -o ff4decompressor

FF4MonstersSorter:
	$(CC) $(XXXCFLAGS) -IMonstersSorter $(SRC_COMMON) $(SRC_SORTER) $(LIBS) -o ff4monsterssorter

install:
	chmod +x ff4archiver ff4miniarchiver ff4decompressor ff4monsterssorter
	cp ff4archiver ff4miniarchiver ff4decompressor ff4monsterssorter /usr/bin

uninstall:
	rm /usr/bin/ff4archiver /usr/bin/ff4miniarchiver /usr/bin/ff4decompressor /usr/bin/ff4monsterssorter 
