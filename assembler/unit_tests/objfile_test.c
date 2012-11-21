#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <arpa/inet.h>

#include "objcode.h"

/*	TODO's
	- test unlinked stuff
*/

static ObjectOutputStream oos;
static ObjectInputStream ois;

typedef struct TestData_
{
	UWord_t address;
	UWord_t data;
} TestData;

typedef struct TestSymbol_
{
	const char* name;
	UWord_t value;
	ObjSize_t offset;
} TestSymbol;

static TestData testData[] =
{
	{ 0x0, 0x5D62 },
	{ 0x1, 0x836E },
	{ 0x2, 0xF3C5 },
	{ 0x3, 0xEA8B },
	{ 0x4, 0xA2BE },
	{ 0x5, 0xB7A0 },
	{ 0x6, 0x2440 },
	{ 0x7, 0x9B62 },
	{ 0x8, 0x42C },
	{ 0x9, 0x3E2D },
	{ 0xA, 0x2321 },
	{ 0xB, 0xCDDE },
	{ 0xC, 0x281C },
	{ 0xD, 0x66A4 },
	{ 0xE, 0x2139 },
	{ 0xF, 0x1BDA },
	{ 0x16, 0x834F },
	{ 0x17, 0xD6D0 },
	{ 0x18, 0x9CD5 },
	{ 0x19, 0x4BC8 },
	{ 0x1A, 0xA336 },
	{ 0x1B, 0x8637 },
	{ 0x1C, 0x7E5B },
	{ 0x1D, 0xF907 },
	{ 0x1E, 0x4AE2 },
	{ 0x24, 0xC519 },
	{ 0x25, 0x6675 },
	{ 0x26, 0xE43B },
	{ 0x27, 0x4887 },
	{ 0x28, 0x5A3A },
	{ 0x29, 0xCEC7 },
	{ 0x2A, 0xEB45 },
	{ 0x2B, 0x11DB },
	{ 0x2C, 0xF307 },
	{ 0x2D, 0x86A7 },
	{ 0x2E, 0x1607 },
	{ 0x2F, 0x3134 },
	{ 0x30, 0xA9C9 },
	{ 0x31, 0xE3E6 },
	{ 0x33, 0x520 },
	{ 0x34, 0x752B },
	{ 0x35, 0x1027 },
	{ 0x36, 0x3CFF },
	{ 0x37, 0xF87B },
	{ 0x38, 0xE6F7 },
	{ 0x39, 0xD9D5 },
	{ 0x3B, 0x600D },
	{ 0x3C, 0xC29F },
	{ 0x3D, 0x8335 },
	{ 0x3E, 0xAAEF },
	{ 0x3F, 0x8817 },
	{ 0x40, 0xA0E },
	{ 0x41, 0x7009 },
	{ 0x42, 0xEE8C },
	{ 0x43, 0xEE49 },
	{ 0x44, 0xB890 },
	{ 0x45, 0x48C7 },
	{ 0x46, 0xBD10 },
	{ 0x47, 0xA3D5 },
	{ 0x48, 0x5AA2 },
	{ 0x4D, 0x70AA },
	{ 0x4E, 0xE14C },
	{ 0x4F, 0xD446 },
	{ 0x50, 0x5490 },
	{ 0x51, 0x3A9D },
	{ 0x52, 0xE4B4 },
	{ 0x53, 0x59B1 },
	{ 0x54, 0xAFC9 },
	{ 0x5B, 0xA845 },
	{ 0x5C, 0xDBD2 },
	{ 0x5D, 0x7086 },
	{ 0x5E, 0xEC89 },
	{ 0x5F, 0x65FF },
	{ 0x60, 0xD094 },
	{ 0x61, 0xAF28 },
	{ 0x62, 0xE935 },
	{ 0x63, 0x7B84 },
	{ 0x64, 0x3740 },
	{ 0x65, 0xF343 },
	{ 0x66, 0xEB8D },
	{ 0x67, 0x25CD },
	{ 0x68, 0xE18D },
	{ 0x69, 0xA41D },
	{ 0x6C, 0x47F3 },
	{ 0x6D, 0xC937 },
	{ 0x6E, 0x4EB5 },
	{ 0x6F, 0x7270 },
	{ 0x70, 0x39E2 },
	{ 0x71, 0x3002 },
	{ 0x72, 0x46B7 },
	{ 0x73, 0x8E73 },
	{ 0x74, 0x6A9F },
	{ 0x75, 0x2B6B },
	{ 0x76, 0xE824 },
	{ 0x77, 0x1A69 },
	{ 0x78, 0x2046 },
	{ 0x79, 0x7ED5 },
	{ 0x7A, 0xC2AE },
	{ 0x81, 0xAF37 },
	{ 0x82, 0x6218 },
	{ 0x83, 0xBFF0 },
	{ 0x84, 0x5E60 },
	{ 0x85, 0x4B4E },
	{ 0x86, 0x3B75 },
	{ 0x87, 0x95A1 },
	{ 0x88, 0x3E91 },
	{ 0x89, 0x2703 },
	{ 0x8A, 0xBB6E },
	{ 0x8B, 0x201F },
	{ 0x8C, 0xCB20 },
	{ 0x8D, 0x2A02 },
	{ 0x8E, 0xBEBC },
	{ 0x8F, 0x1314 },
	{ 0x90, 0xF33A },
	{ 0x91, 0xD72 },
	{ 0x92, 0x8585 },
	{ 0x93, 0x2D1C },
	{ 0x94, 0x3D74 },
	{ 0x9A, 0xA814 },
	{ 0x9B, 0xF7A7 },
	{ 0x9C, 0xA3B3 },
	{ 0x9D, 0xC27D },
	{ 0x9E, 0x17EE },
	{ 0x9F, 0x2288 },
	{ 0xA0, 0x852C },
	{ 0xA1, 0x1407 },
	{ 0xA2, 0x11E5 },
	{ 0xA3, 0x3464 },
	{ 0xA4, 0x761F },
	{ 0xA5, 0xD1D6 },
	{ 0xA6, 0x92C5 },
	{ 0xA7, 0xC16D },
	{ 0xA8, 0xD4B },
	{ 0xA9, 0x2866 },
	{ 0xAA, 0xFFFF },
	{ 0xAB, 0x201E },
	{ 0xAC, 0xFF6F },
	{ 0xAD, 0xDD6 },
	{ 0xAE, 0xDEDB },
	{ 0xAF, 0x1284 },
	{ 0xB0, 0x110 },
	{ 0xB1, 0xEC4E },
	{ 0xB2, 0x9809 },
	{ 0xB3, 0x2E2C },
	{ 0xB4, 0x29C2 },
	{ 0xB5, 0x6445 },
	{ 0xB6, 0xE9BC },
	{ 0xB7, 0xD1D7 },
	{ 0xB8, 0x5BED },
	{ 0xB9, 0x8D6F },
	{ 0xBA, 0x9455 },
	{ 0xBB, 0x73DC },
	{ 0xBC, 0xAFF8 },
	{ 0xBD, 0x1982 },
	{ 0xBE, 0x87E3 },
	{ 0xC4, 0xFE02 },
	{ 0xC5, 0x93B4 },
	{ 0xC6, 0xE0AB },
	{ 0xC7, 0xBF70 },
	{ 0xC8, 0xA100 },
	{ 0xC9, 0x911 },
	{ 0xCA, 0xBF70 },
	{ 0xCB, 0xD54F },
	{ 0xCC, 0xECE5 },
	{ 0xCD, 0xDF8E },
	{ 0xD3, 0xBE6A },
	{ 0xD4, 0xE743 },
	{ 0xD5, 0xFBCC },
	{ 0xD6, 0xAAB8 },
	{ 0xD7, 0x7F4C },
	{ 0xD8, 0x29F9 },
	{ 0xD9, 0xD47B },
	{ 0xDA, 0xE392 },
	{ 0xDB, 0x13B5 },
	{ 0xDC, 0xA653 },
	{ 0xDD, 0x3F7F },
	{ 0xDE, 0xA125 },
	{ 0xDF, 0x3AA8 },
	{ 0xE0, 0xB35B },
	{ 0xE1, 0x511E },
	{ 0xE2, 0x542A },
	{ 0xE3, 0x3B3E },
	{ 0xE4, 0x12FC },
	{ 0xE5, 0xA211 },
	{ 0xE6, 0x3941 },
	{ 0xE7, 0xA6B0 },
	{ 0xEB, 0x47B0 },
	{ 0xEC, 0x8BCD },
	{ 0xED, 0xB821 },
	{ 0xEE, 0x1CFF },
	{ 0xEF, 0x78B3 },
	{ 0xF0, 0x97B0 },
	{ 0xF1, 0xF1BE },
	{ 0xF2, 0x736F },
	{ 0xF3, 0x561B },
	{ 0xF4, 0xD901 },
	{ 0xF5, 0x6F3C },
	{ 0xF6, 0xD3 },
	{ 0xF7, 0x584E },
	{ 0xF8, 0x9936 },
	{ 0xF9, 0xD54F },
	{ 0xFA, 0x3BE0 },
	{ 0xFB, 0xACEB },
	{ 0xFC, 0x7BA2 },
	{ 0xFD, 0x7B60 },
	{ 0xFE, 0x4E11 },
	{ 0xFF, 0xB64B },
	{ 0x100, 0xA76 },
	{ 0x101, 0x69FA },
	{ 0x102, 0xB22B },
	{ 0x103, 0xAC87 },
	{ 0x104, 0xA33C },
	{ 0x105, 0x58DC },
	{ 0x106, 0x2F43 },
	{ 0x107, 0x9BEE },
	{ 0x108, 0xA08D },
	{ 0x109, 0xBB11 },
	{ 0x10A, 0x5410 },
	{ 0x10B, 0xBD8D },
	{ 0x10C, 0x33C4 },
	{ 0x10D, 0xEBC1 },
	{ 0x10E, 0xAF4C },
	{ 0x113, 0x884E },
	{ 0x114, 0x166F },
	{ 0x115, 0x42AF },
	{ 0x116, 0xE09C },
	{ 0x117, 0xAFA5 },
	{ 0x118, 0x17FF },
	{ 0x119, 0x1C7C },
	{ 0x11A, 0x5C91 },
	{ 0x11B, 0x93A1 },
	{ 0x11C, 0x97DC },
	{ 0x121, 0xC698 },
	{ 0x122, 0x49D3 },
	{ 0x123, 0x5463 },
	{ 0x124, 0x3093 },
	{ 0x125, 0xFBFF },
	{ 0x126, 0xEA },
	{ 0x127, 0xD3CF },
	{ 0x128, 0x54DB },
	{ 0x129, 0x302D },
	{ 0x12A, 0x6FBE }
};

