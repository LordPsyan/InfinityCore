/* Hunters Tracking spells */
DELETE FROM `spell_group_stack_rules` WHERE `group_id`=1070;
INSERT INTO `spell_group_stack_rules` (`group_id`, `stack_rule`) VALUES (1070, 1);

DELETE FROM `spell_group` WHERE `id` = 1070;
INSERT INTO `spell_group` (`id`, `spell_id`) VALUES 
(1070, 1494),  /* Track Beast */
(1070, 5225),  /* Track Humanoids */
(1070, 19878), /* Track Demons */
(1070, 19879), /* Track Dragonkin */
(1070, 19880), /* Track Elementals */
(1070, 19882), /* Track Giants */
(1070, 19883), /* Track Humanoids */
(1070, 19884), /* Track Undead */
(1070, 19885); /* Track Hidden*/

/* Hunters Stings */

DELETE FROM `spell_group_stack_rules` WHERE `group_id`=1071;
INSERT INTO `spell_group_stack_rules` (`group_id`, `stack_rule`) VALUES (1071, 2);

DELETE FROM `spell_group` WHERE `id` = 1071;
INSERT INTO `spell_group` (`id`, `spell_id`) VALUES 
(1071, 1978), /* Serpent sting*/
(1071, 3043), /* Scorpid Sting */
(1071, 3034), /* Viper Sting */
(1071, 19386); /*Wyvern Sting*/

/* Hunters Aspects */

DELETE FROM `spell_group_stack_rules` WHERE `group_id`=1072;
INSERT INTO `spell_group_stack_rules` (`group_id`, `stack_rule`) VALUES (1072, 1);
DELETE FROM `spell_group` WHERE `id` = 1072;
INSERT INTO `spell_group` (`id`, `spell_id`) VALUES 
(1072, 13165), /* Hawk */
(1072, 13163), /* Monkey */
(1072, 5118),  /* Cheetah */
(1072, 13159), /* Pack */
(1072, 34074), /* Viper */
(1072, 20043), /* Wild */
(1072, 13161); /* Beast */

