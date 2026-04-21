#!/bin/bash

echo "----- PES TEST START -----"

echo "Initializing repo..."
./pes init

echo "Creating files..."
echo "Hello World" > file1.txt
echo "This is PES" > file2.txt

echo "Running basic commands..."
./pes add file1.txt
./pes add file2.txt
./pes status
./pes commit "Initial commit"

echo "----- PES TEST END -----"
