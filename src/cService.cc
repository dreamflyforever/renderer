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


#include "cService.h"
#include "cRenderer.h"

cService::cService(void* lnkcRenderer)
{
	this->lnkcRenderer=lnkcRenderer;
	service_id = "";
	var_count = 0;
	action_count = 0;

}


cService::~cService() { }

/*
 * Set Variable in service-table
 * and send notification
 */
void cService::setVar(int service, int count, ...) 
{
	char* metaservice="";
	if(service==1)
	{
		metaservice="<Event xmlns=\"urn:schemas-upnp-org:metadata-1-0/AVT/\"></Event>";
	}else if(service==2)
	{
		metaservice="<Event xmlns=\"urn:schemas-upnp-org:metadata-1-0/RCS/\"></Event>";
	}
	
	IXML_Document* LCdoc = NULL;
	IXML_Node* node = NULL;
	IXML_Element* Ele = NULL;
	
	LCdoc = ixmlParseBuffer(metaservice);
	node = ixmlNode_getFirstChild( (IXML_Node*) LCdoc );
	Ele = ixmlDocument_createElement( LCdoc, "InstanceID" );
	ixmlElement_setAttribute(Ele, "val","0");
	ixmlNode_appendChild(  node,(IXML_Node*) Ele);
	
    	va_list list;
	va_start(list,count);
	for (int i=0; i<count; i+=2) {
		int _name=va_arg(list,int);
		char* newvalue=va_arg(list,char*);
		// Wert setzen
		this->value[_name] = newvalue;
		cout << "## SETVAR: " << this->name[_name] << ": " << this->value[_name] << endl;
		
		IXML_Element* Ele2 = NULL;
		Ele2 = ixmlDocument_createElement(LCdoc, this->name[_name]);
		ixmlElement_setAttribute(Ele2, "val",newvalue);
		if(strcmp(this->name[_name],"Volume")==0)
		{
			ixmlElement_setAttribute(Ele2, "channel","Master");
		}
		ixmlNode_appendChild((IXML_Node*)Ele,(IXML_Node*) Ele2);
		
	}
	va_end(list);

	IXML_Document* myDoc2 = UpnpCreatePropertySet(1,
								"LastChange",
								ixmlPrintDocument(LCdoc)
	); 
	//cout << ixmlPrintDocument(myDoc2) << endl;
	int e=UpnpNotifyExt(((cRenderer*)lnkcRenderer)->renderer_hdl,((cRenderer*)lnkcRenderer)->udn,this->service_id,myDoc2);

}

/* 
 * Extracts a XML-Entity(?) of a given document
 */
char* cService::getXMLValue(IXML_Document *doc, char tagName[]) 
{
	IXML_NodeList *nodeList = ixmlDocument_getElementsByTagName(doc, tagName);
	if (nodeList) {
		char* out = strdup(ixmlNode_getNodeValue(ixmlNode_getFirstChild(ixmlNodeList_item( nodeList, 0 ))));
		ixmlNodeList_free(nodeList);
		return out;
	}
	return "";
}


int cService::callAction(int i, IXML_Document* in, IXML_Document** out, Upnp_Action_Request* Event) 
{
	return 0;
}

/*char* cService::getTrackLength()
{
	char* err=((cRenderer*)lnkcRenderer)->mplayer_cmd("get_time_length\n");
	string length;
	if(err)
	{
		length=((cRenderer*)lnkcRenderer)->showOutput();
	}
	length=length.substr(length.find_first_of('=')+1);
	char returnvalue[100];
	strcpy(returnvalue,length.c_str());
	return returnvalue;
}*/
