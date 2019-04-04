#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#include <mysql_connection.h> 
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <driver.h> 
#include <statement.h>
#include <resultset.h>

using namespace std;

struct rest_info{
    string name;
    string addr;
    double latitude;
    double longitude;
    double stars;
    double dis;
    int review_count;
    string attr;
    string cate;
};

struct request{
    double latitude;
    double longitude;
    string date;
    string time;
};

queue<int> gui_clntsockid;

bool comp(rest_info i, rest_info j){return i.dis < j.dis;}

void recmdAlgo(request &ri, vector<rest_info> &rl){
    unsigned int i, len = rl.size();
    double dlong, dlat, a, c;    

    for(i = 0; i != len; i++){
        dlat = ri.latitude - rl[i].latitude;
        dlong = ri.longitude - rl[i].longitude;
        a = pow(sin(dlat/2), 2) + cos(ri.latitude) * cos(rl[i].latitude) * pow(sin(dlong/2), 2);
        c = 2 * atan2(sqrt(a), sqrt(1-a));
        rl[i].dis = c * 3961;//radius of the earth
    }
    sort(rl.begin(), rl.end(), comp);
}

void GUISend(vector<rest_info> &rl){
    char gui_send[100000],//65
         gui_send2[28] = "\r\nConnection: close\r\n\r\n[",
         //gui_send3 = "{'restaurants':{'name':'1','addr':'2','stars':'3','dis':'4'}}",//61
         gui_send4[100000];
    int gui_send_bufsize = 1, minlen = rl.size(), len = min(100, minlen);
    
    memset(gui_send,0,100000);
    memset(gui_send4,0,100000);
    strcpy(gui_send, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: ");//65
    //cout << len << '\n';
    for(int i = 0; i < len; i++){
        strcat(gui_send4, "{\"name\":\"");//9
        strcat(gui_send4, (rl[i].name).c_str());
        strcat(gui_send4, "\",\"address\":\"");//13
        strcat(gui_send4, (rl[i].addr).c_str());
        strcat(gui_send4, "\",\"stars\":");//10
        strcat(gui_send4, to_string(rl[i].stars).c_str());
        //strcat(gui_send4, ",\"dis\":");//7
        //strcat(gui_send4, to_string(rl[i].dis).c_str());
        strcat(gui_send4, ",\"review_count\":");//16
        strcat(gui_send4, to_string(rl[i].review_count).c_str());
        strcat(gui_send4, ",\"attributes\":");//14
        strcat(gui_send4, (rl[i].attr).c_str());
        strcat(gui_send4, ",\"categories\":");//14
        strcat(gui_send4, (rl[i].cate).c_str());
        strcat(gui_send4, "},");//2
        gui_send_bufsize += 9+13+10+16+14+14+2+(rl[i].name).size()+(rl[i].addr).size()+(rl[i].attr).size()+(rl[i].cate).size()+
                            to_string(rl[i].stars).size()+to_string(rl[i].review_count).size();
    }
    gui_send4[gui_send_bufsize-2] = ']';
    strcat(gui_send, to_string(gui_send_bufsize).c_str());
    strcat(gui_send, gui_send2);
    strcat(gui_send, gui_send4);
    //cout << gui_send << "\n\n\n";
    if(send(gui_clntsockid.front(), gui_send, gui_send_bufsize+1000, 0) < 0){perror("socket error"); exit(1);}
    if(close(gui_clntsockid.front()) < 0){perror("socket error"); exit(1);}
    gui_clntsockid.pop();
}

request GUIRecv(char* gui_recv){
    char *start, *end, value[100]; 
    int size, r_size;
    request req;
    
    memset(value, 0, 100);
    for(r_size = 0; r_size != 10000; r_size++){
        if(gui_recv[r_size] == '\0') break;
    }
    cout << r_size << '\n';
    start = strstr(gui_recv, "\r\n\r\n");
    //start = strstr(gui_recv, "{");
    if(r_size <= start - gui_recv || start - gui_recv <= 0){
        req.latitude = 35.5;
        req.longitude = -81;
        req.date = "Fri";
        req.time = "17:00:00";
        return req;
    }
    
    start = strchr(start, ':');
if(r_size <= start - gui_recv || start - gui_recv <= 0){
        req.latitude = 35.5;
        req.longitude = -81;
        req.date = "Fri";
        req.time = "17:00:00";
        return req;
    }
    end = strchr(start+1, ',');
    size = end - start - 1;
    //cout << size << '\n';
    strncpy(value, start+1, size);    
    req.latitude = atof(value);
    cout << req.latitude << '\n';
    memset(value, 0, 100);
    start = strchr(end+1, ':');
if(r_size <= start - gui_recv || start - gui_recv <= 0){
        req.latitude = 35.5;
        req.longitude = -81;
        req.date = "Fri";
        req.time = "17:00:00";
        return req;
    }
    end = strchr(start+1, ',');
    size = end - start - 1;
    //cout << size << '\n';
    strncpy(value, start+1, size);    
    req.longitude = atof(value);
    cout << req.longitude << '\n';
    memset(value, 0, 100);
    start = strchr(end+1, ':');
if(r_size <= start - gui_recv || start - gui_recv <= 0){
        req.latitude = 35.5;
        req.longitude = -81;
        req.date = "Fri";
        req.time = "17:00:00";
        return req;
    }
    end = strchr(start+1, ',');
    size = end - start - 3;
    //cout << size << '\n';
    strncpy(value, start+2, size);    
    req.date = value;
    cout << req.date << '\n';
    memset(value, 0, 100);
    start = strchr(end+1, ':');
if(r_size <= start - gui_recv || start - gui_recv <= 0){
        req.latitude = 35.5;
        req.longitude = -81;
        req.date = "Fri";
        req.time = "17:00:00";
        return req;
    }
    end = strchr(start+1, '}');
    size = end - start - 3;
    //cout << size << '\n';
    strncpy(value, start+2, size);    
    req.time = value;
    cout << req.time << '\n';
    return req;
}

void dbConnection(vector<rest_info> &rest_list, request &req){
    sql::Driver *driver;
    sql::Connection *con;
    sql::ResultSet *res;
    sql::PreparedStatement *pstmt;
    rest_info tmp;
    driver = get_driver_instance();
    con = driver->connect("tcp://128.235.208.157:3306/", "yw662", "MfJCWTaS");
    con->setSchema("yw662");
    if(con->isValid()) cout << "MYSQL connection success\n"; 
    else cout << "MYSQL connection failed\n";

    pstmt = con->prepareStatement("call query_restaurant_by_location_and_time(?,?,?,?,?,?)");
    //pstmt = con->prepareStatement("call query_restaurant_by_location(?,?,?,?)");
    pstmt->setString(1,to_string(req.latitude+0.2));
    pstmt->setString(2,to_string(req.latitude-0.2));
    pstmt->setString(3,to_string(req.longitude-0.2));
    pstmt->setString(4,to_string(req.longitude+0.2));
    pstmt->setString(5,req.date);
    pstmt->setString(6,req.time);
    res = pstmt->executeQuery();
    while(res->next()){
        tmp.name = res->getString(1);
        tmp.addr = res->getString(2);
        tmp.latitude = stod(res->getString(3));
        tmp.longitude = stod(res->getString(4));
        tmp.stars = stod(res->getString(5));
        tmp.dis = 0;
        tmp.review_count = stoi(res->getString(6));
        tmp.attr = res->getString(7);
        tmp.cate = res->getString(8);
        rest_list.push_back(tmp);
    }    
    //cout << rest_list.size() << '\n';
}

int main(int argc, char *argv[]){
    //argv port db_ip db_port
    int mysockid, clntsockid, queuelen = 0;
    unsigned int clntlen;
    int gui_recv_size, gui_recv_bufsize = 10000;
    struct sockaddr_in myaddr, clntaddr;
    char gui_recv[gui_recv_bufsize];
    struct request req;
    vector<rest_info> rest_list;
        
    // GUIRecv
    mysockid = socket(PF_INET, SOCK_STREAM, 0);
    if(mysockid < 0) {perror("socket error"); exit(1);}
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(atoi(argv[1]));
    myaddr.sin_addr.s_addr = INADDR_ANY;
    if(bind(mysockid, (struct sockaddr *) &myaddr, sizeof(myaddr)) < 0) {perror("socket error"); exit(1);}
    if(listen(mysockid, queuelen) < 0) {perror("socket error"); exit(1);}

    while(1){
        //GUIRecv
        clntlen = sizeof(clntaddr);
        clntsockid = accept(mysockid,(struct sockaddr *)&clntaddr, &clntlen);
        if(clntsockid < 0) {perror("socket error"); exit(1);}
        gui_clntsockid.push(clntsockid);

        gui_recv_size = recv(clntsockid, gui_recv, gui_recv_bufsize, 0);
        if(gui_recv_size < 0) {perror("socket error"); exit(1);}      
        
        //cout << "Recv:\n" << gui_recv << '\n';
        if(gui_recv_size != 0)        
            req = GUIRecv(gui_recv);
        
        dbConnection(rest_list, req);
        recmdAlgo(req, rest_list);
        while(!gui_clntsockid.empty()){
            GUISend(rest_list);
        }
        rest_list.clear();
    }
    if(close(mysockid) < 0){perror("socket error"); exit(1);}
    
    return 0;
}
