# Compile all modules
all:
	$(info ******************* Armando todo ... *******************)
	-cd utils && make
	-cd discordiador && make
	-cd Mi-RAM-HQ && make
	-cd i-Mongo-Store  && make

# Clean all ./obj & ./bin
clean:
	$(info ******************* Limpiando todo ... *******************)
	-cd utils && make clean
	-cd discordiador && make clean
	-cd Mi-RAM-HQ && make clean
	-cd i-Mongo-Store && make clean
	
	
	
