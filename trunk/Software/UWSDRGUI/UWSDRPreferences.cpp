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

#include "UWSDRPreferences.h"
#include "UWSDRDefs.h"
#include "UWSDRApp.h"

const int SPIN_WIDTH   = 75;
const int SLIDER_WIDTH = 225;

BEGIN_EVENT_TABLE(CUWSDRPreferences, wxDialog)
	EVT_BUTTON(wxID_OK,   CUWSDRPreferences::onOK)
	EVT_BUTTON(wxID_HELP, CUWSDRPreferences::onHelp)
END_EVENT_TABLE()

CUWSDRPreferences::CUWSDRPreferences(wxWindow* parent, int id, CSDRParameters* parameters) :
wxDialog(parent, id, wxString(_("�Wave SDR Preferences"))),
m_parameters(parameters),
m_maxRXFreq(NULL),
m_minRXFreq(NULL),
m_maxTXFreq(NULL),
m_minTXFreq(NULL),
m_shift(NULL),
m_agcAM(NULL),
m_agcSSB(NULL),
m_agcCW(NULL),
m_filterFM(NULL),
m_filterAM(NULL),
m_filterSSB(NULL),
m_filterCWW(NULL),
m_filterCWN(NULL),
m_tuningFM(NULL),
m_tuningAM(NULL),
m_tuningSSB(NULL),
m_tuningCWW(NULL),
m_tuningCWN(NULL),
m_stepVeryFast(NULL),
m_stepFast(NULL),
m_stepMedium(NULL),
m_stepSlow(NULL),
m_stepVerySlow(NULL),
m_nbButton(NULL),
m_nbValue(NULL),
m_nb2Button(NULL),
m_nb2Value(NULL),
m_spButton(NULL),
m_spValue(NULL),
m_rxIQPhase(NULL),
m_rxIQGain(NULL),
m_txIQPhase(NULL),
m_txIQGain(NULL)
{
	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

	wxNotebook* noteBook = new wxNotebook(this, -1);

	noteBook->AddPage(createFrequencyTab(noteBook), _("Frequencies"), true);

	noteBook->AddPage(createShiftTab(noteBook), _("Shift"), false);

	noteBook->AddPage(createModeTab(noteBook), _("Modes"), false);

	noteBook->AddPage(createStepTab(noteBook), _("Step Size"), false);

	noteBook->AddPage(createRXDSPTab(noteBook), _("RX DSP"), false);

	noteBook->AddPage(createTXDSPTab(noteBook), _("TX DSP"), false);

	noteBook->AddPage(createIQTab(noteBook), _("I + Q"), false);

	mainSizer->Add(noteBook, 1, wxALL | wxGROW, BORDER_SIZE);

	mainSizer->Add(CreateButtonSizer(wxHELP | wxOK | wxCANCEL), 0, wxALL | wxALIGN_RIGHT, BORDER_SIZE);

	SetAutoLayout(true);
	Layout();

	mainSizer->Fit(this);
	mainSizer->SetSizeHints(this);

	SetSizer(mainSizer);

	wxString text;
	text.Printf(wxT("%u"), m_parameters->m_shift / 1000);
	m_shift->SetValue(text);

	m_minRXFreq->SetValue(m_parameters->m_minReceiveFreq.getString());
	m_maxRXFreq->SetValue(m_parameters->m_maxReceiveFreq.getString());
	m_minTXFreq->SetValue(m_parameters->m_minTransmitFreq.getString());
	m_maxTXFreq->SetValue(m_parameters->m_maxTransmitFreq.getString());

	m_agcAM->SetSelection(m_parameters->m_agcAM);
	m_agcSSB->SetSelection(m_parameters->m_agcSSB);
	m_agcCW->SetSelection(m_parameters->m_agcCW);

	m_filterFM->SetSelection(m_parameters->m_filterFM);
	m_filterAM->SetSelection(m_parameters->m_filterAM);
	m_filterSSB->SetSelection(m_parameters->m_filterSSB);
	m_filterCWW->SetSelection(m_parameters->m_filterCWW);
	m_filterCWN->SetSelection(m_parameters->m_filterCWN);

	m_tuningFM->SetSelection(m_parameters->m_vfoSpeedFM);
	m_tuningAM->SetSelection(m_parameters->m_vfoSpeedAM);
	m_tuningSSB->SetSelection(m_parameters->m_vfoSpeedSSB);
	m_tuningCWW->SetSelection(m_parameters->m_vfoSpeedCWW);
	m_tuningCWN->SetSelection(m_parameters->m_vfoSpeedCWN);

	text.Printf(wxT("%.1f"), m_parameters->m_stepVeryFast);
	m_stepVeryFast->SetValue(text);
	text.Printf(wxT("%.1f"), m_parameters->m_stepFast);
	m_stepFast->SetValue(text);
	text.Printf(wxT("%.1f"), m_parameters->m_stepMedium);
	m_stepMedium->SetValue(text);
	text.Printf(wxT("%.1f"), m_parameters->m_stepSlow);
	m_stepSlow->SetValue(text);
	text.Printf(wxT("%.1f"), m_parameters->m_stepVerySlow);
	m_stepVerySlow->SetValue(text);

	m_nbButton->SetValue(m_parameters->m_nbOn);
	m_nbValue->SetValue(m_parameters->m_nbValue);
	m_nb2Button->SetValue(m_parameters->m_nb2On);
	m_nb2Value->SetValue(m_parameters->m_nb2Value);

	m_spButton->SetValue(m_parameters->m_spOn);
	m_spValue->SetValue(m_parameters->m_spValue);

	m_rxIQPhase->SetValue(m_parameters->m_rxIQphase);
	m_rxIQGain->SetValue(m_parameters->m_rxIQgain);
	m_txIQPhase->SetValue(m_parameters->m_txIQphase);
	m_txIQGain->SetValue(m_parameters->m_txIQgain);
}

