/* 
   DwuTlenek

   License information can be found in license.txt.
   Please redistribute only as a whole package.
   
   (c)2004 Rafa³ Lindemann | Stamina
   http://www.konnekt.info
*/ 

#pragma once

namespace dwuTlenek {

	struct TlenConnectionEx : _TlenConnection {
		class cTlen * me;
	};

	class cTlen : public cJabberBase {
	public:
		cTlen(cCtrl * ctrl); // inicjalizacja

	// Podstawowa dzia³alnoœæ

		/* Obs³uga komunikatów Konnekta */

		int IMessageProc(sIMessage_base * msgBase);
//		int ActionCfgProc(sUIActionNotify_base * anBase);
//		int ActionProc(sUIActionNotify_base * anBase);
		void UIPrepare();
		void SetColumns();
		bool CanConnect() {return true;}
		bool Connected() {
			return connection && connection->state != TLEN_STATE_NOT_CONNECTED;
		}

		/* CFG */
		enSynchDirection CFGsynchDirection() {return (enSynchDirection)plug->DTgetInt(DTCFG , 0 , dwuTlenek::CFG::synchDirection);}
		int CFGsynchRemoveContacts() {return plug->DTgetInt(DTCFG , 0 , dwuTlenek::CFG::synchRemoveContacts);}
		int CFGsynchAddContacts() {return plug->DTgetInt(DTCFG , 0 , dwuTlenek::CFG::synchAddContacts);}
		int CFGautoSubscribe() {return plug->DTgetInt(DTCFG , 0 , dwuTlenek::CFG::autoSubscribe);}
		int CFGautoAuthorize() {return plug->DTgetInt(DTCFG , 0 , dwuTlenek::CFG::autoAuthorize);}

		int ACTcfgGroup() {return dwuTlenek::ACT::CfgGroup;}


		/* Interfejs ni¿szego poziomu */

		const CStdString GetUID(); 
		const CStdString GetHost() {return (connection && connection->hostname) ? connection->hostname : "Tlen.pl";}
		int GetPort() {return connection ? connection->port : 0;}
		SOCKET GetSocket();
		const CStdString & GetName() {static CStdString name="Tlen"; return name;}

		static void EventCB (TlenConnectionEx *tc);

		unsigned int ListenThread(void);
		void DisconnectSocket();
		void SetContact(int cntID , TlenRosterItem * item);
		void SetContact(int cntID , void * item) {SetContact(cntID , (TlenRosterItem *) item );}

		void SetRosterItem(int cntID);
		void SetCntIcon(int cntID , int status , const CStdString & jid);
		bool SendPresence(const CStdString & to , int status , const char * info);
		bool SendSubRequest(const CStdString & to , bool subscribe);
		bool SendSubscribed(const CStdString & to , bool subscribed);
		bool SendCntSearch(sCNTSEARCH * sr);
		bool SendVCardRequest(int cntID);
		bool SendVCardSet(bool window);
		bool SendMessage(cMessage * m);
		bool RosterDeleteJID(const CStdString & jid);
		bool RosterContainsJID(const CStdString & JID);
		bool SendPing();
		void SetCntStatusFlag(int cnt , int flag , int mask);

		int FindContact(const CStdString & jid);

		TlenPresenceStatus StatusKonnektToTlen(int status);
		int StatusTlenToKonnekt(TlenPresenceStatus status);


		void OnPresence(TlenEventPresence * ev);
		void OnRoster(TlenEventRoster * ev);
		void OnMessage(TlenEventMessage * ev);
		void OnVCardRequestAnswer(TlenPubdirItem * ev);
		void OnSearchAnswer(TlenEventSearch * ev);
		void OnNewMail(TlenEventMessage * ev);

		static CStdString CheckJID(CStdString jid);

/*
		void OnTransmitXML(const char* c);
		void OnAuthError(int errorcode , const char* errormessage);
		void OnDisconnected();
		void OnConnected(const judo::Element& e);
		void OnRoster(const judo::Element& e);
		void OnMessage(const jabberoo::Message & m);
		void OnPresence(const Presence& presence, Presence::Type previousType);
		void OnPresenceRequest(const Presence & presence);
		void OnVersion(string & name , string & version , string & os);
		void OnLast(string & seconds);
		void OnTime(string & localTime, string & timeZone);
		void OnRosterItemRemoving(Roster::Item &);
		void OnRosterItemUpdated(Roster::Item &);
		void OnRosterItemAdded(Roster::Item &);

		void OnIQSearchReply(const judo::Element&);
		void OnIQVCardRequestReply(const judo::Element&);
		void OnIQVCardSetReply(const judo::Element&);
*/

	private:
		TlenConnectionEx * connection;
		GMainLoop * mainloop;
		list <CStdString> roster; // Lista JIDów w rosterze...
		typedef list<CStdString>::iterator roster_it;
                
	};


}