import time, os, random, getpass, hashlib
from os import system as cmd
from configparser import ConfigParser

source_path = ""
output_build = ""
mysql_user = ""
mysql_pass = ""
RepoToClone = ""
cores = 2

def main():
    cmd("clear")
    print("   ____                              _____           _       _   ")
    print("  / __ \\                            / ____|         (_)     | |  ")
    print(" | |  | |_ __ ___  __ _  ___  _ __ | (___   ___ _ __ _ _ __ | |_ ")
    print(" | |  | | '__/ _ \\/ _` |/ _ \\| '_ \\ \\___ \\ / __| '__| | '_ \\| __|")
    print(" | |__| | | |  __/ (_| | (_) | | | |____) | (__| |  | | |_) | |_ ")
    print("  \\____/|_|  \\___|\\__, |\\___/|_| |_|_____/ \\___|_|  |_| .__/ \\__|")
    print("                   __/ |                              | |        ")
    print("                  |___/                               |_|  \n")
    print("                          1. Download Source & Compile")
    print("                          2. Import Database")
    print("                          3. Pull updates")
    print("                          4. Compile Core")
    print("                          5. Download Data")
    print("                          6. Change Realmlist & Realm Name")
    print("                          7. Exit")

    user_select = input(" >> ")

    if user_select == None or user_select == "" or user_select == str():
        print("Invalid input")
    elif user_select == '1':
        DownloadSource()
        CompileSource()
    elif user_select == '2':
        DownloadDB()
    elif user_select == '3':
        PullUpdates()
    elif user_select == '4':
        CompileSource()
    elif user_select == '5':
        downloadData()
    elif user_select == '6':
        ServerNameRealmList()
    else:
        exit()
    pass

def ServerNameRealmList():
    user_realmlist = input(str("Please input your server realmlist (without set realmlist) \n >> "))
    server_name = input(str("What would you like to call your server? \n >> "))
    if server_name == "" or server_name == None:
        server_name = "OregonCore"
    if user_realmlist == "" or user_realmlist == None:
        user_realmlist = "127.0.0.1"
    cmd("mysql -u root -p%s -e \"UPDATE realmd.realmlist SET address = '%s', name = '%s' WHERE id = '1'\"" %(mysql_pass, user_realmlist, server_name))
    print(" ** Details have been updated! **")
    time.sleep(3)
    main()
    pass

def DownloadSource():
    print("Getting source requirements before we continue")
    time.sleep(2)
    cmd( "sudo apt-get update && apt-get upgrade && sudo apt-get install screen git cmake make gcc g++ clang libmariadbclient-dev libssl-dev libbz2-dev libreadline-dev libncurses-dev mariadb-server libace-6.* libace-dev  build-essential cmake git binutils-dev libiberty-dev libbz2-dev openssl libssl-dev zlib1g-dev libtool mariadb-client unrar libace-dev unzip libncurses-dev -y")
    cmd("clear")
    cmd("** CLONING SELECTED BRANCH **\n\n")
    cmd("git clone %s -b '%s' '%s'" %(RepoToClone, git_branch, source_path))
    cmd("mkdir %s/build" %source_path)
    pass

def DownloadDB():
    print("** Downloading and Importing Database ** \n\n")
    cmd("wget https://github.com/talamortis/OregonCore/releases/download/v1.0/database_17_11_19.zip -P '%s'" %source_path)
    cmd("unzip '%s'/database_17_11_19.zip -d '%s'" %(source_path, source_path))
    cmd("mysql -u root -p%s < %s/sql/create/create_mysql.sql" % (mysql_pass, source_path))
    cmd("mysql -u root -p%s realmd < %s/sql/base/realmd.sql" % (mysql_pass, source_path))
    cmd("mysql -u root -p%s characters < %s/sql/base/characters.sql" %(mysql_pass, source_path))
    cmd("mysql -u root -p%s world < %s/database_17_11_19.sql" %(mysql_pass, source_path))
    main()
    pass

def CompileSource():
    os.chdir("%s/build" %source_path)
    cmd("cmake '%s' -DCMAKE_INSTALL_PREFIX='%s' -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DSERVERS=1 -DTOOLS=1 -DSCRIPTS=1 -DWITH_DOCS=0 -DWITH_WARNINGS=0 -DWITH_COREDEBUG=0" %(source_path, output_build))
    cmd("make -j%s" %(cores))
    cmd("make install")
    cmd("clear")
    main()
    pass

def downloadData():
    cmd("wget http://burninglegion.co.uk/data.zip -P %s"%source_path)
    cmd("mkdir %s/data" %output_build)#make data file in build
    cmd("unzip %s/data.zip -d %s/data" %(source_path, output_build))
    main()
    pass

def PullUpdates():
    os.chdir("%s" %source_path)
    cmd("git pull")
    CompileSource()
    cmd("clear")
    main()
pass

cfg = ConfigParser()
cfg.read('config.ini')

# MySQL Information
mysql_user = (cfg.get('MySQL', 'username'))
mysql_pass = (cfg.get('MySQL', 'password'))

# Other Information
source_path = (cfg.get('Information', 'source_location'))  # get source Locatin
output_build = (cfg.get('Information', 'build_location')) # Where to compile the source
RepoToClone = (cfg.get('Information', 'Repository'))      # which source to download
git_branch = (cfg.get('Information', 'Branch'))  # Which Branch
cores = (cfg.get('Information', 'Cores'))

main()