CUWSDRPreferences::~CUWSDRPreferences()
{
}

void CUWSDRPreferences::onOK(wxCommandEvent& event)
{
	wxString text = m_shift->GetValue();

	if (text.IsEmpty()) {
		::wxMessageBox(_("The shift may not empty."));
		return;
	}

	int shift = ::atoi(text.c_str());

	if (shift < 0) {
		::wxMessageBox(_("The shift may not be negative."));
		return;
	}

	text = m_minRXFreq->GetValue();

	if (text.IsEmpty()) {
		::wxMessageBox(_("The minimum receive frequency may not empty."));
		return;
	}

	CFrequency minRXFreq(text);

	if (minRXFreq < m_parameters->m_minHardwareFreq ||
	    minRXFreq > m_parameters->m_maxHardwareFreq) {
		::wxMessageBox(_("The minimum receive frequency may not be outside the\nrange of the hardware."));
		return;
	}

	text = m_maxRXFreq->GetValue();

	if (text.IsEmpty()) {
		::wxMessageBox(_("The maximum receive frequency may not empty."));
		return;
	}

	CFrequency maxRXFreq(text);

	if (maxRXFreq < m_parameters->m_minHardwareFreq ||
	    maxRXFreq > m_parameters->m_maxHardwareFreq) {
		::wxMessageBox(_("The maximum receive frequency may not be outside the\nrange of the hardware."));
		return;
	}

	if (maxRXFreq <= minRXFreq) {
		::wxMessageBox(_("The maximum receive frequency must be higher than the\nminimum receive frequency."));
		return;
	}

	text = m_minTXFreq->GetValue();

	if (text.IsEmpty()) {
		::wxMessageBox(_("The minimum transmit frequency may not empty."));
		return;
	}

	CFrequency minTXFreq(text);

	if (minTXFreq < minRXFreq ||
	    minTXFreq > maxRXFreq) {
		::wxMessageBox(_("The minimum transmit frequency may not be outside the\nrange of the receive frequencies."));
		return;
	}

	text = m_maxTXFreq->GetValue();

	if (text.IsEmpty()) {
		::wxMessageBox(_("The maximum transmit frequency may not empty."));
		return;
	}

	CFrequency maxTXFreq(text);

	if (maxTXFreq < minRXFreq ||
	    maxTXFreq > maxRXFreq) {
		::wxMessageBox(_("The maximum transmit frequency may not be outside the\nrange of the receive frequencies."));
		return;
	}

	if (maxTXFreq <= minTXFreq) {
		::wxMessageBox(_("The maximum transmit frequency must be higher than the\nminimum transmit frequency."));
		return;
	}

	text = m_stepVeryFast->GetValue();

	if (text.IsEmpty()) {
		::wxMessageBox(_("The Very fast step size may not empty."));
		return;
	}

	double stepVeryFast = ::atof(text.c_str());

	if (stepVeryFast <= 0.0) {
		::wxMessageBox(_("The Very fast step size may not be zero or negative."));
		return;
	}

	text = m_stepFast->GetValue();

	if (text.IsEmpty()) {
		::wxMessageBox(_("The Fast step size may not empty."));
		return;
	}

	int stepFast = ::atof(text.c_str());

	if (stepFast <= 0.0) {
		::wxMessageBox(_("The Fast step size may not be zero or negative."));
		return;
	}

	text = m_stepMedium->GetValue();

	if (text.IsEmpty()) {
		::wxMessageBox(_("The Medium step size may not empty."));
		return;
	}

	int stepMedium = ::atof(text.c_str());

	if (stepMedium <= 0.0) {
		::wxMessageBox(_("The Medium step size may not be zero or negative."));
		return;
	}

	text = m_stepSlow->GetValue();

	if (text.IsEmpty()) {
		::wxMessageBox(_("The Slow step size may not empty."));
		return;
	}

	int stepSlow = ::atof(text.c_str());

	if (stepSlow <= 0.0) {
		::wxMessageBox(_("The Slow step size may not be zero or negative."));
		return;
	}

	text = m_stepVerySlow->GetValue();

	if (text.IsEmpty()) {
		::wxMessageBox(_("The Very slow step size may not empty."));
		return;
	}

	int stepVerySlow = ::atof(text.c_str());

	if (stepVerySlow <= 0.0) {
		::wxMessageBox(_("The Very slow step size may not be zero or negative."));
		return;
	}

	m_parameters->m_shift = shift * 1000;

	m_parameters->m_minReceiveFreq  = minRXFreq;
	m_parameters->m_maxReceiveFreq  = maxRXFreq;
	m_parameters->m_minTransmitFreq = minTXFreq;
	m_parameters->m_maxTransmitFreq = maxTXFreq;

	m_parameters->m_agcAM  = m_agcAM->GetSelection();
	m_parameters->m_agcSSB = m_agcSSB->GetSelection();
	m_parameters->m_agcCW  = m_agcCW->GetSelection();

	m_parameters->m_filterFM  = m_filterFM->GetSelection();
	m_parameters->m_filterAM  = m_filterAM->GetSelection();
	m_parameters->m_filterSSB = m_filterSSB->GetSelection();
	m_parameters->m_filterCWW = m_filterCWW->GetSelection();
	m_parameters->m_filterCWN = m_filterCWN->GetSelection();

	m_parameters->m_vfoSpeedFM  = m_tuningFM->GetSelection();
	m_parameters->m_vfoSpeedAM  = m_tuningAM->GetSelection();
	m_parameters->m_vfoSpeedSSB = m_tuningSSB->GetSelection();
	m_parameters->m_vfoSpeedCWW = m_tuningCWW->GetSelection();
	m_parameters->m_vfoSpeedCWN = m_tuningCWN->GetSelection();

	m_parameters->m_stepVeryFast = stepVeryFast;
	m_parameters->m_stepFast     = stepFast;
	m_parameters->m_stepMedium   = stepMedium;
	m_parameters->m_stepSlow     = stepSlow;
	m_parameters->m_stepVerySlow = stepVerySlow;

	m_parameters->m_nbOn     = m_nbButton->IsChecked();
	m_parameters->m_nbValue  = m_nbValue->GetValue();
	m_parameters->m_nb2On    = m_nb2Button->IsChecked();
	m_parameters->m_nb2Value = m_nb2Value->GetValue();

	m_parameters->m_spOn      = m_spButton->IsChecked();
	m_parameters->m_spValue   = m_spValue->GetValue();

	m_parameters->m_rxIQphase = m_rxIQPhase->GetValue();
	m_parameters->m_rxIQgain  = m_rxIQGain->GetValue();
	m_parameters->m_txIQphase = m_txIQPhase->GetValue();
	m_parameters->m_txIQgain  = m_txIQGain->GetValue();

	wxDialog::OnOK(event);
}

