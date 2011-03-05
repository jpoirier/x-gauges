#ifndef __UNIT_VALUE_H
#define __UNIT_VALUE_H

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

#include "Unit.h"

namespace unit {

// -------------------------------------------------------------------------

class Value
{
public:
	//! Default empty (invalid uv) constructor.
	Value();
	//! Constructor using key index and a uv.
	Value(int iKey, qreal f);
	//! Constructor using string signature index and a uv.
	Value(const QString& qsSig, qreal f);
	//! Copy constructor.
	Value(const Value& C);

	//! Set uvs directly using key index and a uv.
	void Set(int iKey, qreal f);
	//! Set uvs directly using signature string and a uv.
	void Set(const QString& qsSig, qreal f);

	//! Set uv of the unit directly
	void SetValue(qreal f) { m_f = f; }
	//! Get uv of the unit directly.
	qreal GetValue() const { return m_f; }
	
	//! Is valid? - Is unit set?
	bool IsValid() const
	{ return m_pUnit != NULL; }
	//! Is non zero
	bool IsNonZero() const
	{ return m_pUnit != NULL && m_f != 0.0f; }

	//! Convert into unit given as a key index. 
	qreal GetAs(int iKey) const;
	//! Convert into unit given as a signature string. 
	qreal GetAs(const QString& qsSig) const;
	//! Convert into unit given as a raw class. 
	qreal GetAs(const Unit* pUnitTo) const;
	
	//! Convert internaly into unit given as a key index.
	void ConvertTo(int iKey);
	//! Convert internaly into unit given as a singature string.
	void ConvertTo(const QString& qsSig);
	
	//! Format into string
 	QString Format(int iPrec=-1, char chFormat='f') const;
 	//! Format into string, but absolute value is shown.
 	QString FormatAbs(int iPrec=-1, char chFormat='f') const;

	//! Return const access to the unit.
	const Unit* GetUnit() const { return m_pUnit; }

	//! Equality operator.
	bool operator==(const Value& uv) const;
	//! Non-euality operator.
	bool operator!=(const Value& uv) const 
	{ return !operator==(uv); }
	//! Less operator.
	bool operator<(const Value& uv) const;
	//! Greater than operator.
	bool operator>(const Value& uv) const;
	//! Less or euqual operator.
	bool operator<=(const Value& uv) const
		{ return operator<(uv) || operator==(uv); }
	//! Greater than or euqual operator.
	bool operator>=(const Value& uv) const
		{ return operator>(uv) || operator==(uv); }
	//! Assignemet operator.
	const Value& operator=(const Value& uv);
	//! Increment operator.
	const Value& operator+=(const Value& uv);
	//! Decremement operator.
	const Value& operator-=(const Value& uv);
	
	//! Sum operator.
	Value operator+(const Value& uv);
	//! Subtract operator.
	Value operator-(const Value& uv);
	//! Minus sign operator.
	Value operator-();
	
	//! Self multiply operator.
	const Value& operator*=(qreal f);
	//! Self divide operator.
	const Value& operator/=(qreal f);

	//! Multiply operator.
	Value operator*(qreal f);
	//! Divide operator.
	Value operator/(qreal f);

private:
	//! Return const access to the unit group.
	const Group* GetGroup() const;
	//! Internal helper which returns a value converted to the base unit.
	qreal ToBase() const { return m_pUnit->ConvertToBase(m_f); }
private:
	//! Value of given unit.
	qreal m_f;
	//! Unit that uv belongs to.
	const Unit* m_pUnit;
};

// --------------------------------------------------------------------------

} // namespace

#endif // !defined(AFX_AIUNITVALUE_H__482DB077_D5B7_11D1_8DC4_008048D9A15D__INCLUDED_)
