#pragma once
#include "Card.h"
#include "pileRepresentation.h"

class CardMovement {
public:
  CARD_VECTOR fromCard;
  pileRepresentation from, to;
  CardMovement(): fromCard(), from(), to() {}
  CardMovement( CARD_VECTOR fr, pileRepresentation frP, pileRepresentation t): fromCard(fr), from(frP), to(t) {}
  ~CardMovement() {}
};

