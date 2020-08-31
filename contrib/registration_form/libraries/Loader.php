<?php

define("BASE_PATH", dirname(__DIR__) . "/");

class Loader
{
    public static function loadClass($_file)
    {
        foreach(array("config", "libraries", "models") as $dir)
        {
            $file = BASE_PATH . $dir . "/" . $_file . ".php";

            if(file_exists($file))
                require_once $file;
        }
    }

    public static function loadView()
    {
        $url = explode("/", $_SERVER["REQUEST_URI"]);

        if(count($url) > 2)
            header("Location: /error");
        else
        {
            $url = empty($url[1]) ? Config::load("view_default") : $url[1];

            $file = BASE_PATH . "views" . "/" . $url . ".php";

            if(file_exists($file))
                require_once $file;
            else
                header("Location: /error");
        }
    }
}

spl_autoload_register(array("Loader","loadClass"));
