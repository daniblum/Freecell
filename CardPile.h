#pragma once
#include <vector>
#include "pileRepresentation.h"
// #include "../tinyxml/tinyxml.h" // pseudo-serialization

#include "Card.h"
class Table;

class CardPile
{
  friend class Table;
protected:
    CARD_VECTOR pile;
    size_t cursor;
    pileRepresentation pileRep;

public:
    CardPile(void);
    virtual void CleanPile(void);
    virtual ~CardPile(void);
    virtual Card GetCardOnTop() const;
    virtual Card GetCardPrevToTop() const;
    virtual Card GetFirstCard();
    virtual Card GetNextCard();
    virtual void AddCard( Card card );
    virtual bool canAddThisCard( Card candidate ) const = 0;
    virtual void SetPileRep( pileRepresentation pR ) { pileRep = pR; }
    virtual pileRepresentation GetPileRep(void) const;
    virtual int PosWhereCardIs( const Card& card ) const;
    virtual Card NextCardTo(const Card& c) const;
    virtual int size(void) const { return int(pile.size()); }
    virtual int Serialize( wxXmlNode* elem ) const;
};
