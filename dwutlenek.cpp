/* 
   DwuTlenek

   License information can be found in license.txt.
   Please redistribute only as a whole package.
   
   (c)2004 Rafa³ Lindemann | Stamina
   http://www.konnekt.info
*/ 

#include "stdafx.h"
#include "resource.h"
#include <Konnekt/lib.h>

/*
#ifdef _DEBUG
#pragma comment(lib , "../../__libs/jabberoo_d.lib")
#else
#pragma comment(lib , "../../__libs/jabberoo.lib")
#endif
*/

#pragma comment(lib , "Ws2_32.lib")
#pragma comment(lib , "libxml2.lib")
/*#pragma comment(lib , "zlibstat.lib")*/
//#pragma comment(lib , "iconv.lib")
#pragma comment(lib , "glib-2.0.lib")
#pragma comment(lib , "gthread-2.0.lib")
#ifdef _DEBUG
   #pragma comment(lib , "gnet_d.lib")
#else
   #pragma comment(lib , "gnet.lib")
#endif
using namespace kJabber;

cTlen * tlen = 0;

/*HACK: GNet DllMain */
/*
extern "C" {
	BOOL WINAPI 
	GNet_DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);
}

int __stdcall DllMain(void * hinstDLL, unsigned long fdwReason, void * lpvReserved)
{
	return GNet_DllMain((HINSTANCE)hinstDLL , fdwReason , lpvReserved);
}
*/

//---------------------------------------------------------------------------
int Init() {
	WSADATA wsaData;
    WSAStartup( MAKEWORD( 2, 0 ), &wsaData );
	tlen_init ();
	tlen_debug_set (TLEN_DEBUG_HIGH);
	tlen = new cTlen(Ctrl);
    return 1;
}

int DeInit() {
	delete tlen;
	tlen = 0;
    return 1;
}

int IStart() {
	return 1;
}
int IEnd() {
  /* Tutaj wtyczkê wy³¹czamy */


	return 1;
}

int ISetCols() {
  /* Tutaj rejestrujemy kolumny w plikach ustawieñ.
     Pamiêtaj o tym ¿e identyfikatory MUSZ¥ byæ UNIKATOWE? */
	SetColumn(DTCFG , dwuTlenek::CFG::synchAddContacts , DT_CT_INT , 1 , "dwuTlenek/synch/addContacts");
	SetColumn(DTCFG , dwuTlenek::CFG::synchRemoveContacts , DT_CT_INT , 1 , "dwuTlenek/synch/removeContacts");
	SetColumn(DTCFG , dwuTlenek::CFG::autoAuthorize , DT_CT_INT , 1 , "dwuTlenek/auto/authorize");
	SetColumn(DTCFG , dwuTlenek::CFG::autoSubscribe , DT_CT_INT , 1 , "dwuTlenek/auto/subscribe");
	SetColumn(DTCFG , dwuTlenek::CFG::synchDirection , DT_CT_INT , kJabber::sd_toServer , "dwuTlenek/synch/direction");
	SetColumn(DTCFG , dwuTlenek::CFG::pubdirShowStatus , DT_CT_INT , 0 , "dwuTlenek/pubdir/showStatus");
	return 1;
}

/* Wstawiamy akcje Subskrypcji */
void IPrepare_subscription(int parent) {
	UIActionAdd(parent , dwuTlenek::ACT::subShow , ACTT_CHECK , "Wysy³aj status" , 0);
	UIActionAdd(parent , dwuTlenek::ACT::subSubscribed , ACTT_CHECK , "Pobieraj status" , 0);
/*	UIActionAdd(parent , 0 , ACTT_SEPARATOR);
	UIActionAdd(parent , dwuTlenek::ACT::sendStatus , ACTR_INIT , "Wyœlij status" , 0);
	UIActionAdd(parent , dwuTlenek::ACT::sendUnavailable , 0 , "Ukryj (jednorazowo)" , UIIcon(IT_STATUS , kJabber::net , ST_OFFLINE , 0));
*/
}

