#pragma once
#include "CardPile.h"

class Cell: public CardPile
{
public:
    bool canAddThisCard( Card candidate ) const;
};

