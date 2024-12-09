#!/bin/bash

CYAN="\033[0;36m"
WHITE="\033[0m"
CURRENT_PROCESS="NULL"

banner() {
	echo;
	echo  "_____                                                                                 _____"
	echo "( ___ )-------------------------------------------------------------------------------( ___ )"
	echo " |   |                                                                                 |   |"
	echo " |   |                                                                                 |   |"
	echo " |   |                                                                                 |   |"
	echo " |   |                                                                                 |   |"
	echo " |   |                                                                                 |   |"
	echo " |   |      ooooooooo.         .o.       ooooooooo.         .o.        .oooooo..o      |   |"
	echo " |   |      \`888   \`Y88.      .888.      \`888   \`Y88.      .888.      d8P'    \`Y8      |   |"
	echo " |   |       888   .d88'     .8\"888.      888   .d88'     .8\"888.     Y88bo.           |   |"
	echo " |   |       888ooo88P'     .8' \`888.     888ooo88P'     .8' \`888.     \`\"Y8888o.       |   |"
	echo " |   |       888           .88ooo8888.    888\`88b.      .88ooo8888.        \`\"Y88b      |   |"
	echo " |   |       888          .8'     \`888.   888  \`88b.   .8'     \`888.  oo     .d8P      |   |"
	echo " |   |      o888o        o88o     o8888o o888o  o888o o88o     o8888o 8\"\"88888P'       |   |"
	echo " |   |                                                                                 |   |"
	echo " |   |                                                                                 |   |"
	echo " |   |                                                                                 |   |"
	echo " |   |                                                                                 |   |"
	echo " |___|                                                                                 |___|"
	echo "(_____)-------------------------------------------------------------------------------(_____)"
	echo;
}

failure_message() {
	case $1 in
		openssl)
			echo "[PARAS DOWNLOADER ERROR]: Exiting due to error with openssl."
			rm key.pem
			exit
			;;
		curl)
			echo "[PARAS DOWNLOADER ERROR]: Exiting due to error with curl."
			rm data.tar.gz encrypted_text
			exit
			;;
		wget)
			echo "[PARAS DOWLOADER ERROR]: Exiting due to error with wget."
			exit
			;;
		*)
			echo "[PARAS DOWNLOADER ERROR]: Undefined error!"
			exit
			;;
	esac
}

get_unique_identifier() {
	echo "[PARAS DOWNLOADER LOG]: Creating key file ..."
	pfile=key.pem
	echo "-----BEGIN PUBLIC KEY-----" > $pfile
	echo "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAo1jlcVWzk9GssL4WHk/b" >> $pfile
	echo "o3S9G8xFviIRUr7/Vat7pnAOSprRELqmtwpNAWEMQ5qybv1NSVsc/SfDoLPz/tFi" >> $pfile
	echo "FsFmYthgRBetFfoYUcTFjqKyogVh4n7uyIJpZPVUr8odRVKOqvFibxfV09IbzhP7" >> $pfile
	echo "OLmldSJT++D7CrP63RAlLMKqmW08YAWgKDVhZlIwiVvUUbOyVio0hsLzSdS+XNut" >> $pfile
	echo "ag+hlhoUmDpu2sGnrocSy0zZ+kpY+zUtlroIvLhEAcONFcYrQWxsAIicX0P5pgKB" >> $pfile
	echo "+Au3UZM5GW2qWXJYH7F0pIbn6/jRxGTLtyVT+5cIOOe4ektr3yOvK5OxWfz/Qut7" >> $pfile
	echo "owIDAQAB" >> $pfile
	echo "-----END PUBLIC KEY-----" >> $pfile
	echo "[PARAS DOWNLOADER LOG]: Created key."
	echo "[PARAS DOWNLOADER LOG]: Encrypting machine uniqueness ..."
	openssl pkeyutl -encrypt -inkey $pfile -pubin -in /etc/machine-id -out encrypted_text && echo "[PARAS DOWNLOADER LOG]: Completed."
	if [ $? -ne 0 ]; then
		failure_message "openssl"
	fi
	# rm $pfile
}

get_license() {
	echo "[PARAS DOWNLOADER LOG]: Getting license from server ..."
	tar -czf data.tar.gz encrypted_text
	curl -F "tarfiles=@./data.tar.gz" http://44.232.50.92:5000/upload -o license_key
	if [ $? -ne 0 ]; then
		failure_message "curl"
	fi
	mkdir -p ~/.paras/
	mv license_key ~/.paras/license_key-`hostname`
 	chmod 644 ~/.paras/license_key-`hostname`
	echo "[PARAS DOWNLOADER LOG]: License download completed."
	rm encrypted_text data.tar.gz
}

get_paras() {
	echo "[PARAS DOWNLOADER LOG]: Downloading PARAS ..."
	wget https://nsmindia.in/download/ParaS_v0.5.1_x86.sif --no-check-certificate 
	if [ $? -ne 0 ]; then
		failure_message "wget"
	fi
	echo "[PARAS DOWNLOADER LOG]: Download complete!"
}

banner
get_unique_identifier
get_license
get_paras

echo "[PARAS DOWNLOADER LOG]: ParaS v0.5.1 Setup complete." 

