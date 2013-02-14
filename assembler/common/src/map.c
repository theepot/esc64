#include <esc64asm/map.h>

#include <string.h>
#include <assert.h>

static void Write(MapFile* map, const void* data, size_t size);
static void Read(MapFile* map, void* data, size_t size);

static void MapWriteName(MapFile* map, const char* name, size_t nameSize);
static void MapWriteAddr(MapFile* map, unsigned addr);

void MapOpen(MapFile* map, const char* path, int readOnly)
{
	map->readOnly = readOnly;
	FILE* stream = fopen(path, readOnly ? "rb" : "wb+");
	assert(stream);
	map->stream = stream;

	if(readOnly)
	{
		Read(map, &(map->entryCount), sizeof(size_t));
	}
	else
	{
		map->entryCount = 0;
		map->entryCountLocation = ftell(stream);
		Write(map, &(map->entryCount), sizeof(size_t));
	}
}

void MapClose(MapFile* map)
{
	if(!map->readOnly)
	{
		fseek(map->stream, map->entryCountLocation, SEEK_SET);
		Write(map, &(map->entryCount), sizeof(size_t));
		fclose(map->stream);
	}
}

void MapWrite(MapFile* map, const char* name, size_t nameSize, unsigned addr)
{
	MapWriteName(map, name, nameSize);
	MapWriteAddr(map, addr);
	++map->entryCount;
}

size_t MapReadNameSize(MapFile* map)
{
	size_t size;
	Read(map, &size, sizeof(size_t));
	return size;
}

char* MapReadName(MapFile* map, char* buf, size_t nameSize)
{
	Read(map, buf, nameSize);
	return buf;
}

unsigned MapReadAddr(MapFile* map)
{
	unsigned addr;
	Read(map, &addr, sizeof(unsigned));
	return addr;
}

void MapDump(FILE* stream, MapFile* map)
{
	fputs("MAP FILE DUMP BEGIN\n", stream);
	
	fprintf(stream, "\tentryCount=%u\n", map->entryCount);
	
	size_t i;
	for(i = 0; i < map->entryCount; ++i)
	{
		size_t nameSize = MapReadNameSize(map);
		char name[nameSize + 1];
		name[nameSize] = '\0';
		MapReadName(map, name, nameSize);
		unsigned addr = MapReadAddr(map);
		fprintf(stream, "\t[%04u] nameSize=%u; name=%s; addr=%u\n", i, nameSize, name, addr);
	}
	
	fputs("MAP FILE DUMP END\n", stream);
}

static void Write(MapFile* map, const void* data, size_t size)
{
	assert(fwrite(data, size, 1, map->stream) == 1);
}

static void Read(MapFile* map, void* data, size_t size)
{
	assert(fread(data, size, 1, map->stream) == 1);
}

static void MapWriteName(MapFile* map, const char* name, size_t nameSize)
{
	Write(map, &nameSize, sizeof(size_t));
	Write(map, name, nameSize);
}

static void MapWriteAddr(MapFile* map, unsigned addr)
{
	Write(map, &addr, sizeof(size_t));
}

