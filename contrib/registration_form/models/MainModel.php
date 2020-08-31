<?php

defined("BASE_PATH") OR header("Location: /error");

class MainModel extends Database
{
	public function check_username($username)
	{
		$stmt = $this->db->prepare("SELECT username FROM realmd.account WHERE username = ?");
		$stmt->bindValue(1, $username);
		$stmt->execute();
		return $stmt->rowCount();
	}
	
    public function insert_account($username, $sha_pass_hash_, $email, $expansion, $token_key)
    {
        $stmt = $this->db->prepare("INSERT INTO realmd.account(username, sha_pass_hash, email, expansion, security_flag, token_key) VALUES(?, ?, ?, ?, ?, ?)");
        $stmt->bindValue(1, $username);
        $stmt->bindValue(2, $sha_pass_hash_);
        $stmt->bindValue(3, $email);
        $stmt->bindValue(4, $expansion);
		$stmt->bindValue(5, '1');
		$stmt->bindValue(6, $token_key);
        $stmt->execute();
    }
	
	public function online_players()
	{
		$stmt = $this->db->prepare("SELECT * FROM characters.characters WHERE online = ?");
		$stmt->bindValue(1, '1');
		$stmt->execute();
		return $stmt->rowCount();
	}
}
