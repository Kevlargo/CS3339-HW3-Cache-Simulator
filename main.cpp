// Adriel Largo
// CS 3390
// HW 3
//4-16-2026





#include <fstream> // include file stream library
#include <climits> // include Climits to help use INT MAX
#include <iostream> // include input/output library
#include <set>// for EC
using namespace std;

// Entry class
// will be used to check if its valid, the tag and how recently it was used
class Entry {
public:
    Entry(); // constructor when created
    ~Entry(); // destructor
    void display(ofstream& outfile); // print to the output file
    void set_tag(int _tag) { tag = _tag; }// setter for tag value
    int get_tag() { return tag; }// getter for tag value

    void set_valid(bool _valid){ valid = _valid; } // setter for the valid and set true if entry is valid
    bool get_valid() { return valid; } // getter for the valid entry

    void set_ref(int _ref) { ref = _ref; } // setter for the time stamp to determine used more recently
    int get_ref() { return ref; } // getter for the timestamp

private:
    // declare variables
    bool valid; //  tracks if the address is real and valid
    unsigned tag; // stores the tag variable and helps identify address
    int ref; // LRU counter — higher = more recently used
};
///////////////////////////////////////////////////////////

// Constructor sets the slot as empty, tag and ref to 0 aswell
Entry::Entry() : valid(false), tag(0), ref(0) {}
Entry::~Entry() {}   // destructor

// print to user the output file
void Entry::display(ofstream& outfile) {
    outfile << " [valid=" << valid << " tag=" << tag << " ref=" << ref << "]";
}
///////////////////////////////////////////////////////////
// Extra credit classify what kind of miss is it
class ClassifyCache {
public:
    ClassifyCache(int num_entries);
    ~ClassifyCache();
    bool hit(unsigned long addr);   // returns true if address is in class
    void update(unsigned long addr);// loads address into classify class
private:
    int      size;    // total slots
    Entry*   slots;   // array of entries
    int      time;    // LRU clock


};


ClassifyCache::ClassifyCache(int num_entries) : size(num_entries), time(0) {
    slots = new Entry[size]; // invalid by default
}

ClassifyCache::~ClassifyCache() {

    delete[] slots;
}
// Check if addr is in the classify cache
bool ClassifyCache::hit(unsigned long addr) {
    for (int w = 0; w < size; w++) {
        // class will stores full address
        if (slots[w].get_valid() && (unsigned long)slots[w].get_tag() == addr) {
            slots[w].set_ref(++time); // update LRU
            return true;
        }
    }
    return false;
}

// Load address into new class
void ClassifyCache::update(unsigned long addr) {
    // for loop to look for empty slot first
    for (int w = 0; w < size; w++) {
        if (!slots[w].get_valid()) {
            slots[w].set_tag((int)addr);
            slots[w].set_valid(true);
            slots[w].set_ref(++time);
            return;
        }
    }
    // no empty slot
    int lru_way = 0;
    int lru_ref = INT_MAX;
    for (int w = 0; w < size; w++) {
        if (slots[w].get_ref() < lru_ref) {
            lru_ref = slots[w].get_ref();
            lru_way = w;
        }
    }
    //update
    slots[lru_way].set_tag((int)addr);
    slots[lru_way].set_valid(true);
    slots[lru_way].set_ref(++time);
}


////////////////////////////////////////////////////////////
// Cache Class
// Handles Hit and Misses

class Cache {
public:
    Cache(int num_entries, int associativity); // Constructor
    ~Cache(); // Destructor

    void display(ofstream& outfile); // print to user the output file
    int get_index(unsigned long addr);// Calculate which SET an address belongs to
    int get_tag (unsigned long addr); // Calculate Tag of an address
    unsigned long retrieve_addr(int way, int index); // get address from stored entry
    bool hit (ofstream& outfile, unsigned long addr); // check if address is in cache
    void update(ofstream& outfile, unsigned long addr); // used when a miss occurs, load new address into cache
private:
    // declare variables
    int assoc; // slots per set
    unsigned num_entries; // number of cache entries
    int num_sets; // number of sets
    Entry** entries; // 2-D array of entry objects
    int time; // global counter to track LRU order

    // EC variables
    set<unsigned long> seen;
    ClassifyCache* TypeClassify;
    string classify_miss(unsigned long addr);
};

// Constructor — allocate 2-D and  all entries start invalid
Cache::Cache(int num_entries, int associativity)
    : assoc(associativity),  // store associativity
        num_entries(num_entries), // store number of entries
        num_sets(num_entries / associativity), // calc number of sets
        time(0) // set time LRU to 0
        {
    entries = new Entry*[num_sets]; // allocate an array of pointer for the sets
    for (int i = 0; i < num_sets; i++) //for loop to allocate the array
        entries[i] = new Entry[assoc];// by default the entry starts with a false valid
    TypeClassify = new ClassifyCache(num_entries);
}