static TestSymbol testSymbols[] =
{
	{ "Adult", 76 },
	{ "Aeroplane", 33816 },
	{ "Air", 11599 },
	{ "Aircraft Carrier", 48525 },
	{ "Airforce", 35101 },
	{ "Airport", 28793 },
	{ "Album", 21018 },
	{ "Alphabet", 53758 },
	{ "Apple", 49371 },
	{ "Arm", 44797 },
	{ "Army", 10447 },
	{ "Baby", 51537 },
	{ "Backpack", 30813 },
	{ "Balloon", 18240 },
	{ "Banana", 900 },
	{ "Bank", 5485 },
	{ "Barbecue", 21868 },
	{ "Bathroom", 8839 },
	{ "Bathtub", 9954 },
	{ "Bed", 59312 },
	{ "Bee", 7145 },
	{ "Bible", 281 },
	{ "Bird", 22673 },
	{ "Bomb", 31900 },
	{ "Book", 34272 },
	{ "Boss", 36106 },
	{ "Bottle", 36430 },
	{ "Bowl", 9975 },
	{ "Box", 39783 },
	{ "Boy", 27048 },
	{ "Brain", 21128 },
	{ "Bridge", 3423 },
	{ "Butterfly", 36462 },
	{ "Button", 47000 },
	{ "Cappuccino", 48157 },
	{ "Car", 31193 },
	{ "Car-race", 18118 },
	{ "Carpet", 52559 },
	{ "Carrot", 43601 },
	{ "Cave", 57355 },
	{ "Chair", 55544 },
	{ "Chess Board", 32362 },
	{ "Chief", 8541 },
	{ "Child", 27725 },
	{ "Chisel", 14120 },
	{ "Chocolates", 51735 },
	{ "Church", 12577 },
	{ "Circle", 50620 },
	{ "Circus", 59808 },
	{ "Clock", 36690 },
	{ "Clown", 23547 },
	{ "Coffee", 15973 },
	{ "Coffee-shop", 17837 },
	{ "Comet", 50409 },
	{ "Compact Disc", 1425 },
	{ "Compass", 22552 },
	{ "Computer", 5558 },
	{ "Crystal", 40633 },
	{ "Cup", 3373 },
	{ "Cycle", 528 },
	{ "Data Base", 55128 },
	{ "Desk", 16554 },
	{ "Diamond", 16374 },
	{ "Dress", 35275 },
	{ "Drill", 41471 },
	{ "Drink", 50257 },
	{ "Drum", 43590 },
	{ "Dung", 29097 },
	{ "Ears", 12322 },
	{ "Earth", 44625 },
	{ "Egg", 28108 },
	{ "Electricity", 27478 },
	{ "Elephant", 56950 },
	{ "Eraser", 44667 },
	{ "Explosive", 6497 },
	{ "Eyes", 35943 },
	{ "Family", 23115 },
	{ "Fan", 44101 },
	{ "Feather", 36538 },
	{ "Festival", 34345 },
	{ "Film", 21681 },
	{ "Finger", 9094 },
	{ "Fire", 13507 },
	{ "Floodlight", 25510 },
	{ "Flower", 48173 },
	{ "Foot", 31027 },
	{ "Fork", 59400 },
	{ "Freeway", 45093 },
	{ "Fruit", 20734 },
	{ "Fungus", 10139 },
	{ "Game", 39439 },
	{ "Garden", 29514 },
	{ "Gas", 3813 },
	{ "Gate", 41986 },
	{ "Gemstone", 30289 },
	{ "Girl", 8850 },
	{ "Gloves", 56976 },
	{ "God", 8495 },
	{ "Grapes", 54308 },
	{ "Guitar", 41574 },
	{ "Hammer", 18183 },
	{ "Hat", 25594 },
	{ "Hieroglyph", 4223 },
	{ "Highway", 57997 },
	{ "Horoscope", 40992 },
	{ "Horse", 9195 },
	{ "Hose", 52636 },
	{ "Ice", 49301 },
	{ "Ice-cream", 34923 },
	{ "Insect", 11482 },
	{ "Jet fighter", 10674 },
	{ "Junk", 49032 },
	{ "Kaleidoscope", 28516 },
	{ "Kitchen", 9334 },
	{ "Knife", 30236 },
	{ "Leather jacket", 43922 },
	{ "Leg", 24336 },
	{ "Library", 16775 },
	{ "Liquid", 34125 },
	{ "Magnet", 40935 },
	{ "Man", 45352 },
	{ "Map", 43315 },
	{ "Maze", 28518 },
	{ "Meat", 7382 },
	{ "Meteor", 22069 },
	{ "Microscope", 50081 },
	{ "Milk", 2106 },
	{ "Milkshake", 31021 },
	{ "Mist", 39780 },
	{ "Money $$$$", 25574 },
	{ "Monster", 6281 },
	{ "Mosquito", 56961 },
	{ "Mouth", 55284 },
	{ "Nail", 32973 },
	{ "Navy", 20760 },
	{ "Necklace", 28304 },
	{ "Needle", 22499 },
	{ "Onion", 50819 },
	{ "PaintBrush", 19013 },
	{ "Pants", 27366 },
	{ "Parachute", 16314 },
	{ "Passport", 58979 },
	{ "Pebble", 17868 },
	{ "Pendulum", 44352 },
	{ "Pepper", 34037 },
	{ "Perfume", 11760 },
	{ "Pillow", 45679 },
	{ "Plane", 50367 },
	{ "Planet", 23860 },
	{ "Pocket", 30055 },
	{ "Post-office", 53410 },
	{ "Potato", 1648 },
	{ "Printer", 59678 },
	{ "Prison", 34356 },
	{ "Pyramid", 3031 },
	{ "Radar", 31880 },
	{ "Rainbow", 11645 },
	{ "Record", 50583 },
	{ "Restaurant", 37606 },
	{ "Rifle", 39457 },
	{ "Ring", 11872 },
	{ "Robot", 50530 },
	{ "Rock", 7400 },
	{ "Rocket", 6596 },
	{ "Roof", 44588 },
	{ "Room", 18844 },
	{ "Rope", 56465 },
	{ "Saddle", 17165 },
	{ "Salt", 20179 },
	{ "Sandpaper", 8416 },
	{ "Sandwich", 43986 },
	{ "Satellite", 50078 },
	{ "School", 42480 },
	{ "Sex", 36015 },
	{ "Ship", 44833 },
	{ "Shoes", 15164 },
	{ "Shop", 8669 },
	{ "Shower", 98 },
	{ "Signature", 3661 },
	{ "Skeleton", 48375 },
	{ "Slave", 51158 },
	{ "Snail", 12635 },
	{ "Software", 6937 },
	{ "Solid", 33193 },
	{ "Space Shuttle", 856 },
	{ "Spectrum", 6827 },
	{ "Sphere", 27271 },
	{ "Spice", 45134 },
	{ "Spiral", 41169 },
	{ "Spoon", 32607 },
	{ "Sports-car", 4434 },
	{ "Spot Light", 26204 },
	{ "Square", 12117 },
	{ "Staircase", 41774 },
	{ "Star", 17422 },
	{ "Stomach", 26202 },
	{ "Sun", 13946 },
	{ "Sunglasses", 34673 },
	{ "Surveyor", 31955 },
	{ "Swimming Pool", 37721 },
	{ "Sword", 9612 },
	{ "Table", 30249 },
	{ "Tapestry", 57783 },
	{ "Teeth", 41746 },
	{ "Telescope", 55489 },
	{ "Television", 11397 },
	{ "Tennis racquet", 20157 },
	{ "Thermometer", 10702 },
	{ "Tiger", 59711 },
	{ "Toilet", 27447 },
	{ "Tongue", 59881 },
	{ "Torch", 5851 },
	{ "Torpedo", 37511 },
	{ "Train", 5664 },
	{ "Treadmill", 26264 },
	{ "Triangle", 55891 },
	{ "Tunnel", 2906 },
	{ "Typewriter", 53678 },
	{ "Umbrella", 17402 },
	{ "Vacuum", 13639 },
	{ "Vampire", 46144 },
	{ "Videotape", 24644 },
	{ "Vulture", 12119 },
	{ "Water", 37685 },
	{ "Weapon", 36249 },
	{ "Web", 27097 },
	{ "Wheelchair", 27982 },
	{ "Window", 35870 },
	{ "Woman", 38084 },
	{ "Worm", 51288 },
	{ "XRay", 49728 }
};

