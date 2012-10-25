#ifndef MAP_INCLUDED
#define MAP_INCLUDED

#include <stdlib.h>
#include <stdio.h>

typedef struct MapFile_
{
	FILE* stream;
	int readOnly;

	int entryCountLocation;
	size_t entryCount;
} MapFile;

void MapOpen(MapFile* map, const char* path, int readOnly);

void MapClose(MapFile* map);

void MapWrite(MapFile* map, const char* name, size_t nameSize, unsigned addr);

size_t MapReadNameSize(MapFile* map);
char* MapReadName(MapFile* map, char* buf, size_t nameSize);
unsigned MapReadAddr(MapFile* map);

void MapDump(FILE* stream, MapFile* map);

#endif

