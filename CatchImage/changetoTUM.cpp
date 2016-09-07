#include <fstream>
#include <sstream>

using namespace std;

int main()
{
    ifstream inputfile;
    inputfile.open("../CatchTime.txt");
    ofstream outputfile;
    outputfile.open("../rgb.txt");
    outputfile << "# color images" << endl << "# file: Wei's data" << endl << "# timestamp filename" << endl;
    while(!inputfile.eof())
    {
	string s;
	getline(inputfile,s);
	if(!s.empty())
	{
	    stringstream ss;
	    double t;
	    string st;
	    ss << s;
	    ss >> st;
	    ss >> t;
	    outputfile << fixed << t << " mydata/" << st << endl;
	}
    }
    inputfile.close();
    outputfile.close();
    return 0;
}

