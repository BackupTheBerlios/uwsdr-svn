/*
 *   Copyright (C) 2006-2008 by Jonathan Naylor G4KLX
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

#include "UWSDRController.h"

const unsigned int MAX_TRIES = 3U;


CUWSDRController::CUWSDRController(CUDPDataReader* reader, CUDPDataWriter* writer, unsigned int version) :
wxThread(),
m_reader(reader),
m_writer(writer),
m_callback(NULL),
m_version(version),
m_txFreq(),
m_rxFreq(),
m_enableTX(false),
m_enableRX(false),
m_tx(false),
m_clock(99999),
m_replies(false),
m_tries(0U),
m_connected(true),
m_commands(),
m_flag()
{
	wxASSERT(m_reader != NULL);
	wxASSERT(m_writer != NULL);
}

CUWSDRController::~CUWSDRController()
{
}

void CUWSDRController::setCallback(IControlInterface* callback)
{
	wxASSERT(callback != NULL);

	m_callback = callback;
}

bool CUWSDRController::open()
{
	bool ret = m_writer->open();
	if (!ret)
		return false;

	ret = m_reader->open();
	if (!ret)
		return false;

	m_reader->setCallback(this, 1);

	Create();
	Run();

	return true;
}

void CUWSDRController::enableTX(bool on)
{
	if (m_enableTX == on)
		return;

	switch (m_version) {
		case 1: {
				wxString command = on ? wxT("ET1;") : wxT("ET0;");
				m_commands.push_back(command);
				m_flag.Post();
			}
			break;
		default:
			wxASSERT(false);
			break;
	}

	m_enableTX = on;;
}

void CUWSDRController::enableRX(bool on)
{
	if (m_enableRX == on)
		return;

	switch (m_version) {
		case 1: {
				wxString command = on ? wxT("ER1;") : wxT("ER0;");
				m_commands.push_back(command);
				m_flag.Post();
			}
			break;
		default:
			wxASSERT(false);
			break;
	}

	m_enableRX = on;
}

void CUWSDRController::setTXAndFreq(bool transmit, const CFrequency& freq)
{
	if (transmit && m_tx && freq == m_txFreq)
		return;

	if (!transmit && !m_tx && freq == m_rxFreq)
		return;

	if (transmit && freq != m_txFreq) {
		switch (m_version) {
			case 1: {
					wxString command;
					command.Printf(wxT("FT%s;"), freq.getString().c_str());
					m_commands.push_back(command);
					m_flag.Post();
				}
				break;
			default:
				wxASSERT(false);
				break;
		}

		m_txFreq = freq;
	} else if (!transmit && freq != m_rxFreq) {
		switch (m_version) {
			case 1: {
					wxString command;
					command.Printf(wxT("FR%s;"), freq.getString().c_str());
					m_commands.push_back(command);
					m_flag.Post();
				}
				break;
			default:
				wxASSERT(false);
				break;
		}

		m_rxFreq = freq;
	}

	if (transmit == m_tx)
		return;

	switch (m_version) {
		case 1: {
				wxString command = transmit ? wxT("TX1;") : wxT("TX0;");
				m_commands.push_back(command);
				m_flag.Post();
			}
			break;
		default:
			wxASSERT(false);
			break;
	}

	m_tx = transmit;
}

void CUWSDRController::setClockTune(unsigned int clock)
{
	clock += 32768;

	if (clock == m_clock)
		return;

	switch (m_version) {
		case 1: {
				wxString command;
				command.Printf(wxT("CF%u;"), clock);
				m_commands.push_back(command);
				m_flag.Post();
			}
			break;
		default:
			wxASSERT(false);
			break;
	}

	m_clock = clock;
}

bool CUWSDRController::sendCommand(const wxString& command)
{
	bool ret = m_writer->write(command.c_str(), command.Length());
	if (!ret)
		::wxLogError(wxT("Error sending command to the SDR - ") + command);

	return ret;
}

void* CUWSDRController::Entry()
{
	wxASSERT(m_callback != NULL);

	while (!TestDestroy()) {
		wxSemaError ret = m_flag.WaitTimeout(100UL);

		switch (ret) {
			case wxSEMA_TIMEOUT:
				// No replies, so retransmit the last command
				if (m_commands.size() > 0U && !m_replies) {
					if (m_tries >= MAX_TRIES) {
						if (m_connected) {
							::wxLogError(wxT("No reply from the SDR for a command after %u tries"), MAX_TRIES);
							m_callback->connectionLost();
							m_connected = false;
						}
					} else {
						sendCommand(m_commands.front());
						m_tries++;
					}

					break;
				}

				// We have an Ack or Nak, clear the last command
				if (m_commands.size() > 0U && m_replies) {
					m_replies = false;
					m_tries   = 0U;

					m_commands.pop_front();

					if (m_commands.size() > 0U) {
						sendCommand(m_commands.front());
						m_tries = 1U;
					}

					break;
				}
				break;

			// Triggered by an incoming command
			case wxSEMA_NO_ERROR:
				if (m_commands.size() == 1U) {
					sendCommand(m_commands.front());
					m_replies = false;
					m_tries   = 1U;
				}
				break;

			default:
				break;
		}
	}

	m_reader->close();
	m_writer->close();

	return (void*)0;
}

void CUWSDRController::close()
{
	Delete();
}

bool CUWSDRController::callback(char* buffer, unsigned int len, int WXUNUSED(id))
{
	wxASSERT(m_callback != NULL);

	switch (m_version) {
		case 1:
			if (::strncmp(buffer, "AK", 2) == 0) {
				m_replies = true;
				buffer[len] = '\0';
				m_callback->commandAck(buffer);
				return true;
			} else if (::strncmp(buffer, "NK", 2) == 0) {
				m_replies = true;
				buffer[len] = '\0';
				m_callback->commandNak(buffer);
				return true;
			} else if (::strncmp(buffer, "DR", 2) != 0) {
				buffer[len] = '\0';
				m_callback->commandMisc(buffer);
				return true;
			}

			return false;

		default:
			return false;
	}
}
