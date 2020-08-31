<?php

defined("BASE_PATH") OR header("Location: /error");

class Database
{
    protected $db;

    public function __construct()
    {
        $dsn = "mysql:host=" . Config::load("db_host") . ";port=" . Config::load("db_port") . ";charset=" . Config::load("db_charset");

        if(empty($this->db))
        {
            try
            {
                $this->db = new PDO($dsn, Config::load("db_user"), Config::load("db_pass"));
                $this->db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
            }
            catch (PDOException $e)
            {
                exit("DB ERROR: " . $e->getMessage());
            }
        }
    }

    public function __destruct()
    {
        unset($this->db);
    }
}
