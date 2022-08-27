/*
 *MIT License
 *
 *Copyright (c) 2022 Azerothcore
 *
 *Permission is hereby granted, free of charge, to any person obtaining a copy
 *of this software and associated documentation files (the "Software"), to deal
 *in the Software without restriction, including without limitation the rights
 *to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *copies of the Software, and to permit persons to whom the Software is
 *furnished to do so, subject to the following conditions:
 *
 *The above copyright notice and this permission notice shall be included in all
 *copies or substantial portions of the Software.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *SOFTWARE.
 */

#include "Language.h"
#include "ScriptMgr.h"
#include "ObjectMgr.h"
#include "Chat.h"
#include "AnticheatMgr.h"
#include "Configuration/Config.h"
#include "Player.h"
#include "SpellAuras.h"

#if AC_COMPILER == AC_COMPILER_GNU
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

using namespace Acore::ChatCommands;

enum Spells
{
    SHACKLES = 38505,
    LFG_SPELL_DUNGEON_DESERTER = 71041,
    BG_SPELL_DESERTER = 26013,
    SILENCED = 23207
};

class anticheat_commandscript : public CommandScript
{
public:
    anticheat_commandscript() : CommandScript("anticheat_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> anticheatCommandTable =
        {
            { "global",         SEC_GAMEMASTER,     true,    &HandleAntiCheatGlobalCommand,  "" },
            { "player",         SEC_GAMEMASTER,     true,    &HandleAntiCheatPlayerCommand,  "" },
            { "delete",         SEC_ADMINISTRATOR,  true,    &HandleAntiCheatDeleteCommand,  "" },
            { "jail",           SEC_GAMEMASTER,     true,    &HandleAnticheatJailCommand,    "" },
            { "parole",         SEC_ADMINISTRATOR,  true,    &HandleAnticheatParoleCommand,  "" },
            { "purge",          SEC_ADMINISTRATOR,  true,    &HandleAntiCheatPurgeCommand,   "" },
            { "warn",           SEC_GAMEMASTER,     true,    &HandleAnticheatWarnCommand,    "" }
        };

        static std::vector<ChatCommand> commandTable =
        {
            { "anticheat",      SEC_GAMEMASTER,     true,   NULL, "",  anticheatCommandTable},
        };

        return commandTable;
    }

    static bool HandleAnticheatWarnCommand(ChatHandler* handler, Optional<PlayerIdentifier> player)
    {
        if (!sConfigMgr->GetOption<bool>("Anticheat.Enabled", 0))
            return false;

        if (!player)
            player = PlayerIdentifier::FromTarget(handler);
        if (!player || !player->IsConnected())
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        ChatHandler(player->GetConnectedPlayer()->GetSession()).SendSysMessage("The anticheat system has reported several times that you may be cheating. You will be monitored to confirm if this is accurate.");
        return true;
    }

    static bool HandleAnticheatJailCommand(ChatHandler* handler, Optional<PlayerIdentifier> player)
    {
        if (!sConfigMgr->GetOption<bool>("Anticheat.Enabled", 0))
            return false;

        if (!player)
            player = PlayerIdentifier::FromTarget(handler);
        if (!player || !player->IsConnected())
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* pTarget = player->GetConnectedPlayer();

        // teleport both to jail.
        if (!handler->IsConsole())
        {
            handler->GetSession()->GetPlayer()->TeleportTo(1, 16226.5f, 16403.6f, -64.5f, 3.2f);
        }

        WorldLocation loc = WorldLocation(1, 16226.5f, 16403.6f, -64.5f, 3.2f);// GM Jail Location
        pTarget->TeleportTo(loc);
        pTarget->SetHomebind(loc, 876);// GM Jail Homebind location
        pTarget->CastSpell(pTarget, SHACKLES);// shackle him in place to ensure no exploit happens for jail break attempt
        if (Aura* dungdesert = pTarget->AddAura(LFG_SPELL_DUNGEON_DESERTER, pTarget))// LFG_SPELL_DUNGEON_DESERTER
        {
            dungdesert->SetDuration(-1);
        }
        if (Aura* bgdesert = pTarget->AddAura(BG_SPELL_DESERTER, pTarget))// BG_SPELL_DESERTER
        {
            bgdesert->SetDuration(-1);
        }
        if (Aura* silent = pTarget->AddAura(SILENCED, pTarget))// SILENCED
        {
            silent->SetDuration(-1);
        }

        return true;
    }

