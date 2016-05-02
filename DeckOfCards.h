#pragma once
#include <vector>

#include "Card.h"

class DeckOfCards
{
    bool isShuffled;
    CARD_VECTOR origPile, tempPile;
    int maxNumOnDeck;
    size_t maxNumCount;
    void RecalcMaxNum();

public:
    DeckOfCards(void);
    int LoadFromXML(const wxXmlNode* td);
    virtual ~DeckOfCards(void);
    void Shuffle(void);
    Card DealOneCard(void);
    Card Peek(void) const;
    void PutBackOnFront(void);
    bool empty() { return tempPile.empty(); }
    void Reset() { tempPile = origPile; RecalcMaxNum(); }
    size_t size() { return tempPile.size(); }
    int getMaxNum() { return maxNumOnDeck; }
};
