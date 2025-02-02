/*
 * gipPlayfab.cpp
 *
 *  Created on: 13 Sep 2021
 *      Author: noyan
 *      		Sinem Algul
*      			Furkan Celenk
 */

#include "gipPlayfab.h"

std::string gipPlayfab::myid;
std::string gipPlayfab::mytitleid;
std::string gipPlayfab::myentitytype;
std::string gipPlayfab::matchid;
std::string gipPlayfab::myqueuename;
std::string gipPlayfab::createdticketid;

bool gipPlayfab::ticketidreceived = false;
bool gipPlayfab::matchfound = false;
bool gipPlayfab::finished = false;
const std::string gipPlayfab::titleID = "6AFF4";

gipPlayfab::gipPlayfab() {
	myid = "";
	mytitleid = "";
	myentitytype = "";
	matchid = "";
	myqueuename = "TestQueue";
	ticketidreceived = false;
	matchfound = false;
	PlayFabSettings::staticSettings->titleId = titleID;
}

gipPlayfab::~gipPlayfab() {
}

void gipPlayfab::registerPlayFabAccount(const std::string& username, const std::string& email, const std::string& password, const std::string& displayName) {
	RegisterPlayFabUserRequest request;
	request.DisplayName = displayName;
	request.Email = email;
	request.Password = password;
	request.Username = username;

	PlayFabClientAPI::RegisterPlayFabUser(request, OnRegisterSuccess, OnRegisterFail);
}

void gipPlayfab::login(const std::string& username, const std::string& password) {
	LoginWithPlayFabRequest request;
	request.Username = username;
	request.Password = password;

	PlayFabClientAPI::LoginWithPlayFab(request, OnLoginSuccess, OnLoginFail);

	while (PlayFabClientAPI::Update() != 0) sleep(1);
}

void gipPlayfab::createMatchmakingTicket(const int& giveupafterseconds) {
	CreateMatchmakingTicketRequest request;
	MatchmakingPlayer creator;
	MultiplayerModels::EntityKey entity;
	MatchmakingPlayerAttributes attributes;
	Json::Value dataObject;

	entity.Id = mytitleid;
	entity.Type = myentitytype;

	dataObject["Skill"] = 24.4;
	attributes.DataObject = dataObject;

	creator.Entity = entity;
	creator.Attributes = attributes;

	request.Creator = creator;
	request.GiveUpAfterSeconds = giveupafterseconds;
	request.QueueName = myqueuename;

	PlayFabMultiplayerAPI::CreateMatchmakingTicket(request, OnMatchmakingTicketCreated, OnMatchmakingTicketFail);

	while (PlayFabMultiplayerAPI::Update() != 0) sleep(1);
}

void gipPlayfab::checkMatchmakingTicket(const std::string& ticketid) {
	 while (true) {
		GetMatchmakingTicketRequest request;

		request.TicketId = ticketid;
		request.QueueName = myqueuename;

		PlayFabMultiplayerAPI::GetMatchmakingTicket(request, OnCheckMatchmakingTicket, OnCheckMatchmakingFail);

		while (PlayFabMultiplayerAPI::Update() != 0) sleep(1);
		sleep(6);
	}
}

void gipPlayfab::getMatch() {
	GetMatchRequest request;

	request.MatchId = matchid;
	request.QueueName = myqueuename;

	PlayFabMultiplayerAPI::GetMatch(request, OnGetMatchResult, OnGetMatchFail);

	while (PlayFabMultiplayerAPI::Update() != 0) sleep(1);
}

void gipPlayfab::OnGetMatchResult(const GetMatchResult& result, void* customData) {
	 gLogi("gipPlayfab") << "OnGetMatchResult";
}

void gipPlayfab::OnGetMatchFail(const PlayFabError& error, void* customData) {
	gLoge("gipPlayfab") << "getMatch failed. \n";
	gLoge("gipPlayfab") << error.GenerateErrorReport();
}

void gipPlayfab::OnCheckMatchmakingTicket(const GetMatchmakingTicketResult& result, void* customData) {
       if (result.Status == "Matched") {
    	   gLogi("gipPlayfab") << "Match Found. " << result.MatchId;
    	   matchfound = true ;
    	   matchid = result.MatchId;
	   } else if (result.Status == "WaitingForMatch") {
		   gLogi("gipPlayfab") << "Waiting for Match...";
	   } else if (result.Status == "WaitingForPlayers") {
		   gLogi("gipPlayfab") << "Waiting for Players...";
	   } else if (result.Status == "WaitingForServer...") {
		   gLogi("gipPlayfab") <<  "Waiting for Server";
	   } else if (result.Status == "Canceled") {
		   gLogi("gipPlayfab") << "Canceled";
		   createdticketid = "";
	   } else {
		   gLogi("gipPlayfab") << "Not Found";
	   }
}


void gipPlayfab::OnCheckMatchmakingFail(const PlayFabError& error, void* customData) {
    gLoge("gipPlayfab") << "Matchmaking failed.\n";
    gLoge("gipPlayfab") << "Here's some debug information:\n";
    gLoge("gipPlayfab") << error.GenerateErrorReport();
}

void gipPlayfab::OnMatchmakingTicketCreated(const CreateMatchmakingTicketResult& result, void* customData) {
    gLogi("gipPlayfab") << "Matchmaking Ticket created. " << "Your Ticket ID is: " << result.TicketId;
    createdticketid = result.TicketId;
    ticketidreceived = true;
}

void gipPlayfab::OnMatchmakingTicketFail(const PlayFabError& error, void* customData) {
    gLoge("gipPlayfab") << "Problem occurred while creating matchmaking ticket.\n";
    gLoge("gipPlayfab") << "Here's some debug information:\n";
    gLoge("gipPlayfab") << error.GenerateErrorReport();
}

void gipPlayfab::OnLoginSuccess(const LoginResult& result, void* customData) {
	gLogi("gipPlayfab") << "Login successful. " << "Session Information: " << result.SessionTicket << " \n";
	myid = result.PlayFabId;
	mytitleid = result.EntityToken->Entity->Id;
	myentitytype = result.EntityToken->Entity->Type;
	finished = true;
}

void gipPlayfab::OnLoginFail(const PlayFabError& error, void* customData) {
	gLoge("gipPlayfab") << "Something went wrong with your first API call.\n";
	gLoge("gipPlayfab") << "Here's some debug information:\n";
	gLoge("gipPlayfab") << error.GenerateErrorReport();
	finished = true;
}

void gipPlayfab::OnRegisterSuccess(const RegisterPlayFabUserResult& result, void* customData) {
	gLogi("gipPlayfab") << "Register Success user: " << result.Username << " id: " << result.PlayFabId;
}

void gipPlayfab::OnRegisterFail(const PlayFabError& error, void* customData) {
	gLoge("gipPlayfab") << "Something went wrong with register.\n";
	gLoge("gipPlayfab") << "Here's some debug information:\n";
	gLoge("gipPlayfab") << error.GenerateErrorReport();
}

bool gipPlayfab::isTicketIdReceived() {
	return ticketidreceived;
}

bool gipPlayfab::isMatchFound() {
	return matchfound;
}

std::string gipPlayfab::getMatchId() {
	return matchid;
}

std::string gipPlayfab::getTicketId() {
	return createdticketid;
}
