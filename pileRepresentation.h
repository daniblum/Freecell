#pragma once
#include <string>
class Table;

class pileRepresentation
{
    char group;
    int index;
    friend Table;

public:
    pileRepresentation(char gr, int idx): group(gr), index(idx) {}
    pileRepresentation(const char* s);
    pileRepresentation() : group('A'), index(0) {}
    bool Validate() const;
    ~pileRepresentation(void);
    char GetGroup(void) const { return group; }
    int GetIdx(void) const { return index; }
    int ToString(std::string& str) const;
};

