<?php

defined("BASE_PATH") OR header("Location: /error");

class Util
{
    public function sendEmail($sender, $receiver, $subject, $message)
    {
        $name = explode("@", $sender)[0];
        $cc = "";
        $bcc = "";
        $type = "text/html";
        $charset = "utf-8";
        $language = "pt-br";
        $phpVersion = phpversion();
        $originIp = $_SERVER["SERVER_ADDR"];
        $encoding = "7bit";
        $date = date("r", $_SERVER["REQUEST_TIME"]);
        $msgId = $_SERVER["REQUEST_TIME"] . md5($_SERVER["REQUEST_TIME"]) . "@" . $_SERVER["SERVER_NAME"];
        $body = wordwrap($message, 70, "\r\n");

        $headers   = array();
        $headers[] = "From: {$name} <{$sender}>";
        $headers[] = "Reply-To: {$name} <{$sender}>";
        $headers[] = "Return-Path: {$sender}";
        $headers[] = "Cc: {$cc}";
        $headers[] = "Bcc: {$bcc}";
        $headers[] = "X-Sender: {$name} <{$sender}>";
        $headers[] = "X-Mailer: PHP/{$phpVersion}";
        $headers[] = "X-Priority: 1 (Higuest)";
        $headers[] = "X-MSMail-Priority: High";
        $headers[] = "X-Importance: High";
        $headers[] = "X-Originating-IP: ${originIp}";
        $headers[] = "MIME-Version: 1.0";
        $headers[] = "Content-type: {$type}; charset={$charset}";
        $headers[] = "Content-language: {$language}";
        $headers[] = "Content-Transfer-Encoding: {$encoding}";
        $headers[] = "Date: {$date}";
        $headers[] = "Message-ID: <{$msgId}>";
        $headers[] = "Subject: {$subject}";
        $headers = implode("\r\n", $headers);

        if (mail($receiver, $subject, $body, $headers))
            return true;
        else
            return false;
    }

    public function generateHash($string1, $string2)
    {
        $hash = sha1(strtoupper($string1) . ":" . strtoupper($string2));
        return strtoupper($hash);
    }

    public function generateRandomCharacters($length)
    {
        $characters = "abcdefghijklmnopqrstuvwxyz0123456789";

        $random = null;
        for ($i = 0; $i < $length; $i++)
            $random .= $characters[mt_rand(0, strlen($characters) -1)];
        return $random;
    }

    public function formatTime($secs)
    {
        $bit = array(
            "y" => $secs / 31556926 % 12,
            "w" => $secs / 604800 % 52,
            "d" => $secs / 86400 % 7,
            "h" => $secs / 3600 % 24,
            "m" => $secs / 60 % 60,
            "s" => $secs % 60
        );
        foreach($bit as $k => $v)
            if($v > 0)
                $ret[] = $v . $k;
        return join(":", $ret);
    }

    public function formatFileSize($bytes)
    {
        if ($bytes >= 1073741824)
            $bytes = number_format($bytes / 1073741824, 2) . " GB";
        else if ($bytes >= 1048576)
            $bytes = number_format($bytes / 1048576, 2) . " MB";
        else if ($bytes >= 1024)
            $bytes = number_format($bytes / 1024, 2) . " KB";
        else if ($bytes > 1)
            $bytes = $bytes . " bytes";
        else if ($bytes == 1)
            $bytes = $bytes . " byte";
        else
            $bytes = "0 bytes";

        return $bytes;
    }

    public function getClientIp()
    {
        $ip = null;
        if (getenv("HTTP_CLIENT_IP"))
            $ip = getenv("HTTP_CLIENT_IP");
        else if(getenv("HTTP_X_FORWARDED_FOR"))
            $ip = getenv("HTTP_X_FORWARDED_FOR");
        else if(getenv("HTTP_X_FORWARDED"))
            $ip = getenv("HTTP_X_FORWARDED");
        else if(getenv("HTTP_FORWARDED_FOR"))
            $ip = getenv("HTTP_FORWARDED_FOR");
        else if(getenv("HTTP_FORWARDED"))
            $ip = getenv("HTTP_FORWARDED");
        else if(getenv("REMOTE_ADDR"))
            $ip = getenv("REMOTE_ADDR");
        else
            $ip = null;

        return $ip;
    }
}
