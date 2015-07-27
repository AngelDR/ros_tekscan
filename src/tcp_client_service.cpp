/**
    C++ client example using sockets
*/
#include<iostream>    //cout
#include<stdio.h> //printf
#include<string.h>    //strlen
#include<string>  //string
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
#include<netdb.h> //hostent
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include "ros/ros.h"
#include "tekscan_client/fingertips_calib_data.h"
#include "tekscan_client/GetPressureMap.h"
#include <math.h>


using namespace std;
using namespace boost;
 
/**
    TCP Client class
*/
class tcp_client
{
private:
    int sock;
    std::string address;
    int port;
    struct sockaddr_in server;
     
public:
    tcp_client();
    bool conn(string, int);
    bool send_data(string data);
    string receive(int);
    char* receive_byte_array(int);
};
 
tcp_client::tcp_client()
{
    sock = -1;
    port = 0;
    address = "";
}
 
/**
    Connect to a host on a certain port number
*/
bool tcp_client::conn(string address , int port)
{
    //create socket if it is not already created
    if(sock == -1)
    {
        //Create socket
        sock = socket(AF_INET , SOCK_STREAM , 0);
        if (sock == -1)
        {
            perror("Could not create socket");
        }
         
        cout<<"Socket created\n";
    }
    else    {   /* OK , nothing */  }
     
    //setup address structure
    if(inet_addr(address.c_str()) == -1)
    {
        struct hostent *he;
        struct in_addr **addr_list;
         
        //resolve the hostname, its not an ip address
        if ( (he = gethostbyname( address.c_str() ) ) == NULL)
        {
            //gethostbyname failed
            herror("gethostbyname");
            cout<<"Failed to resolve hostname\n";
             
            return false;
        }
         
        //Cast the h_addr_list to in_addr , since h_addr_list also has the ip address in long format only
        addr_list = (struct in_addr **) he->h_addr_list;
 
        for(int i = 0; addr_list[i] != NULL; i++)
        {
            //strcpy(ip , inet_ntoa(*addr_list[i]) );
            server.sin_addr = *addr_list[i];
             
            cout<<address<<" resolved to "<<inet_ntoa(*addr_list[i])<<endl;
             
            break;
        }
    }
     
    //plain ip address
    else
    {
        server.sin_addr.s_addr = inet_addr( address.c_str() );
    }
     
    server.sin_family = AF_INET;
    server.sin_port = htons( port );
     
    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }
     
    cout<<"Connected\n";
    return true;
}
 
/**
    Send data to the connected host
*/
bool tcp_client::send_data(string data)
{
    //Send some data
    if( send(sock , data.c_str() , strlen( data.c_str() ) , 0) < 0)
    {
        perror("Send failed : ");
        return false;
    }
    cout<<"Data send\n";
     
    return true;
}
 
/**
    Receive data from the connected host
*/
string tcp_client::receive(int size=512)
{
    char buffer[size];
    string reply;
     
    //Receive a reply from the server
    if( recv(sock , buffer , sizeof(buffer) , 0) < 0)
    {
        puts("recv failed");
    }
     
    reply = buffer;
    return reply;
}


tcp_client c;

