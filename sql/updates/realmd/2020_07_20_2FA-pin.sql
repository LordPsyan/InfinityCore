ALTER TABLE `account`
ADD COLUMN `security_flag`  tinyint(3) UNSIGNED NOT NULL DEFAULT 0 AFTER `token_key`;