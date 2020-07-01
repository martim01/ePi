#include "../include/dlgInfo.h"

//(*InternalHeaders(dlgInfo)
#include <wx/font.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(dlgInfo)
const long dlgInfo::ID_STATICTEXT1 = wxNewId();
const long dlgInfo::ID_STATICTEXT2 = wxNewId();
const long dlgInfo::ID_STATICTEXT3 = wxNewId();
const long dlgInfo::ID_STATICTEXT4 = wxNewId();
const long dlgInfo::ID_STATICTEXT5 = wxNewId();
const long dlgInfo::ID_STATICTEXT36 = wxNewId();
const long dlgInfo::ID_STATICTEXT6 = wxNewId();
const long dlgInfo::ID_STATICTEXT37 = wxNewId();
const long dlgInfo::ID_STATICTEXT7 = wxNewId();
const long dlgInfo::ID_STATICTEXT38 = wxNewId();
const long dlgInfo::ID_STATICTEXT8 = wxNewId();
const long dlgInfo::ID_STATICTEXT39 = wxNewId();
const long dlgInfo::ID_STATICTEXT9 = wxNewId();
const long dlgInfo::ID_STATICTEXT40 = wxNewId();
const long dlgInfo::ID_STATICTEXT10 = wxNewId();
const long dlgInfo::ID_STATICTEXT41 = wxNewId();
const long dlgInfo::ID_STATICTEXT11 = wxNewId();
const long dlgInfo::ID_STATICTEXT42 = wxNewId();
const long dlgInfo::ID_STATICTEXT12 = wxNewId();
const long dlgInfo::ID_STATICTEXT43 = wxNewId();
const long dlgInfo::ID_STATICTEXT13 = wxNewId();
const long dlgInfo::ID_STATICTEXT44 = wxNewId();
const long dlgInfo::ID_STATICTEXT14 = wxNewId();
const long dlgInfo::ID_STATICTEXT45 = wxNewId();
const long dlgInfo::ID_STATICTEXT15 = wxNewId();
const long dlgInfo::ID_STATICTEXT46 = wxNewId();
const long dlgInfo::ID_STATICTEXT16 = wxNewId();
const long dlgInfo::ID_STATICTEXT47 = wxNewId();
const long dlgInfo::ID_STATICTEXT17 = wxNewId();
const long dlgInfo::ID_STATICTEXT48 = wxNewId();
const long dlgInfo::ID_STATICTEXT18 = wxNewId();
const long dlgInfo::ID_STATICTEXT21 = wxNewId();
const long dlgInfo::ID_STATICTEXT19 = wxNewId();
const long dlgInfo::ID_STATICTEXT22 = wxNewId();
const long dlgInfo::ID_STATICTEXT20 = wxNewId();
const long dlgInfo::ID_STATICTEXT23 = wxNewId();
const long dlgInfo::ID_STATICTEXT24 = wxNewId();
const long dlgInfo::ID_STATICTEXT49 = wxNewId();
const long dlgInfo::ID_STATICTEXT25 = wxNewId();
const long dlgInfo::ID_STATICTEXT50 = wxNewId();
const long dlgInfo::ID_STATICTEXT26 = wxNewId();
const long dlgInfo::ID_STATICTEXT51 = wxNewId();
const long dlgInfo::ID_STATICTEXT27 = wxNewId();
const long dlgInfo::ID_STATICTEXT52 = wxNewId();
const long dlgInfo::ID_STATICTEXT28 = wxNewId();
const long dlgInfo::ID_STATICTEXT53 = wxNewId();
const long dlgInfo::ID_STATICTEXT29 = wxNewId();
const long dlgInfo::ID_STATICTEXT54 = wxNewId();
const long dlgInfo::ID_STATICTEXT32 = wxNewId();
const long dlgInfo::ID_STATICTEXT30 = wxNewId();
const long dlgInfo::ID_STATICTEXT33 = wxNewId();
const long dlgInfo::ID_STATICTEXT31 = wxNewId();
const long dlgInfo::ID_STATICTEXT34 = wxNewId();
const long dlgInfo::ID_STATICTEXT55 = wxNewId();
const long dlgInfo::ID_STATICTEXT35 = wxNewId();
const long dlgInfo::ID_STATICTEXT56 = wxNewId();
const long dlgInfo::ID_BUTTON1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(dlgInfo,wxDialog)
	//(*EventTable(dlgInfo)
	//*)
