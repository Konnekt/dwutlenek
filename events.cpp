/* 
   DwuTlenek

   License information can be found in license.txt.
   Please redistribute only as a whole package.
   
   (c)2004 Rafa³ Lindemann | Stamina
   http://www.konnekt.info
*/ 

#include "stdafx.h"

void __tlen_debug(int level, const gchar *name, const gchar *format, va_list ap) {
	if(tlen_debug_get () < level)
		return;
    char buff[502];
    _vsnprintf(buff , 500 , ("   lT| " + string(name) + " | " + CStdString(format).Trim()).c_str() , ap);
    buff[500]=0;
    IMessage(IMC_LOG , 0,0,(int)buff,0);
}


void tlen_debug(const gchar *format, ...) {
	va_list ap;
	va_start(ap, format);
	__tlen_debug(TLEN_DEBUG_NORMAL , ".N." , format , ap);
	va_end(ap);
}

void tlen_debug_high(const gchar *format, ...) {
	va_list ap;
	va_start(ap, format);
	__tlen_debug(TLEN_DEBUG_HIGH , "!H!" , format , ap);
	va_end(ap);
}


void cTlen::EventCB (TlenConnectionEx *tc) {
	TlenEvent *ev;
	cTlen * me = tc->me;
	while ((ev = tlen_event_get (tc))) {
		switch (ev->type) {
        case TLEN_EVENT_CONNECTED:
			me->plug->IMDEBUG(DBG_NET , "- TLEN_EVENT_CONNECTED");
	        break;
        case TLEN_EVENT_CONNECTION_FAILED:
			me->plug->IMDEBUG(DBG_ERROR , "! TLEN_EVENT_CONNECTION_FAILED");
            g_main_loop_quit (me->mainloop);
			/* nic, po prostu siê roz³¹czamy */
			break;
        case TLEN_EVENT_DISCONNECTED:
			me->plug->IMDEBUG(DBG_NET , "- TLEN_EVENT_DISCONNECTED");
            g_main_loop_quit (me->mainloop);
            break;
        case TLEN_EVENT_CRITICAL_FAILURE:
			me->plug->IMDEBUG(DBG_ERROR , "- TLEN_EVENT_CRITICAL_FAILURE");
            g_main_loop_quit (me->mainloop);
            break;
        case TLEN_EVENT_UNAUTHORIZED:
			me->plug->IMDEBUG(DBG_ERROR , "- TLEN_EVENT_UNAUTHORIZED");
			me->plug->IMessage(&sIMessage_msgBox(IMI_ERROR , "B³¹d autoryzacji! Na pewno poda³eœ odpowiedni login i has³o?", me->GetName()));
            g_main_loop_quit(me->mainloop);
            break;
        case TLEN_EVENT_LOGGED:
			me->plug->IMDEBUG(DBG_NET , "- TLEN_EVENT_LOGGED");
			me->OnConnected(); /* chodzi o po³¹czenie PE£NE, a nie tylko z hostem */
            tlen_roster_get (tc);
			break;
		case TLEN_EVENT_PRESENCE:
			me->OnPresence((TlenEventPresence*)ev->event);
			break;
		case TLEN_EVENT_ROSTER:
			me->OnRoster((TlenEventRoster*)ev->event);
			break;
		case TLEN_EVENT_MESSAGE:
			me->OnMessage((TlenEventMessage*)ev->event);
			break;
		case TLEN_EVENT_PUBDIR:
			me->OnVCardRequestAnswer(((TlenEventPubdir*)ev->event)->item);
			break;
		case TLEN_EVENT_PUBDIR_CHANGED:
			me->OnVCardSetAnswer();
			break;
		case TLEN_EVENT_SEARCH:
			me->OnSearchAnswer((TlenEventSearch*)ev->event);
			break;
		case TLEN_EVENT_NEWMAIL:
			me->OnNewMail((TlenEventMessage*)ev->event);
			break;
		}
		tlen_event_free (ev);
	}
}

void cTlen::SetCntStatusFlag(int cnt , int flag , int mask) {
	if (cnt <= 0) return;
	plug->DTsetInt(DTCNT , cnt , CNT_STATUS , flag , mask);
	plug->ICMessage(IMI_REFRESH_CNT , cnt);
}