    static bool HandleAnticheatParoleCommand(ChatHandler* handler, Optional<PlayerIdentifier> player)
    {
        if (!sConfigMgr->GetOption<bool>("Anticheat.Enabled", 0))
            return false;

        if (!player)
            player = PlayerIdentifier::FromTarget(handler);
        if (!player || !player->IsConnected())
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* pTarget = player->GetConnectedPlayer();

        WorldLocation Aloc = WorldLocation(0, -8833.37f, 628.62f, 94.00f, 1.06f);// Stormwind
        WorldLocation Hloc = WorldLocation(1, 1569.59f, -4397.63f, 16.06f, 0.54f);// Orgrimmar

        if (pTarget->GetTeamId() == TEAM_ALLIANCE)
        {
            pTarget->TeleportTo(0, -8833.37f, 628.62f, 94.00f, 1.06f);//Stormwind
            pTarget->SetHomebind(Aloc, 1519);// Stormwind Homebind location
        }
        else
        {
            pTarget->TeleportTo(1, 1569.59f, -4397.63f, 7.7f, 0.54f);//Orgrimmar
            pTarget->SetHomebind(Hloc, 1653);// Orgrimmar Homebind location
        }
        pTarget->RemoveAura(SHACKLES);// remove shackles
        pTarget->RemoveAura(LFG_SPELL_DUNGEON_DESERTER);// LFG_SPELL_DUNGEON_DESERTER
        pTarget->RemoveAura(BG_SPELL_DESERTER);// BG_SPELL_DESERTER
        pTarget->RemoveAura(SILENCED);// SILENCED
        sAnticheatMgr->AnticheatDeleteCommand(pTarget->GetGUID());// deletes auto reports on player
        return true;
    }