END_EVENT_TABLE()

dlgInfo::dlgInfo(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(dlgInfo)
	wxBoxSizer* BoxSizer1;
	wxBoxSizer* BoxSizer2;
	wxBoxSizer* BoxSizer3;
	wxFlexGridSizer* FlexGridSizer1;
	wxFlexGridSizer* FlexGridSizer2;
	wxFlexGridSizer* FlexGridSizer3;
	wxFlexGridSizer* FlexGridSizer4;
	wxFlexGridSizer* FlexGridSizer5;
	wxFlexGridSizer* FlexGridSizer6;
	wxFlexGridSizer* FlexGridSizer7;
	wxStaticBoxSizer* StaticBoxSizer1;
	wxStaticBoxSizer* StaticBoxSizer2;
	wxStaticBoxSizer* StaticBoxSizer3;
	wxStaticBoxSizer* StaticBoxSizer4;
	wxStaticBoxSizer* StaticBoxSizer5;
	wxStaticBoxSizer* StaticBoxSizer6;
	wxStaticBoxSizer* StaticBoxSizer7;

	Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxNO_BORDER, _T("id"));
	SetClientSize(wxDefaultSize);
	Move(wxDefaultPosition);
	SetBackgroundColour(wxColour(255,255,255));
	BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
	BoxSizer2 = new wxBoxSizer(wxVERTICAL);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, _("Application"));
	FlexGridSizer1 = new wxFlexGridSizer(0, 8, 0, 0);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Time:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	wxFont StaticText1Font(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,wxEmptyString,wxFONTENCODING_DEFAULT);
	StaticText1->SetFont(StaticText1Font);
	FlexGridSizer1->Add(StaticText1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	m_plblApplicationTime = new wxStaticText(this, ID_STATICTEXT2, _("2020-07-01T12:47:30"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	FlexGridSizer1->Add(m_plblApplicationTime, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxFIXED_MINSIZE, 2);
	StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("Start Time:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	wxFont StaticText3Font(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,wxEmptyString,wxFONTENCODING_DEFAULT);
	StaticText3->SetFont(StaticText3Font);
	FlexGridSizer1->Add(StaticText3, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	m_plblApplicationlStartTime = new wxStaticText(this, ID_STATICTEXT4, _("2020-07-01T10:30:30"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	FlexGridSizer1->Add(m_plblApplicationlStartTime, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	StaticText5 = new wxStaticText(this, ID_STATICTEXT5, _("Up Time:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
	wxFont StaticText5Font(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,wxEmptyString,wxFONTENCODING_DEFAULT);
	StaticText5->SetFont(StaticText5Font);
	FlexGridSizer1->Add(StaticText5, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	m_plblApplicationUptime = new wxStaticText(this, ID_STATICTEXT36, _("12:00:20"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT36"));
	FlexGridSizer1->Add(m_plblApplicationUptime, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	StaticText6 = new wxStaticText(this, ID_STATICTEXT6, _("Version:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT6"));
	wxFont StaticText6Font(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,wxEmptyString,wxFONTENCODING_DEFAULT);
	StaticText6->SetFont(StaticText6Font);
	FlexGridSizer1->Add(StaticText6, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	m_plblApplicationVersion = new wxStaticText(this, ID_STATICTEXT37, _("0.1.0.567"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT37"));
	FlexGridSizer1->Add(m_plblApplicationVersion, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	StaticBoxSizer1->Add(FlexGridSizer1, 0, wxALL|wxEXPAND, 0);
	BoxSizer2->Add(StaticBoxSizer1, 0, wxALL|wxEXPAND, 2);
	StaticBoxSizer2 = new wxStaticBoxSizer(wxVERTICAL, this, _("CPU"));
	FlexGridSizer2 = new wxFlexGridSizer(0, 10, 0, 0);
	StaticText7 = new wxStaticText(this, ID_STATICTEXT7, _("Overall:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT7"));
	wxFont StaticText7Font(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,wxEmptyString,wxFONTENCODING_DEFAULT);
	StaticText7->SetFont(StaticText7Font);
	FlexGridSizer2->Add(StaticText7, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	m_plblCPUOverall = new wxStaticText(this, ID_STATICTEXT38, _("0.1.0.567"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT38"));
	FlexGridSizer2->Add(m_plblCPUOverall, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	StaticText8 = new wxStaticText(this, ID_STATICTEXT8, _("CPU 1:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT8"));
	wxFont StaticText8Font(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,wxEmptyString,wxFONTENCODING_DEFAULT);
	StaticText8->SetFont(StaticText8Font);
	FlexGridSizer2->Add(StaticText8, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	m_plblCPU0 = new wxStaticText(this, ID_STATICTEXT39, _("0.1.0.567"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT39"));
	FlexGridSizer2->Add(m_plblCPU0, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	StaticText9 = new wxStaticText(this, ID_STATICTEXT9, _("CPU 2:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT9"));
	wxFont StaticText9Font(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,wxEmptyString,wxFONTENCODING_DEFAULT);
	StaticText9->SetFont(StaticText9Font);
	FlexGridSizer2->Add(StaticText9, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	m_plblCPU1 = new wxStaticText(this, ID_STATICTEXT40, _("0.1.0.567"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT40"));
	FlexGridSizer2->Add(m_plblCPU1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	StaticText10 = new wxStaticText(this, ID_STATICTEXT10, _("CPU 3:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT10"));
	wxFont StaticText10Font(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,wxEmptyString,wxFONTENCODING_DEFAULT);
	StaticText10->SetFont(StaticText10Font);
	FlexGridSizer2->Add(StaticText10, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	m_plblCPU2 = new wxStaticText(this, ID_STATICTEXT41, _("0.1.0.567"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT41"));
	FlexGridSizer2->Add(m_plblCPU2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	StaticText11 = new wxStaticText(this, ID_STATICTEXT11, _("CPU 4:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT11"));
	wxFont StaticText11Font(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,wxEmptyString,wxFONTENCODING_DEFAULT);
	StaticText11->SetFont(StaticText11Font);
	FlexGridSizer2->Add(StaticText11, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	m_plblCPU3 = new wxStaticText(this, ID_STATICTEXT42, _("0.1.0.567"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT42"));
	FlexGridSizer2->Add(m_plblCPU3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	StaticBoxSizer2->Add(FlexGridSizer2, 1, wxALL|wxEXPAND, 0);
	BoxSizer2->Add(StaticBoxSizer2, 0, wxALL|wxEXPAND, 2);
	StaticBoxSizer3 = new wxStaticBoxSizer(wxVERTICAL, this, _("Disk"));
	FlexGridSizer3 = new wxFlexGridSizer(0, 6, 0, 0);
	StaticText12 = new wxStaticText(this, ID_STATICTEXT12, _("Total Bytes:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT12"));
	wxFont StaticText12Font(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,wxEmptyString,wxFONTENCODING_DEFAULT);
	StaticText12->SetFont(StaticText12Font);
	FlexGridSizer3->Add(StaticText12, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	m_plblBytesTotal = new wxStaticText(this, ID_STATICTEXT43, _("0.1.0.567"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT43"));
	FlexGridSizer3->Add(m_plblBytesTotal, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	StaticText13 = new wxStaticText(this, ID_STATICTEXT13, _("Free Bytes:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT13"));
	wxFont StaticText13Font(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,wxEmptyString,wxFONTENCODING_DEFAULT);
	StaticText13->SetFont(StaticText13Font);
	FlexGridSizer3->Add(StaticText13, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	m_plblBytesFree = new wxStaticText(this, ID_STATICTEXT44, _("0.1.0.567"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT44"));
	FlexGridSizer3->Add(m_plblBytesFree, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	StaticText14 = new wxStaticText(this, ID_STATICTEXT14, _("Available Bytes:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT14"));
	wxFont StaticText14Font(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,wxEmptyString,wxFONTENCODING_DEFAULT);
	StaticText14->SetFont(StaticText14Font);
	FlexGridSizer3->Add(StaticText14, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	m_plblBytesAvailable = new wxStaticText(this, ID_STATICTEXT45, _("0.1.0.567"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT45"));
	FlexGridSizer3->Add(m_plblBytesAvailable, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	StaticText15 = new wxStaticText(this, ID_STATICTEXT15, _("Total Inodes:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT15"));
	wxFont StaticText15Font(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,wxEmptyString,wxFONTENCODING_DEFAULT);
	StaticText15->SetFont(StaticText15Font);
	FlexGridSizer3->Add(StaticText15, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	m_plblInodesTotal = new wxStaticText(this, ID_STATICTEXT46, _("0.1.0.567"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT46"));
	FlexGridSizer3->Add(m_plblInodesTotal, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	StaticText16 = new wxStaticText(this, ID_STATICTEXT16, _("Free Inodes:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT16"));
	wxFont StaticText16Font(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,wxEmptyString,wxFONTENCODING_DEFAULT);
	StaticText16->SetFont(StaticText16Font);
	FlexGridSizer3->Add(StaticText16, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	m_plblInodesFree = new wxStaticText(this, ID_STATICTEXT47, _("0.1.0.567"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT47"));
	FlexGridSizer3->Add(m_plblInodesFree, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	StaticText17 = new wxStaticText(this, ID_STATICTEXT17, _("Available Inodes"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT17"));
	wxFont StaticText17Font(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,wxEmptyString,wxFONTENCODING_DEFAULT);
	StaticText17->SetFont(StaticText17Font);
	FlexGridSizer3->Add(StaticText17, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	m_plblInodesAvailable = new wxStaticText(this, ID_STATICTEXT48, _("0.1.0.567"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT48"));
	FlexGridSizer3->Add(m_plblInodesAvailable, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	StaticBoxSizer3->Add(FlexGridSizer3, 1, wxALL|wxEXPAND, 0);
	BoxSizer2->Add(StaticBoxSizer3, 0, wxALL|wxEXPAND, 2);
	StaticBoxSizer4 = new wxStaticBoxSizer(wxVERTICAL, this, _("Loads"));
	FlexGridSizer4 = new wxFlexGridSizer(0, 6, 0, 0);
	StaticText18 = new wxStaticText(this, ID_STATICTEXT18, _("1 Minute:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT18"));
	wxFont StaticText18Font(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,wxEmptyString,wxFONTENCODING_DEFAULT);
	StaticText18->SetFont(StaticText18Font);
	FlexGridSizer4->Add(StaticText18, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	m_plblLoads1 = new wxStaticText(this, ID_STATICTEXT21, _("0.1.0.567"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT21"));
	FlexGridSizer4->Add(m_plblLoads1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	StaticText19 = new wxStaticText(this, ID_STATICTEXT19, _("5 Minute:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT19"));
	wxFont StaticText19Font(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,wxEmptyString,wxFONTENCODING_DEFAULT);
	StaticText19->SetFont(StaticText19Font);
	FlexGridSizer4->Add(StaticText19, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	m_plblLoads5 = new wxStaticText(this, ID_STATICTEXT22, _("0.1.0.567"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT22"));
	FlexGridSizer4->Add(m_plblLoads5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	StaticText20 = new wxStaticText(this, ID_STATICTEXT20, _("15 Minute:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT20"));
	wxFont StaticText20Font(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,wxEmptyString,wxFONTENCODING_DEFAULT);
	StaticText20->SetFont(StaticText20Font);
	FlexGridSizer4->Add(StaticText20, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	m_plblLoads15 = new wxStaticText(this, ID_STATICTEXT23, _("0.1.0.567"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT23"));
	FlexGridSizer4->Add(m_plblLoads15, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	StaticBoxSizer4->Add(FlexGridSizer4, 1, wxALL|wxEXPAND, 0);
	BoxSizer2->Add(StaticBoxSizer4, 0, wxALL|wxEXPAND, 2);
	StaticBoxSizer5 = new wxStaticBoxSizer(wxVERTICAL, this, _("Memory"));
	FlexGridSizer5 = new wxFlexGridSizer(0, 8, 0, 0);
	StaticText24 = new wxStaticText(this, ID_STATICTEXT24, _("Total:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT24"));
	wxFont StaticText24Font(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,wxEmptyString,wxFONTENCODING_DEFAULT);
	StaticText24->SetFont(StaticText24Font);
	FlexGridSizer5->Add(StaticText24, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	m_plblRamTotal = new wxStaticText(this, ID_STATICTEXT49, _("0.1.0.567"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT49"));
	FlexGridSizer5->Add(m_plblRamTotal, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	StaticText25 = new wxStaticText(this, ID_STATICTEXT25, _("Buffered:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT25"));
	wxFont StaticText25Font(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,wxEmptyString,wxFONTENCODING_DEFAULT);
	StaticText25->SetFont(StaticText25Font);
	FlexGridSizer5->Add(StaticText25, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	m_plblRamBuffered = new wxStaticText(this, ID_STATICTEXT50, _("0.1.0.567"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT50"));
	FlexGridSizer5->Add(m_plblRamBuffered, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	StaticText26 = new wxStaticText(this, ID_STATICTEXT26, _("Shared:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT26"));
	wxFont StaticText26Font(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,wxEmptyString,wxFONTENCODING_DEFAULT);
	StaticText26->SetFont(StaticText26Font);
	FlexGridSizer5->Add(StaticText26, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	m_plblRamShared = new wxStaticText(this, ID_STATICTEXT51, _("0.1.0.567"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT51"));
	FlexGridSizer5->Add(m_plblRamShared, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	StaticText27 = new wxStaticText(this, ID_STATICTEXT27, _("Free:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT27"));
	wxFont StaticText27Font(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,wxEmptyString,wxFONTENCODING_DEFAULT);
	StaticText27->SetFont(StaticText27Font);
	FlexGridSizer5->Add(StaticText27, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	m_plblRamFree = new wxStaticText(this, ID_STATICTEXT52, _("0.1.0.567"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT52"));
	FlexGridSizer5->Add(m_plblRamFree, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	StaticText28 = new wxStaticText(this, ID_STATICTEXT28, _("Total High:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT28"));
	wxFont StaticText28Font(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,wxEmptyString,wxFONTENCODING_DEFAULT);
	StaticText28->SetFont(StaticText28Font);
	FlexGridSizer5->Add(StaticText28, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	m_plblHighTotal = new wxStaticText(this, ID_STATICTEXT53, _("0.1.0.567"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT53"));
	FlexGridSizer5->Add(m_plblHighTotal, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	StaticText29 = new wxStaticText(this, ID_STATICTEXT29, _("Free High:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT29"));
	wxFont StaticText29Font(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,wxEmptyString,wxFONTENCODING_DEFAULT);
	StaticText29->SetFont(StaticText29Font);
	FlexGridSizer5->Add(StaticText29, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	m_plblHighFree = new wxStaticText(this, ID_STATICTEXT54, _("0.1.0.567"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT54"));
	FlexGridSizer5->Add(m_plblHighFree, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	StaticBoxSizer5->Add(FlexGridSizer5, 1, wxALL|wxEXPAND, 0);
	BoxSizer2->Add(StaticBoxSizer5, 0, wxALL|wxEXPAND, 2);
	StaticBoxSizer6 = new wxStaticBoxSizer(wxVERTICAL, this, _("Swap"));
	FlexGridSizer6 = new wxFlexGridSizer(0, 4, 0, 0);
	StaticText32 = new wxStaticText(this, ID_STATICTEXT32, _("Total:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT32"));
	wxFont StaticText32Font(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,wxEmptyString,wxFONTENCODING_DEFAULT);
	StaticText32->SetFont(StaticText32Font);
	FlexGridSizer6->Add(StaticText32, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	m_plblSwapTotal = new wxStaticText(this, ID_STATICTEXT30, _("0.1.0.567"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT30"));
	FlexGridSizer6->Add(m_plblSwapTotal, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	StaticText33 = new wxStaticText(this, ID_STATICTEXT33, _("Free:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT33"));
	wxFont StaticText33Font(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,wxEmptyString,wxFONTENCODING_DEFAULT);
	StaticText33->SetFont(StaticText33Font);
	FlexGridSizer6->Add(StaticText33, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	m_plblSwapFree = new wxStaticText(this, ID_STATICTEXT31, _("0.1.0.567"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT31"));
	FlexGridSizer6->Add(m_plblSwapFree, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	StaticBoxSizer6->Add(FlexGridSizer6, 1, wxALL|wxEXPAND, 0);
	BoxSizer2->Add(StaticBoxSizer6, 0, wxALL|wxEXPAND, 2);
	StaticBoxSizer7 = new wxStaticBoxSizer(wxVERTICAL, this, _("System"));
	FlexGridSizer7 = new wxFlexGridSizer(0, 4, 0, 0);
	StaticText34 = new wxStaticText(this, ID_STATICTEXT34, _("Up Time:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT34"));
	wxFont StaticText34Font(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,wxEmptyString,wxFONTENCODING_DEFAULT);
	StaticText34->SetFont(StaticText34Font);
	FlexGridSizer7->Add(StaticText34, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	m_plblSystemUpTime = new wxStaticText(this, ID_STATICTEXT55, _("0.1.0.567"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT55"));
	FlexGridSizer7->Add(m_plblSystemUpTime, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	StaticText35 = new wxStaticText(this, ID_STATICTEXT35, _("Running Procs:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT35"));
	wxFont StaticText35Font(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,wxEmptyString,wxFONTENCODING_DEFAULT);
	StaticText35->SetFont(StaticText35Font);
	FlexGridSizer7->Add(StaticText35, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	m_plblSystemProcs = new wxStaticText(this, ID_STATICTEXT56, _("0.1.0.567"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT56"));
	FlexGridSizer7->Add(m_plblSystemProcs, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	StaticBoxSizer7->Add(FlexGridSizer7, 1, wxALL|wxEXPAND, 0);
	BoxSizer2->Add(StaticBoxSizer7, 0, wxALL|wxEXPAND, 2);
	BoxSizer1->Add(BoxSizer2, 1, wxALL|wxEXPAND, 0);
	BoxSizer3 = new wxBoxSizer(wxVERTICAL);
	m_pbtnClose = new wxButton(this, ID_BUTTON1, _("Close"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	BoxSizer3->Add(m_pbtnClose, 0, wxALL|wxEXPAND, 0);
	BoxSizer1->Add(BoxSizer3, 0, wxALL|wxALIGN_BOTTOM, 2);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	//*)
}

dlgInfo::~dlgInfo()
{
	//(*Destroy(dlgInfo)
	//*)
}


void dlgInfo::OnbtnCloseClick(wxCommandEvent& event)
{
    EndModal(wxID_OK);
}