void CUWSDRPreferences::onHelp(wxCommandEvent& event)
{
	::wxGetApp().showHelp(_("Preferences"));
}

wxPanel* CUWSDRPreferences::createFrequencyTab(wxNotebook* noteBook)
{
	wxPanel* panel = new wxPanel(noteBook, -1);

	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

	wxStaticText* label = new wxStaticText(panel, -1,
		_("These are the highest and lowest transmit and receive frequencies that the SDR\n"
		    "may use. They may not be outside of the limits set for the hardware. The\n"
			"transmit frequency range must be within the receive frequency range."));
	mainSizer->Add(label, 0, wxALL, BORDER_SIZE);

	wxFlexGridSizer* sizer = new wxFlexGridSizer(2);

	wxStaticText* label1 = new wxStaticText(panel, -1, _("Minimum Receive Frequency (MHz)"));
	sizer->Add(label1, 0, wxALL, BORDER_SIZE);

	m_minRXFreq = new wxTextCtrl(panel, -1);
	sizer->Add(m_minRXFreq, 0, wxALL, BORDER_SIZE);

	wxStaticText* label2 = new wxStaticText(panel, -1, _("Maximum Receive Frequency (MHz)"));
	sizer->Add(label2, 0, wxALL, BORDER_SIZE);

	m_maxRXFreq = new wxTextCtrl(panel, -1);
	sizer->Add(m_maxRXFreq, 0, wxALL, BORDER_SIZE);

	wxStaticText* label3 = new wxStaticText(panel, -1, _("Minimum Transmit Frequency (MHz)"));
	sizer->Add(label3, 0, wxALL, BORDER_SIZE);

	m_minTXFreq = new wxTextCtrl(panel, -1);
	sizer->Add(m_minTXFreq, 0, wxALL, BORDER_SIZE);

	wxStaticText* label4 = new wxStaticText(panel, -1, _("Maximum Transmit Frequency (MHz)"));
	sizer->Add(label4, 0, wxALL, BORDER_SIZE);

	m_maxTXFreq = new wxTextCtrl(panel, -1);
	sizer->Add(m_maxTXFreq, 0, wxALL, BORDER_SIZE);

	mainSizer->Add(sizer, 0, wxALL, BORDER_SIZE);

	panel->SetAutoLayout(true);

	sizer->Fit(panel);
	sizer->SetSizeHints(panel);

	panel->SetSizer(mainSizer);

	return panel;
}

