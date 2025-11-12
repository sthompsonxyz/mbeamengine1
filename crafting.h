#pragma once

#include <string>
#include <map>
#include <vector>

//crafting
//
// item 
// craft step produces item, has item deps, has tech requirements



struct item {
	unsigned int itemID;
	std::string name;
	std::string description;
};

struct itemtable {
	
};

struct manufacture {
	std::vector<item> deps;
	float timereq;
};


struct itemcrafting {

	std::map<std::string, manufacture> crafttree;


};

struct gameitem {
	std::string name;
	int itemType;
	//item img
	//item inv size/shape
};

struct items {
	std::map<std::string, gameitem> itemmap;
};

struct inventory {

	inventory(int w, int h):width(w),height(h){
		capacity = w * h;
	}
	int width;
	int height;
	int capacity;

};

struct gameitemstate {
	
};