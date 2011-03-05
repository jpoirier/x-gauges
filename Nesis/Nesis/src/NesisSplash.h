#ifndef __NESIS_SPLASH_H
#define __NESIS_SPLASH_H

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

// -------------------------------------------------------------------------

class NesisSplash
{
// Q_OBJECT
protected:
	//! Constructor
	NesisSplash();
	//! Destructor
	~NesisSplash();

public:
	//! Return the only instance of this singelton class.
	static NesisSplash* GetInstance();

	//! Delete object.
	void Destroy();

	//! Set progress of the bootloader.
	void SetProgress(unsigned int uiProgress);

private:
	//! Pointer on instance of this class.
	static NesisSplash* s_pInstance;

	//! Current progress.
	unsigned int m_uiProgress;

	//! Proc file for setting the progress bar.
	QFile* m_fileSplash;
};

// -------------------------------------------------------------------------

#endif	// __NESIS_BROWSER_H
