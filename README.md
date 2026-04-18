# CS3339-HW3-Cache-Simulator

How to Obtain the Code
Clone this repository to your working directory: 
git clone https://github.com/Kevlargo/CS3339-HW3-Cache-Simulator

Then copy the file to your directory:
cp CS3339-HW3-Cache-Simulator/cache_sim.cpp

How to Build
Make sure you have g++ installed, then compile:
g++ -o cache_sim cache_sim.cpp -std=c++17

How to Run
./cache_sim <num_entries> <associativity> <memory_reference_file>

Output is written to a file called cache_sim_output

Extra Credit: Miss classification — each miss is labeled as:

COMPULSORY
CONFLICT 
CAPACITY 

Known Bugs :

Only handles positive integer memory addresses
