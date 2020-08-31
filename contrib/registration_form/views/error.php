<?php

defined("BASE_PATH") OR header("Location: /error");

?>
<!DOCTYPE html>
<html lang="en">
<head>
    <title>Mthsena's Free Web</title>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta name="description" content="Website para servidores de World of Warcraft">
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
            <h1>Server Error</h1>
            <p>We're sorry! The server encountered an internal error and was unable to complete your request. Please try again later.</p>
            <div id="author">Designed by: <a href="http://www.wowcore.com.br" target="_blank">WoWCore</a></div>
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
					<td><a>3</a>
                    <td><a>set realmlist <?php echo Config::load("server_realm"); ?></a></td>
                </tr>
            </table>
        </div>
    </div>
</body>
</html>
