all: L1_lang L2_lang

L1_lang:
	cd L1 ; make 

L2_lang:
	cd L2 ; make

clean:
	cd L1 ; make clean ; cd ../L2 ; make clean ; 
