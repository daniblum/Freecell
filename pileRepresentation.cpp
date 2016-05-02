#include <sstream>
#include "pileRepresentation.h"
#include "Table.h"

bool pileRepresentation::Validate(void) const
{
    int maxIdx = 0;
    bool result = true;
    switch( this->group ) {
        case 'C':
            maxIdx = Table::NUM_CELLS;
            break;

        case 'F':
            maxIdx = Table::NUM_FOUNDATIONPILES;
            break;

        case 'T':
            maxIdx = Table::NUM_TABLEAUPILES;
            break;

        default:
            result = false;
            break;
        
    } // switch
    if (result) {
        result = ( ( index > 0 ) && (index <= maxIdx) );
    }
    return result;
}

pileRepresentation::~pileRepresentation(void)
{
}


int pileRepresentation::ToString(std::string& str) const
{
  str.clear();
  // char n[4];
  std::ostringstream stringStream;
  stringStream << GetGroup() << GetIdx();
  str.append( stringStream.str() );
  // sprintf(n, "%c%d", this->GetGroup(), this->GetIdx());
  // str.append(n);
  return 0;
}


pileRepresentation::pileRepresentation(const char* s)
{
  this->group = s[0];
  this->index = s[1] - '0';
}
