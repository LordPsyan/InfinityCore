<?php

defined("BASE_PATH") OR header("Location: /error");

class Cookie
{
    public function getCookie($cookieName)
    {
        return isset($_COOKIE[$cookieName]) ? $_COOKIE[$cookieName] : false;
    }

    public function setCookie($cookieName, $cookieValue, $cookieTime = 0)
    {
        setcookie($cookieName, $cookieValue, $cookieTime);
    }

    public function cleanCookies()
    {
        unset($_COOKIE);
    }
}
