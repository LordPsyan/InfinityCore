<?php

require_once "../libraries/Loader.php";

$username = isset($_POST["username"]) ? $_POST["username"] : null;
$password = isset($_POST["password"]) ? $_POST["password"] : null;
$email = isset($_POST["email"]) ? $_POST["email"] : null;
$expansion = isset($_POST["expansion"]) ? $_POST["expansion"] : null;
$token_key = isset($_POST["token_key"]) ? $_POST["token_key"] : null;


if($username && $password && $email && $expansion && $token_key)
{
	$main = new MainModel();
    $username = strtoupper($username);
	//echo($username);
	if ($main->check_username($username) > 0)
	{
		exit("2");
	}

    $util = new Util();
    $sha_pass_hash = $util->generateHash($username, $password);

    
    $main->insert_account($username, $sha_pass_hash, $email, $expansion, $token_key);

    exit("1");
}
else
{
    exit("0");
}
