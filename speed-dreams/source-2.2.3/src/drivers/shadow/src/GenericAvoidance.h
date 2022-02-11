#ifndef _GENERICAVOIDANCE_H_
#define _GENERICAVOIDANCE_H_

#include "Avoidance.h"

class GenericAvoidance : public Avoidance
{
public:
	GenericAvoidance();
	virtual ~GenericAvoidance();

	virtual int		priority( const Info& ai, const CarElt* pCar ) const;
    virtual Vec2d	calcTarget(const Info& ai, const CarElt* pCar,
                                const TDriver &me );
};

#endif
