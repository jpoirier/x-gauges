#ifndef ALARM_CONTAINER_H
#define ALARM_CONTAINER_H

/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2008 by Kanardia d.o.o. [see www.kanardia.eu]           *
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
#include "Alarms.h"

// -------------------------------------------------------------------------
/** \brief Container of all alarms in the Nesis.
 */

class AlarmContainer
{
public:
	typedef QVector<Alarm*> Con;
	typedef QVector<const Alarm*> ConstCon;
private:
	//! Constructor
	AlarmContainer();
public:
	//! Destructor
	~AlarmContainer();
	
	//! The only static access to the container.
	static AlarmContainer* GetInstance();
	
	//! Update alarms status.
	bool Update();
	//! Get container of alarm objects with sPanic status.
	ConstCon GetPanic() const;
	//! Process any messages with the panic status.
	void Process();

	//! Update active alarms - build a list of active alarms.
	void UpdateActive();

	//! Get container of alarm object with instrument::AlarmStatus == asUser.
	ConstCon GetAlarms(instrument::Parameter::AlarmType eAT) const;

private:
	//! Container of all alarms.
	Con m_conAlarm;
	//! Container of active alarms.
	Con m_conActive;
	//! Static instance of the only object.
	static std::auto_ptr<AlarmContainer> s_apInstance;
};

// -------------------------------------------------------------------------
#endif
