project(xComponent)


set(INC TrialCheck.h)


set(SRC  
	main.cpp
	TrialCheck.cpp)	


add_executable(xComponent 
				${INC}
				${SRC}
			   )