int IPrepare() {
	IconRegister(IML_16 , UIIcon(IT_LOGO , dwuTlenek::net , 0 , 0) , Ctrl->hDll() , IDI_LOGO);
	IconRegister(IML_16 , UIIcon(IT_STATUS , dwuTlenek::net , ST_ONLINE , 0) , Ctrl->hDll() , IDI_ST_ONLINE);
	IconRegister(IML_16 , UIIcon(IT_STATUS , dwuTlenek::net , ST_AWAY , 0) , Ctrl->hDll() , IDI_ST_AWAY);
	IconRegister(IML_16 , UIIcon(IT_STATUS , dwuTlenek::net , ST_OFFLINE , 0) , Ctrl->hDll() , IDI_ST_OFFLINE);
	IconRegister(IML_16 , UIIcon(IT_STATUS , dwuTlenek::net , ST_AWAY , 0) , Ctrl->hDll() , IDI_ST_AWAY);
	IconRegister(IML_16 , UIIcon(IT_STATUS , dwuTlenek::net , ST_NA , 0) , Ctrl->hDll() , IDI_ST_NA);
	IconRegister(IML_16 , UIIcon(IT_STATUS , dwuTlenek::net , ST_DND , 0) , Ctrl->hDll() , IDI_ST_DND);
	IconRegister(IML_16 , UIIcon(IT_STATUS , dwuTlenek::net , ST_CHAT , 0) , Ctrl->hDll() , IDI_ST_CHAT);
	IconRegister(IML_16 , UIIcon(IT_STATUS , dwuTlenek::net , ST_HIDDEN , 0) , Ctrl->hDll() , IDI_ST_HIDDEN);
	IconRegister(IML_16 , UIIcon(IT_STATUS , dwuTlenek::net , ST_CONNECTING , 0) , Ctrl->hDll() , IDI_ST_CONNECTING);
	IconRegister(IML_ICO , UIIcon(IT_OVERLAY , dwuTlenek::net , OVR_NETLOGO , 0) , Ctrl->hDll() , IDI_OVERLAY);

	UIGroupAdd(IMIG_CNT , dwuTlenek::ACT::subscription , ACTR_INIT , "Subskrypcja" , UIIcon(IT_LOGO , dwuTlenek::net , 0 , 0));
	IPrepare_subscription(dwuTlenek::ACT::subscription);
	UIGroupAdd(IMIG_NFO_TB , dwuTlenek::ACT::subscriptionNfo , ACTR_INIT , "Subskrypcja" , UIIcon(IT_LOGO , dwuTlenek::net , 0 , 0));
	IPrepare_subscription(dwuTlenek::ACT::subscriptionNfo);

	UIActionAdd(IMIG_NFO_SAVE , dwuTlenek::ACT::vcardSet , ACTR_INIT , "Zapisz jako wizytówkê (Tlen)", UIIcon(IT_LOGO , dwuTlenek::net , 0 , 0));
	UIActionAdd(IMIG_NFO_REFRESH , dwuTlenek::ACT::vcardRequest , ACTR_INIT , "Pobierz wizytówkê (Tlen)", UIIcon(IT_LOGO , dwuTlenek::net , 0 , 0));

//	IconRegister(IML_16 , UIIcon(IT_MESSAGE , kJabber::net , MT_MESSAGE , 0) , Ctrl->hDll() , IDI_MT_MESSAGE);

	UIGroupAdd(IMIG_CFG_USER , dwuTlenek::ACT::CfgGroup , 0 , "Tlen" , UIIcon(IT_LOGO , dwuTlenek::net , 0 , 0));
	UIActionCfgAddPluginInfoBox2(dwuTlenek::ACT::CfgGroup, 
			"<b>DwuTlenek</b> pozwala rozmawiaæ z u¿ytkownikami sieci <b>Tlen.pl</b>."
			AP_MINWIDTH "340"
			, "Za obs³ugê protoko³u odpowiada biblioteka <b>LibTlen2</b> (http://www.libtlen.eu.org/?page=libtlen2)"
			", która korzysta z bibliotek <b>GLib</b>, <b>GNet</b> i <b>DomXML2</b>."
			"<br/>Kod Ÿród³owy jest na licencji MIT/LGPL."
			"<br/><br/>Copyright ©2004 <b>Stamina</b>."
			, "res://dll/630.ico", -3);
	if (ShowBits::checkBits(ShowBits::showInfoNormal)) {
		UIActionCfgAddInfoBox(dwuTlenek::ACT::CfgGroup, "Uwaga!"
			, "Obs³uga protoko³u ogranicza siê do przesy³ania wiadomoœci, statusów i autoryzacji. Do zak³adania konta, zmiany has³a, przesy³ania plików, czy ³¹czenia siê przez proxy bêdziesz musia³ u¿yæ innego klienta."
			, ICON_WARNING, -3, 32);
	}

	tlen->UIPrepare();

	if (ShowBits::checkLevel(ShowBits::levelNormal)) {
	UIActionAdd(dwuTlenek::ACT::CfgGroup , 0 , ACTT_GROUP ,  "Synchronizacja listy kontaktów");{
	//		UIActionAdd(kJabber::ACT::CfgGroup , 0 , ACTT_INFO , "");
			UIActionAdd(dwuTlenek::ACT::CfgGroup , 0 , ACTT_COMBO | ACTSCOMBO_BYPOS | ACTSCOMBO_LIST ,  "Nie synchronizuj" CFGICO "116" "\nSynchronizuj do serwera" CFGICO "0x23F00000" "\nSynchronizuj do Konnekta" CFGICO "0x65" 
				AP_PARAMS AP_TIPRICH "W normalnych warunkach powinieneœ u¿ywaæ synchronizacji <b>do serwera</b>. Pamiêtaj jednak, ¿e w tej sytuacji zmiany na liœcie kontaktów w trybie offline zostan¹ skasowane po po³¹czeniu z serwerem!"
				"<br/><div class=\"warn\">Wy³¹czenie synchronizacji mo¿e zupe³nie uniemo¿liwiæ porozumiewanie siê!</div>" 
				, dwuTlenek::CFG::synchDirection , 170);
			if (ShowBits::checkLevel(ShowBits::levelAdvanced)) {
				UIActionAdd(dwuTlenek::ACT::CfgGroup , 0 , ACTT_CHECK , "Dodawaj kontakty na synchronizowanej liœcie" , dwuTlenek::CFG::synchAddContacts);
				UIActionAdd(dwuTlenek::ACT::CfgGroup , 0 , ACTT_CHECK , "Usuwaj kontakty z synchronizowanej listy" , dwuTlenek::CFG::synchAddContacts);
			}
		} UIActionAdd(dwuTlenek::ACT::CfgGroup , 0 , ACTT_GROUPEND);
	}
	if (ShowBits::checkLevel(ShowBits::levelIntermediate)) {
		UIActionAdd(dwuTlenek::ACT::CfgGroup , 0 , ACTT_GROUP ,  "Autoryzacja");{
			if (ShowBits::checkBits(ShowBits::showInfoBeginner)) {
				UIActionAdd(dwuTlenek::ACT::CfgGroup , 0 , ACTT_TIPBUTTON | ACTSC_INLINE , AP_TIP  AP_TIPIMAGEURL "file://%KonnektData%/img/co2_auth.png");
				UIActionAdd(dwuTlenek::ACT::CfgGroup , 0 , ACTT_INFO , "Bez autoryzacji nie bêdziesz widzia³ czyjegoœ statusu. Jej stan mo¿esz podejrzeæ/zmieniæ bezpoœrednio na liœcie kontaktów." , 0, 0, -2);
			}
			UIActionAdd(dwuTlenek::ACT::CfgGroup , 0 , ACTT_CHECK , "Automatycznie pytaj o autoryzacjê nowe kontakty" , dwuTlenek::CFG::autoSubscribe);
			UIActionAdd(dwuTlenek::ACT::CfgGroup , 0 , ACTT_CHECK , "Automatycznie potwierdzaj autoryzacjê" , dwuTlenek::CFG::autoAuthorize);
		} UIActionAdd(dwuTlenek::ACT::CfgGroup , 0 , ACTT_GROUPEND);
		UIActionAdd(dwuTlenek::ACT::CfgGroup , 0 , ACTT_GROUP ,  "Katalog u¿ytkowników");{
			UIActionAdd(dwuTlenek::ACT::CfgGroup , 0 , ACTT_CHECK , "Pokazuj mój status w katalogu u¿ytkowników" AP_TIP "Ustawienie zostanie zastosowane nastêpnym razem, gdy zapiszesz informacje o sobie w katalogu Tlen.pl" , dwuTlenek::CFG::pubdirShowStatus);
		} UIActionAdd(dwuTlenek::ACT::CfgGroup , 0 , ACTT_GROUPEND);
	}
    return 1;
}

