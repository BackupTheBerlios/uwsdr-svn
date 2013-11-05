/*
 *   Copyright (C) 2013 by Jonathan Naylor G4KLX
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

#include "FrequencyRangeArray.h"

CFrequencyRange::CFrequencyRange(const wxString& start, const wxString& stop) :
m_start(),
m_stop()
{
	m_start.set(start);
	m_stop.set(stop);
}

CFrequencyRange::~CFrequencyRange()
{
}

bool CFrequencyRange::inRange(const CFrequency& freq) const
{
	return freq >= m_start && freq <= m_stop;
}

CFrequencyRangeArray::CFrequencyRangeArray() :
m_range()
{
}

CFrequencyRangeArray::~CFrequencyRangeArray()
{
	// for (std::vector<CFrequencyRange*>::const_iterator it = m_range.begin(); it != m_range.end(); ++it)
	//	delete (*it);
}

void CFrequencyRangeArray::addRange(const wxString& start, const wxString& stop)
{
	CFrequencyRange* range = new CFrequencyRange(start, stop);

	m_range.push_back(range);
}

bool CFrequencyRangeArray::inRange(const CFrequency& freq) const
{
	for (std::vector<CFrequencyRange*>::const_iterator it = m_range.begin(); it != m_range.end(); ++it) {
		if ((*it)->inRange(freq))
			return true;
	}

	return false;
}

bool CFrequencyRangeArray::isEmpty() const
{
	return m_range.empty();
}

unsigned int CFrequencyRangeArray::getCount() const
{
	return m_range.size();
}
