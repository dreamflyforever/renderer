/* 
 *  Copyright (C) 2006 by:
 *  Da He <da.he@uni-dortmund.de>
 *  Julian Flake <julian.flake@uni-dortmund.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "cAVTransport.h"
#include "cRenderer.h"

cAVTransport::cAVTransport(void* lnkcRenderer) :cService(lnkcRenderer)
{
	service_id = "urn:upnp-org:serviceId:AVTransport1";
	var_count = 30;
	action_count = 17;
	
	// state variables
	// 0	
	name[AV_CurrentPlayMode] = "CurrentPlayMode";
	value[AV_CurrentPlayMode] = "NORMAL";

	name[AV_RecordStorageMedium] = "RecordStorageMedium";
	value[AV_RecordStorageMedium] = "NOT_IMPLEMENTED";

	name[AV_LastChange] = "LastChange";
	value[AV_LastChange] = "<Event xmlns=\"urn:schemas-upnp-org:metadata-1-0/AVT/\"></Event>";

	name[AV_RelativeTimePosition] = "RelativeTimePosition";
	value[AV_RelativeTimePosition] = "00:00:00";

	name[AV_CurrentTrackURI] = "CurrentTrackURI";
	value[AV_CurrentTrackURI] = "";
	// 5
	name[AV_CurrentTrackDuration] = "CurrentTrackDuration";
	value[AV_CurrentTrackDuration] = "NOT_IMPLEMENTED";

	name[AV_CurrentRecordQualityMode] = "CurrentRecordQualityMode";
	value[AV_CurrentRecordQualityMode] = "NOT_IMPLEMENTED";
	
	name[AV_CurrentMediaDuration] = "CurrentMediaDuration";
	value[AV_CurrentMediaDuration] = "";
	
	name[AV_AbsoluteCounterPosition] = "AbsoluteCounterPosition";
	value[AV_AbsoluteCounterPosition] = "";
	
	name[AV_RelativeCounterPosition] = "RelativeCounterPosition";
	value[AV_RelativeCounterPosition] = "";
	//10
	name[AV_A_ARG_TYPE_InstanceID] = "A_ARG_TYPE_InstanceID";
	value[AV_A_ARG_TYPE_InstanceID] = "";
	
	name[AV_AVTransportURI] = "AVTransportURI";
	value[AV_AVTransportURI] = "";
	
	name[AV_TransportState] = "TransportState";
	value[AV_TransportState] = "STOPPED";
	
	name[AV_CurrentTrackMetaData] = "CurrentTrackMetaData";
	value[AV_CurrentTrackMetaData] = "";
	
	name[AV_NextAVTransportURI] = "NextAVTransportURI";
	value[AV_NextAVTransportURI] = "";
	
	//15
	name[AV_PossibleRecordQualityModes] = "PossibleRecordQualityModes";
	value[AV_PossibleRecordQualityModes] = "NOT_IMPLEMENTED";
	
	name[AV_AbsoluteTimePosition] = "AbsoluteTimePosition";
	value[AV_AbsoluteTimePosition] = "";
	
	name[AV_CurrentTrack] = "CurrentTrack";
	value[AV_CurrentTrack] = "0";

	name[AV_NextAVTransportURIMetaData] = "NextAVTransportURIMetaData";
	value[AV_NextAVTransportURIMetaData] = "";
	
	name[AV_PlaybackStorageMedium] = "PlaybackStorageMedium";
	value[AV_PlaybackStorageMedium] = "NETWORK"; // or NOT_IMPLEMENTED?
	
	//20
	name[AV_CurrentTransportActions] = "CurrentTransportActions";
	value[AV_CurrentTransportActions] = "";
	
	name[AV_RecordMediumWriteStatus] = "RecordMediumWriteStatus";
	value[AV_RecordMediumWriteStatus] = "NOT_IMPLEMENTED";
	
	name[AV_PossiblePlaybackStorageMedia] = "PossiblePlaybackStorageMedia";
	value[AV_PossiblePlaybackStorageMedia] = "";
	
	name[AV_AVTransportURIMetaData] = "AVTransportURIMetaData";
	value[AV_AVTransportURIMetaData] = "";
	
	name[AV_NumberOfTracks] = "NumberOfTracks";
	value[AV_NumberOfTracks] = "0";
	
	//25
	name[AV_A_ARG_TYPE_SeekMode] = "A_ARG_TYPE_SeekMode";
	value[AV_A_ARG_TYPE_SeekMode] = "ABS_TIME";
	
	name[AV_A_ARG_TYPE_SeekTarget] = "A_ARG_TYPE_SeekTarget";
	value[AV_A_ARG_TYPE_SeekTarget] = "";
	
	name[AV_PossibleRecordStorageMedia] = "PossibleRecordStorageMedia";
	value[AV_PossibleRecordStorageMedia] = "";
	
	name[AV_TransportStatus] = "TransportStatus";
	value[AV_TransportStatus] = "OK";
	
	name[AV_TransportPlaySpeed] = "TransportPlaySpeed";
	value[AV_TransportPlaySpeed] = "1";


	action_name[0]="SetAVTransportURI";
	action_name[1]="SetNextTranportURI";
	action_name[2]="GetMediaInfo";
	action_name[3]="GetTransportInfo";
	action_name[4]="GetPositionInfo";
	action_name[5]="GetDeviceCapabilities";
	action_name[6]="GetTransportSettings";
	action_name[7]="Stop";
	action_name[8]="Play";
	action_name[9]="Pause";
	action_name[10]="Record";
	action_name[11]="Seek";
	action_name[12]="Next";
	action_name[13]="Previous";
	action_name[14]="SetPlayMode";
	action_name[15]="SetRecordQualityMode";
	action_name[16]="GetCurrentTransportActions";
	



	actions[0] = &cAVTransport::SetAVTransportURI;
	actions[1] = &cAVTransport::SetNextAVTransportURI;
	actions[2] = &cAVTransport::GetMediaInfo;
	actions[3] = &cAVTransport::GetTransportInfo;
	actions[4] = &cAVTransport::GetPositionInfo;

	actions[5] = &cAVTransport::GetDeviceCapabilities;
	actions[6] = &cAVTransport::GetTransportSettings;
	actions[7] = &cAVTransport::Stop;
	actions[8] = &cAVTransport::Play;
	actions[9] = &cAVTransport::Pause;

	actions[10] = &cAVTransport::Record;
	actions[11] = &cAVTransport::Seek;
	actions[12] = &cAVTransport::Next;
	actions[13] = &cAVTransport::Previous;
	actions[14] = &cAVTransport::SetPlayMode;

	actions[15] = &cAVTransport::SetRecordQualityMode;
	actions[16] = &cAVTransport::GetCurrentTransportActions;
}


cAVTransport::~cAVTransport() { }


int cAVTransport::callAction(int i,IXML_Document* in, IXML_Document** out, Upnp_Action_Request* Event)
{
	return (this->*actions[i])(in,out,Event);
}

/*
 * Actions
 */