const static size_t testDataSize = sizeof(testData) / sizeof(TestData);
const static size_t testSymbolsSize = sizeof(testSymbols) / sizeof(TestSymbol);

typedef struct IOHelper_
{
	size_t index;
	size_t step;
	size_t size;
	size_t (*proc)(size_t, size_t);
} IOHelper;

static int IOProcess(IOHelper* helper);

static void TestWrite(void);
static void TestRead(void);

static size_t TestDataSegment(size_t testDataIndex);

static size_t ReadTestData(size_t from, size_t to);
static size_t ReadTestSymbolTable(size_t from, size_t to);
static size_t ReadTestUnlinked(size_t from, size_t to);

static size_t WriteTestData(size_t from, size_t to);
static size_t WriteTestSymbolTable(size_t from, size_t to);
static size_t WriteTestUnlinked(size_t from, size_t to);

static IOHelper ioData;
static IOHelper ioSym;
static IOHelper ioUnl;

static ObjectSymbolIterator symIt;
static ObjectUnlinkedIterator unlinkedIt;

void TestObjFile(const char* path)
{
	ObjectOutputStreamOpen(&oos, path);
	TestWrite();
	ObjectOutputStreamClose(&oos);

	UWord_t dataSize;
	UWord_t symTableEntries;
	ObjectInputStreamOpen(&ois, path, &dataSize, &symTableEntries);
	assert(dataSize == testDataSize * sizeof(UWord_t));
	TestRead();
	ObjectInputStreamClose(&ois);
}

