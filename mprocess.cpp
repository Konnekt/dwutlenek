/* 
   DwuTlenek

   License information can be found in license.txt.
   Please redistribute only as a whole package.
   
   (c)2004 Rafa³ Lindemann | Stamina
   http://www.konnekt.info
*/ 

#include "stdafx.h"
void cTlen::SetColumns() {
	cJabberBase::SetColumns();
}

void cTlen::UIPrepare() {

	plug->UIActionInsert(IMIG_STATUS , dwuTlenek::ACT::Status , -1 , ACTS_GROUP , "Tlen" , UIIcon(IT_LOGO , this->net , 0 , 0));

	plug->UIActionInsert(dwuTlenek::ACT::Status , kJabber::ACT::stOnline , -1 , 0 , "Dostêpny" , UIIcon(IT_STATUS , this->net , ST_ONLINE , 0));
	plug->UIActionInsert(dwuTlenek::ACT::Status , kJabber::ACT::stChat , -1 , 0 , "Pogadam" , UIIcon(IT_STATUS , this->net , ST_CHAT  , 0));
	plug->UIActionInsert(dwuTlenek::ACT::Status , kJabber::ACT::stAway , -1 , 0 , "Zaraz wracam" , UIIcon(IT_STATUS , this->net , ST_AWAY  , 0));
	plug->UIActionInsert(dwuTlenek::ACT::Status , kJabber::ACT::stNA , -1 , 0 , "Wrócê póŸniej" , UIIcon(IT_STATUS , this->net , ST_NA  , 0));
	plug->UIActionInsert(dwuTlenek::ACT::Status , kJabber::ACT::stDND , -1 , 0 , "Nie przeszkadzaæ" , UIIcon(IT_STATUS , this->net , ST_DND  , 0));
	plug->UIActionInsert(dwuTlenek::ACT::Status , kJabber::ACT::stHidden , -1 , 0 , "Ukryty" , UIIcon(IT_STATUS , this->net , ST_HIDDEN  , 0));
	plug->UIActionInsert(dwuTlenek::ACT::Status , kJabber::ACT::stOffline , -1 , 0 , "Niedostêpny" , UIIcon(IT_STATUS , this->net , ST_OFFLINE , 0));

//	plug->UIActionInsert(IMIG_CFG_USER , dwuTlenek::ACT::CfgGroup , -1 , ACTS_GROUP , "Tlen");
	plug->UIActionInsert(dwuTlenek::ACT::CfgGroup , 0 , -1 , ACTT_GROUP , "Konto"); {
		plug->UIActionInsert(dwuTlenek::ACT::CfgGroup , 0 , -1 , ACTT_COMMENT | ACTSC_INLINE , "Login");
		plug->UIActionInsert(dwuTlenek::ACT::CfgGroup , IMIB_CFG , -1 , ACTT_EDIT | ACTSC_INLINE , "" AP_TIPRICH "Nazwa <b>istniej¹cego</b> konta.<br/>Mo¿esz za³o¿yæ nowe przy pomocy strony http://www.tlen.pl/, lub komunikatora Tlen." , plug->DTgetNameID(DTCFG , dtPrefix + "/user"));
		plug->UIActionInsert(dwuTlenek::ACT::CfgGroup , 0 , -1 , ACTT_COMMENT , "@tlen.pl");
		plug->UIActionInsert(dwuTlenek::ACT::CfgGroup , 0 , -1 , ACTT_COMMENT | ACTSC_INLINE , "Has³o");
		plug->UIActionInsert(dwuTlenek::ACT::CfgGroup , IMIB_CFG , -1 , ACTT_PASSWORD , "" , plug->DTgetNameID(DTCFG , dtPrefix + "/pass"));
	} plug->UIActionInsert(dwuTlenek::ACT::CfgGroup , 0 , -1 , ACTT_GROUPEND);

	cJabberBase::UIPrepare();
}



int cTlen::IMessageProc(sIMessage_base * msgBase) {
    sIMessage_2params * msg = (msgBase->s_size>=sizeof(sIMessage_2params))?static_cast<sIMessage_2params*>(msgBase):0;
    switch (msgBase->id) {
	case IM_END: {
		this->Disconnect();
		plug->IMDEBUG(DBG_MISC , "- Waiting for cJabber thread");
		while (thread) {
		    Ctrl->Sleep(10);
		}
		return 1;} /* NIE wywo³ujemy funkcji z jabberBase */
	case IM_CNT_ADD: {
		if (plug->DTgetInt(DTCNT , msg->p1 , CNT_NET) == this->GetNet())
			plug->DTsetStr(DTCNT , msg->p1 , CNT_UID , CheckJID(plug->DTgetStr(DTCNT , msg->p1 , CNT_UID)));
		break;
		}
	case IM_CNT_CHANGED: {
		sIMessage_CntChanged cc(msgBase);
		if ((cc._changed.uid || cc._changed.net) && plug->DTgetInt(DTCNT , cc._cntID , CNT_NET) == this->GetNet())
			plug->DTsetStr(DTCNT , cc._cntID , CNT_UID , CheckJID(plug->DTgetStr(DTCNT , cc._cntID , CNT_UID)));
		}
	}
	return cJabberBase::IMessageProc(msgBase);
}
