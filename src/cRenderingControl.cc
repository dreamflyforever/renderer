//
// File: cRenderingControl.cc
// Created by: Julian Flake <julian.flake@cs.uni-dortmund.de>
// Created on: Fri Apr 28 11:19:52 2006
//

#include "cRenderingControl.h"
#include "cRenderer.h"

cRenderingControl::cRenderingControl(void* lnkcRenderer) :cService(lnkcRenderer)
{
	service_id = "urn:upnp-org:serviceId:RenderingControl1";
	var_count = 7;
	action_count = 36;
	
	name[RC_Mute] = "Mute";
	value[RC_Mute] = "0";

	name[RC_LastChange] = "LastChange";
	value[RC_LastChange] = "<Event xmlns=\"urn:schemas-upnp-org:metadata-1-0/RCS/\"></Event>";

	name[RC_A_ARG_TYPE_InstanceID] = "A_ARG_TYPE_InstanceID";
	value[RC_A_ARG_TYPE_InstanceID] = "";

	name[RC_A_ARG_TYPE_PresetName] = "A_ARG_TYPE_PresetName";
	value[RC_A_ARG_TYPE_PresetName] = "";

	name[RC_PresetNameList] = "PresetNameList";
	value[RC_PresetNameList] = "";

	name[RC_Volume] = "Volume";
	value[RC_Volume] = "75";

	name[RC_A_ARG_TYPE_Channel] = "A_ARG_TYPE_Channel";
	value[RC_A_ARG_TYPE_Channel] = "";
	
	action_name[0]="ListPresets";
	action_name[1]="SelectPreset";
	action_name[2]="GetBrightness";
	action_name[3]="SetBrightness";
	action_name[4]="GetContrast";
	action_name[5]="SetContrast";
	action_name[6]="GetSharpness";
	action_name[7]="Setsharpness";
	action_name[8]="GetRedVideoGain";
	action_name[9]="SetRedVideoGain";
	action_name[10]="GetGreenVideoGain";
	action_name[11]="SetGreenVideoGain";
	action_name[12]="GetBlueVideoGain";
	action_name[13]="SetBlueVideoGain";
	action_name[14]="GetRedVideoBlackLevel";
	action_name[15]="SetRedVideoBlackLevel";
	action_name[16]="GetGreenVideoBlackLevel";
	action_name[17]="SetGreenVideoBlackLevel";
	action_name[18]="GetBlueVideoBlackLevel";
	action_name[19]="SetBlueVideoBlackLevel";
	action_name[20]="GetColorTemperature";
	action_name[21]="SetColorTemperature";
	action_name[22]="GetHorizontalKeystone";
	action_name[23]="SetHorizontalKeystone";
	action_name[24]="GetVerticalKeystone";
	action_name[25]="SetVerticalKeystone";
	action_name[26]="GetMute";
	action_name[27]="SetMute";
	action_name[28]="GetVolume";
	action_name[29]="SetVolume";
	action_name[30]="GetVolumeDB";
	action_name[31]="SetVolumeDB";
	action_name[32]="GetVolumeDBRange";
	action_name[33]="GetLoudness";
	action_name[34]="SetLoudness";
	action_name[35]="SetPlayMode";
	
	
	actions[0]=&cRenderingControl::ListPresets;
	actions[1]=&cRenderingControl::SelectPreset;
	actions[2]=&cRenderingControl::GetBrightness;
	actions[3]=&cRenderingControl::SetBrightness;
	actions[4]=&cRenderingControl::GetContrast;
	actions[5]=&cRenderingControl::SetContrast;
	actions[6]=&cRenderingControl::GetSharpness;
	actions[7]=&cRenderingControl::Setsharpness;
	actions[8]=&cRenderingControl::GetRedVideoGain;
	actions[9]=&cRenderingControl::SetRedVideoGain;
	actions[10]=&cRenderingControl::GetGreenVideoGain;
	actions[11]=&cRenderingControl::SetGreenVideoGain;
	actions[12]=&cRenderingControl::GetBlueVideoGain;
	actions[13]=&cRenderingControl::SetBlueVideoGain;
	actions[14]=&cRenderingControl::GetRedVideoBlackLevel;
	actions[15]=&cRenderingControl::SetRedVideoBlackLevel;
	actions[16]=&cRenderingControl::GetGreenVideoBlackLevel;
	actions[17]=&cRenderingControl::SetGreenVideoBlackLevel;
	actions[18]=&cRenderingControl::GetBlueVideoBlackLevel;
	actions[19]=&cRenderingControl::SetBlueVideoBlackLevel;
	actions[20]=&cRenderingControl::GetColorTemperature;
	actions[21]=&cRenderingControl::SetColorTemperature;
	actions[22]=&cRenderingControl::GetHorizontalKeystone;
	actions[23]=&cRenderingControl::SetHorizontalKeystone;
	actions[24]=&cRenderingControl::GetVerticalKeystone;
	actions[25]=&cRenderingControl::SetVerticalKeystone;
	actions[26]=&cRenderingControl::GetMute;
	actions[27]=&cRenderingControl::SetMute;
	actions[28]=&cRenderingControl::GetVolume;
	actions[29]=&cRenderingControl::SetVolume;
	actions[30]=&cRenderingControl::GetVolumeDB;
	actions[31]=&cRenderingControl::SetVolumeDB;
	actions[32]=&cRenderingControl::GetVolumeDBRange;
	actions[33]=&cRenderingControl::GetLoudness;
	actions[34]=&cRenderingControl::SetLoudness;
	actions[35]=&cRenderingControl::SetPlayMode;
	
	/*setVar(NULL,1,4,
				RC_LastChange,"-1000",
				RC_Volume,"1000"
				);*/
/*	setVar(NULL,1,4,
				RC_Volume,"2000",
				RC_Mute,"-2000"
				);
*/
	//exit(0);
}


