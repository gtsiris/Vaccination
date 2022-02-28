#!/bin/bash

if [ "$#" -ne 4 ]    # Check number of arguments
	then echo Please try: ./testFile.sh virusesFile countriesFile numLines duplicatesAllowed
	exit 1
fi

if [ ! -e $1 ]    # Check if file exists
  then echo $1 does not exist
  exit 1
fi

if [ ! -f $1 ]    # Check if it is a regular file
	then echo $1 is not a regular file
	exit 1
fi

if [ ! -r $1 ]    # Check read rights
   then echo Read rights required for $1
   exit 1
fi

if [ ! -e $2 ]    # Check if file exists
  then echo $2 does not exist
  exit 1
fi

if [ ! -f $2 ]    # Check if it is a regular file
	then echo $2 is not a regular file
	exit 1
fi

if [ ! -r $2 ]    # Check read rights
   then echo Read rights required for $2
   exit 1
fi

if [ "$3" -le 0 ]    # Check requested number of lines
	then echo numLines must be positive number
	exit 1
fi

if [ "$4" -eq 0 ]    # Let user know whether there will duplicates or not
	then echo "There won't be any duplicates..."
	idCollision=1    # Backup id in case of collision
else
	echo "Duplicates will be allowed..."
fi

touch inputFile; rm inputFile; touch inputFile;    # Create an empty file

MAXID=9999    # Max number with 4 digits
MAXAGE=120    # Maximum age
VIRUSES=$(wc -l < "$1")    # Count lines to determine how many viruses
let "VIRUSES += 1"    # Add 1, because it's counting from zero
COUNTRIES=$(wc -l < "$2")    # Count lines to determine how many countries
let "COUNTRIES += 1"    # Add 1, because it's counting from zero

