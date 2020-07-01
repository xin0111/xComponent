project(xComponent)


set(INC 
	TrialCheck.h
	AngleTools.h)


set(SRC  
	main.cpp
	TrialCheck.cpp
	AngleTools.cpp)	


add_executable(xComponent 
				${INC}
				${SRC}
			   )