int cAVTransport::SetAVTransportURI(IXML_Document* in, IXML_Document** out, Upnp_Action_Request* Event)
{	
	setVar(AV_id,1,AV_AVTransportURI,getXMLValue(in, "CurrentURI"));
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,service_id,NULL,NULL);
	cout << "set TransportURI to: " << this->value[cAVTransport::AV_AVTransportURI] << "!" << endl;
	//cout << "metadata #####################################" << ixmlPrintDocument(in) << "############################################################" << endl;
	return 0;
}

int cAVTransport::SetNextAVTransportURI(IXML_Document* in, IXML_Document** out, Upnp_Action_Request* Event)
{
	std::cout << "SetNextAVTransportURI: " << in << std::endl;
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,service_id,NULL,NULL);
	return 0;
}
int cAVTransport::GetMediaInfo(IXML_Document* in, IXML_Document** out, Upnp_Action_Request* Event)
{
	std::cout << "GetMediaInfo: " << in << std::endl;
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,this->service_id,"NrTracks",this->value[AV_NumberOfTracks]);
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,this->service_id,"MediaDuration",this->value[AV_CurrentMediaDuration]);
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,this->service_id,"CurrentURI",this->value[AV_AVTransportURI]);
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,this->service_id,"CurrentURIMetaData",this->value[AV_AVTransportURIMetaData]);
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,this->service_id,"NextURI",this->value[AV_NextAVTransportURI]);
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,this->service_id,"NextURIMetaData",this->value[AV_NextAVTransportURIMetaData]);
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,this->service_id,"PlayMedium",this->value[AV_PlaybackStorageMedium]);
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,this->service_id,"RecordMedium",this->value[AV_RecordStorageMedium]);
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,this->service_id,"WriteStatus",this->value[AV_RecordMediumWriteStatus]);
	return 0;
}