int ActionCfgProc(sUIActionNotify_base * anBase) {
  /* Tutaj obs³ugujemy akcje dla okna konfiguracji */ 
  /* Sytuacja taka sama jak przy ActionProc  */
  sUIActionNotify_2params * an = (anBase->s_size>=sizeof(sUIActionNotify_2params))?static_cast<sUIActionNotify_2params*>(anBase):0;
  switch (anBase->act.id & ~IMIB_CFG) {
  default:
	  return tlen->ActionCfgProc(anBase);
  }
  return 0;
}

ActionProc(sUIActionNotify_base * anBase) {
  /* Tutaj obs³ugujemy akcje */ 
  /* Poni¿sza linijka s³u¿y TYLKO waszej wygodzie! 
     Wiêkszoœæ (o ile nie wszystkie) powiadomieñ przesy³ana jest jako sUIActionNotify_2params,
     korzystamy wtedy z obiektu an, w przeciwnym razie z anBase, lub castujemy do spodziewanego typu.
  */
  sUIActionNotify_2params * an = (anBase->s_size>=sizeof(sUIActionNotify_2params))?static_cast<sUIActionNotify_2params*>(anBase):0;

  if ((anBase->act.id & IMIB_) == IMIB_CFG) return ActionCfgProc(anBase); 
  switch (anBase->act.id) {
  default:
	  return tlen->ActionProc(anBase);
  }
  return 0;
}



