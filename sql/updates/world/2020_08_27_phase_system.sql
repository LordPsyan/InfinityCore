ALTER TABLE creature
ADD COLUMN `phaseMask` INT(10) UNSIGNED NOT NULL DEFAULT '1' AFTER `spawnMask`;

ALTER TABLE gameobject
ADD COLUMN `phaseMask` INT(5) unsigned NOT NULL default '1' AFTER `spawnMask`;