count=1
while [ "$count" -le $3 ]    # Generate numLines random records
do
  id=$RANDOM    # Generate random id
	let "id %= $MAXID"    # Range: 0 to MAXID-1
	let "id += 1"    # Range: 1 to MAXID
	line=$(grep -m 1 "^$id " inputFile)    # Search for previous line with the same id
	if [ ! -z "$line" ] && [ "$4" -eq 0 ]    # If it's going to be duplicate and that isn't allowed (collision)
		then id="$idCollision"    # Use the backup id
		let "idCollision += 1"    # Backup id is now used, so update it
		line=$(grep -m 1 "^$idCollision " inputFile)    # Search for line with the same id as idCollision
		while [ ! -z "$line" ]    # As long as such line exists
		do
			let "idCollision += 1"    # Keep increasing idCollision
			line=$(grep -m 1 "^$idCollision " inputFile)    # Search for line with the same id as idCollision
		done
		if [ "$id" -gt "$MAXID" ]    # Couldn't find a unique id less than MAXID
			then echo Only "$MAXID" unique IDs can be created
			exit 1
		fi
	elif [ ! -z "$line" ] && [ ! "$4" -eq 0 ]    # If it's going to be duplicate and that is allowed
		then printf "%s " $id >> inputFile    # Use the randomly chosen id
		
		name=$(cut -d " " -f 2 <<<"$line")    # Get the name from the previous line with the same id
		printf "%s " $name >> inputFile
		
		surname=$(cut -d " " -f 3 <<<"$line")    # Get the surname from the previous line with the same id
		printf "%s " $surname >> inputFile
		
		country=$(cut -d " " -f 4 <<<"$line")    # Get the country from the previous line with the same id
		printf "%s " $country >> inputFile
		
		age=$(cut -d " " -f 5 <<<"$line")    # Get the age from the previous line with the same id
		printf "%s " $age >> inputFile
		
		virus=$RANDOM    # Choose random virus
		let "virus %= VIRUSES"    # Range: 0 to VIRUSES-1
		let "virus += 1"    # Range: 1 to VIRUSES
		#echo "$virus"
		printf "%s " $(head "-$virus" $1 | tail -1) >> inputFile    # Isolate the line of the choosen virus
		
		status=$RANDOM    # Generate random status
		let "status %= 2"    # 0 = NO, 1 = YES
		if [ "$status" -eq 0 ]    # Case "NO"
			then printf "NO" >> inputFile
		else    # Case "YES"
			day=$RANDOM    # Generate random day
			let "day %= 30"    # Range: 0 to 29
			let "day += 1"    # Range: 1 to 30
			month=$RANDOM    # Generate random month
			let "month %= 12"    # Range: 0 to 11
			let "month += 1"    # Range: 1 to 12
			year=$RANDOM    # Generate random year
			let "year %= 21"    # Range: 0 to 20
			let "year += 2000"    # Range: 2000 to 2020
			printf "YES $day-$month-$year" >> inputFile
		fi
		echo >> inputFile    # Line feed
		let "count += 1"    # Increase count
		continue    # Continue to next record
	elif [ -z "$line" ] && [ "$4" -eq 0 ] && [ "$id" -eq "$idCollision" ]    # Case "No duplicates": If needed update backup id that is meant for collision
		then let "idCollision += 1"    # Increase idCollision
		line=$(grep -m 1 "^$idCollision " inputFile)    # Search for line with the same id as idCollision
		while [ ! -z "$line" ]    # As long as such line exists
		do
			let "idCollision += 1"    # Keep increasing idCollision
			line=$(grep -m 1 "^$idCollision " inputFile)    # Search for line with the same id as idCollision
		done
	fi
	
	printf "$id " >> inputFile
	
	nameLen=$RANDOM    # Generate random name length
	let "nameLen %= 10"    # Range: 0 to 9
	let "nameLen += 3"    # Range: 3 to 12
	tr -dc A-Z </dev/urandom | head -c 1 >> inputFile    # Create one random capital letter
	tr -dc a-z </dev/urandom | head -c `expr $nameLen - 1` >> inputFile    # The rest of the ramdom letters are lowercase
	printf " " >> inputFile
	
	surnameLen=$RANDOM    # Generate random surname length
	let "surnameLen %= 10"    # Range: 0 to 9
	let "surnameLen += 3"    # Range: 3 to 12
	tr -dc A-Z </dev/urandom | head -c 1 >> inputFile    # Create one random capital letter
	tr -dc a-z </dev/urandom | head -c `expr $surnameLen - 1` >> inputFile    # The rest of the ramdom letters are lowercase
	printf " " >> inputFile
	
	country=$RANDOM    # Choose random country
	let "country %= COUNTRIES"    # Range: 0 to COUNTRIES-1
	let "country += 1"    # Range: 1 to COUNTRIES
	#echo "$country"
	printf "%s " $(head "-$country" $2 | tail -1) >> inputFile    # Isolate the line of the choosen country
	
	age=$RANDOM    # Generate random age
	let "age %= MAXAGE"    # Range: 0 to MAXAGE-1
	let "age += 1"    # Range: 1 to MAXAGE
	printf "$age " >> inputFile
	
	virus=$RANDOM    # Choose random virus
	let "virus %= VIRUSES"    # Range: 0 to VIRUSES-1
	let "virus += 1"    # Range: 1 to VIRUSES
	#echo "$virus"
	printf "%s " $(head "-$virus" $1 | tail -1) >> inputFile    # Isolate the line of the choosen virus
	
	status=$RANDOM    # Generate random status
	let "status %= 2"    # 0 = NO, 1 = YES
	if [ "$status" -eq 0 ]    # Case "NO"
		then printf "NO" >> inputFile
	else    # Case "YES"
		day=$RANDOM    # Generate random day
		let "day %= 30"    # Range: 0 to 29
		let "day += 1"    # Range: 1 to 30
		month=$RANDOM    # Generate random month
		let "month %= 12"    # Range: 0 to 11
		let "month += 1"    # Range: 1 to 12
		year=$RANDOM    # Generate random year
		let "year %= 21"    # Range: 0 to 20
		let "year += 2000"    # Range: 2000 to 2020
		printf "YES $day-$month-$year" >> inputFile
	fi
	
	echo >> inputFile    # Line feed
  let "count += 1"    # Increase count
done
