all: 
	make -C lib/
	make -C cat/
	make -C revwords/
	make -C filter/
	make -C simplesh/
	make -C bufcat/
	make -C filesender/


clean:
	make -C lib/ clean
	make -C cat/ clean
	make -C revwords/ clean
	make -C filter/ clean
	make -C bufcat/ clean
	make -C simplesh/ clean
	make -C filesender/ clean

