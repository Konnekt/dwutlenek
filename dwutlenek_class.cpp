/* 
   DwuTlenek

   License information can be found in license.txt.
   Please redistribute only as a whole package.
   
   (c)2004 Rafa³ Lindemann | Stamina
   http://www.konnekt.info
*/ 

#include "stdafx.h"
#include "resource.h"
extern "C" {
#include "base64.h"
};



cTlen::cTlen(cCtrl * ctrl):cJabberBase(ctrl) {

	net = dwuTlenek::net;
	dtPrefix = "dwuTlenek/slot";
	dtPrefix += slotID;
	connection = 0;

}// inicjalizacja

SOCKET cTlen::GetSocket() {
	 return 0;//connection ? connection->socket->sockfd : 0;
}


// g³ówny w¹tek...

unsigned int cTlen::ListenThread(void) {
	state = cs_connecting;
	roster.clear();
	synchronized = false;
	plug->ICMessage(IMC_SETCONNECT , 0);
	plug->IMessage(&sIMessage_StatusChange(IMC_STATUSCHANGE , 0 , ST_CONNECTING , ""));
	currentStatus = ST_CONNECTING;	
	try {
		{
			CStdString login , pass;
			this->GetUserAndPass(login , pass);
			TlenConnection * tc = tlen_connection_new(login , pass);
			if (!tc) throw kJabber::cException(this , "Login" , "Problemy z pamiêci¹?");
			// Zamieniamy na Ex, tworzymy w ten sam deseñ...
		    connection = g_new0 (TlenConnectionEx, 1);
			connection->me = this;
			memcpy(connection , tc , sizeof(*tc));
			g_free(tc);
		}
		tlen_connection_open (connection , (TlenEventCallback) cTlen::EventCB);
		mainloop = g_main_loop_new (NULL, FALSE);
		g_main_loop_run (mainloop);
		g_main_loop_unref(mainloop);

		if (connection->state == TLEN_STATE_LOGGED) {
			tlen_connection_close(connection);
		}

		tlen_connection_delete (connection);
		connection = 0;
		mainloop = 0;
	} catch (kJabber::cExceptionQuiet e) {
		plug->IMDEBUG(DBG_ERROR , "!ListenThread Exception - %s :: %s" , e.GetWhere().c_str() , e.GetMsg().c_str());
	}

	bool onRequest = (currentStatus == ST_OFFLINE);
	if (!onRequest) {
		plug->IMessage(&sIMessage_StatusChange(IMC_STATUSCHANGE , 0 , ST_OFFLINE , ""));
		currentStatus = ST_OFFLINE;	
		plug->ICMessage(IMC_SETCONNECT , 1);
	}

    int c = plug->ICMessage(IMC_CNT_COUNT);
    for (int i=1 ; i<c;i++) {
        if (GETCNTI(i,CNT_NET) == this->net) {
			CntSetStatus(i , ST_OFFLINE , "");
			plug->ICMessage(IMI_CNT_DEACTIVATE , i);
			SETCNTI(i , CNT_STATUS , 0 , ST_HIDEBOTH);
			SetCntIcon(i , ST_OFFLINE , plug->DTgetStr(DTCNT , i , CNT_UID));
        }
    }
    plug->ICMessage(IMI_REFRESH_LST);
	plug->IMDEBUG(DBG_MISC , "- thread finished");
	state = cs_offline;
	CloseHandle(thread);
	thread = 0;
	return 0;
}

void cTlen::DisconnectSocket() {
	tlen_connection_close (connection);
    g_main_loop_quit (this->mainloop);
}
bool cTlen::SendPresence(const CStdString & to , int status , const char * info) {
	if (!this->Connected()) return false;
	TlenPresenceStatus show = StatusKonnektToTlen(status);
	return tlen_presence(connection , (TlenPresenceType)show , cp2iso(info)) != 0;
}

bool cTlen::SendSubscribed(const CStdString & to , bool subscribed) {
	if (!this->Connected()) return false;
	if (subscribed) {
//		SetCntStatusFlag(this->FindContact(to) , 0 , ST_HIDEMYSTATUS);
		return tlen_subscribe_accept(connection , to) != 0;
	} else {
//		SetCntStatusFlag(this->FindContact(to) , ST_HIDEMYSTATUS , ST_HIDEMYSTATUS);
		return tlen_unsubscribe_accept(connection , to) != 0;
	}
}
bool cTlen::SendSubRequest(const CStdString & to , bool subscribe) {
	if (!this->Connected()) return false;
	if (subscribe)
		return tlen_subscribe_request(connection , to) != 0;
	else
		return tlen_unsubscribe_request(connection , to) != 0;
	return true;
}