static int IOProcess(IOHelper* helper)
{
	if(helper->index >= helper->size)
	{
		return -1;
	}

	size_t to = helper->index + helper->step;
	if(to > helper->size)
	{
		to = helper->size;
	}

	size_t nwIndex = helper->proc(helper->index, to);
	if(nwIndex == helper->index)
	{
		return -1;
	}
	
	helper->index = nwIndex;
	
	return 0;
}

static void TestWrite(void)
{
	ioData.index = 0;
	ioData.step = testDataSize / 6;
	ioData.size = testDataSize;
	ioData.proc = WriteTestData;
	
	ioSym.index = 0;
	ioSym.step = testSymbolsSize / 6;
	ioSym.size = testSymbolsSize;
	ioSym.proc = WriteTestSymbolTable;
	
	ioUnl.index = 0;
	ioUnl.step = testSymbolsSize / 6;
	ioUnl.size = testSymbolsSize;
	ioUnl.proc = WriteTestUnlinked;
	
	while(!IOProcess(&ioData) || !IOProcess(&ioSym) || !IOProcess(&ioUnl)) continue;
}

static void TestRead(void)
{
	ioData.index = 0;
	ioData.proc = ReadTestData;
	
	ioSym.index = 0;
	ioSym.proc = ReadTestSymbolTable;
	
	ioUnl.index = 0;
	ioUnl.proc = ReadTestUnlinked;
	
	ObjectSymbolIteratorInit(&symIt, &ois);
	ObjectUnlinkedIteratorInit(&unlinkedIt, &ois);
	
	while(!IOProcess(&ioData) || !IOProcess(&ioSym) || !IOProcess(&ioUnl)) continue;

	assert(ioData.index == ioData.size);
	assert(ioSym.index == ioSym.size);
	assert(ioUnl.index == ioUnl.size);
}

