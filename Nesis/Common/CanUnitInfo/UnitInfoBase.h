#ifndef UNIT_INFO_BASE_H
#define UNIT_INFO_BASE_H

/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2008 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   Licence: GPL - GNU General Public License                             *
 *            See 'COPYING.html' for more details about the license.       *
 *                                                                         *
 ***************************************************************************/

#include <QObject>
#include <QVector>
#include <QTime>
#include <QDomElement>

#include "Matrix/Matrix.h"
#include "JulianDay.h"
#include "CanAerospace/AbstractAerospaceDataManager.h"

// ------------------------------------------------------------------------

//#define TAG_VERSION					"version"

// ------------------------------------------------------------------------

namespace can {

// ------------------------------------------------------------------------

class UnitInfoBase
{
public:
	//! Constructor
	UnitInfoBase(uint8_t byNodeId=0, uint8_t byHwId=0, 
		uint32_t uiSerial=0, common::JulianDay jdDate=common::JulianDay());
	//! Copy constructor
	UnitInfoBase(const UnitInfoBase& C);
	//! Destructor
   virtual ~UnitInfoBase();

	//! Return Node id.
	uint8_t GetNodeId() const { return m_byNodeId; }
	//! Set new node id.
	void SetNodeId(uint8_t byNewId)
	{ m_byNodeId = byNewId; }

	//! Return Original node id.
	uint8_t GetHardwareId() const { return m_byHardwareId; }
	//! Set original node id.
	void SetHardwareId(uint8_t byHWId)
	{ m_byHardwareId = byHWId; }

	//! Return serial number.
	uint32_t GetSerial() const { return m_uiSerial; }
	//! Set serial number
	void SetSerial(uint32_t uiSerial)
	{ m_uiSerial = uiSerial; }

	//! Set the date in JulianDay format.
	void SetDate(common::JulianDay jdDate)
	{ m_jdDate = jdDate; }
	//! Return date when serial number was burned.
	common::JulianDay GetDate() const { return m_jdDate; }

	//! Set version (it containes both, sw and hw version)
	void SetVersion(uint16_t uiVersions);
	//! Get hardware version.
	uint8_t GetHardwareVersion() const
	{ return m_byHWVersion; }
	//! Get software version.
	uint8_t GetSoftwareVersion() const
	{ return m_bySWVersion; }
	//! Get hardware version formated as string (x.x).
	QString GetHardwareVersionString() const;
	//! Get software version formated as string (x.x).
	QString GetSoftwareVersionString() const;
	
	//! Get vendor number - this is a kind of cryptic id.
	uint32_t GetVendorNumber() const
	{ return m_uiVendorNumber; }
	//! Set vendor number.
	void SetVendorNumber(uint32_t uiVN)
	{ m_uiVendorNumber = uiVN; }
	
	//! Set SVN revision.
	void SetSVN(uint32_t uiSVN)
	{ m_uiSVN = uiSVN; }
	//! Get SVN revision. This is from CAN.
	uint32_t GetSVN() const
	{ return m_uiSVN; }
	
	//! Update the sign of life status. We may also request some info.
	virtual void UpdateSignOfLife(AbstractAerospaceDataManager *pADM);
	//! Time since the last sign of life call.
	int GetTimeSinceSignOfLife() const;
	
	//! Is it a valid object?
	virtual bool IsValid() const;

	//! Static wait function.
	static void Wait(can::AbstractAerospaceDataManager* m_pADM, qint32 msToWait);

	#if defined (USE_CAN_DUS_A)
	//! Store uploading message.
	bool StoreUploadMessage(const can::MessageAerospace& msg);
	#endif 

	#if defined (USE_CAN_DDS_A)
	//! Do the download service for this node.
	virtual bool Download(can::AbstractAerospaceDataManager* pADM, 
		uint32_t uiDwnDataId, const QVector<float>& vData);
	
	//! Get proper value from the download buffer.
	virtual bool GetDownloadData(uint8_t byIndex, can::DataType& eDataType, can::Register& rData);
	#endif
	
	//! Derive classes may process responce from the Module Configuration Service.
	virtual void ProcessModuleConfigResponse(const can::MessageAerospace& msg);
	//! Process MIS response
	virtual void ProcessModuleInfoResponse(const can::MessageAerospace& msg);

protected:
	#if defined (USE_CAN_DUS_A)
	//! The upload service which takes vector of floats.
	bool Upload(can::AbstractAerospaceDataManager* pADM, uint32_t uiUplDataId, QVector<float>& vData, int iSize=0);
	//! Do the upload service. The receiving buffer must be set by the caller.
	virtual bool Upload(can::AbstractAerospaceDataManager* pADM, uint32_t uiUplDataId);
	#endif 

	#if defined (USE_CAN_DDS_A)
	//! Do the download service for this node.
	virtual bool Download(can::AbstractAerospaceDataManager* pADM, uint32_t uiDwnDataId);
	#endif

protected:
	//! Node id
	uint8_t m_byNodeId;
	//! Original node id
	uint8_t m_byHardwareId;
	//! Date when the serial was created.
	common::JulianDay m_jdDate;
	//! Serial number (0 means no number).
	uint32_t m_uiSerial;
	
	//! Hardware version
	uint8_t m_byHWVersion;
	//! Software version
	uint8_t m_bySWVersion;
	//! Vendor number
	uint32_t m_uiVendorNumber;
	//! SVN revision
	uint32_t m_uiSVN;
	
	//! Upload transfer buffer
	QVector<can::Register> m_vUploadBuffer;
	//! Download transfer buffer
	QVector<can::Register> m_vDownloadBuffer;
	//! Current download data type
	can::DataType m_eDataType;
	
	//! Time of the last "SignOfLive" message.
	QTime m_tLastSOL;
};

// --------------------------------------------------------------------------

} // namespace

#endif
