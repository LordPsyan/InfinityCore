-- 
DELETE FROM `creature` WHERE  `guid` IN (40489, 40490, 40491, 40492, 40493, 40494, 40495, 40496) AND `id`=3254;
DELETE FROM `creature_addon` WHERE  `guid` IN (40489, 40490, 40491, 40492, 40493, 40494, 40495, 40496);
DELETE FROM `spawn_group` WHERE  `groupID`=2 AND `spawnType`=0 AND `spawnId` IN (40489, 40490, 40491, 40492, 40493, 40494, 40495, 40496);