wxPanel* CUWSDRPreferences::createShiftTab(wxNotebook* noteBook)
{
	wxPanel* panel = new wxPanel(noteBook, -1);

	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

	wxStaticText* label = new wxStaticText(panel, -1,
		_("This is the frequency shift that is invoked by the Shift + and Shift - buttons on\n"
		    "the main panel."));
	mainSizer->Add(label, 0, wxALL, BORDER_SIZE);

	wxFlexGridSizer* sizer = new wxFlexGridSizer(2);

	wxStaticText* label1 = new wxStaticText(panel, -1, _("Shift (kHz)"));
	sizer->Add(label1, 0, wxALL, BORDER_SIZE);

	m_shift = new wxTextCtrl(panel, -1);
	sizer->Add(m_shift, 0, wxALL, BORDER_SIZE);

	mainSizer->Add(sizer, 0, wxALL, BORDER_SIZE);

	panel->SetAutoLayout(true);

	sizer->Fit(panel);
	sizer->SetSizeHints(panel);

	panel->SetSizer(mainSizer);

	return panel;
}

wxPanel* CUWSDRPreferences::createModeTab(wxNotebook* noteBook)
{
	wxPanel* panel = new wxPanel(noteBook, -1);

	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

	wxStaticText* label = new wxStaticText(panel, -1,
		_("Set the filter bandwidth, tuning rate and AGC speed for each mode. The AGC\n"
		    "speed for CW Wide also applies to CW Narrow."));
	mainSizer->Add(label, 0, wxALL, BORDER_SIZE);

	wxFlexGridSizer* sizer = new wxFlexGridSizer(4);

	wxStaticText* labelA = new wxStaticText(panel, -1, _("Mode"));
	sizer->Add(labelA, 0, wxALL, BORDER_SIZE);

	wxStaticText* labelB = new wxStaticText(panel, -1, _("Filter"));
	sizer->Add(labelB, 0, wxALL, BORDER_SIZE);

	wxStaticText* labelC = new wxStaticText(panel, -1, _("Tuning"));
	sizer->Add(labelC, 0, wxALL, BORDER_SIZE);

	wxStaticText* labelD = new wxStaticText(panel, -1, _("AGC"));
	sizer->Add(labelD, 0, wxALL, BORDER_SIZE);

	wxStaticText* label2 = new wxStaticText(panel, -1, _("FM"));
	sizer->Add(label2, 0, wxALL, BORDER_SIZE);

	m_filterFM = createFilterChoice(panel);
	sizer->Add(m_filterFM, 0, wxALL, BORDER_SIZE);

 	m_tuningFM = createTuningChoice(panel);
	sizer->Add(m_tuningFM, 0, wxALL, BORDER_SIZE);

	wxStaticText* dummy2 = new wxStaticText(panel, -1, wxEmptyString);
	sizer->Add(dummy2, 0, wxALL, BORDER_SIZE);

	wxStaticText* label3 = new wxStaticText(panel, -1, _("AM"));
	sizer->Add(label3, 0, wxALL, BORDER_SIZE);

	m_filterAM = createFilterChoice(panel);
	sizer->Add(m_filterAM, 0, wxALL, BORDER_SIZE);

	m_tuningAM = createTuningChoice(panel);
	sizer->Add(m_tuningAM, 0, wxALL, BORDER_SIZE);

	m_agcAM = createAGCChoice(panel);
	sizer->Add(m_agcAM, 0, wxALL, BORDER_SIZE);

	wxStaticText* label4 = new wxStaticText(panel, -1, _("SSB"));
	sizer->Add(label4, 0, wxALL, BORDER_SIZE);

	m_filterSSB = createFilterChoice(panel);
	sizer->Add(m_filterSSB, 0, wxALL, BORDER_SIZE);

	m_tuningSSB = createTuningChoice(panel);
	sizer->Add(m_tuningSSB, 0, wxALL, BORDER_SIZE);

	m_agcSSB = createAGCChoice(panel);
	sizer->Add(m_agcSSB, 0, wxALL, BORDER_SIZE);

	wxStaticText* label5 = new wxStaticText(panel, -1, _("CW Wide"));
	sizer->Add(label5, 0, wxALL, BORDER_SIZE);

	m_filterCWW = createFilterChoice(panel);
	sizer->Add(m_filterCWW, 0, wxALL, BORDER_SIZE);

	m_tuningCWW = createTuningChoice(panel);
	sizer->Add(m_tuningCWW, 0, wxALL, BORDER_SIZE);

	m_agcCW = createAGCChoice(panel);
	sizer->Add(m_agcCW, 0, wxALL, BORDER_SIZE);

	wxStaticText* label6 = new wxStaticText(panel, -1, _("CW Narrow"));
	sizer->Add(label6, 0, wxALL, BORDER_SIZE);

	m_filterCWN = createFilterChoice(panel);
	sizer->Add(m_filterCWN, 0, wxALL, BORDER_SIZE);

	m_tuningCWN = createTuningChoice(panel);
	sizer->Add(m_tuningCWN, 0, wxALL, BORDER_SIZE);

	mainSizer->Add(sizer, 0, wxALL, BORDER_SIZE);

	panel->SetAutoLayout(true);

	sizer->Fit(panel);
	sizer->SetSizeHints(panel);

	panel->SetSizer(mainSizer);

	return panel;
}

