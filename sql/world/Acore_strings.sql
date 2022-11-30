DELETE FROM `acore_string` WHERE `entry` IN (30087,30088,30089,30090);
INSERT INTO `acore_string` (`entry`, `content_default`, `locale_koKR`, `locale_frFR`, `locale_deDE`, `locale_zhCN`, `locale_zhTW`, `locale_esES`, `locale_esMX`, `locale_ruRU`) VALUES 
(30087, '|cffffff00[|cffff0000反作弊警报|r|cffffff00]:|r |cFFFF8C00|r |cFFFF8C00[|H玩家：%s|h%s|h|r|cFFFF8C00] - 延迟: %u ms', NULL, NULL, NULL, NULL, NULL, '|cffffff00[|cffff0000反作弊警报|r|cffffff00]:|r |cFFFF8C00|r |cFFFF8C00[|H玩家：%s|h%s|h|r|cFFFF8C00] - 延迟: %u ms', '|cffffff00[|cffff0000反作弊警报|r|cffffff00]:|r |cFFFF8C00|r |cFFFF8C00[|H玩家：%s|h%s|h|r|cFFFF8C00] - 延迟: %u ms', NULL),
(30088, '|cffffff00[|cffff0000反作弊警报|r|cffffff00]:|r 检测到可能的非法传送|cFFFF8C00 %s|r - 延迟: %u ms', NULL, NULL, NULL, NULL, NULL, '|cffffff00[|cffff0000反作弊警报|r|cffffff00]:|r 检测到可能的黑客传送|cFFFF8C00 %s|r - 延迟: %u ms', '|cffffff00[|cffff0000反作弊警报|r|cffffff00]:|r 检测到可能的黑客传送|cFFFF8C00 %s|r - 延迟: %u ms', NULL),
(30089, '|cffffff00[|cffff0000反作弊警报|r|cffffff00]:|r 检测到可能的控制攻击|cFFFF8C00 %s|r - 延迟: %u ms', NULL, NULL, NULL, NULL, NULL, '|cffffff00[|cffff0000反作弊警报|r|cffffff00]:|r 检测到可能被黑客控制|cFFFF8C00 %s|r - 延迟: %u ms', '|cffffff00[|cffff0000反作弊警报|r|cffffff00]:|r 检测到可能被黑客控制|cFFFF8C00 %s|r - 延迟: %u ms', NULL),
(30090, '|cffffff00[|cffff0000反作弊警报|r|cffffff00]:|r 决斗时使用的传送技巧|cFFFF8C00 %s|r - 延迟: %u ms vs |cFFFF8C00 %s|r - 延迟: %u ms.', NULL, NULL, NULL, NULL, NULL, '', '', NULL);

DELETE FROM `command` WHERE  `name`='anticheat jail';
DELETE FROM `command` WHERE  `name`='anticheat delete';
DELETE FROM `command` WHERE  `name`='anticheat player';
DELETE FROM `command` WHERE  `name`='anticheat';
DELETE FROM `command` WHERE  `name`='anticheat global';
DELETE FROM `command` WHERE  `name`='anticheat parole';
DELETE FROM `command` WHERE  `name`='anticheat purge';
DELETE FROM `command` WHERE  `name`='anticheat warn';
INSERT INTO `command` (`name`, `security`, `help`) VALUES 
('anticheat', 2, '语法：.anticheat\r\n\r\n如果您拥有必要的权限，则显示反作弊命令的访问级别。'),
('anticheat global', 2, '语法：.anticheat global\r\n\r\n显示反作弊是否在全局统计中处于活动状态。'),
('anticheat player', 2, '语法：.anticheat player [$charactername]\r\n\r\n显示玩家当前会话的反作弊统计。'),
('anticheat delete', 3, '语法：.anticheat delete [$charactername]\r\n\r\n删除当前玩家会话的反作弊统计。'),
('anticheat jail', 2, '语法：.anticheat jail [$charactername]\r\n\r\n监禁和限制玩家，并将 GM cmd 用户无限制地传送到监狱'),
('anticheat parole', 3, '语法：.anticheat parole [$charactername]\r\n\r\n删除反作弊数据，解除监狱限制，发送至玩家阵营。'),
('anticheat purge', 3, '语法：.anticheat purge\r\n\r\n删除 daily_players_reports 表中存储的统计信息。'),
('anticheat warn', 2, '语法：.anticheat warn [$charactername]\r\n\r\n向个别玩家发送他们正在被监控的可能作弊行为。');
