<?php

defined("BASE_PATH") OR header("Location: /error");

?>
<!DOCTYPE html>
<html lang="en">
<head>
    <title><?php echo Config::load("server_name"); ?></title>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta name="description" content="<?php echo Config::load("server_detail"); ?>">
    <meta name="keywords" content="wowcore, wow core, mthsena">
    <meta name="author" content="WoWCore • mthsena">
    <meta name="robots" content="index, follow">
    <link rel="icon" href="_public/img/favicon.png">
    <link rel="stylesheet" href="_public/lib/animate.css">
    <link rel="stylesheet" href="_public/lib/font-awesome/font-awesome.css">
    <script src="_public/lib/jquery.js"></script>
    <link rel="stylesheet" href="_public/css/reset.css">
    <link rel="stylesheet" href="_public/css/main.css">
    <script src="_public/js/main.js"></script>
</head>
<body>
    <div id="header">
    </div>
    <div id="box">
        <hr />
        <div id="content">
            <h1><?php echo Config::load("server_name"); ?></h1>
            <div id="message"></div>
            <form id="form-register">
                <table>
                    <tr>
                        <td><i class="fa fa-user fa-2x"></i></td>
                        <td><input type="text" name="username" placeholder="Username"></td>
                    </tr>
                    <tr>
                        <td><i class="fa fa-key fa-2x"></i></td>
                        <td><input type="password" name="password" placeholder="Password"></td>
                    </tr>
                    <tr>
                        <td><i class="fa fa-key fa-2x"></i></td>
                        <td><input type="password" name="repeatpassword" placeholder="Repeat Password"></td>
                    </tr>
					<tr>
                        <td><i class="fa fa-key fa-2x"></i></td>
                        <td><input type="text" name="token_key" placeholder="Pin"></td>
                    </tr>
                    <tr>
                        <td><i class="fa fa-envelope fa-2x"></i></td>
                        <td><input type="text" name="email" placeholder="Email"></td>
                    </tr>
                    <tr>
                        <td></td>
                        <td>
                            <input type="radio" name="expansion" value="1" checked/> The Burning Crusade <br />
                        </td>
                    </tr>
                    <tr>
                        <td></td>
                        <td><input type="submit" value="Register"/></td>
                    </tr>
                </table>
            </form>
            <div id="author"><p>Designed by: <a href="http://www.wowcore.com.br" target="_blank">WoWCore</a></p>
			<p>Edited By: <a href="http://www.github.com/talamortis" target="_blank">Talamortis</a> & <a href="http://www.github.com/Coolzoom" target="_blank">Coolzoom</a></p></div>
            <div id="stats"><a title="Server information."><i class="fa fa-angle-down fa-2x"></i></a></div>
        </div>
        <div id="stats-box">
            <table>
                <tr>
                    <td>Server</td>
                    <td>Version</td>
                    <td>Type</td>
                    <td>Exp</td>
					<td>Online Players</td>
                    <td>Realmlist</td>
                </tr>
                <tr>
                    <td><a><?php echo Config::load("server_name"); ?></a></td>
                    <td><a><?php echo Config::load("server_version"); ?></a></td>
                    <td><a><?php echo Config::load("server_type"); ?></a></td>
                    <td><a><?php echo Config::load("server_exp"); ?></a></td>
					<td><a><?php $main = new MainModel(); echo ($main->online_players()) ?></a></td>
                    <td><a>set realmlist <?php echo Config::load("server_realm"); ?></a></td>
                </tr>
            </table>
        </div>
    </div>
	
	<!-- The video -->
<video autoplay muted loop id="myVideo">
  <source src="../_public/img/animated1.webm" type="video/mp4">
</video>
</body>
</html>