    static bool HandleAntiCheatDeleteCommand(ChatHandler* handler, Optional<PlayerIdentifier> player)
    {
        if (!sConfigMgr->GetOption<bool>("Anticheat.Enabled", 0))
            return false;

        if (!player)
            player = PlayerIdentifier::FromTarget(handler);
        if (!player || !player->IsConnected())
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }
        sAnticheatMgr->AnticheatDeleteCommand(player->GetGUID());
        handler->PSendSysMessage("Anticheat players_reports_status deleted for player %s", player->GetName().c_str());
        return true;
    }

    static bool HandleAntiCheatPlayerCommand(ChatHandler* handler, Optional<PlayerIdentifier> player)
    {
        if (!sConfigMgr->GetOption<bool>("Anticheat.Enabled", 0))
            return false;

        if (!player)
        {
            player = PlayerIdentifier::FromTarget(handler);
        }
        if (!player || !player->IsConnected())
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        ObjectGuid guid = player->GetGUID();
        Player* playerTarget = player->GetConnectedPlayer();
        if (playerTarget)
        {
            float average = sAnticheatMgr->GetAverage(guid);
            uint32 total_reports = sAnticheatMgr->GetTotalReports(guid);
            uint32 speed_reports = sAnticheatMgr->GetTypeReports(guid, 0);
            uint32 fly_reports = sAnticheatMgr->GetTypeReports(guid, 1);
            uint32 jump_reports = sAnticheatMgr->GetTypeReports(guid, 3);
            uint32 waterwalk_reports = sAnticheatMgr->GetTypeReports(guid, 2);
            uint32 teleportplane_reports = sAnticheatMgr->GetTypeReports(guid, 4);
            uint32 climb_reports = sAnticheatMgr->GetTypeReports(guid, 5);
            uint32 teleport_reports = sAnticheatMgr->GetTypeReports(guid, 6);
            uint32 ignorecontrol_reports = sAnticheatMgr->GetTypeReports(guid, 7);
            uint32 zaxis_reports = sAnticheatMgr->GetTypeReports(guid, 8);
            uint32 antiswim_reports = sAnticheatMgr->GetTypeReports(guid, 9);
            uint32 gravity_reports = sAnticheatMgr->GetTypeReports(guid, 10);
            uint32 antiknockback_reports = sAnticheatMgr->GetTypeReports(guid, 11);
            uint32 no_fall_damage_reports = sAnticheatMgr->GetTypeReports(guid, 12);
            uint32 op_ack_reports = sAnticheatMgr->GetTypeReports(guid, 13);

            Player* playerTarget = player->GetConnectedPlayer();
            uint32 latency = 0;
            latency = playerTarget->GetSession()->GetLatency();
            if (!handler->IsConsole())
            {
                handler->PSendSysMessage("|cFFFFA500-----------------------------------------------------------------");
                handler->PSendSysMessage("|cFF20B2AAInformation about player: |cffffff00%s", player->GetName().c_str());
                handler->PSendSysMessage("|cffff0000IP Address: |cffffff00%s |cffff0000Latency |cffffff00%u ms", playerTarget->GetSession()->GetRemoteAddress().c_str(), latency);
                handler->PSendSysMessage("|cffff0000Average: |cffffff00%f |cffff0000Total Reports: |cffffff00%u ", average, total_reports);
                handler->PSendSysMessage("|cffff0000Speed Reports: |cffffff00%u |cffff0000Fly Reports: |cffffff00%u |cffff0000Jump Reports: |cffffff00%u ", speed_reports, fly_reports, jump_reports);
                handler->PSendSysMessage("|cffff0000Walk On Water Reports:|cffffff00 %u |cffff0000Teleport To Plane Reports: |cffffff00%u", waterwalk_reports, teleportplane_reports);
                handler->PSendSysMessage("|cffff0000Teleport Reports: |cffffff00%u |cffff0000Climb Reports: |cffffff00%u", teleport_reports, climb_reports);
                handler->PSendSysMessage("|cffff0000Ignore Control Reports: |cffffff00%u |cffff0000Ignore Z-Axis Reports: |cffffff00%u", ignorecontrol_reports, zaxis_reports);
                handler->PSendSysMessage("|cffff0000Ignore Anti-Swim Reports: |cffffff00%u |cffff0000Gravity Reports: |cffffff00%u", antiswim_reports, gravity_reports);
                handler->PSendSysMessage("|cffff0000Anti-Knock Back Reports: |cffffff00%u |cffff0000No Fall Damage Reports: |cffffff00%u", antiknockback_reports, no_fall_damage_reports);
                handler->PSendSysMessage("|cffff0000Op Ack Reports: |cffffff00%u", op_ack_reports);
            }
            if (handler->IsConsole())
            {
                handler->PSendSysMessage("-----------------------------------------------------------------");
                handler->PSendSysMessage("Information about player %s", player->GetName().c_str());
                handler->PSendSysMessage("IP Address: %s || Latency %u ms", playerTarget->GetSession()->GetRemoteAddress().c_str(), latency);
                handler->PSendSysMessage("Average: %f || Total Reports: %u ", average, total_reports);
                handler->PSendSysMessage("Speed Reports: %u || Fly Reports: %u || Jump Reports: %u ", speed_reports, fly_reports, jump_reports);
                handler->PSendSysMessage("Walk On Water Reports: %u  || Teleport To Plane Reports: %u", waterwalk_reports, teleportplane_reports);
                handler->PSendSysMessage("Teleport Reports: %u || Climb Reports: %u", teleport_reports, climb_reports);
                handler->PSendSysMessage("Ignore Control Reports: %u || Ignore Z-Axis Reports: %u", ignorecontrol_reports, zaxis_reports);
                handler->PSendSysMessage("Ignore Anti-Swim Reports: %u || Gravity Reports: %u", antiswim_reports, gravity_reports);
                handler->PSendSysMessage("Anti-Knock Back Reports: %u || No Fall Damage Reports: %u", antiknockback_reports, no_fall_damage_reports);
                handler->PSendSysMessage("Op Ack Reports: %u", op_ack_reports);
            }
            return true;
        }
        else
        {
            return false;
        }
    }

    static bool HandleAntiCheatGlobalCommand(ChatHandler* handler)
    {
        if (!sConfigMgr->GetOption<bool>("Anticheat.Enabled", 0))
        {
            handler->PSendSysMessage("禁用防作弊系统。");
            return true;
        }

        sAnticheatMgr->AnticheatGlobalCommand(handler);

        return true;
    }

    static bool HandleAntiCheatPurgeCommand(ChatHandler* handler)
    {
        sAnticheatMgr->AnticheatPurgeCommand(handler);
        handler->PSendSysMessage("The Anticheat daily_players_reports has been purged.");
        return true;
    }
};

void AddSC_anticheat_commandscript()
{
    new anticheat_commandscript();
}
