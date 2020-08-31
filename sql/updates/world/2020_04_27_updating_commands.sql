-- REMOVE DOT FROM COMMAND NAME
DELETE FROM `command` WHERE `name` LIKE '%ticket closedlist';
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('ticket closedlist', 1, 'Displays a list of closed GM tickets.');

-- RESTRUCTURE SEND COMMAND
DELETE FROM `command` WHERE `name` LIKE '%send%' AND `name` LIKE '%items%';
DELETE FROM `command` WHERE `name` LIKE '%send%' AND `name` LIKE '%mail%';
DELETE FROM `command` WHERE `name` LIKE '%send%' AND `name` LIKE '%message%';
DELETE FROM `command` WHERE `name` LIKE '%send%' AND `name` LIKE '%money%';
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('send items', 3, 'Syntax: .send items #playername "#subject" "#text" itemid1[:count1] itemid2[:count2] ... itemidN[:countN]\r\nSend a mail to a player. Subject and mail text must be in "". If for itemid not provided related count values then expected 1, if count > max items in stack then items will be send in required amount stacks. All stacks amount in mail limited to 12.');
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('send mail', 1, 'Syntax: .send mail #playername "#subject" "#text"\r\nSend a mail to a player. Subject and mail text must be in "".');
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('send money', 3, 'Syntax: .send money #playername "#subject" "#text" #money\r\nSend mail with money to a player. Subject and mail text must be in "".');
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('send message', 3, 'Syntax: .send message $playername $message\r\nSend screen message to player from ADMINISTRATOR.');

