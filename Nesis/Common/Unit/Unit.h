#ifndef __UNIT_UNIT_H
#define __UNIT_UNIT_H

/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2007 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   License: GPL - GNU General Public License                             *
 *            See 'COPYING.html' for more details about the license.       *
 *                                                                         *
 ***************************************************************************/

#include <QString>

namespace unit {

class Group;

/** \brief This class defines a physical unit. Units are used to pair some value.

	Units object know how to convert some given real value into a base unit 
	and vice versa. They also store signature and description. The conversion
	of some value expressed in this unit to a value in the base unit is made using 
	linear function y = C0 + x*C1. The coefficients m_fC0 and m_fC1 are set in the 
	constructor. Note that most units will have m_fC0 set to zero.
	
	Each unit must belong to some group of units of the same class 
	(e.g. distance, mass, temperature, etc). Hence, the unit object holds a
	pointer to the Group object.
	
	We will rarely use this class directly. Since the constructor is privare,
	only Manager class can create units.
*/

class Unit  
{
//! Only the unit::Manager object can create units.
friend class Manager; 

private:
	//! Constructor
	Unit(const QString& qsSig, const QString& qsDesc, 
		  const float fC1, const float dC0, 
		  const Group* pGroup);
public:
	//! Destructor
	~Unit();

	//! Converts \a x from this unit to the base unit.
	float ConvertToBase(float x) const {	return (x * m_fC1 + m_fC0); } 
	//! Converts \a x from base unit to this unit. 
	float ConvertFromBase(float x) const { return ((x - m_fC0)/ m_fC1); }
	
	//! Returns the signature of the unit.
	const QString& GetSignature() const { return m_qsSig; }
	//! Returns the description - long name of the unit.
	const QString& GetDescription() const { return m_qsDesc; }
	
	//! Returns group object that the this unit belongs to.
	const Group* GetGroup() const { return m_pGroup; }

private:
	//! Pointer to group that unit belongs to.
	const Group *m_pGroup;
	
	//! Signature of the unit. E.g. cm
	const QString m_qsSig;
	//! Description of the unit - long name: E.g. centimeter
	const QString m_qsDesc;
	
	//! First coefficient (offset - contant part).
	const float m_fC0;
	//! Second convert coefficient (linear part).
	const float m_fC1;
};

// --------------------------------------------------------------------------

} // namespace

#endif 
