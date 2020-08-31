/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET NAMES utf8 */;
/*!50503 SET NAMES utf8mb4 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;

CREATE TABLE IF NOT EXISTS `module_config` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `config` varchar(50) DEFAULT NULL,
  `value` varchar(50) DEFAULT NULL,
  `comment` longtext,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8;

DELETE FROM `module_config`;
/*!40000 ALTER TABLE `module_config` DISABLE KEYS */;
INSERT INTO `module_config` (`id`, `config`, `value`, `comment`) VALUES
	(1, 'modsample.enableHelloWorld', '0', NULL),
	(2, 'modsample.stringtest', 'String is working! :D', NULL),
	(3, 'modsample.intTest', '1908', NULL);
/*!40000 ALTER TABLE `module_config` ENABLE KEYS */;

/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
