#include "Configuration/Config.h"
#include "AnticheatMgr.h"
#include "Object.h"
#include "AccountMgr.h"
#include "Chat.h"
#include "Player.h"

int64 resetTime = 0;
int64 lastIterationPlayer = sWorld->GetUptime() + 30;//TODO: change 30 secs static to a configurable option
class AnticheatPlayerScript : public PlayerScript
{
public:
	AnticheatPlayerScript()
		: PlayerScript("AnticheatPlayerScript")
	{
	}

	void OnLogout(Player* player) override
	{
		sAnticheatMgr->HandlePlayerLogout(player);
	}

	void OnLogin(Player* player) override
	{
		sAnticheatMgr->HandlePlayerLogin(player);
		if(sConfigMgr->GetOption<bool>("Anticheat.LoginMessage", true))
			ChatHandler(player->GetSession()).PSendSysMessage("This server is running an Anticheat Module.");
	}
};
class AnticheatWorldScript : public WorldScript
{
public:
	AnticheatWorldScript()
		: WorldScript("AnticheatWorldScript")
	{
	}
	void OnUpdate(uint32 /* diff */) override // unusued parameter
	{
		if (sWorld->GetGameTime() > resetTime)
		{
			sLog->outString( "Anticheat: Resetting daily report states.");
			sAnticheatMgr->ResetDailyReportStates();
			UpdateReportResetTime();
			sLog->outString( "Anticheat: Next daily report reset: %ld", resetTime);
		}
		if (sWorld->GetUptime() > lastIterationPlayer)
		{
			lastIterationPlayer = sWorld->GetUptime() + sConfigMgr->GetOption<uint32>("Anticheat.SaveReportsTime", 60);
			sLog->outString( "Saving reports for %u players.", sWorld->GetPlayerCount());

			for (SessionMap::const_iterator itr = sWorld->GetAllSessions().begin(); itr != sWorld->GetAllSessions().end(); ++itr)
				if (Player* plr = itr->second->GetPlayer())
					sAnticheatMgr->SavePlayerData(plr);
		}
	}
	void OnAfterConfigLoad(bool /* reload */) override // unusued parameter
	{
		sLog->outString("AnticheatModule Loaded.");
	}
	void UpdateReportResetTime()
	{
		resetTime = sWorld->GetNextTimeWithDayAndHour(-1, 6);
	}
};
class AnticheatMovementHandlerScript : public MovementHandlerScript
{
	public:
	AnticheatMovementHandlerScript()
		: MovementHandlerScript("AnticheatMovementHandlerScript")
	{
	}
    void OnPlayerMove(Player* player, MovementInfo mi, uint32 opcode) override
    {
		if (!AccountMgr::IsGMAccount(player->GetSession()->GetSecurity()) || sConfigMgr->GetOption<bool>("Anticheat.EnabledOnGmAccounts", false))
			sAnticheatMgr->StartHackDetection(player, mi, opcode);
    }
};
void startAnticheatScripts()
{
	new AnticheatWorldScript();
	new AnticheatPlayerScript();
	new AnticheatMovementHandlerScript();
}