wxChoice* CUWSDRPreferences::createAGCChoice(wxPanel* panel)
{
	wxChoice* agc = new wxChoice(panel, -1, wxDefaultPosition, wxSize(CONTROL_WIDTH, -1));

	agc->Append(_("Fast"));
	agc->Append(_("Medium"));
	agc->Append(_("Slow"));
	agc->Append(_("None"));

	return agc;
}

wxChoice* CUWSDRPreferences::createFilterChoice(wxPanel* panel)
{
	wxChoice* filter = new wxChoice(panel, -1, wxDefaultPosition, wxSize(CONTROL_WIDTH, -1));

	filter->Append(wxT("10.0 kHz"));
	filter->Append(wxT("6.0 kHz"));
	filter->Append(wxT("4.0 kHz"));
	filter->Append(wxT("2.6 kHz"));
	filter->Append(wxT("2.1 kHz"));
	filter->Append(wxT("1.0 kHz"));
	filter->Append(wxT("500 Hz"));
	filter->Append(wxT("250 Hz"));
	filter->Append(wxT("100 Hz"));
	filter->Append(wxT("50 Hz"));
	filter->Append(wxT("25 Hz"));

	return filter;
}

wxChoice* CUWSDRPreferences::createTuningChoice(wxPanel* panel)
{
	wxChoice* tuning = new wxChoice(panel, -1, wxDefaultPosition, wxSize(CONTROL_WIDTH, -1));

	tuning->Append(_("Very fast"));
	tuning->Append(_("Fast"));
	tuning->Append(_("Medium"));
	tuning->Append(_("Slow"));
	tuning->Append(_("Very slow"));

	return tuning;
}

