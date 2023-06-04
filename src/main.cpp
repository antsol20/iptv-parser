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
static std::string DB;
static std::string TABLE;

////////////
struct TvStream {
    std::string cgroup;
    std::string title;
    std::string link;
};


std::string batchData(std::vector<TvStream>& data) {

  std::string query;

  query = "INSERT INTO " + TABLE + " (cgroup,title,link) VALUES\n";
    
  for (int i = 0; i < data.size(); i++) 
      {
          TvStream element = data[i];
          query = query + "(\"" + element.cgroup + "\",\"" + element.title + "\",\"" + element.link + "\"),"; 
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

    
	con->setSchema(DB);

    stmt = con->createStatement();
	stmt->execute("DELETE from " + TABLE + ";");
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
DB = argv[3];
TABLE = argv[4];

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
                
                std::size_t found = line.find("group-title=");
              
                std::string sub_string = line.substr(found+12, line.size());
                
                std::size_t found2 = sub_string.find("\",");

                std::string cgroup = sub_string.substr(1, found2 - 1);
                
                std::string title = sub_string.substr(found2 + 2, sub_string.size());
                
                streamEntry.title = ReplaceAll(ReplaceAll(trim(title), std::string("\""), std::string("")), std::string("\\"), std::string(""));
                streamEntry.cgroup = ReplaceAll(trim(cgroup), std::string("\""), std::string(""));
            }

            else {
                streamEntry.link = ReplaceAll(trim(line), std::string("\""), std::string(""));
                data.push_back(streamEntry);
                }            
        }
    }    
        file.close();            


insertData(data);

return 0;

}
