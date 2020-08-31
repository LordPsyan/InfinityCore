<?php

defined("BASE_PATH") OR header("Location: /error");

class Config
{
    private static $settings = array
    (
        // MAIN SETTINGS
        "view_default" => "home",

        // DATABASE SETTINGS
        "db_host"      => "127.0.0.1",
        "db_port"      => "3306",
        "db_user"      => "root",
        "db_pass"      => "ascent",
        "db_charset"   => "utf8",

        // CUSTOM SETTINGS
        "server_name"    => "server",
        "server_version" => "2.4.3",
        "server_type"    => "Blizzlike",
        "server_exp"     => "5x",
        "server_realm"   => "logon.server.co.uk",
        "server_detail"  => "2.4.3 Private Server"
    );

    public static function load($setting)
    {
        return self::$settings[$setting];
    }
}