wxPanel* CUWSDRPreferences::createStepTab(wxNotebook* noteBook)
{
	wxPanel* panel = new wxPanel(noteBook, -1);

	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

	wxStaticText* label = new wxStaticText(panel, -1,
		_("These are the tuning step sizes for the VFO knob. Using the concentric bands,\n"
		    "these can be increased by 1x, 4x, and 9x. The actual tuning step size used for\n"
		    "each mode is set in the Modes tab."));
	mainSizer->Add(label, 0, wxALL, BORDER_SIZE);

	wxFlexGridSizer* sizer = new wxFlexGridSizer(2);

	wxStaticText* label1 = new wxStaticText(panel, -1, _("Very fast (Hz/step)"));
	sizer->Add(label1, 0, wxALL, BORDER_SIZE);

	m_stepVeryFast = new wxTextCtrl(panel, -1);
	sizer->Add(m_stepVeryFast, 0, wxALL, BORDER_SIZE);

	wxStaticText* label2 = new wxStaticText(panel, -1, _("Fast (Hz/step)"));
	sizer->Add(label2, 0, wxALL, BORDER_SIZE);

	m_stepFast = new wxTextCtrl(panel, -1);
	sizer->Add(m_stepFast, 0, wxALL, BORDER_SIZE);

	wxStaticText* label3 = new wxStaticText(panel, -1, _("Medium (Hz/step)"));
	sizer->Add(label3, 0, wxALL, BORDER_SIZE);

	m_stepMedium = new wxTextCtrl(panel, -1);
	sizer->Add(m_stepMedium, 0, wxALL, BORDER_SIZE);

	wxStaticText* label4 = new wxStaticText(panel, -1, _("Slow (Hz/step)"));
	sizer->Add(label4, 0, wxALL, BORDER_SIZE);

	m_stepSlow = new wxTextCtrl(panel, -1);
	sizer->Add(m_stepSlow, 0, wxALL, BORDER_SIZE);

	wxStaticText* label5 = new wxStaticText(panel, -1, _("Very slow (Hz/step)"));
	sizer->Add(label5, 0, wxALL, BORDER_SIZE);

	m_stepVerySlow = new wxTextCtrl(panel, -1);
	sizer->Add(m_stepVerySlow, 0, wxALL, BORDER_SIZE);

	mainSizer->Add(sizer, 0, wxALL, BORDER_SIZE);

	panel->SetAutoLayout(true);

	sizer->Fit(panel);
	sizer->SetSizeHints(panel);

	panel->SetSizer(mainSizer);

	return panel;
}

wxPanel* CUWSDRPreferences::createRXDSPTab(wxNotebook* noteBook)
{
	wxPanel* panel = new wxPanel(noteBook, -1);

	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

	wxStaticText* label = new wxStaticText(panel, -1,
		_("The DSP includes two advanced notch blankers, they can be enabled here, and\n"
		  "their detection threshold values set."));
	mainSizer->Add(label, 0, wxALL, BORDER_SIZE);

	wxFlexGridSizer* sizer = new wxFlexGridSizer(3);

	wxStaticText* nbLabel = new wxStaticText(panel, -1, _("Impulse noise blanker"));
	sizer->Add(nbLabel, 0, wxALL, BORDER_SIZE);

	m_nbButton = new wxCheckBox(panel, -1, wxEmptyString);
	sizer->Add(m_nbButton, 0, wxALL, BORDER_SIZE);

	m_nbValue = new wxSlider(panel, -1, 1, 1, 200, wxDefaultPosition, wxSize(SLIDER_WIDTH, -1));
	sizer->Add(m_nbValue, 0, wxALL, BORDER_SIZE);

	wxStaticText* nb2Label = new wxStaticText(panel, -1, _("Mean noise blanker"));
	sizer->Add(nb2Label, 0, wxALL, BORDER_SIZE);

	m_nb2Button = new wxCheckBox(panel, -1, wxEmptyString);
	sizer->Add(m_nb2Button, 0, wxALL, BORDER_SIZE);

	m_nb2Value = new wxSlider(panel, -1, 1, 1, 1000, wxDefaultPosition, wxSize(SLIDER_WIDTH, -1));
	sizer->Add(m_nb2Value, 0, wxALL, BORDER_SIZE);

	mainSizer->Add(sizer, 0, wxALL, BORDER_SIZE);

	panel->SetAutoLayout(true);

	sizer->Fit(panel);
	sizer->SetSizeHints(panel);

	panel->SetSizer(mainSizer);

	return panel;
}

