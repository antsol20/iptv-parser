#include <fstream>
#include <iostream>
#include <vector>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include "trim.h"

#define PRINT_OUT(x)  std::cout << x << std::endl
//////////////////

const std::string server = "tcp://localhost:3306";
const std::string username = "admin";
static std::string INPUT_FILE;
static std::string PASSWORD;

////////////
struct TvStream {
    std::string lang;
    std::string title;
    std::string link;
    std::string mtype;
};


std::string batchData(std::vector<TvStream>& data) {

  std::string query;

  query = "INSERT INTO data (lang,title,link,mtype) VALUES\n";
    
  for (int i = 0; i < data.size(); i++) 
      {
          TvStream element = data[i];
          query = query + "(\"" + element.lang + "\",\"" + element.title + "\",\"" + element.link + "\", \"" + element.mtype + "\"),"; 
      }

    query[query.size() - 1] = ';';

   return query;
}


void insertData(std::vector<TvStream>& data)
{
    sql::Driver *driver;
	sql::Connection *con;
    sql::PreparedStatement *prep_stmt;
    sql::Statement *stmt;

    try
	{
		driver = get_driver_instance();
		con = driver->connect(server, username, PASSWORD);
	}
	catch (sql::SQLException e)
	{
        std::cout << "Could not connect to server. Error message: " << e.what() << std::endl;
        std::system("pause");
        std::exit(1);
	}

    
	con->setSchema("iptv");

    stmt = con->createStatement();
	stmt->execute("DELETE from data;");
    delete stmt;


    auto parts = partitionA(data.begin(), data.end(), 1000);
    PRINT_OUT(parts.size());

    for (unsigned batchCount = 0; batchCount < parts.size(); ++batchCount) {

         auto batch = parts[batchCount];
         auto query = batchData(batch);

         PRINT_OUT(batchCount);
         stmt = con->createStatement();
         stmt->execute(query);
         delete stmt;
    }

    delete con;
}


int main(int argc, char** argv) {

INPUT_FILE = argv[1];
PASSWORD = argv[2];

std::vector<TvStream> data;

std::ifstream file(INPUT_FILE);

int counter = -1;

if (file.is_open()) {
        std::string line;
        TvStream streamEntry;
        
        while (std::getline(file, line)) {
            counter++;
            if (counter == 0) continue;


            if (line[0] == '#') {
                std::size_t found = line.find_first_of("[");
                std::string lang = line.substr(found+1,2);
                std::string title = line.substr(found+4, line.size()-found-2);
                streamEntry.lang = lang;
                streamEntry.title = trim(title);
            }
            else {
                streamEntry.link = trim(line);
                if (line.find("/movie/") != std::string::npos) {
                    streamEntry.mtype = "m";
                }
                else if (line.find("/series/") != std::string::npos){
                    streamEntry.mtype = "s";
                }
                else {
                    streamEntry.mtype = "l";
                }

                if (streamEntry.lang == "EN" || streamEntry.lang == "UK" || streamEntry.lang == "US" || streamEntry.lang == "NO" || streamEntry.lang == "FR") {
                   data.push_back(streamEntry);
                }            
        }
    }    
        file.close();            
}

insertData(data);

return 0;

}
