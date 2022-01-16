CFLAGS = -fopenmp

archivo = images/pc.bmp
archivo2 = images/paisaje.bmp

images = carro.bmp ciudad.bmp laptop.bmp paisaje.bmp pc.bmp

salidas = salida.md salida.txt salida*

all: targetsecuencial targetparalelo

programasecpgm: 
	@gcc -o programasec $(CFLAGS) convertir_secuencial.c && ./programasec.exe $(archivo) > salida.md

programaparpgm: 
	gcc -o programapar $(CFLAGS) convertir_paralelo.c && ./programapar.exe $(archivo2) > salida.txt

programasecbmp: 
	@gcc -o programasec2 $(CFLAGS) convertir_secuencial.c && ./programasec2.exe $(archivo) b

programaparbmp: 
	gcc -o programapar2 $(CFLAGS) convertir_paralelo.c && ./programapar2.exe $(archivo2) b >> salida.md

clean:
	rm -f programa *.exe images/*.pgm images/*_gris.bmp $(salidas)

target:
	@gcc -o programasecuencial $(CFLAGS) convertir_secuencial.c ;
	for img in $(images) ; do \
		./programasecuencial.exe images/$$img ; >> salidasec.md\
	done

targetsecuencial:
	@gcc -o programasecuencial $(CFLAGS) convertir_secuencial.c ; for img in $(images) ; do ./programasecuencial.exe images/$$img >> salida_sec.md; done

targetparalelo:
	@gcc -o programaparalelo $(CFLAGS) convertir_secuencial.c ; for img in $(images) ; do ./programaparalelo.exe images/$$img >> salida_par.md; done