wxPanel* CUWSDRPreferences::createTXDSPTab(wxNotebook* noteBook)
{
	wxPanel* panel = new wxPanel(noteBook, -1);

	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

	wxStaticText* label = new wxStaticText(panel, -1,
		_("The DSP is capable of processing the transmitted signal to increase its\n"
		  "average power."));
	mainSizer->Add(label, 0, wxALL, BORDER_SIZE);

	wxFlexGridSizer* sizer = new wxFlexGridSizer(3);

	wxStaticText* spLabel = new wxStaticText(panel, -1, _("Speech processor (dB)"));
	sizer->Add(spLabel, 0, wxALL, BORDER_SIZE);

	m_spButton = new wxCheckBox(panel, -1, wxEmptyString);
	sizer->Add(m_spButton, 0, wxALL, BORDER_SIZE);

	m_spValue = new wxSlider(panel, -1, 0, 0, 20, wxDefaultPosition, wxSize(SLIDER_WIDTH, -1));
	sizer->Add(m_spValue, 0, wxALL, BORDER_SIZE);

	mainSizer->Add(sizer, 0, wxALL, BORDER_SIZE);

	panel->SetAutoLayout(true);

	sizer->Fit(panel);
	sizer->SetSizeHints(panel);

	panel->SetSizer(mainSizer);

	return panel;
}

wxPanel* CUWSDRPreferences::createIQTab(wxNotebook* noteBook)
{
	wxPanel* panel = new wxPanel(noteBook, -1);

	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

	wxStaticText* label = new wxStaticText(panel, -1,
		_("For optimum performance the I and Q elements of the receive and transmit\n"
		  "signal need to be balanced. These controls allow them to be changed."));
	mainSizer->Add(label, 0, wxALL, BORDER_SIZE);

	wxFlexGridSizer* sizer = new wxFlexGridSizer(4);

	wxStaticText* label1 = new wxStaticText(panel, -1, _("Receive phase"));
	sizer->Add(label1, 0, wxALL, BORDER_SIZE);

	m_rxIQPhase = new wxSpinCtrl(panel, -1, wxEmptyString, wxDefaultPosition, wxSize(SPIN_WIDTH, -1));
	m_rxIQPhase->SetRange(-400, 400);
	sizer->Add(m_rxIQPhase, 0, wxALL, BORDER_SIZE);

	wxStaticText* label3 = new wxStaticText(panel, -1, _("Transmit phase"));
	sizer->Add(label3, 0, wxALL, BORDER_SIZE);

	m_txIQPhase = new wxSpinCtrl(panel, -1, wxEmptyString, wxDefaultPosition, wxSize(SPIN_WIDTH, -1));
	m_txIQPhase->SetRange(-400, 400);
	sizer->Add(m_txIQPhase, 0, wxALL, BORDER_SIZE);

	wxStaticText* label2 = new wxStaticText(panel, -1, _("Receive gain"));
	sizer->Add(label2, 0, wxALL, BORDER_SIZE);

	m_rxIQGain = new wxSpinCtrl(panel, -1, wxEmptyString, wxDefaultPosition, wxSize(SPIN_WIDTH, -1));
	m_rxIQGain->SetRange(-500, 500);
	sizer->Add(m_rxIQGain, 0, wxALL, BORDER_SIZE);

	wxStaticText* label4 = new wxStaticText(panel, -1, _("Transmit gain"));
	sizer->Add(label4, 0, wxALL, BORDER_SIZE);

	m_txIQGain = new wxSpinCtrl(panel, -1, wxEmptyString, wxDefaultPosition, wxSize(SPIN_WIDTH, -1));
	m_txIQGain->SetRange(-500, 500);
	sizer->Add(m_txIQGain, 0, wxALL, BORDER_SIZE);

	mainSizer->Add(sizer, 0, wxALL, BORDER_SIZE);

	panel->SetAutoLayout(true);

	sizer->Fit(panel);
	sizer->SetSizeHints(panel);

	panel->SetSizer(mainSizer);

	return panel;
}
