/* 
   DwuTlenek

   License information can be found in license.txt.
   Please redistribute only as a whole package.
   
   (c)2004 Rafa³ Lindemann | Stamina
   http://www.konnekt.info
*/ 

#pragma once
/*
Nag³ówki kJabbera zawieraj¹ kilka deklaracji, z których korzysta równie¿ DwuTleneK.
*/
#include "konnekt/kJabber.h"
namespace dwuTlenek {

	const int net = NET_TLEN;
	namespace CFG {
/*		const int server = 12100;
		const int user   = 12101;
		const int pass   = 12102;
		const int lastStatus = 12103;
		const int userDescription = 12104;
*/
		const int synchDirection = (net * 1000) + 10;
		const int synchAddContacts = (net * 1000) + 11;
		const int synchRemoveContacts = (net * 1000) + 12;
		const int autoSubscribe = (net * 1000) + 13;
		const int autoAuthorize = (net * 1000) + 14;

		const int pubdirShowStatus = (net * 1000) + 15;
	}
	namespace ACT {
		const int CfgGroup = (net * 1000) + 0;

		/* Identyfikatory statusów te same co w kJabber */
		const int Status = (net * 1000) + 50;


		const int subscription = (net * 1000) + kJabber::ACT::subscription_off;
		const int subscriptionNfo = (net * 1000) + kJabber::ACT::subscriptionNfo_off;
		const int subSubscribed = kJabber::ACT::subSubscribed;
		const int subShow = kJabber::ACT::subShow;
		const int sendStatus = kJabber::ACT::sendStatus;
		const int sendUnavailable = kJabber::ACT::sendUnavailable;

		const int vcardRequest = (net * 1000) + kJabber::ACT::vcardRequest_off;
		const int vcardSet = (net * 1000) + kJabber::ACT::vcardSet_off;

//		const int evtAuthorize = 12010;
	}
	namespace ICO {
		const int logo = 12000;
		const int online = 12001;
		const int away = 12002;
		const int offline = 12003;
	}
}