cmake_minimum_required(VERSION 3.13)

project(jobs)

set(QT_USE_QTMAIN TRUE) 
set(CMAKE_AUTOMOC ON) 
set(CMAKE_INCLUDE_CURRENT_DIR ON) 

set(FILE_DIR . tasks)
foreach(dir ${FILE_DIR})
	string(TOUPPER "${dir}" dir_u)			
	file(GLOB ${dir_u}_UI_FILES ${dir}/*.ui)
	file(GLOB ${dir_u}_H_FILES ${dir}/*.h)
	file(GLOB ${dir_u}_CXX_FILES ${dir}/*.cpp)	
	list(APPEND UI_FILES ${${dir_u}_UI_FILES})		
	list(APPEND SRCS_FILES ${${dir_u}_CXX_FILES})
	list(APPEND INC_FILES ${${dir_u}_H_FILES})
endforeach()

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)

qt5_wrap_ui( UIS ${UI_FILES} ) 
	
add_executable(${PROJECT_NAME}  WIN32
			${UI_FILES} 
			${SRCS_FILES}
			${INC_FILES})
			
target_link_libraries(${PROJECT_NAME} 
				Qt5::Core	
				Qt5::Gui
				Qt5::Widgets			
				)		
				
set_target_properties(${PROJECT_NAME}
        PROPERTIES VS_GLOBAL_KEYWORD
        "Qt4VSv1.0")
	