#include <unordered_map>
#include <string>
#include <iostream>

using namespace std;

struct state {
        int id;
        string name;
};


int main(int argc, char **argv) {
    unordered_map<int, state> m;
    state tmp1 = {1, "First state"};
    state tmp2 = {2, "Second state"};
    m[1] = tmp1; 
    m[2] = tmp2;

    unordered_map<int, state>::iterator i = m.find(1);

    if (i != m.end())
        cout  << "Key: " << i->first << ", Value: " << i->second.name << endl;

    return 0;


}