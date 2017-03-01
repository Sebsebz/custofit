APP_NAME=FITIF

CC=/usr/bin/gcc

exe: mkdir
	@echo Building BIN/$(APP_NAME)
	@$(CC) SRC/FIT.c -IINC -DAPP_NAME="\"$(APP_NAME)\"" -o BIN/$(APP_NAME)

mkdir:
	@echo Creating Folder
	@mkdir -p BIN
	@mkdir -p OBJ

#CLEAN
.PHONY: clean mrproper

mrproper: clean

clean: 
	@echo Cleanning
	@rm -Rf OBJ
	@rm -Rf BIN