int cAVTransport::GetTransportInfo(IXML_Document* in, IXML_Document** out, Upnp_Action_Request* Event)
{
	std::cout << "GetTransportInfo: " << in << std::endl;
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,this->service_id,"CurrentTransportState",this->value[AV_TransportState]);
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,this->service_id,"CurrentTransportStatus",this->value[AV_TransportStatus]);
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,this->service_id,"CurrentSpeed",this->value[AV_TransportPlaySpeed]);
	return 0;
}

int cAVTransport::GetPositionInfo(IXML_Document* in, IXML_Document** out, Upnp_Action_Request* Event)
{
	std::cout << "GetPositionInfo "  << std::endl;
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,this->service_id,"Track",this->value[AV_CurrentTrack]);
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,this->service_id,"TrackDuration",this->value[AV_CurrentTrackDuration]);
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,this->service_id,"TrackMetaData",this->value[AV_CurrentTrackMetaData]);
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,this->service_id,"TrackURI",this->value[AV_CurrentTrackURI]);
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,this->service_id,"RelTime",this->value[AV_RelativeTimePosition]);
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,this->service_id,"AbsTime",this->value[AV_AbsoluteTimePosition]);
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,this->service_id,"RelCount",this->value[AV_RelativeCounterPosition]);
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,this->service_id,"AbsCount",this->value[AV_AbsoluteCounterPosition]);
	return 0;
}

int cAVTransport::GetDeviceCapabilities(IXML_Document* in, IXML_Document** out, Upnp_Action_Request* Event)
{
	std::cout << "GetDeviceCapabilities" << std::endl;
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,this->service_id,"PlayMedia",this->value[AV_PossiblePlaybackStorageMedia]);
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,this->service_id,"RecMedia",this->value[AV_PossibleRecordStorageMedia]);
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,this->service_id,"RecQualityModes",this->value[AV_PossibleRecordQualityModes]);
	return 0;
}

int cAVTransport::GetTransportSettings(IXML_Document* in, IXML_Document** out, Upnp_Action_Request* Event)
{
	std::cout << "GetTransportSettings: " << in << std::endl;
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,this->service_id,"PlayMode",this->value[AV_CurrentPlayMode]);
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,this->service_id,"RecQualityMode",this->value[AV_CurrentRecordQualityMode]);
	return 0;
}

int cAVTransport::Stop(IXML_Document* in, IXML_Document** out, Upnp_Action_Request* Event)
{
	std::cout << "Stop: " << in << std::endl;
	if (strcmp(this->value[cAVTransport::AV_TransportState],"PAUSED_PLAYBACK") == 0
		||
		strcmp(this->value[cAVTransport::AV_TransportState],"PLAYING") == 0) {
		cout << "unpause!" << endl;
		((cRenderer*)lnkcRenderer)->mplayer_cmd("quit",false);
		setVar(AV_id,1,cAVTransport::AV_TransportState,"STOPPED");
		((cRenderer*)lnkcRenderer)->mplayer_is_running = false;
	}
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,this->service_id,NULL,NULL);
	return 0;
}

