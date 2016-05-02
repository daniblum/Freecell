#pragma once
#include "CardPile.h"

class FoundationPile: public CardPile
{

public:
    bool canAddThisCard( Card candidate ) const;
};