static size_t TestDataSegment(size_t testDataIndex)
{
	size_t dataSegSize = ObjectReadData(&ois);
	assert(testData[testDataIndex].address == ois.dataBaseAddr);
	size_t i;
	for(i = 0; i < dataSegSize; i += sizeof(UWord_t))
	{
		UWord_t* raw = ois.dataBuf.buf + i;
		UWord_t conv = ntohs(*raw);
		assert(conv == testData[testDataIndex].data);
		++testDataIndex;
	}

	return testDataIndex;
}

static size_t WriteTestData(size_t from, size_t to)
{
	size_t i;
	for(i = from; i < to; ++i)
	{
		ObjectWriteData(&oos, testData[i].address, testData[i].data);
	}

	return i;
}

static size_t ReadTestData(size_t from, size_t to)
{
	size_t i = from;
	while(i < to)
	{
		i = TestDataSegment(i);
	}

	return i;
}

static size_t WriteTestSymbolTable(size_t from, size_t to)
{
	size_t i;
	for(i = from; i < to; ++i)
	{
		const char* name = testSymbols[i].name;
		ObjectWriteSymbol(&oos, name, strlen(name), testSymbols[i].value);
	}

	return i;
}

static size_t WriteTestUnlinked(size_t from, size_t to)
{
	size_t i;
	for(i = from; i < to; ++i)
	{
		const char* name = testSymbols[i].name;
		ObjectWriteUnlinked(&oos, name, strlen(name), testSymbols[i].value);
	}
	
	return i;
}

static size_t ReadTestSymbolTable(size_t from, size_t to)
{
	ObjSize_t offset;
	const char* name;
	size_t nameSize;
	UWord_t value;
	size_t i = 0;
	for(i = from; i < to && !ObjectSymbolIteratorNext(&symIt, &offset, &name, &nameSize, &value); ++i)
	{
		assert(!strcmp(name, testSymbols[i].name));
		assert(value == testSymbols[i].value);
	}

	return i;
}

static size_t ReadTestUnlinked(size_t from, size_t to)
{
	const char* name;
	size_t nameSize;
	UWord_t addr;
	size_t i = 0;
	for(i = from; i < to && !ObjectUnlinkedIteratorNext(&unlinkedIt, &addr, &name, &nameSize); ++i)
	{
		assert(!strcmp(name, testSymbols[i].name));
		assert(addr == testSymbols[i].value);
	}
	
	return i;
}