cRenderingControl::~cRenderingControl() { }

int cRenderingControl::callAction(int i, IXML_Document* in, IXML_Document** out, Upnp_Action_Request* Event)
{
	return (this->*actions[i])(in,out,Event);
}


/*
 * Actions
 */
int cRenderingControl::ListPresets(IXML_Document * in, IXML_Document ** out,Upnp_Action_Request *Event)
{
	cout << "ListPresets" << endl;
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,service_id,"CurrentPresetNameList",value[RC_PresetNameList]);
	return 0;
}


int cRenderingControl::SelectPreset(IXML_Document * in, IXML_Document ** out,Upnp_Action_Request *Event)
{
	cout << "SelectPreset" << endl;
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,service_id,NULL,NULL);
	return 0;
}

// not implemented
int cRenderingControl::GetBrightness(IXML_Document * in, IXML_Document ** out,Upnp_Action_Request *Event)
{
	cout << "GetBrightnesst" << endl;
	return 0;
}

// not implemented
int cRenderingControl::SetBrightness(IXML_Document * in, IXML_Document ** out,Upnp_Action_Request *Event)
{
	cout << "SetBrightness" << endl;
	return 0;
}

// not implemented
int cRenderingControl::GetContrast(IXML_Document * in, IXML_Document ** out,Upnp_Action_Request *Event)
{
	cout << "GetContrast" << endl;
	return 0;
}

// not implemented
int cRenderingControl::SetContrast(IXML_Document * in, IXML_Document ** out,Upnp_Action_Request *Event)
{
	cout << "SetContrast" << endl;
	return 0;
}

// not implemented
int cRenderingControl::GetSharpness(IXML_Document * in, IXML_Document ** out,Upnp_Action_Request *Event)
{
	cout << "GetSharpness" << endl;
	return 0;
}

// not implemented
int cRenderingControl::Setsharpness(IXML_Document * in, IXML_Document ** out,Upnp_Action_Request *Event)
{
	cout << "Setsharpness" << endl;
	return 0;
}

// not implemented
int cRenderingControl::GetRedVideoGain(IXML_Document * in, IXML_Document ** out,Upnp_Action_Request *Event)
{
	cout << "GetRedVideoGain" << endl;
	return 0;
}

// not implemented
int cRenderingControl::SetRedVideoGain(IXML_Document * in, IXML_Document ** out,Upnp_Action_Request *Event)
{
	cout << "SetRedVideoGain" << endl;
	return 0;
}

// not implemented
int cRenderingControl::GetGreenVideoGain(IXML_Document * in, IXML_Document ** out,Upnp_Action_Request *Event)
{
	cout << "GetGreenVideoGain" << endl;
	return 0;
}

// not implemented
int cRenderingControl::SetGreenVideoGain(IXML_Document * in, IXML_Document ** out,Upnp_Action_Request *Event)
{
	cout << "SetGreenVideoGain" << endl;
	return 0;
}

// not implemented
int cRenderingControl::GetBlueVideoGain(IXML_Document * in, IXML_Document ** out,Upnp_Action_Request *Event)
{
	cout << "GetBlueVideoGain" << endl;
	return 0;
}

// not implemented
int cRenderingControl::SetBlueVideoGain(IXML_Document * in, IXML_Document ** out,Upnp_Action_Request *Event)
{
	cout << "SetBlueVideoGain" << endl;
	return 0;
}

// not implemented
int cRenderingControl::GetRedVideoBlackLevel(IXML_Document * in, IXML_Document ** out,Upnp_Action_Request *Event)
{
	cout << "GetRedVideoBlackLevel" << endl;
	return 0;
}

// not implemented
int cRenderingControl::SetRedVideoBlackLevel(IXML_Document * in, IXML_Document ** out,Upnp_Action_Request *Event)
{
	cout << "SetRedVideoBlackLevel" << endl;
	return 0;
}

// not implemented
int cRenderingControl::GetGreenVideoBlackLevel(IXML_Document * in, IXML_Document ** out,Upnp_Action_Request *Event)
{
	cout << "GetGreenVideoBlackLevel" << endl;
	return 0;
}

