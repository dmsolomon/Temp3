
compiler = mpiCC
FLAGS = -O3 

p1_files = driver.cpp X_matrix.cpp rand_vector.cpp random.cpp

all:	p1_exe	

p1_exe: $(p1_files)
		$(compiler) $(FLAGS) $(p1_files) -o $@

clean:
		rm *_exe