-- NEW NPC INFO ADDED, EQUIPMENT
DELETE FROM `oregon_string` WHERE `entry`=616;
INSERT INTO `oregon_string` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`) VALUES (616, 'EquipmentId: %u (Original: %u).', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

-- NEW NPC NEAR COMMAND ADDED
DELETE FROM `oregon_string` WHERE `entry`=617;
INSERT INTO `oregon_string` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`) VALUES (617, 'Found near creatures (distance %f): %u ', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
DELETE FROM `command` WHERE `name`='npc near';
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('npc near', 2, 'Syntax: .npc near #distance\r\nSee all NPCs near you.');

-- NEW NPC ADD TEMP COMMAND
DELETE FROM `command` WHERE `name` LIKE '%npc%' AND `name` LIKE '%add%' AND `name` LIKE '%temp%';
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('npc add temp', 3, 'Syntax: .npc add temp #creatureid\r\nAdds temporary NPC, not saved to database.');

-- NEW NPC ADD ITEM COMMAND
DELETE FROM `command` WHERE `name` LIKE '%npc%' AND `name` LIKE '%add%' AND `name` LIKE '%item%';
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('npc add item', 3, 'Syntax: .npc add item #itemId <#maxcount><#incrtime><#extendedcost>\r\nAdd item #itemid to item list of selected vendor. Also optionally set max count item in vendor item list and time to item count restoring and items ExtendedCost.');

-- NEW NPC ADD FORMATION COMMAND
DELETE FROM `command` WHERE `name` LIKE '%npc%' AND `name` LIKE '%add%' AND `name` LIKE '%formation%';
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('npc add formation', 3, 'Syntax: .npc add formation #leader\r\nAdd selected creature to a leader\'s formation with formationAI.');


-- CHANGED ALLOW MOVE TO .npc set allowmove
DELETE FROM `command` WHERE `name` LIKE '%npc%' AND `name` LIKE '%allowmove%';
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('npc set allowmove', 3, 'Syntax: .npc set allowmove\r\nEnable or disable movement for the selected creature.');

-- NEW NPC SET ENTRY COMMAND
DELETE FROM `command` WHERE `name` LIKE '%npc%' AND `name` LIKE '%set%' AND `name` LIKE '%entry%';
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('npc set entry', 3, 'Syntax: .npc set entry $entry\r\nSwitch selected creature with another entry from creature_template. - New creature.id value not saved to DB.');

-- NEW NPC SET LEVEL COMMAND
DELETE FROM `command` WHERE `name` LIKE '%npc%' AND `name` LIKE '%level%' AND ( `name` LIKE '%set%' or `name` LIKE '%change%' );
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('npc set level', 3, 'Syntax: .npc set level #level\r\nChange the level of the selected creature to #level.\r\n\r\n#level may range from 1 to 63.');

-- NEW SECURITY FOR .npc add
DELETE FROM `command` WHERE `name`='npc add';
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('npc add', 3, 'Syntax: .npc add #creatureid\r\nSpawn a creature by the given template id of #creatureid.');

-- NEW SECURITY FOR .npc move
DELETE FROM `command` WHERE `name`='npc move';
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('npc move', 3, 'Syntax: .npc move [#creature_guid]\r\nMove the targeted creature spawn point to your coordinates.');

-- NEW NPC SET MODEL
DELETE FROM `command` WHERE `name` LIKE '%npc%' AND `name` LIKE '%set%' AND `name` LIKE '%model%';
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('npc set model', 3, 'Syntax: .npc set model #displayid\r\nChange the model id of the selected creature to #displayid.');

-- NEW NPC SET LINK .npc set link
DELETE FROM `command` WHERE `name` LIKE '%npc%' AND `name` LIKE '%set%' AND `name` LIKE '%link%';
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('npc set link', 3, 'Syntax: .npc set link $creatureGUID\r\nLinks respawn of selected creature to the condition that $creatureGUID defined is alive.');

-- NEW NPC SET MOVE TYPE
DELETE FROM `command` WHERE `name` LIKE '%npc%' AND `name` LIKE '%set%' AND `name` LIKE '%movetype%';
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('npc set movetype', 3, 'Syntax: .npc set movetype [#creature_guid] stay/random/way [NODEL]\r\nSet for creature pointed by #creature_guid (or selected if #creature_guid not provided) movement type and move it to respawn position (if creature alive). Any existing waypoints for creature will be removed from the database if you do not use NODEL. If the creature is dead then movement type will applied at creature respawn.\r\nMake sure you use NODEL, if you want to keep the waypoints.');

-- NEW NPC SET FLAG
DELETE FROM `command` WHERE `name` LIKE '%npc%' AND `name` LIKE '%flag%';
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('npc set flag', 3, 'Syntax: .npc set flag #npcflag\r\nSet the NPC flags of creature template of the selected creature and selected creature to #npcflag. NPC flags will applied to all creatures of selected creature template after server restart or grid unload/load.');

-- NEW NPC SPAWNTIME
DELETE FROM `command` WHERE `name` LIKE '%npc%' AND `name` LIKE '%spawntime%';
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('npc set spawntime', 3, 'Syntax: .npc spawntime #time \r\n\r\nAdjust spawntime of selected creature to time.');

-- NEW NPC FACTION .npc set faction perm/original/temp
DELETE FROM `command` WHERE `name` LIKE '%npc%' AND `name` LIKE '%faction%';
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('npc set faction permanent', 3, 'Syntax: .npc set faction permanent #factionid\r\nSet the faction of the selected creature to #factionid.');
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('npc set faction temp', 3, 'Syntax: .npc set faction temp #factionid\r\nSet the faction of the selected creature to #factionid until server restart.');
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('npc set faction original', 3, 'Syntax: .npc set faction original\r\nSet the faction of the selected creature to the default factionid.');

-- NEW NPC FOLLOW
DELETE FROM `command` WHERE `name` LIKE '%npc%' AND `name` LIKE '%follow%';
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('npc follow', 3, 'Syntax: .npc follow\r\nSelected creature start follow you until death/fight/etc.');
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('npc follow stop', 3, 'Syntax: .npc follow stop\r\nSelected creature (non pet) stop follow you.');

-- NEW NPC DELETE
DELETE FROM `command` WHERE `name` LIKE '%npc%' AND `name` LIKE '%delete%';
DELETE FROM `command` WHERE `name`='npc delitem';
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('npc delete item', 3, 'Syntax: .npc delete item #itemId\r\nRemove item #itemid from item list of selected vendor.');
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('npc delete', 3, 'Syntax: .npc delete [#guid]\r\nDelete creature with guid #guid (or the selected if no guid is provided)');

-- NEW GO TAXINODE COMMAND
DELETE FROM `oregon_string` WHERE `entry`=288;
INSERT INTO `oregon_string` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`) VALUES (288, 'TaxiNode ID %u not found!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
DELETE FROM `command` WHERE `name`='go taxinode';
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('go taxinode', 1, 'Syntax: .go taxinode #taxinode\r\nTeleport player to taxinode coordinates. You can look up zone using .lookup taxinode $namepart');

-- NEW GO SECURITY LEVELS
DELETE FROM `command` WHERE `name`='go creature' or `name`='go graveyard' or `name`='go grid' or `name`='go object' or `name`='go taxinode' or `name`='go ticket' or `name`='go trigger' or `name`='go xy' or `name`='go xyz' or `name`='go zonexy' or `name`='go';
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('go creature', 1, 'Syntax: .go creature #creature_guid\r\nTeleport your character to creature with guid #creature_guid.\r\n.gocreature #creature_name\r\nTeleport your character to creature with this name.\r\n.gocreature id #creature_id\r\nTeleport your character to a creature that was spawned from the template with this entry.\r\n*If* more than one creature is found, then you are teleported to the first that is found inside the database.');
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('go graveyard', 1, 'Syntax: .go graveyard #graveyardId\r\n Teleport to graveyard with the graveyardId specified.');
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('go grid', 1, 'Syntax: .go grid #gridX #gridY [#mapId]\r\nTeleport the gm to center of grid with provided indexes at map #mapId (or current map if it not provided).');
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('go object', 1, 'Syntax: .go object #object_guid\r\nTeleport your character to gameobject with guid #object_guid');
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('go taxinode', 1, 'Syntax: .go taxinode #taxinode\r\nTeleport player to taxinode coordinates. You can look up zone using .lookup taxinode $namepart');
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('go ticket', 1, 'Syntax: .go ticket #ticketid\r\nTeleports the user to the location where $ticketid was created.');
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('go trigger', 1, 'Syntax: .go trigger #trigger_id\r\nTeleport your character to areatrigger with id #trigger_id. Character will be teleported to trigger target if selected areatrigger is telporting trigger.');
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('go xy', 1, 'Syntax: .go xy #x #y [#mapid]\r\nTeleport player to point with (#x,#y) coordinates at ground(water) level at map #mapid or same map if #mapid not provided.');
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('go', 1, 'Syntax: .go #x #y #z [#mapid]\r\nTeleport player to point with (#x,#y,#z) coordinates at ground(water) level at map #mapid or same map if #mapid not provided.');
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('go xyz', 1, 'Syntax: .go xyz #x #y #z [#mapid]\r\nTeleport player to point with (#x,#y,#z) coordinates at ground(water) level at map #mapid or same map if #mapid not provided.');
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('go zonexy', 1, 'Syntax: .go zonexy #x #y [#zone]\r\nTeleport player to point with (#x,#y) client coordinates at ground(water) level in zone #zoneid or current zone if #zoneid not provided. You can look up zone using .lookup area $namepart');

-- NEW LEARN SECURITY LEVELS AND COMMAND
DELETE FROM `command` WHERE `name`='learn';
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('learn', 2, 'Syntax: .learn #parameter\r\nSelected character learn a spell of id #parameter.');
DELETE FROM `command` WHERE `name`='learn all';
DELETE FROM `command` WHERE `name` LIKE '%learn%' AND `name` LIKE '%all%' AND `name` LIKE '%crafts%';
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('learn all crafts', 2, 'Syntax: .learn all crafts\r\nLearn all professions and recipes.');
DELETE FROM `command` WHERE `name` LIKE '%learn%' AND `name` LIKE '%all%' AND `name` LIKE '%default%';
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('learn all default', 2, 'Syntax: .learn all default [$playername]\r\nLearn for selected/$playername player all default spells for his race/class and spells rewarded by completed quests.');
DELETE FROM `command` WHERE `name` LIKE '%learn%' AND `name` LIKE '%all%' AND `name` LIKE '%gm%';
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('learn all gm', 2, 'Syntax: .learn all gm\r\nLearn all default spells for Game Masters.');
DELETE FROM `command` WHERE `name` LIKE '%learn%' AND `name` LIKE '%all%' AND `name` LIKE '%lang%';
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('learn all lang', 2, 'Syntax: .learn all lang\r\nLearn all languages');
DELETE FROM `command` WHERE `name` LIKE '%learn%' AND `name` LIKE '%all%' AND `name` LIKE '%my%' AND `name` LIKE '%class%';
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('learn all my class', 2, 'Syntax: .learn all my class\r\nLearn all spells and talents available for his class.');
DELETE FROM `command` WHERE `name` LIKE '%learn%' AND `name` LIKE '%all%' AND `name` LIKE '%my%' AND `name` LIKE '%spells%';
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('learn all my spells', 2, 'Syntax: .learn all my spells\r\nLearn all spells (except talents and spells with first rank learned as talent) available for his class.');
DELETE FROM `command` WHERE `name` LIKE '%learn%' AND `name` LIKE '%all%' AND `name` LIKE '%my%' AND `name` LIKE '%talents%';
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('learn all my talents', 2, 'Syntax: .learn all my talents\r\nLearn all talents (and spells with first rank learned as talent) available for his class.');
DELETE FROM `command` WHERE `name` LIKE '%learn%' AND `name` LIKE '%all%' AND `name` LIKE '%recipes%';
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('learn all recipes', 2, 'Syntax: .learn all recipes [$profession]\r\nLearns all recipes of specified profession and sets skill level to max.\r\nExample: .learn all recipes enchanting');
DELETE FROM `command` WHERE `name`='unlearn';
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('unlearn', 2, 'Syntax: .unlearn #startspell #endspell\r\nUnlearn for selected player the range of spells between id #startspell and #endspell. If no #endspell is provided, just unlearn spell of id #startspell.');

-- NEW MOD DEMORPH COMMAND
DELETE FROM `command` WHERE `name`='modify demorph';
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('modify demorph', 1, 'Syntax: .modify demorph \r\nChange your current model id to the default one.');
-- SET CORRECT SECURITY LEVEL
DELETE FROM `command` WHERE `name`='modify morph';
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('modify morph', 1, 'Syntax: .modify morph #displayid\r\nChange your current model id to #displayid.');
DELETE FROM `command` WHERE `name` LIKE '%modify%' AND `name` LIKE '%rep%';
INSERT INTO `command` (`name`, `security`, `help`) VALUES ('modify reputation', 1, 'Syntax: .modify reputation #repId (#repvalue | $rankname [#delta])\r\nSets the selected players reputation with faction #repId to #repvalue or to $reprank.\r\nIf the reputation rank name is provided, the resulting reputation will be the lowest reputation for that rank plus the delta amount, if specified.\r\nYou can use \'.pinfo rep\' to list all known reputation ids, or use \'.lookup faction $name\' to locate a specific faction id.');

-- COMMANDS THAT DON'T EXISTS
DELETE FROM `command` WHERE `name` = 'gm online';
DELETE FROM `command` WHERE `name` LIKE '%npc%' AND `name` LIKE '%add%' AND `name` LIKE '%group%';
DELETE FROM `command` WHERE `name`='npc addweapon';
DELETE FROM `command` WHERE `name`='npc phase';
DELETE FROM `command` WHERE `name`='npc name';
DELETE FROM `command` WHERE `name`='npc subname';
DELETE FROM `command` WHERE `name`='modify phase';
DELETE FROM `command` WHERE `name`='npc spawndist';
DELETE FROM `command` WHERE `name`='demorph';