// Destructor — free 2-D array
Cache::~Cache() { // loop to delete each row
    for (int i = 0; i < num_sets; i++)
        delete[] entries[i];
    delete[] entries;// delete pointers
    delete TypeClassify;
}

// print the cache table to output file
void Cache::display(ofstream& outfile) {
    outfile << "\n-- Cache State --\n"; // print to user
    for (int i = 0; i < num_sets; i++) { // loop through each set
        outfile << "Set " << i << ":\n"; //print to user
        for (int w = 0; w < assoc; w++) {// loop through each way
        entries[i][w].display(outfile); // print to user
        outfile << "\n";
}
}
outfile << "-----------------\n";
}


// Calculate which set this address maps to
int Cache::get_index(unsigned long addr) {
    return addr % num_sets; // formula for the index and return index__
}

// calculate the tag for the address
int Cache::get_tag(unsigned long addr) {
  return addr / num_sets; // formula for tag and return the value for tag
}



// reconstruct the address from the cache entry
unsigned long Cache::retrieve_addr(int way, int index) {
  return (unsigned long)entries[index][way].get_tag() * num_sets + index;
}

// Classify  the miss
string Cache::classify_miss(unsigned long addr) {
// COMPULSORY:
    if (seen.find(addr) == seen.end())
    return "COMPULSORY";
// CONFLICT:
// CAPACITY:
// use if statement to determine

    if (TypeClassify->hit(addr))
        return "CONFLICT";
return "CAPACITY";
}

// Check whether addr is in cache;
bool Cache::hit(ofstream& outfile, unsigned long addr) {
  int idx = get_index(addr); // which set correlates to the address
  int tg  = get_tag(addr); // which tag correlates to the tag

  for (int w = 0; w < assoc; w++) { // for loop to search for the slots
    // if statement to check slot is valid and has a tag that matches it
    if (entries[idx][w].get_valid() && entries[idx][w].get_tag() == tg) {
      // For a hit
      entries[idx][w].set_ref(++time);
      outfile << addr << " : HIT\n"; // print HIT
      TypeClassify->hit(addr);
      seen.insert(addr);

      return true; // return true to signal it was a HIT
    }
  }

  // For a MISS when the slot doesn't match
    string miss_type = classify_miss(addr);
    outfile << addr << " : MISS (" << miss_type << ")\n";
    seen.insert(addr);
    TypeClassify->update(addr);
    update(outfile, addr);
    return false;
}


// load address into cache after a miss
void Cache::update(ofstream& outfile, unsigned long addr) {
  int idx = get_index(addr); // find set the address belongs to
  int tg  = get_tag(addr); // find the tag the address belongs to

  // look for an empty slot in the set
  for (int w = 0; w < assoc; w++) { // for loop to look for an empty slot

    if (!entries[idx][w].get_valid()) { // if statement to check if empty
      entries[idx][w].set_tag(tg);// store the tag
      entries[idx][w].set_valid(true);// mark the slog as a valid slot
      entries[idx][w].set_ref(++time);// set the LRU time
      return;
    }
  }

  //  theres no empty slot
  int lru_way = 0; // set LRU way to 0
  int lru_ref = INT_MAX; // use int max to compare values
  for (int w = 0; w < assoc; w++) { //for loop to find smallest ref
    if (entries[idx][w].get_ref() < lru_ref) { // check if way was used less recently
      lru_ref = entries[idx][w].get_ref(); // update
      lru_way = w;
    }
  }

  entries[idx][lru_way].set_tag(tg); // update the LRU entry with new tag
  entries[idx][lru_way].set_valid(true); // mark it as a valid entry
  entries[idx][lru_way].set_ref(++time); // update the LRU time
}

// last step : MAIN where it reads arg , open files and runs the Cache simulation


int main(int argc, char* argv[]) {
  if (argc != 4) {// for exactly 3 arguments
    cerr << "Usage: ./cache_sim <num_entries> <associativity> <memory_reference_file>\n";
    return 1; //exit code
  }
    //
  int num_entries   = stoi(argv[1]); // read 1st argument for number of entries
  int associativity = stoi(argv[2]); // read 2nd argument for associativity
  string ref_file   = argv[3]; // read 3rd argument for reference file

  // Validate the three inputs
  // use if statement to check if it evenly divides
  if (associativity > num_entries || num_entries % associativity != 0) {
    cerr << "Error: associativity must evenly divide num_entries.\n";
    return 1;
  }

  Cache cache(num_entries, associativity); // create object

  ifstream infile(ref_file); // open file
  if (!infile) { // if statement to check if it opened successfully
    cerr << "Error: cannot open input file: " << ref_file << "\n";
    return 1;
  }
    // open hit or miss file
  ofstream outfile("cache_sim_output");
  if (!outfile) { // if statement if the file was opened successfully
    cerr << "Error: cannot open output file.\n";
    return 1;
  }

  unsigned long addr; // variable to hold the address
  while (infile >> addr) { // while loop to read each address from input file
    cache.hit(outfile, addr);
  }

  infile.close(); // close input file
  outfile.close(); // close output file
  return 0;
}