bool cTlen::SendCntSearch(sCNTSEARCH * sr) {
	TlenPubdirItem * item = tlen_pubdir_item_new();
	memset(item , 0 , sizeof(*item));
	__time64_t currentTime = _time64(0);
	tm date = *_localtime64(&currentTime);
	date.tm_year += 1900;
	if (sr->born_max)
		item->age_max = date.tm_year - sr->born_max;
	if (sr->born_min)
		item->age_min = date.tm_year - sr->born_min;
	if (sr->city[0])
		item->city = _iso2cp( g_strdup(sr->city) );
	if (sr->email[0])
		item->email = _iso2cp( g_strdup(sr->email) );
	if (sr->gender) 
		item->gender = sr->gender == GENDER_MALE ? TLEN_PUBDIR_GENDER_MALE : TLEN_PUBDIR_GENDER_FEMALE ;
	if (sr->uid[0])
		item->id = _iso2cp( g_strdup(sr->uid) );
	if (sr->name[0])
		item->firstname = _iso2cp( g_strdup(sr->name) );
	if (sr->surname[0])
		item->lastname = _iso2cp( g_strdup(sr->surname) );
	if (sr->nick[0])
		item->nick = _iso2cp( g_strdup(sr->nick) );
	if (sr->status)
		item->status = TLEN_PRESENCE_AVAILABLE;
	tlen_pubdir_search(connection , item);
	tlen_pubdir_item_free(item);
	return true;
}
bool cTlen::SendVCardRequest(int cntID) {
	
	if (plug->DTgetPos(DTCNT , cntID) == 0) {
		tlen_pubdir_get(connection);
	} else {
		TlenPubdirItem * item = tlen_pubdir_item_new();
		memset(item , 0 , sizeof(*item));
		item->id = g_strdup(plug->DTgetStr(DTCNT , cntID , CNT_UID));
		if (strchr(item->id , '@'))
			*strchr(item->id , '@') = 0; // bez hosta..
		tlen_pubdir_search(connection , item);
		tlen_pubdir_item_free(item);
	}
	return true;
}
bool cTlen::SendVCardSet(bool byWnd) {
	TlenPubdirItem * item = tlen_pubdir_item_new();
	memset(item , 0 , sizeof(*item));
	/* Sprawdzamy, czy okno wysy³ania jest otwarte */
//	bool byWnd = UIGroupHandle(sUIAction(0 , IMIG_NFO , 0)) != 0;
	/* Pobieramy informacje funkcj¹, która w zale¿noœci od byWnd pobiera
	wartoœæ pola z bazy danych lub okna z informacjami o kontakcie...
	*/
	item->firstname = _iso2cp( g_strdup(CntGetInfoValue(byWnd , 0 , CNT_NAME)) );
	item->lastname = _iso2cp( g_strdup(CntGetInfoValue(byWnd , 0 , CNT_SURNAME)) );
	item->nick = _iso2cp( g_strdup(CntGetInfoValue(byWnd , 0 , CNT_NICK)) );
	item->gender = atoi(CntGetInfoValue(byWnd , 0 , CNT_GENDER)) == GENDER_MALE ? TLEN_PUBDIR_GENDER_MALE : plug->DTgetInt(DTCNT , 0 , CNT_GENDER) == GENDER_FEMALE ? TLEN_PUBDIR_GENDER_FEMALE : TLEN_PUBDIR_GENDER_NONE;
	item->city = _iso2cp( g_strdup(CntGetInfoValue(byWnd , 0 , CNT_CITY)) );
	item->email = _iso2cp( g_strdup(CntGetInfoValue(byWnd , 0 , CNT_EMAIL)) );
	item->birthyear = atoi(CntGetInfoValue(byWnd , 0 , CNT_BORN)) >> 16;
	item->visible = plug->DTgetInt(DTCFG , 0 , dwuTlenek::CFG::pubdirShowStatus);
	tlen_pubdir_set(connection , item);
	tlen_pubdir_item_free(item);
	return true;
}


bool cTlen::SendMessage(cMessage * m) {
	if (!this->Connected()) 
		return false;

	return tlen_message(connection , this->CheckJID(m->toUid) , cp2iso( m->body ) , TLEN_MESSAGE_TYPE_CHAT) != 0;
}

bool cTlen::SendPing() {
	return 1;
	//return tlen_connection_socket_writen(this->connection , " " , 1 , 0) != 0;
}


bool cTlen::RosterDeleteJID(const CStdString & jid) {
	roster_it it = find(roster.begin() , roster.end() , CheckJID(jid));
	if (it != roster.end()) roster.erase(it);
	return tlen_contact_remove(connection , jid) != 0;
}

bool cTlen::RosterContainsJID(const CStdString & jid) {
	roster_it it = find(roster.begin() , roster.end() , CheckJID(jid));
	return (it != roster.end());
}