// not implemented
int cRenderingControl::SetGreenVideoBlackLevel(IXML_Document * in, IXML_Document ** out,Upnp_Action_Request *Event)
{
	cout << "SetGreenVideoBlackLevel" << endl;
	return 0;
}

// not implemented
int cRenderingControl::GetBlueVideoBlackLevel(IXML_Document * in, IXML_Document ** out,Upnp_Action_Request *Event)
{
	cout << "GetBlueVideoBlackLevel" << endl;
	return 0;
}

// not implemented
int cRenderingControl::SetBlueVideoBlackLevel(IXML_Document * in, IXML_Document ** out,Upnp_Action_Request *Event)
{
	cout << "SetBlueVideoBlackLeve" << endl;
	return 0;
}

// not implemented
int cRenderingControl::GetColorTemperature(IXML_Document * in, IXML_Document ** out,Upnp_Action_Request *Event)
{
	cout << "GetColorTemperature" << endl;
	return 0;
}

// not implemented
int cRenderingControl::SetColorTemperature(IXML_Document * in, IXML_Document ** out,Upnp_Action_Request *Event)
{
	cout << "SetColorTemperature" << endl;
	return 0;
}

// not implemented
int cRenderingControl::GetHorizontalKeystone(IXML_Document * in, IXML_Document ** out,Upnp_Action_Request *Event)
{
	cout << "GetHorizontalKeystone" << endl;
	return 0;
}

// not implemented
int cRenderingControl::SetHorizontalKeystone(IXML_Document * in, IXML_Document ** out,Upnp_Action_Request *Event)
{
	cout << "SetHorizontalKeystone" << endl;
	return 0;
}

// not implemented
int cRenderingControl::GetVerticalKeystone(IXML_Document * in, IXML_Document ** out,Upnp_Action_Request *Event)
{
	cout << "GetVerticalKeystone" << endl;
	return 0;
}

// not implemented
int cRenderingControl::SetVerticalKeystone(IXML_Document * in, IXML_Document ** out,Upnp_Action_Request *Event)
{
	cout << "SetVerticalKeystone" << endl;
	return 0;
}


int cRenderingControl::GetMute(IXML_Document * in, IXML_Document ** out,Upnp_Action_Request *Event)
{
	cout << "GetMute" << endl;
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,service_id,"CurrentMute",value[RC_Mute]);
	return 0;
}


int cRenderingControl::SetMute(IXML_Document * in, IXML_Document ** out,Upnp_Action_Request *Event)
{
	cout << "SetMute" << endl;
	setVar(RC_id,1,RC_Mute,this->getXMLValue(in,"DesiredMute"));
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,service_id,NULL,NULL);
	return 0;
}


int cRenderingControl::GetVolume(IXML_Document * in, IXML_Document ** out,Upnp_Action_Request *Event)
{
	cout << "GetVolume" << endl;
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,service_id,"CurrentVolume",value[RC_Volume]);
	return 0;
}


int cRenderingControl::SetVolume(IXML_Document * in, IXML_Document ** out,Upnp_Action_Request *Event)
{
	cout << "SetVolume" << endl;
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,service_id,NULL,NULL);
	char cmd[80];
	sprintf(cmd,"volume %s 1",this->getXMLValue(in,"DesiredVolume"));
	((cRenderer*)lnkcRenderer)->mplayer_cmd(cmd,false);
	setVar(RC_id,2,cRenderingControl::RC_Volume,this->getXMLValue(in,"DesiredVolume"));
	//UpnpMakeActionResponse(Event->ActionName,this->service_id,1,name[RC_Volume],value[RC_Volume]);
	return 0;
}

// not implemented
int cRenderingControl::GetVolumeDB(IXML_Document * in, IXML_Document ** out,Upnp_Action_Request *Event)
{
	cout << "GetVolumeDB" << endl;
	return 0;
}

// not implemented
int cRenderingControl::SetVolumeDB(IXML_Document * in, IXML_Document ** out,Upnp_Action_Request *Event)
{
	cout << "SetVolumeDB" << endl;
	return 0;
}

// not implemented
int cRenderingControl::GetVolumeDBRange(IXML_Document * in, IXML_Document ** out,Upnp_Action_Request *Event)
{
	cout << "GetVolumeDBRange" << endl;
	return 0;
}

// not implemented
int cRenderingControl::GetLoudness(IXML_Document * in, IXML_Document ** out,Upnp_Action_Request *Event)
{
	cout << "GetLoudness" << endl;
	return 0;
}

// not implemented
int cRenderingControl::SetLoudness(IXML_Document * in, IXML_Document ** out,Upnp_Action_Request *Event)
{
	cout << "SetLoudness" << endl;
	return 0;
}

// not implemented
int cRenderingControl::SetPlayMode(IXML_Document * in, IXML_Document ** out,Upnp_Action_Request *Event)
{
	cout << "SetPlayMode" << endl;
	return 0;
}
