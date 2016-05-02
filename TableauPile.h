#pragma once
#include "CardPile.h"

class TableauPile: public CardPile
{
protected:
    void CalcFirstCardMovable();

public:
    bool canAddThisCard( Card candidate ) const;
};
