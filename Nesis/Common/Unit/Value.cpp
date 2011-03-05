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

#include <math.h>
#include "Manager.h"
#include "Value.h"

namespace unit {

// --------------------------------------------------------------------------

Value::Value()
{
	m_f = 0;
	m_pUnit = NULL;
}

Value::Value(int iKey, qreal f)
{
	Set(iKey, f);
}

Value::Value(const QString& qsSig, qreal f)
{
	Set(qsSig, f);
}

Value::Value(const Value& C)
{
	m_f = C.m_f;
	m_pUnit = C.m_pUnit;
}

// --------------------------------------------------------------------------

void Value::Set(int iKey, qreal f)
{
	Manager* um = Manager::GetInstance();
	m_pUnit = um->GetUnit(iKey);
	Q_ASSERT(m_pUnit);
	m_f = f;
}

// --------------------------------------------------------------------------

void Value::Set(const QString& qsSig, qreal f)
{
	Manager* um = Manager::GetInstance();
	m_pUnit = um->GetUnit(qsSig);
	Q_ASSERT(m_pUnit);
	m_f = f;
}

// --------------------------------------------------------------------------
/** Convert current uv/unit into a uv required by the
    given iKey index. 
 */
qreal Value::GetAs(
	int iKey	//!< Unit to convert to.
) const
{
	Manager* um = Manager::GetInstance();
	return um->Convert(m_f, m_pUnit, um->GetUnit(iKey));
}

// --------------------------------------------------------------------------

qreal Value::GetAs(const QString& qsSig) const
{
	Manager* um = Manager::GetInstance();
	return um->Convert(m_f, m_pUnit, um->GetUnit(qsSig));
}
// --------------------------------------------------------------------------

qreal Value::GetAs(const Unit* pUnitTo) const
{
	Manager* um = Manager::GetInstance();
	return um->Convert(m_f, m_pUnit, pUnitTo);
}

// --------------------------------------------------------------------------

void Value::ConvertTo(int iKey)
{
	Q_ASSERT(iKey >=0);
	Manager* um = Manager::GetInstance();
	
	const Unit* pTo = um->GetUnit(iKey);
	m_f = um->Convert(m_f, m_pUnit, pTo);
	m_pUnit = pTo;
}

// --------------------------------------------------------------------------

void Value::ConvertTo(const QString& qsSig)
{
	Manager* um = Manager::GetInstance();
	
	const Unit* pTo = um->GetUnit(qsSig);
	m_f = um->Convert(m_f, m_pUnit, pTo);
	m_pUnit = pTo;
}

// --------------------------------------------------------------------------

QString Value::Format(int iPrec, char chFormat) const
{
	if(m_pUnit) {
		return QString("%1 %2")
			.arg(m_f, 0, chFormat, iPrec)
			.arg(m_pUnit->GetSignature());
	}
	else
		return QString();
}
// --------------------------------------------------------------------------

QString Value::FormatAbs(int iPrec, char chFormat) const
{
	if(m_pUnit) {
		return QString("%1 %2")
			.arg(fabs(m_f), 0, chFormat, iPrec)
			.arg(m_pUnit->GetSignature());
	}
	else {
		return QString();
	}
}

// --------------------------------------------------------------------------

const Group* Value::GetGroup() const
{ 
	return m_pUnit ? m_pUnit->GetGroup() : NULL; 
}

// --------------------------------------------------------------------------

bool Value::operator==(const Value& uv) const
{
	Q_ASSERT(GetGroup() == uv.GetGroup());
	return (ToBase() == uv.ToBase());
}

// --------------------------------------------------------------------------

bool Value::operator<(const Value& uv) const
{
	Q_ASSERT(GetGroup() == uv.GetGroup());
	return (ToBase() < uv.ToBase());
}

// --------------------------------------------------------------------------

bool Value::operator>(const Value& uv) const
{
	Q_ASSERT(GetGroup() == uv.GetGroup());
	return (ToBase() > uv.ToBase());
}

// --------------------------------------------------------------------------

const Value& Value::operator=(const Value& uv)
{
	m_f = uv.m_f;
	m_pUnit = uv.m_pUnit;
	return *this;
}

// --------------------------------------------------------------------------

const Value& Value::operator+=(const Value& uv)
{
	Q_ASSERT(GetGroup() == uv.GetGroup());

	Manager* um = Manager::GetInstance();
	m_f += um->Convert(uv.GetValue(), uv.GetUnit(), m_pUnit);
	return *this;
}

// --------------------------------------------------------------------------

const Value& Value::operator-=(const Value& uv)
{
	Q_ASSERT(GetGroup() == uv.GetGroup());

	Manager* um = Manager::GetInstance();
	m_f -= um->Convert(uv.GetValue(), uv.GetUnit(), m_pUnit);
	return *this;
}

// --------------------------------------------------------------------------

Value Value::operator+(const Value& uv)
{
	Q_ASSERT(GetGroup() == uv.GetGroup());
	Value v(*this);
	v += uv;
	return v;
}

// --------------------------------------------------------------------------

Value Value::operator-(const Value& uv)
{
	Q_ASSERT(GetGroup() == uv.GetGroup());
	Value v(*this);
	v -= uv;
	return v;
}

// --------------------------------------------------------------------------

Value Value::operator-()
{
	Value v(*this);
	v.SetValue(-m_f);
	return v;
}

// --------------------------------------------------------------------------

const Value& Value::operator*=(qreal f)
{
	m_f *= f;
	return *this;
}

// --------------------------------------------------------------------------

const Value& Value::operator/=(qreal d)
{
	m_f /= d;
	return *this;
}

// --------------------------------------------------------------------------

Value Value::operator*(qreal f)
{
	Value v(*this);
	v *= f;
	return v;
}

// --------------------------------------------------------------------------

Value Value::operator/(qreal f)
{
	Value v(*this);
	v /= f;
	return v;
}

// --------------------------------------------------------------------------

} // namespace unit