int cAVTransport::Play(IXML_Document* in, IXML_Document** out, Upnp_Action_Request* Event)
{
	if (!strcmp("STOPPED",this->value[cAVTransport::AV_TransportState])) {
		if (strlen(this->value[cAVTransport::AV_AVTransportURI])) {
			cout << "play!" << endl;
			((cRenderer*)lnkcRenderer)->mplayer_start();

		}
	} else if (!strcmp(this->value[cAVTransport::AV_TransportState],"PAUSED_PLAYBACK")) {
		cout << "unpause!" << endl;
		((cRenderer*)lnkcRenderer)->mplayer_cmd("pause",false);
		setVar(AV_id,1,cAVTransport::AV_TransportState,"PLAYING");
	}
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,this->service_id,NULL, NULL);
	return 0;
}

int cAVTransport::Pause(IXML_Document* in, IXML_Document** out, Upnp_Action_Request* Event)
{
	std::cout << "Pause: " << in << std::endl;
	if (strcmp(this->value[cAVTransport::AV_TransportState],"PLAYING") == 0) {
		cout << "pause!" << endl;
		setVar(AV_id,1,cAVTransport::AV_TransportState,"PAUSED_PLAYBACK");
		((cRenderer*)lnkcRenderer)->mplayer_cmd("pause",false);
	} else if (strcmp(this->value[cAVTransport::AV_TransportState],"PAUSED_PLAYBACK") == 0) {
		cout << "unpause!" << endl;
		((cRenderer*)lnkcRenderer)->mplayer_cmd("pause",false);
		setVar(AV_id,1,cAVTransport::AV_TransportState,"PLAYING");
	}
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,this->service_id,NULL,NULL);
	return 0;
}

int cAVTransport::Record(IXML_Document* in, IXML_Document** out, Upnp_Action_Request* Event)
{
	std::cout << "Record: " << in << std::endl;
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,service_id,NULL,NULL);
	return 0;
}

int cAVTransport::Seek(IXML_Document* in, IXML_Document** out, Upnp_Action_Request* Event)
{
	std::cout << "Seek: " << ixmlPrintDocument(in) << std::endl;
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,service_id,NULL,NULL);
	if (strcmp(getXMLValue(in, "Unit"),"REL_TIME")==0) {
		//postition2seek setzen
		((cRenderer*)lnkcRenderer)->positionToSeek = ((cRenderer*)lnkcRenderer)->time2seconds(getXMLValue(in,"Target"))-10;
		char cmd[100];
		sprintf(cmd,"seek %i",((cRenderer*)lnkcRenderer)->positionToSeek);
		((cRenderer*)lnkcRenderer)->mplayer_cmd(cmd,false);
	}
	return 0;
}

int cAVTransport::Next(IXML_Document* in, IXML_Document** out, Upnp_Action_Request* Event)
{
	std::cout << "Next: " << in << std::endl;
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,service_id,NULL,NULL);
	return 0;
}

int cAVTransport::Previous(IXML_Document* in, IXML_Document** out, Upnp_Action_Request* Event)
{
	std::cout << "Previous: " << in << std::endl;
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,service_id,NULL,NULL);
	return 0;
}

int cAVTransport::SetPlayMode(IXML_Document* in, IXML_Document** out, Upnp_Action_Request* Event)
{
	std::cout << "SetPlayMode: " << in << std::endl;
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,service_id,NULL,NULL);
	return 0;
}

int cAVTransport::SetRecordQualityMode(IXML_Document* in, IXML_Document** out, Upnp_Action_Request* Event)
{
	std::cout << "SetRecordQualityMode: " << in << std::endl;
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,service_id,NULL,NULL);
	return 0;
}

int cAVTransport::GetCurrentTransportActions(IXML_Document* in, IXML_Document** out, Upnp_Action_Request* Event)
{
	std::cout << "GetCurrentTransportActions: " << in << std::endl;
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,this->service_id,"Actions",this->value[AV_CurrentTransportActions]);
	return 0;
}
