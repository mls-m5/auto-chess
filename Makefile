
# build settings is in file 'Matmakefile'

m=matmake/matmake

all: $m
	@echo using Matmake buildsystem
	@echo for more options use 'matmake -h'
	$m

clean:
	$m clean
	
	
$m:
	make -C matmake matmake

