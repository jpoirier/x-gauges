#ifndef INSTRUMENT_PARAMETER_CONTAINER_H
#define INSTRUMENT_PARAMETER_CONTAINER_H

/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2009 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   License: GPL - GNU General Public License                             *
 *            See 'COPYING.html' for more details about the license.       *
 *                                                                         *
 ***************************************************************************/

#include <memory>
#include "Parameter.h"

// -------------------------------------------------------------------------

namespace instrument {

// -------------------------------------------------------------------------
/** \brief Container (the only one) of parameters used in the system.
 */
class ParameterContainer
{
	typedef QHash<QString, Parameter*> Container;
	//! Default private constructor.
	ParameterContainer();

public:
	//! Destructor
	~ParameterContainer();
	//! The only access to the object.
	static ParameterContainer* GetInstance();

	//! Parse parameters from QDomDociment object.
	bool Load(const QDomDocument& doc);

	//! Return the required paramter (by name).
	Parameter* GetParameter(const QString& qsName);

	//! Helper function that sets callback and user key for given parameter.
	bool SetCallbackUserKey(const QString& qsParam, 
		Delegate0<float> D, const QString& qsUserUnitKey);
	//! Helper function that sets callback and user key for given parameter.
	bool SetCallbackUserKey(const QString& qsParam, 
		Delegate<float, int> D, const QString& qsUserUnitKey);

private:
	//! Create the object according to parameter name.
	Parameter* CreateParameter(const QDomElement& el);

	//! Parameter and unit key check.
	QPair<Parameter*, int> CheckParameterAndKey(const QString& qsParam, const QString& qsUnitKey);
	
private:
	//! The only instance of the container object.
	static std::auto_ptr<ParameterContainer> s_apInstance;
	
	//! Container of Parameters.
	Container m_con;
};

} // namespace

#endif
