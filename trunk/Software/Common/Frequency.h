/*
 *   Copyright (C) 2006 by Jonathan Naylor G4KLX
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef	Frequency_H
#define	Frequency_H

#include <wx/wx.h>

class CFrequency {

    public:
	CFrequency(unsigned int mhz, double hz);
	CFrequency(const CFrequency& frequency);
	CFrequency(const wxString& freq);
	CFrequency();
	virtual ~CFrequency();

	virtual void addMHz(int mhz);
	virtual void addHz(double hz);

	virtual bool setFrequency(const wxString& freq);

	virtual void setHz(double hz);
	virtual void setMHz(unsigned int mhz);

	virtual wxString     getString(unsigned int decimals = 8) const;
	virtual unsigned int getMHz() const;
	virtual double       getHz() const;

	CFrequency operator+(double hz)
	{
		CFrequency temp(*this);
		temp.addHz(hz);
		return temp;
	}

	CFrequency operator-(double hz)
	{
		CFrequency temp(*this);
		temp.addHz(-hz);
		return temp;
	}

	CFrequency operator+(const CFrequency& freq)
	{
		unsigned int mhz = this->m_mhz + freq.m_mhz;
		double        hz = this->m_hz  + freq.m_hz;

		if (hz > 1000000.0) {
			hz  -= 1000000.0;
			mhz += 1;
		}

		CFrequency temp(mhz, hz);
		return temp;
	}

	CFrequency operator-(const CFrequency& freq)
	{
		unsigned int mhz = (this->m_mhz - 1) - freq.m_mhz;
		double        hz = (this->m_hz + 1000000.0) - freq.m_hz;

		if (hz > 1000000.0) {
			hz  -= 1000000.0;
			mhz += 1;
		}

		CFrequency temp(mhz, hz);
		return temp;
	}

	CFrequency& operator+=(double hz)
	{
		this->addHz(hz);
		return *this;
	}

	CFrequency& operator-=(double hz)
	{
		this->addHz(-hz);
		return *this;
	}		

	CFrequency& operator=(const CFrequency& frequency)
	{
		this->setMHz(frequency.getMHz());
		this->setHz(frequency.getHz());
		return *this;
	}

	bool operator==(const CFrequency& freq) const
	{
		return freq.m_mhz == this->m_mhz && freq.m_hz == this->m_hz;
	}

	bool operator!=(const CFrequency& freq) const
	{
		return freq.m_mhz != this->m_mhz || freq.m_hz != this->m_hz;
	}

	bool operator<(const CFrequency& freq) const
	{
		return freq.m_mhz > this->m_mhz || (freq.m_mhz == this->m_mhz && freq.m_hz > this->m_hz);
	}

	bool operator<=(const CFrequency& freq) const
	{
		return freq.m_mhz > this->m_mhz || (freq.m_mhz == this->m_mhz && freq.m_hz >= this->m_hz);
	}

	bool operator>(const CFrequency& freq) const
	{
		return freq.m_mhz < this->m_mhz || (freq.m_mhz == this->m_mhz && freq.m_hz < this->m_hz);
	}

	bool operator>=(const CFrequency& freq) const
	{
		return freq.m_mhz < this->m_mhz || (freq.m_mhz == this->m_mhz && freq.m_hz <= this->m_hz);
	}

    private:
	unsigned int m_mhz;
	double       m_hz;
};

#endif
