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

#ifndef	UWSDRFrame_H
#define	UWSDRFrame_H

#include <wx/wx.h>
#include <wx/socket.h>
#include <wx/tglbtn.h>

#include "SpectrumDisplay.h"
#include "SDRParameters.h"
#include "SDRControl.h"
#include "DialInterface.h"
#include "ControlInterface.h"
#include "DSPControl.h"
#include "VolumeDial.h"
#include "FreqDisplay.h"
#include "FreqDial.h"
#include "Frequency.h"
#include "InfoBox.h"
#include "SMeter.h"
#include "VoiceKeyboard.h"
#include "CWKeyboard.h"

class CUWSDRFrame : public wxFrame, public IDialInterface, public IControlInterface {
    public:
	CUWSDRFrame(const wxString& title);
	~CUWSDRFrame();

	void onMenuButton(wxCommandEvent& event);
	void onMenuSelection(wxCommandEvent& event);
	void onVFOButton(wxCommandEvent& event);
	void onMHzButton(wxCommandEvent& event);
	void onModeChoice(wxCommandEvent& event);
	void onFilterChoice(wxCommandEvent& event);
	void onMuteButton(wxCommandEvent& event);
	void onRITButton(wxCommandEvent& event);
	void onTXButton(wxCommandEvent& event);
	void onMouseWheel(wxMouseEvent& event);
	void onTimer(wxTimerEvent& event);
	void onClose(wxCloseEvent& event);

	void            setParameters(CSDRParameters* parameters);
	CSDRParameters* getParameters();

	void dialMoved(int id, int value);

	void sdrCommandNAK(int id);
	void sdrConnectionLost(int id);

	void sendCW(unsigned int speed, const wxString& text);
	void sendAudio(const wxString& fileName, int state);

    private:
	wxTimer           m_timer;
	CSDRParameters*   m_parameters;
	CDSPControl*      m_dsp;
	ISDRControl*      m_sdr;
	bool              m_rxOn;
	bool              m_txOn;
	double            m_stepSize;
	bool              m_record;
	float             m_offset;

	wxMenu*           m_menu;
	CFreqDisplay*     m_freqDisplay;
	CSpectrumDisplay* m_spectrumDisplay;
	CInfoBox*         m_infoBox;
	wxChoice*         m_mode;
	wxChoice*         m_filter;
	wxToggleButton*   m_ritCtrl;
	CVolumeDial*      m_rit;
	wxToggleButton*   m_mute;
	wxToggleButton*   m_transmit;
	CSMeter*          m_sMeter;
	CVolumeDial*      m_micGain;
	CVolumeDial*      m_power;
	CVolumeDial*      m_afGain;
	CVolumeDial*      m_squelch;

	float*            m_spectrum;

	CVoiceKeyboard*   m_voiceKeyboard;
	CCWKeyboard*      m_cwKeyboard;

	void      createMenu();
	wxButton* createMenuButton(wxWindow* window);
	wxSizer*  createVFOButtons(wxWindow* window);
	wxPanel*  createFreqDial(wxWindow* window);
	wxSizer*  createMHzButtons(wxWindow* window);
	wxSizer*  createModeChoice(wxWindow* window);
	wxSizer*  createFilterChoice(wxWindow* window);
	wxSizer*  createRITButton(wxWindow* window);
	wxSizer*  createMuteButton(wxWindow* window);
	wxSizer*  createRIT(wxWindow* window);
	wxSizer*  createTransmitButton(wxWindow* window);
	wxPanel*  createFreqDisplay(wxWindow* window);
	wxPanel*  createSpectrumDisplay(wxWindow* window);
	wxPanel*  createInfoBox(wxWindow* window);
	wxPanel*  createSMeter(wxWindow* window);
	wxSizer*  createMicGain(wxWindow* window);
	wxSizer*  createPowerControl(wxWindow* window);
	wxSizer*  createAFGain(wxWindow* window);
	wxSizer*  createSquelch(wxWindow* window);

	DECLARE_EVENT_TABLE()

	void freqChange(double value);
	void normaliseFreq();
	void normaliseMode();

	bool setTransmit();
};

#endif
