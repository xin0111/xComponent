#include <iostream>
#include <string>
#include <vector>

#include "xAJson.hpp"
#include "xJson.hpp"


using namespace std;
using namespace xComponent;
using namespace json;

struct Person
{
	QString	Name;
	
	double _Weight;
};

namespace xComponent
{
	AJSON(Person, Name, _Weight)
}
int main(int argc, char * argv[])
{
	Person person;
	person.Name = QString::fromLocal8Bit("ั๔นโ");
	person._Weight = DBL_MAX;
	std::string strPerson = ajson::to_str(ajson::to_qjson(person));
	HJSON obj = Object();
	obj["Person"] = strPerson;
	//////////////////////////////////////////////////////////////////////////
	QString qsPerson = ajson::to_qstr(obj.dump());
	HJSON obj1 = HJSON::Load(qsPerson);
	Person person1;
	ajson::load_from_buff(person1, obj1.at("Person").dump().c_str());
	return 0;
}