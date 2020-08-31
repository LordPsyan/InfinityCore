# 2FA instruction
- 2FA was used by blizzard to secure the user account, it include 3 types in TBC
- a) 2FA pin code
- b) 2FA TOTP based on Google TOTP
- c) Matrix

# 2FA based on google TOTP(implanted in oregoncore), generate 16 bit alphabet and insert into account->token_key, set security_flag to 4
- to enable TOTP 2fa, you will have to generate 16 bit alphabet number as token key, example ABCD EFGH IJKL MNOP, you will have to use correct email address
- for each account you will have to generate a tokey key during account registration
- you can use fixed secret like ABCD EFGH IJKL MNOP, but it is not safe
- an official way to generate the secret is using this php module so it generate differnt secret for differnt account 
- https://packagist.org/packages/phpgangsta/googleauthenticator
- https://github.com/PHPGangsta/GoogleAuthenticator
- you will have to tell the user their secret key so they can use Eagle 2FA or Google authenticator both are mobile apps

# 2FA pin code(implanted in oregoncore), generate 6~7 bit numeric and insert into account->token_key, set security_flag to 1
- pin code method is basically 6 to 7 bit numeric pin stored in account table as token key, client will prompt the pin code input after password, server will verify the user input and give access