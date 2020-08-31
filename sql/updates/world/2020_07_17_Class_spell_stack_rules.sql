/* Priest Divine Spirit & Prayer of Spirit stack */
DELETE FROM `spell_group_stack_rules` WHERE `group_id`=1066;
INSERT INTO `spell_group_stack_rules` (`group_id`, `stack_rule`) VALUES (1066, 1);

DELETE FROM `spell_group` WHERE `id` = 1066;
INSERT INTO `spell_group` (`id`, `spell_id`) VALUES 
(1066, 14752),
(1066, 14818),
(1066, 14819),
(1066, 16875),
(1066, 25312),
(1066, 27681),
(1066, 27841),
(1066, 32999);


/* Prist Power of Fortitude & Prayer of Fortitude */
DELETE FROM `spell_group_stack_rules` WHERE `group_id`= 1067;
INSERT INTO `spell_group_stack_rules` (`group_id`, `stack_rule`) VALUES (1067, 1);

DELETE FROM `spell_group` WHERE `id` = 1067;
INSERT INTO `spell_group` (`id`, `spell_id`) VALUES 
(1067, 1243),
(1067, 1244),
(1067, 1245),
(1067, 2791),
(1067, 10937),
(1067, 10938),
(1067, 25389),
(1067, 21562),
(1067, 21564),
(1067, 25392);

/* Priest Shadow Protection & Prayer of Shadow Protection */
DELETE FROM `spell_group_stack_rules` WHERE `group_id`= 1068;
INSERT INTO `spell_group_stack_rules` (`group_id`, `stack_rule`) VALUES (1068, 1);

DELETE FROM `spell_group` WHERE `id` = 1068;
INSERT INTO `spell_group` (`id`, `spell_id`) VALUES 
(1068, 976),
(1068, 10957),
(1068, 10958),
(1068, 25433),
(1068, 27683),
(1068, 39374);

/* Druid Mark of the Wild & Gift of the Wild */
DELETE FROM `spell_group_stack_rules` WHERE `group_id`= 1069;
INSERT INTO `spell_group_stack_rules` (`group_id`, `stack_rule`) VALUES (1069, 1);

DELETE FROM `spell_group` WHERE `id` = 1069;
INSERT INTO `spell_group` (`id`, `spell_id`) VALUES 
(1069, 1126),
(1069, 5232),
(1069, 6756),
(1069, 5234),
(1069, 8907),
(1069, 9885),
(1069, 9884),
(1069, 26990),
(1069, 21849),
(1069, 21850),
(1069, 26991);