int __stdcall IMessageProc(sIMessage_base * msgBase) {

	sIMessage_2params * msg = (msgBase->s_size>=sizeof(sIMessage_2params))?static_cast<sIMessage_2params*>(msgBase):0;
    switch (msgBase->id) {
    /* Wiadomoœci na które TRZEBA odpowiedzieæ */
	case IM_PLUG_NET:        return dwuTlenek::net; // Zwracamy wartoœæ NET, która MUSI byæ ró¿na od 0 i UNIKATOWA!
	case IM_PLUG_TYPE:       return IMT_CONFIG | IMT_CONTACT | IMT_MESSAGE | IMT_MSGUI | IMT_NET | IMT_NETSEARCH | IMT_NETUID | IMT_PROTOCOL | IMT_UI; // Zwracamy jakiego typu jest nasza wtyczka (które wiadomoœci bêdziemy obs³ugiwaæ)
    case IM_PLUG_VERSION:    return 0; // Wersja wtyczki tekstowo major.minor.release.build ...
    case IM_PLUG_SDKVERSION: return KONNEKT_SDK_V;  // Ta linijka jest wymagana!
    case IM_PLUG_SIG:        return (int)"KTLEN"; // Sygnaturka wtyczki (krótka, kilkuliterowa nazwa)
    case IM_PLUG_CORE_V:     return (int)"W98"; // Wymagana wersja rdzenia
    case IM_PLUG_UI_V:       return 0; // Wymagana wersja UI
    case IM_PLUG_NAME:       return (int)"dwuTleneK"; // Pe³na nazwa wtyczki
    case IM_PLUG_NETNAME:    return (int)"Tlen"; // Nazwa obs³ugiwanej sieci (o ile jak¹œ sieæ obs³uguje)
	case IM_PLUG_NETSHORTNAME: return (int)"Tlen";
	case IM_PLUG_UIDNAME:    return (int)"TlenID";
    case IM_PLUG_INIT:       Plug_Init(msg->p1,msg->p2);return Init();
    case IM_PLUG_DEINIT:     Plug_Deinit(msg->p1,msg->p2);return DeInit();
    case IM_PLUG_PRIORITY:   return PLUGP_LOW;

    case IM_SETCOLS:         ISetCols();break;

	case IM_UI_PREPARE:      IPrepare();  return 1;
	case IM_START:           IStart();  break;
    case IM_END:             IEnd();  break; 

    case IM_UIACTION:        return ActionProc((sUIActionNotify_base*)msg->p1);

    /* Tutaj obs³ugujemy wszystkie pozosta³e wiadomoœci */
	}
	if (tlen) {
		return tlen->IMessageProc(msgBase);
	} else {
        if (Ctrl) Ctrl->setError(IMERROR_NORESULT);
		return 0;
	}
}

