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
//	struct TestPair
//	{
//		const char* key;
//		unsigned value;
//	} pairs[] =
//	{
//		{ "Adult", 1252 },
//		{ "Aeroplane", 563585 },
//		{ "Air", 193305 },
//		{ "Aircraft Carrier", 808740 },
//		{ "Airforce", 585009 },
//		{ "Airport", 479873 },
//		{ "Album", 350292 },
//		{ "Alphabet", 895962 },
//		{ "Apple", 822840 },
//		{ "Arm", 746605 },
//		{ "Army", 174108 },
//		{ "Baby", 858943 },
//		{ "Backpack", 513535 },
//		{ "Balloon", 303995 },
//		{ "Banana", 14985 },
//		{ "Bank", 91403 },
//		{ "Barbecue", 364452 },
//		{ "Bathroom", 147313 },
//		{ "Bathtub", 165899 },
//		{ "Bed", 988525 },
//		{ "Bee", 119084 },
//		{ "Bible", 4670 },
//		{ "Bird", 377880 },
//		{ "Bomb", 531663 },
//		{ "Book", 571184 },
//		{ "Boss", 601764 },
//		{ "Bottle", 607166 },
//		{ "Bowl", 166235 },
//		{ "Box", 663045 },
//		{ "Boy", 450789 },
//		{ "Brain", 352123 },
//		{ "Bridge", 57040 },
//		{ "Butterfly", 607684 },
//		{ "Button", 783318 },
//		{ "Cappuccino", 802606 },
//		{ "Car", 519883 },
//		{ "Car-race", 301950 },
//		{ "Carpet", 875972 },
//		{ "Carrot", 726676 },
//		{ "Cave", 955900 },
//		{ "Chair", 925718 },
//		{ "Chess Board", 539354 },
//		{ "Chief", 142339 },
//		{ "Child", 462081 },
//		{ "Chisel", 235328 },
//		{ "Chocolates", 862239 },
//		{ "Church", 209601 },
//		{ "Circle", 843653 },
//		{ "Circus", 996795 },
//		{ "Clock", 611499 },
//		{ "Clown", 392438 },
//		{ "Coffee", 266213 },
//		{ "Coffee-shop", 297281 },
//		{ "Comet", 840144 },
//		{ "Compact Disc", 23744 },
//		{ "Compass", 375866 },
//		{ "Computer", 92624 },
//		{ "Crystal", 677206 },
//		{ "Cup", 56216 },
//		{ "Cycle", 8790 },
//		{ "Data Base", 918790 },
//		{ "Desk", 275888 },
//		{ "Diamond", 272897 },
//		{ "Dress", 587908 },
//		{ "Drill", 691183 },
//		{ "Drink", 837611 },
//		{ "Drum", 726493 },
//		{ "Dung", 484939 },
//		{ "Ears", 205359 },
//		{ "Earth", 743736 },
//		{ "Egg", 468459 },
//		{ "Electricity", 457961 },
//		{ "Elephant", 949156 },
//		{ "Eraser", 744438 },
//		{ "Explosive", 108280 },
//		{ "Eyes", 599048 },
//		{ "Family", 385235 },
//		{ "Fan", 735008 },
//		{ "Feather", 608966 },
//		{ "Festival", 572405 },
//		{ "Film", 361339 },
//		{ "Finger", 151555 },
//		{ "Fire", 225105 },
//		{ "Floodlight", 425153 },
//		{ "Flower", 802881 },
//		{ "Foot", 517106 },
//		{ "Fork", 989989 },
//		{ "Freeway", 751549 },
//		{ "Fruit", 345561 },
//		{ "Fungus", 168981 },
//		{ "Game", 657307 },
//		{ "Garden", 491897 },
//		{ "Gas", 63540 },
//		{ "Gate", 699759 },
//		{ "Gemstone", 504807 },
//		{ "Girl", 147496 },
//		{ "Gloves", 949583 },
//		{ "God", 141576 },
//		{ "Grapes", 905118 },
//		{ "Guitar", 692892 },
//		{ "Hammer", 303049 },
//		{ "Hat", 426557 },
//		{ "Hieroglyph", 70376 },
//		{ "Highway", 966612 },
//		{ "Horoscope", 683187 },
//		{ "Horse", 153234 },
//		{ "Hose", 877254 },
//		{ "Ice", 821680 },
//		{ "Ice-cream", 582049 },
//		{ "Insect", 191351 },
//		{ "Jet fighter", 177893 },
//		{ "Junk", 817194 },
//		{ "Kaleidoscope", 475265 },
//		{ "Kitchen", 155553 },
//		{ "Knife", 503922 },
//		{ "Leather jacket", 732017 },
//		{ "Leg", 405591 },
//		{ "Library", 279580 },
//		{ "Liquid", 568743 },
//		{ "Magnet", 682241 },
//		{ "Man", 755852 },
//		{ "Map", 721915 },
//		{ "Maze", 475295 },
//		{ "Meat", 123020 },
//		{ "Meteor", 367809 },
//		{ "Microscope", 834681 },
//		{ "Milk", 35097 },
//		{ "Milkshake", 517014 },
//		{ "Mist", 662984 },
//		{ "Money $$$$", 426222 },
//		{ "Monster", 104679 },
//		{ "Mosquito", 949339 },
//		{ "Mouth", 921384 },
//		{ "Nail", 549547 },
//		{ "Navy", 345988 },
//		{ "Necklace", 471725 },
//		{ "Needle", 374981 },
//		{ "Onion", 846980 },
//		{ "PaintBrush", 316874 },
//		{ "Pants", 456099 },
//		{ "Parachute", 271890 },
//		{ "Passport", 982970 },
//		{ "Pebble", 297800 },
//		{ "Pendulum", 739189 },
//		{ "Pepper", 567278 },
//		{ "Perfume", 195990 },
//		{ "Pillow", 761314 },
//		{ "Plane", 839442 },
//		{ "Planet", 397656 },
//		{ "Pocket", 500900 },
//		{ "Post-office", 890163 },
//		{ "Potato", 27467 },
//		{ "Printer", 994628 },
//		{ "Prison", 572588 },
//		{ "Pyramid", 50509 },
//		{ "Radar", 531327 },
//		{ "Rainbow", 194068 },
//		{ "Record", 843043 },
//		{ "Restaurant", 626759 },
//		{ "Rifle", 657613 },
//		{ "Ring", 197852 },
//		{ "Robot", 842158 },
//		{ "Rock", 123326 },
//		{ "Rocket", 109928 },
//		{ "Roof", 743125 },
//		{ "Room", 314066 },
//		{ "Rope", 941068 },
//		{ "Saddle", 286081 },
//		{ "Salt", 336314 },
//		{ "Sandpaper", 140263 },
//		{ "Sandwich", 733085 },
//		{ "Satellite", 834620 },
//		{ "School", 707999 },
//		{ "Sex", 600238 },
//		{ "Ship", 747215 },
//		{ "Shoes", 252724 },
//		{ "Shop", 144475 },
//		{ "Shower", 1618 },
//		{ "Signature", 61007 },
//		{ "Skeleton", 806238 },
//		{ "Slave", 852626 },
//		{ "Snail", 210578 },
//		{ "Software", 115604 },
//		{ "Solid", 553209 },
//		{ "Space Shuttle", 14253 },
//		{ "Spectrum", 113773 },
//		{ "Sphere", 454512 },
//		{ "Spice", 752220 },
//		{ "Spiral", 686147 },
//		{ "Spoon", 543443 },
//		{ "Sports-car", 73886 },
//		{ "Spot Light", 436720 },
//		{ "Square", 201941 },
//		{ "Staircase", 696219 },
//		{ "Star", 290353 },
//		{ "Stomach", 436689 },
//		{ "Sun", 232429 },
//		{ "Sunglasses", 577868 },
//		{ "Surveyor", 532578 },
//		{ "Swimming Pool", 628681 },
//		{ "Sword", 160192 },
//		{ "Table", 504135 },
//		{ "Tapestry", 963042 },
//		{ "Teeth", 695761 },
//		{ "Telescope", 924802 },
//		{ "Television", 189948 },
//		{ "Tennis racquet", 335948 },
//		{ "Thermometer", 178350 },
//		{ "Tiger", 995178 },
//		{ "Toilet", 457442 },
//		{ "Tongue", 998016 },
//		{ "Torch", 97507 },
//		{ "Torpedo", 625172 },
//		{ "Train", 94394 },
//		{ "Treadmill", 437727 },
//		{ "Triangle", 931516 },
//		{ "Tunnel", 48433 },
//		{ "Typewriter", 894619 },
//		{ "Umbrella", 290018 },
//		{ "Vacuum", 227302 },
//		{ "Vampire", 769066 },
//		{ "Videotape", 410718 },
//		{ "Vulture", 201972 },
//		{ "Water", 628071 },
//		{ "Weapon", 604144 },
//		{ "Web", 451613 },
//		{ "Wheelchair", 466353 },
//		{ "Window", 597827 },
//		{ "Woman", 634724 },
//		{ "Worm", 854793 },
//		{ "XRay", 828791 }
//	};
//
//	const size_t pairCount = sizeof(pairs) / sizeof(struct TestPair);
//	const size_t memSize = pairCount * sizeof(struct TestPair) * 2;
//
//	HashMap map;
//	char mem[memSize];
//	HashMapInit(&map, mem, memSize, sizeof(char*), sizeof(unsigned), HashString, StrEquals);
//
//	size_t i;
//	for(i = 0; i < pairCount; ++i)
//	{
//		switch(HashMapInsertStrInt(&map, pairs[i].key, pairs[i].value))
//		{
//		case HASHMAP_ERROR_DUPLICATE:
//			assert(0 && "duplicate value");
//			break;
//		case HASHMAP_ERROR_INSUFFICIENT_MEM:
//		{
//			Hash_t hash = HashString(&(pairs[i].key)) | (1 << (8 * sizeof(Hash_t) - 1));
//			size_t toSlot = hash % map.elemCount;
//			HashMapDump(stdout, &map, HashDumpStrInt);
//			assert(0 && "insufficient memory");
//		}	break;
//		default:
//			break;
//		}
//	}
//
//	puts("successfully inserted all pairs");
//	HashMapDump(stdout, &map, HashDumpStrInt);
//
//	for(i = 0; i < pairCount; ++i)
//	{
//		unsigned* v = HashMapFindStrInt(&map, pairs[i].key);
//		assert(v);
//		assert(*v == pairs[i].value);
//	}

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














