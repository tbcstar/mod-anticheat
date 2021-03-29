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
		if(sConfigMgr->GetBoolDefault("Anticheat.LoginMessage", true))
			ChatHandler(player->GetSession()).PSendSysMessage("服务器已启用防作弊模块。");
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
			sLog->outString( "防作弊:重置每日报表状态。");
			sAnticheatMgr->ResetDailyReportStates();
			UpdateReportResetTime();
			sLog->outString( "防作弊:下次每日报告重置: %ld", resetTime);
		}
		if (sWorld->GetUptime() > lastIterationPlayer)
		{
			lastIterationPlayer = sWorld->GetUptime() + sConfigMgr->GetIntDefault("Anticheat.SaveReportsTime", 60);
			sLog->outString( "为 %u 个玩家保存报告。", sWorld->GetPlayerCount());

			for (SessionMap::const_iterator itr = sWorld->GetAllSessions().begin(); itr != sWorld->GetAllSessions().end(); ++itr)
				if (Player* plr = itr->second->GetPlayer())
					sAnticheatMgr->SavePlayerData(plr);
		}
	}
	void OnAfterConfigLoad(bool /* reload */) override // unusued parameter
	{
		sLog->outString("防作弊模块加载。");
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
		if (!AccountMgr::IsGMAccount(player->GetSession()->GetSecurity()) || sConfigMgr->GetBoolDefault("Anticheat.EnabledOnGmAccounts", false))
			sAnticheatMgr->StartHackDetection(player, mi, opcode);
    }
};
void startAnticheatScripts()
{
	new AnticheatWorldScript();
	new AnticheatPlayerScript();
	new AnticheatMovementHandlerScript();
}
