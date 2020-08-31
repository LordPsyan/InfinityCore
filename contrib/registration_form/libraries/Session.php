<?php

defined("BASE_PATH") OR header("Location: /error");

class Session
{
    public function __construct()
    {
        if(session_status() !== PHP_SESSION_ACTIVE)
            session_start();
    }

    public function getSession($sessionName)
    {
        return isset($_SESSION[$sessionName]) ? $_SESSION[$sessionName] : false;
    }

    public function setSession($sessionName, $sessionValue)
    {
        $_SESSION[$sessionName] = $sessionValue;
    }

    public function cleanSessions()
    {
        session_destroy();
    }
}