bool pressure_service(tekscan_client::GetPressureMap::Request &req, tekscan_client::GetPressureMap::Response &res)
{
  
      std::vector<int> ftips_values(80);
      std::vector<float> ftips_calib_values(80);
      std::vector<float> finger_total_pressure(5);
      std::vector<float> finger_forces(5);
      std::vector<float> fi_(16);
      std::vector<float> forces_deviation(5);
      int value, pos;
      float sum_deviation;
      c.send_data("TIPSCALDATA");
      //receive and echo reply
      //cout<<"----------------------------\n\n";
      string response = c.receive(1024);
      // formatear respuesta
      std::replace( response.begin(), response.end(), ',', '.'); // replace all ',' to '.'
    
      // Convertir a array int[] o float[]
      char_separator<char> sep(" ");
      tokenizer<char_separator<char> > tokens(response, sep);
      pos=0;
      int aux_pos = 0; 
      int map_pos = 0;
      
      
      BOOST_FOREACH (const string& t, tokens) {
	  if(pos<=79){
	    ftips_calib_values.at(pos) = atof (t.c_str());
	    // mostrar por pantalla
	    if( pos == 0) printf("Thumb: \n");
	    else if (pos == 16) printf("First finger: \n");
	    else if (pos == 32) printf("Middle finger: \n");
	    else if (pos == 48) printf("Ring finger: \n");
	    else if (pos == 64) printf("Little finger: \n");
	    printf("%f       ",ftips_calib_values.at(pos));
	  	  
	    if( pos <= 15) res.th_values[map_pos] = ftips_calib_values.at(pos);
	    else if (pos <= 31) res.ff_values[map_pos] = ftips_calib_values.at(pos);
	    else if (pos <= 47) res.mf_values[map_pos] = ftips_calib_values.at(pos);
	    else if (pos <= 63) res.rf_values[map_pos] = ftips_calib_values.at(pos);
	    else if (pos <= 79) res.lf_values[map_pos] = ftips_calib_values.at(pos);
	  
	    aux_pos+=1;	  
	    map_pos+=1;
	    pos+=1;
	  
	    if (aux_pos == 4){ printf("\n \n"); aux_pos=0;}
	    if (map_pos == 16){ map_pos=0;} 
	  }
	}
	
	
	
	// Obtener fuerza aplicada, y centroide de la fuerza aplicada para cada dedo
	//float superficie = 2.03; // cm cuadrados 1.4 x 1.4
	float superficie = 2.56; // cm cuadrados 1.6 x 1.6
	//float superficie = 2.89; // cm cuadrados 1.7 x 1.7
	float force = 0.0;
	
	
	// Thumb
	for(int finger=0; finger<5; finger++){
	  
	  if (finger == 0){
	    float th_pressure_sum = 0;
	    for(int pos=0; pos<16; pos++){
	      th_pressure_sum += res.th_values[pos];
	      fi_[pos] = res.th_values[pos] * (2.56 / 16);  // F = p*s ; s = Superficie_total / num_celdas
	      force += fi_[pos];
	    }
	    //force = superficie * (th_pressure_sum / 16);
	    // actualizar mensaje /pressure_map
	    res.applied_force[0] = force;
	    res.total_pressure[0] = th_pressure_sum;	
	    
	    // calcular desviación tipica
	    sum_deviation = 0;
	    for(int pos=0; pos<16; pos++){
	      sum_deviation += pow( (fi_[pos] - (force/16)),2);
	    }
	    res.force_deviation[0] = (float) sqrt(sum_deviation/15);
	  }
	   
	  // First finger 
	  if (finger == 1){
	    float ff_pressure_sum = 0;
	    force = 0;
	    for(int pos=0; pos<16; pos++){
	      ff_pressure_sum += res.ff_values[pos];
	      fi_[pos] = res.ff_values[pos] * (2.56 / 16);  // F = p*s ; s = Superficie_total / num_celdas
	      force += fi_[pos];
	    }
	    //force = superficie * (ff_pressure_sum / 16);
	    
	    // actualizar mensaje /pressure_map
	    res.applied_force[1] = force;
	    res.total_pressure[1] = ff_pressure_sum;	
	    
	    // calcular desviación tipica
	    sum_deviation = 0;
	    for(int pos=0; pos<16; pos++){
	      sum_deviation += pow( (fi_[pos] - (force/16)),2);
	    }
	    res.force_deviation[1] = (float) sqrt(sum_deviation/15);

	   }
	   
	   // middle finger
	   if (finger == 2){
	    float mf_pressure_sum = 0;
	    force = 0;
	    for(int pos=0; pos<16; pos++){
	      mf_pressure_sum += res.mf_values[pos];
	      fi_[pos] = res.mf_values[pos] * (2.56 / 16);  // F = p*s ; s = Superficie_total / num_celdas
	      force += fi_[pos];
	    }
	   
	    //force = superficie * (mf_pressure_sum / 16);
	    // actualizar mensaje /pressure_map
	    res.applied_force[2] = force;
	    res.total_pressure[2] = mf_pressure_sum;
	    
	    // calcular desviación tipica
	    sum_deviation = 0;
	    for(int pos=0; pos<16; pos++){
	      sum_deviation += pow( (fi_[pos] - (force/16)),2);
	    }
	    res.force_deviation[2] = (float) sqrt(sum_deviation/15);

	   }
	   
	   
	  // ring finger 
	  if (finger == 3){
	    float rf_pressure_sum = 0;
	    force = 0;
	    for(int pos=0; pos<16; pos++){
	      rf_pressure_sum += res.rf_values[pos];
	      fi_[pos] = res.rf_values[pos] * (2.56 / 16);  // F = p*s ; s = Superficie_total / num_celdas
	      force += fi_[pos];
	    }
	    //force = superficie * (rf_pressure_sum /16);
	    // actualizar mensaje /pressure_map
	    res.applied_force[3] = force;
	    res.total_pressure[3] = rf_pressure_sum;	
	    
	    // calcular desviación tipica
	    sum_deviation = 0;
	    for(int pos=0; pos<16; pos++){
	      sum_deviation += pow( (fi_[pos] - (force/16)),2);
	    }
	    res.force_deviation[3] = (float) sqrt(sum_deviation/15);

	   }
	   
	  // little finger 
	  if (finger == 4){
	    float lf_pressure_sum = 0;
	    force = 0;
	    for(int pos=0; pos<16; pos++){
	      lf_pressure_sum += res.lf_values[pos];
	      fi_[pos] = res.lf_values[pos] * (2.56 / 16);  // F = p*s ; s = Superficie_total / num_celdas
	      force += fi_[pos];
	    }
	    //force = superficie * (lf_pressure_sum / 16);
	    
	    // actualizar mensaje /pressure_map
	    res.applied_force[4] = force;
	    res.total_pressure[4] = lf_pressure_sum;	
	    
	    // calcular desviación tipica
	    sum_deviation = 0;
	    for(int pos=0; pos<16; pos++){
	      sum_deviation += pow( (fi_[pos] - (force/16)),2);
	    }
	    res.force_deviation[4] = (float) sqrt(sum_deviation/15);

	   }
	   
	   
	 }
	 
}



 
int main(int argc , char *argv[])
{
    /**
     * Definir nodo publisher
     * 
     */
  
    ros::init(argc, argv, "pressure_map_service");
    ros::NodeHandle n;
    ros::Publisher pressure_pub = n.advertise<tekscan_client::fingertips_calib_data>("pressure_map",1000);
    ros::ServiceServer service = n.advertiseService("GetPressureMap", pressure_service);
    
    // Conexion a servidor tekscan server
    string host;
    string opc;
    host = "172.18.34.134";
    //connect to host
    c.conn(host , 13000);
    
    ros::spin();
    return 0;
}