/*
#include "scanner.h"
#include "symtable.h"
#include "map.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int TestScanner(int argc, char** argv);
static int TestSymTable(int argc, char** argv);
static int TestHashMap(int argc, char** argv);
static int TestMap(int argc, char** argv);

int main(int argc, char** argv)
{
	puts("BEGIN");

//	int e = TestScanner(argc, argv);
	int e = TestSymTable(argc, argv);
//	int e = TestMap(argc, argv);
	
	puts("");
	puts("END");
	
	return e;
}

static int TestScanner(int argc, char** argv)
{
	assert(argc == 2);
	FILE* scannerInput = fopen(argv[1], "r");
	assert(scannerInput);
	
	Scanner scanner;
	ScannerInit(&scanner, scannerInput);

	ScannerDumpPretty(stdout, &scanner);

	return 0;
}

static int TestSymTable(int argc, char** argv)
{
	struct TestPair
	{
		const char* key;
		UWord_t value;
	} pairs[] =
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

	const size_t pairCount = sizeof(pairs) / sizeof(struct TestPair);
	const size_t symTableSize = (pairCount << 1) - (pairCount >> 1);

	SymTable symTable;
	SymTableInit(&symTable, symTableSize);

	size_t i;
	for(i = 0; i < pairCount; ++i)
	{
		switch(SymTableInsert(&symTable, pairs[i].key, pairs[i].value))
		{
		case HASHSET_ERROR_DUPLICATE:
			assert(0 && "insert: duplicate");
			break;
		case HASHSET_ERROR_INSUFFICIENT_MEM:
			assert(0 && "insert: insufficient memory");
			break;
		default:
			break;
		}
	}

	for(i = 0; i < pairCount; ++i)
	{
		UWord_t value = SymTableFind(&symTable, pairs[i].key);
		assert(pairs[i].value == value);

//		switch(SymTableFind(&symTable, pairs[i].key))
//		{
//		case HASHSET_ERROR_DUPLICATE:
//			assert(0 && "find: duplicate");
//			break;
//		case HASHSET_ERROR_INSUFFICIENT_MEM:
//			assert(0 && "find: insufficient memory");
//			break;
//		case HASHSET_ERROR_NOT_FOUND:
//			assert(0 && "find: not found");
//			break;
//		default:
//			break;
//		}
	}

	return 0;
}

static int TestMap(int argc, char** argv)
{
	assert(argc == 2);

	MapFile map;
	MapOpen(&map, argv[1], 0);

#define write(k, v) do \
	{ \
		const char* k_ = (k); \
		MapWrite(&map, k_, strlen(k_), (v)); \
	} while(0);

	write("jan", 123);
	write("piet", 456);
	write("kees", 789);

	MapClose(&map);
	MapOpen(&map, argv[1], 1);

	MapDump(stdout, &map);
	MapClose(&map);

	return 0;
}
*/