void cTlen::SetContact(int cntID , TlenRosterItem * item) {
	bool changed = false;
	enSynchDirection direction = CFGsynchDirection();
	if (cntID == -1) {
		if (!plug->DTgetInt(DTCFG , 0 , dwuTlenek::CFG::synchAddContacts) || direction == sd_disabled)
			return;
		cntID = plug->ICMessage(IMC_CNT_ADD , this->net , (int)item->jid);
		changed = true;
	}
	if (direction != sd_toClient && direction != sd_disabled) {
		plug->DTsetStr(DTCNT , cntID , CNT_DISPLAY , iso2cp( item->name ));
		CStdString group = iso2cp( item->group ? item->group : "" );
		plug->ICMessage(IMC_GRP_ADD , (int)group.c_str());
		plug->DTsetStr(DTCNT , cntID , CNT_GROUP , group);
	}
	int subs = 0;
	if (!item->subscription)
		subs = ST_HIDEBOTH;
	else if (!strcmp(item->subscription , "both"))
		subs = 0;
	else if (!strcmp(item->subscription , "from"))
		subs = ST_HIDESHISSTATUS;
	else if (!strcmp(item->subscription , "to"))
		subs = ST_HIDEMYSTATUS;
	else 
		subs = ST_HIDEBOTH;
	plug->DTsetInt(DTCNT , cntID , CNT_STATUS , subs , ST_HIDEBOTH);
	plug->ICMessage(changed?IMC_CNT_CHANGED : IMI_REFRESH_CNT , cntID);
}

void cTlen::SetRosterItem(int cntID) {
	if (cntID == -1) 
		return;
	CStdString jid = plug->DTgetStr(DTCNT , cntID , CNT_UID);
	if (plug->DTgetInt(DTCNT , cntID , CNT_STATUS) & ST_NOTINLIST)
		return;
	if (!RosterContainsJID(jid)
		&& !plug->DTgetInt(DTCFG , 0 , dwuTlenek::CFG::synchAddContacts))
		return;
	tlen_contact_add(connection , jid , cp2iso( plug->DTgetStr(DTCNT , cntID , CNT_DISPLAY) ) , cp2iso( plug->DTgetStr(DTCNT , cntID , CNT_GROUP) ));
}

void cTlen::SetCntIcon(int cntID , int status , const CStdString & jid) {
}

const CStdString cTlen::GetUID() {
	return CStdString(GETSTR(plug->DTgetNameID(DTCFG , dtPrefix + "/user"))) + "@tlen.pl";
}

int cTlen::FindContact(const CStdString & _jid) {
	if (_jid.c_str() == 0) return -1; // Uuuuu... powa¿na sprawa
	CStdString jid = _jid.substr(0 , _jid.find('@'));
	if (!jid.CompareNoCase(connection->username)) return 0;
	int c = plug->ICMessage(IMC_CNT_COUNT);
	plug->DTlock(DTCNT , -1);
	int found = -1;
	for (int i=1; i < c; i++) {
		if (plug->DTgetInt(DTCNT , i , CNT_NET) != this->net)
			continue;
		const char * uid = plug->DTgetStr(DTCNT , i , CNT_UID);
		const char * at = strchr(uid , '@');
		if (at && (at - uid) != jid.length())
			continue;
		if (!strnicmp(uid , jid.c_str() , jid.length())) {
			found = plug->DTgetID(DTCNT , i);
			break;
		}
	}
	plug->DTunlock(DTCNT , -1);
	return found;
}


TlenPresenceStatus cTlen::StatusKonnektToTlen(int status) {
	return status == ST_ONLINE ? TLEN_PRESENCE_AVAILABLE
		: status == ST_AWAY ? TLEN_PRESENCE_AWAY 
		: status == ST_NA ? TLEN_PRESENCE_EXT_AWAY
		: status == ST_CHAT ? TLEN_PRESENCE_CHATTY
		: status == ST_DND ? TLEN_PRESENCE_DND 
		: status == ST_HIDDEN ? TLEN_PRESENCE_INVISIBLE 
		: TLEN_PRESENCE_UNAVAILABLE
		;
}

int cTlen::StatusTlenToKonnekt(TlenPresenceStatus status) {
	return status == TLEN_PRESENCE_AVAILABLE ? ST_ONLINE
		: status == TLEN_PRESENCE_AWAY ? ST_AWAY
		: status == TLEN_PRESENCE_EXT_AWAY ? ST_NA
		: status == TLEN_PRESENCE_CHATTY ? ST_CHAT
		: status == TLEN_PRESENCE_DND ? ST_DND
		: status == TLEN_PRESENCE_INVISIBLE ? ST_HIDDEN
		: ST_OFFLINE
		;
}

CStdString cTlen::CheckJID(CStdString jid) {
	if (jid.find('@') == -1) 
		return jid += "@tlen.pl";
	return jid;
}