void cTlen::OnPresence(TlenEventPresence * ev) {
	int cntID = FindContact(SAFECHAR(ev->from));
	plug->IMDEBUG(DBG_MISC , "- Presence from '%s' type %d status %d" , ev->from , ev->type , ev->status);
	if (ev->type) {
		if (!strcmp(ev->type , "subscribe")) {
			cJabberBase::OnPresenceRequest(ev->from , true);
			return;
		} else if (!strcmp(ev->type , "unsubscribe")) {
			//SetCntStatusFlag(cntID , ST_HIDEMYSTATUS , ST_HIDEMYSTATUS);
			cJabberBase::OnPresenceRequest(ev->from , false);
			return;
		} else if (!strcmp(ev->type , "subscribed")) {
			//SetCntStatusFlag(cntID , 0 , ST_HIDESHISSTATUS);
			plug->IMessage(&KNotify::sIMessage_notify(CStdString("Zosta³eœ autoryzowany przez ") + ev->from , UIIcon(IT_MESSAGE,0,MT_EVENT,0),KNotify::sIMessage_notify::tInform,4));
			return;
		} else if (!strcmp(ev->type ,  "unsubscribed")) {
			//SetCntStatusFlag(cntID , ST_HIDESHISSTATUS , ST_HIDESHISSTATUS);
			plug->IMessage(&KNotify::sIMessage_notify(CStdString("Subskrypcja zosta³a odrzucona przez ") + ev->from , UIIcon(IT_MESSAGE,0,MT_EVENT,0),KNotify::sIMessage_notify::tInform,4));
			return;
		}
		/* pozosta³e typy to available i unavailable czyli powinien przeskoczyæ dalej... */
	}
	if (cntID > 0) {
		plug->ICMessage(IMI_CNT_ACTIVITY , cntID);
		int status = ev->status == TLEN_PRESENCE_AVAILABLE ? ST_ONLINE 
			: ev->status == TLEN_PRESENCE_AWAY ? ST_AWAY
			: ev->status == TLEN_PRESENCE_CHATTY ? ST_CHAT
			: ev->status == TLEN_PRESENCE_DND ? ST_DND
			: ev->status == TLEN_PRESENCE_EXT_AWAY ? ST_NA
			: ST_OFFLINE
			;
		CStdString info = iso2cp( ev->description );
		if (status == (plug->DTgetInt(DTCNT , cntID , CNT_STATUS) & ST_MASK)
			&& info == plug->DTgetStr(DTCNT , cntID , CNT_STATUSINFO))
			return;
		CntSetStatus(cntID , status , info.c_str());
		plug->ICMessage(IMI_CNT_ACTIVITY , cntID);
		plug->ICMessage(IMI_REFRESH_CNT , cntID);
	}
}
void cTlen::OnRoster(TlenEventRoster * ev) {
	GSList * l = ev->list;
	while (l) {
		TlenRosterItem* item = (TlenRosterItem*) l->data;
		if (!IsSynchronized()) {
			roster.push_back(CheckJID(item->jid));
			cJabberBase::OnRosterItemAdded(item->jid , item);
		} else {
			if (item->subscription && !strcmp(item->subscription , "remove")) {
				//cJabberBase::OnRosterItemRemoving(item->jid);
			} else {
				cJabberBase::OnRosterItemUpdated(item->jid , item);
			}
		}
		l = l->next;
	}
	cJabberBase::OnRosterAnswer();
}
void cTlen::OnMessage(TlenEventMessage * ev) {
	if (!ev->body || !*ev->body || !ev->from || !*ev->from) return;
	cMessage m;
	m.type = MT_MESSAGE;
	m.flag = MF_HANDLEDBYUI;
	m.body = _iso2cp( ev->body );
	CStdString from = ev->from;
	int cntID = this->FindContact(from);
	if (cntID > 0) {
		from = plug->DTgetStr(DTCNT , cntID , CNT_UID); /* ¿eby nie by³o niejasnoœci... */
		plug->ICMessage(IMI_CNT_ACTIVITY , cntID);
	}
	m.fromUid = (char*)from.c_str();
	m.net = this->net;
	/* 20040406T21:45:50 */
	m.time = 0;
	if (ev->stamp && *ev->stamp && strlen(ev->stamp) == 17) {
		tm mtime;
		memset(&mtime , 0 , sizeof(mtime));
		if (sscanf(ev->stamp , "%4u%2u%2uT%2u:%2u:%2u" , &mtime.tm_year , &mtime.tm_mon , &mtime.tm_mday , &mtime.tm_hour , &mtime.tm_min , &mtime.tm_sec) > 0) {
			mtime.tm_year -= 1900;
			mtime.tm_mon -= 1;
			/* w mtime czas jest podany w GMT, mktime uznaje go za lokalny, wiec koryguje jeszcze raz o timezone... 
			musimy wiêc "podnieœæ" czas z powrotem do GMT */
			_tzset();
			m.time = _mktime64(&mtime) - _timezone;
		}
	}
	if (!m.time)
		m.time = _time64(0);

	if ((cntID > 0 && plug->DTgetInt(DTCNT , cntID , CNT_STATUS) & ST_IGNORED)
		|| (cntID <=0 && plug->ICMessage(IMC_CNT_IGNORED , this->net , (int)ev->from)))
		return IgnoreMessage(&m , cntID);
	sMESSAGESELECT ms;
	ms.id = plug->ICMessage(IMC_NEWMESSAGE , (int) &m);
	if (ms.id > 0)
		plug->ICMessage(IMC_MESSAGEQUEUE , (int) &ms);
}
void cTlen::OnNewMail(TlenEventMessage * ev) {
	/*na razie olaæ...*/
}
void cTlen::OnVCardRequestAnswer(TlenPubdirItem * item) {
	if (!this->dlgVCardReq) return;
	int cnt = (int)this->dlgVCardReq->param; // this->FindContact(iq.getAttrib("from"));
    bool window = UIGroupHandle(sUIAction(0 , IMIG_NFOWND , cnt)) != 0;

	if (item) {
		char buff [251];
		if (item->birthyear) {
			int born = ((item->birthyear) << 16) | 0x0101;
			CntSetInfoValue(window , cnt , CNT_BORN , itoa(born , buff , 10));
		}
		if (item->city)
			CntSetInfoValue(window , cnt , CNT_CITY , _iso2cp( item->city ));
		if (item->email)
			CntSetInfoValue(window , cnt , CNT_EMAIL , _iso2cp( item->email ));
		if (item->firstname)
			CntSetInfoValue(window , cnt , CNT_NAME , _iso2cp( item->firstname ));
		if (item->gender)
			CntSetInfoValue(window , cnt , CNT_GENDER , itoa(TLEN_PUBDIR_GENDER_MALE ? GENDER_MALE : item->gender == TLEN_PUBDIR_GENDER_FEMALE ? GENDER_FEMALE : GENDER_UNKNOWN , buff , 10));
	//	if (item->job)
	//		CntSetInfoValue(window , cnt , CNT_WORK_TITLE , item->job);
		if (item->lastname)
			CntSetInfoValue(window , cnt , CNT_SURNAME , _iso2cp( item->lastname ));
		if (item->nick)
			CntSetInfoValue(window , cnt , CNT_NICK , _iso2cp( item->nick ));
		_snprintf(buff , 250 , "Szko³a:\t%s\r\nPlany:\t%s" , item->school , item->plans);
		CntSetInfoValue(window , cnt , CNT_DESCRIPTION , _iso2cp( buff ));
	}
	cJabberBase::OnVCardRequestAnswer();
}
void cTlen::OnSearchAnswer(TlenEventSearch * ev) {
	GSList * l = ev->list;

	if (this->dlgVCardReq) { /* libTlen jakoœ tego nie rozró¿nia... */

		OnVCardRequestAnswer(l ? (TlenPubdirItem*)l->data : 0);
		return;

	}

	while (l) {
		TlenPubdirItem * item = (TlenPubdirItem*)l->data;
		sCNTSEARCH sr;
		time_t currTime = time(0);
		tm date = *localtime(&currTime);
		sr.born_max = date.tm_year + 1900 - item->age;
		if (item->city)
			strncpy(sr.city , _iso2cp( item->city ) , sizeof(sr.city));
		if (item->email)
			strncpy(sr.email , _iso2cp( item->email ) , sizeof(sr.email));
		sr.gender = item->gender == TLEN_PUBDIR_GENDER_MALE ? GENDER_MALE : item->gender == TLEN_PUBDIR_GENDER_FEMALE ? GENDER_FEMALE : GENDER_UNKNOWN; 
		if (item->firstname)
			strncpy(sr.name , _iso2cp( item->firstname ) , sizeof(sr.name));
		sr.net = this->net;
		sr.status = StatusTlenToKonnekt((TlenPresenceStatus)item->status);
		if (item->nick)
			strncpy(sr.nick , _iso2cp( item->nick ) , sizeof(sr.nick));
		if (item->lastname)
			strncpy(sr.surname , _iso2cp( item->lastname ) , sizeof(sr.surname));
		if (item->id)
			strncpy(sr.uid , item->id , sizeof(sr.uid));
		ICMessage(IMI_CNT_SEARCH_FOUND , (int) &sr);
		l = l->next;
	}
	cJabberBase::OnSearchAnswer();
}
