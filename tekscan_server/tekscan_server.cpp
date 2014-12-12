// tekscan_server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h> 
#include <stdio.h> 
#include <atlstr.h>
#include <iostream>
 
#using <System.dll>

using namespace System;
using namespace System::IO;
using namespace System::Net;
using namespace System::Net::Sockets;
using namespace System::Text;
using namespace System::Threading;


typedef BOOL (GETRTDATA) (int*, int*, BYTE**, BOOL);
typedef BOOL (GETCALDATA) (int*, int*, float**, BOOL); 
GETRTDATA* pGetData;
GETCALDATA* pGetCalData;
HINSTANCE hinstLib; 
BYTE* pData;
float* pFloatData;


// OBTENER INFORMACION DESCALIBRADA DE LOS FINGER TIPS
void getFingerTipsData(BYTE* f_pFilteredData)
{
	//If the function address is valid, call the function. 
	BYTE* f_pData = NULL;

	// Array para almacenar solo valores de fingertips = 5 dedos x 16 elems.
	//BYTE f_pFilteredData[81];
	if (NULL != pGetData) 
	{
		// Llamar a la funcion
		int row,col;
		BOOL bStat = (*pGetData) (&row, &col, &f_pData, TRUE);
				
		// primera posicion de pData con valores de sensor = 4 -> MANO IZQUIERDA
		// primera posicion con valores reales = 10 -> MANO DERECHA
		// Ver API2_demo -> real time data, para ver distribucion de sensels en el array de salida
		// Imagen presion (25x29) pixeles:
		// posicion en array salida: (posy-1)*25 + posx + offset; offset = 3

		int out_position=1;
		//Thumb: 
		printf( " \n Thumb finger: \n \n");
		int position = 379;
		// int position = 400; -> mano izquierda
		while(position<476)
		// position = 500 -> mano izquierda
		{
			for(int i=1;i<=4;i++)
			{
				printf("%d  ", f_pData[position+(i-1)]);
				f_pFilteredData[out_position]=f_pData[position+(i-1)];
				out_position += 1;
			}
			printf("\n");
			position +=25;
		}


		// first finger
		printf( " \n First finger: \n \n");
		position = 10;
		//position = 4; -> mano izquierda
		while(position<95)
		// position 88 -> mano izquierda
		{
			for(int i=1;i<=4;i++)
			{
				printf("%d  ", f_pData[position+(i-1)]);
				f_pFilteredData[out_position]=f_pData[position+(i-1)];
				out_position += 1;
			}
			printf("\n");
			position +=25;
		}


		// Middle finger
		printf( " \n \n Middle finger: \n \n");
		position = 15;
		// position = 9; -> izquierda
		while(position<101)
		// position = 93 -> izquierda
		{
			for(int i=1;i<=4;i++)
			{
				printf("%d  ", f_pData[position+(i-1)]);
				f_pFilteredData[out_position]=f_pData[position+(i-1)];
				out_position += 1;
			}
			printf("\n");
			position +=25;
		}


		// Ring finger
		printf( " \n \n Ring finger: \n \n");
		position = 20;
		// position = 14; -> izquierda
		while(position<105)
		// position 98 -> izquierda
		{
			for(int i=1;i<=4;i++)
			{
				printf("%d  ", f_pData[position+(i-1)]);
				f_pFilteredData[out_position]=f_pData[position+(i-1)];
				out_position += 1;
				//out_position++;
			}
			printf("\n");
			position +=25;
		}

		// Little finger
		printf( " \n \n Little finger: \n \n");
		position = 25;
		// position = 19; -> izquierda
		while(position<110)
		// position 103 -> izquierda
		{
			for(int i=1;i<=4;i++)
			{
				printf("%d  ", f_pData[position+(i-1)]);
				f_pFilteredData[out_position]=f_pData[position+(i-1)];
				out_position += 1;
				//out_position++;
			}
			printf("\n");
			position +=25;
		}

	}
	else
		printf("Error al cargar el procedimiento getData de la dll");


	printf("Posiciones filtradas para los fingertips: \n");
	for(int i=1;i<=80;i++)
	{
		printf("%d  ", f_pFilteredData[i]);
	}

}

// OBTENER INFORMACION TOTAL DESCALIBRADA

void getData(BYTE* f_pFilteredData)
{
	//If the function address is valid, call the function. 
	BYTE* f_pData = NULL;
	if (NULL != pGetData) 
	{
		// Llamar a la funcion
		int row,col;

		BOOL bStat = (*pGetData) (&row, &col, &f_pData, TRUE);
		printf( " Received data rows: %i, columns: %i \n \n \n",row, col);
				
		// primera posicion de pData con valores de sensor = 4

		int i=4;
		while(i<row*col)
		{
			for(int k=i;k<i+25;k++)
			{
				if(f_pData[k]==100)	printf(" ");
				else printf("%d ", f_pData[k]);
			}
			printf("\n \n");
			i= i+25;
		}
		printf("\n \n");

	}
	else
		printf("Error al cargar el procedimiento getData de la dll");

	for(int i=1;i<=726;i++)
	{
		f_pFilteredData[i] = f_pData[i];
	}

}


// OBTENER INFORMACION CALIBRADA DE LOS FINGER TIPS
void getFingerTipsCalData(float* f_pFilteredData)
{
	//If the function address is valid, call the function. 
	float* f_pData = NULL;
	if (NULL != pGetCalData) 
	{
		// Llamar a la funcion
		int row,col;

		BOOL bStat = (*pGetCalData) (&row, &col, &f_pData, TRUE);
		printf( " Received data rows: %i, columns: %i \n \n \n",row, col);
				
		// primera posicion de pData con valores de sensor = 4 -> mano izquierda
		// primera posicion con valores reales = 10
		// Ver API2_demo -> real time data, para ver distribucion de sensels en el array de salida
		// Imagen presion (25x29) pixeles:
		// posicion en array salida: (posy-1)*25 + posx + offset; offset = 3

		int out_position=1;
		//Thumb: 
		printf( " \n Thumb finger: \n \n");
		int position = 376;
		//int position = 397; -> mano izquierda
		while(position<455)
		// position 477 -> mano izquierda
		{
			for(int i=1;i<=4;i++)
			{
				printf("%f  ", f_pData[position+(i-1)]);
				f_pFilteredData[out_position]=f_pData[position+(i-1)];
				out_position += 1;
			}
			printf("\n");
			position +=25;
		}


		// First finger
		printf( " \n First finger: \n \n");
		position = 7;
		// position = 1; -> mano izquierda
		while(position<86)
		// position = 80 -> mano izquierda
		{
			for(int i=1;i<=4;i++)
			{
				printf("%f  ", f_pData[position+(i-1)]);
				f_pFilteredData[out_position]=f_pData[position+(i-1)];
				out_position += 1;
			}
			printf("\n");
			position +=25;
		}


		// Middle finger
		printf( " \n \n Middle finger: \n \n");
		position = 12;
		// position = 6;  -> mano izquierda
		while(position<91)
		// position = 85 -> mano izquierda
		{
			for(int i=1;i<=4;i++)
			{
				printf("%f  ", f_pData[position+(i-1)]);
				f_pFilteredData[out_position]=f_pData[position+(i-1)];
				out_position += 1;
			}
			printf("\n");
			position +=25;
		}


		// Ring finger
		printf( " \n \n Ring finger: \n \n");
		position = 17;
		// position = 11; -> mano izquierda
		while(position<96)
		// position = 90 -> mano izquierda
		{
			for(int i=1;i<=4;i++)
			{
				printf("%f  ", f_pData[position+(i-1)]);
				f_pFilteredData[out_position]=f_pData[position+(i-1)];
				out_position += 1;
			}
			printf("\n");
			position +=25;
		}

		// Little finger
		printf( " \n \n Little finger: \n \n");
		position = 22;
		// position = 16; -> mano izquierda
		while(position<101)
		// position = 95 -> mano izquierda
		{
			for(int i=1;i<=4;i++)
			{
				printf("%f  ", f_pData[position+(i-1)]);
				f_pFilteredData[out_position]=f_pData[position+(i-1)];
				out_position += 1;
			}
			printf("\n");
			position +=25;
		}

	}
	else
		printf("Error al cargar el procedimiento getData de la dll");

}

// OBTENER INFORMACION CALIBRADA

void getCalData(float* f_pFilteredData)
{
	//If the function address is valid, call the function. 

	float* f_pData = NULL;
	if (NULL != pGetCalData) 
	{
		// Llamar a la funcion
		int row,col;

		BOOL bStat = (*pGetCalData) (&row, &col, &f_pData, TRUE);
		printf( " Received data rows: %i, columns: %i \n \n \n",row, col);
				
		// primera posicion de pData con valores de sensor = 4
		// int = 4;
		int i=1;
		while(i<row*col)
		{
			for(int k=i;k<i+25;k++)
			{
				if(f_pData[k]== -1.0)	printf("1 ");
				else //printf("%f ", f_pData[k]); 
					printf("X ");
			}
			printf("\n \n");
			i= i+25;
		}
		printf("\n \n");

		for(int i=1;i<=726;i++)
		{
			f_pFilteredData[i] = f_pData[i];
		}

	}
	else
		printf("Error al cargar el procedimiento getData de la dll");

}


int main()
{
   try
   {
	BOOL fFreeResult = FALSE;

    // Get a handle to the DLL module.
    hinstLib = LoadLibrary(TEXT("C:\\Tekscan\\Research\\API2\\scr\\TekUser Dll\\TekUser.dll")); 
	printf(" TekUser.dll : ok \n"); 
 
    // If the handle is valid, try to get the function address
    if (hinstLib != NULL) 
    { 
        pGetData = (GETRTDATA*)::GetProcAddress((HMODULE)hinstLib, "GetFrameData"); 
		pGetCalData = (GETCALDATA*)::GetProcAddress((HMODULE)hinstLib, "GetCalFrameData"); 
		printf("Handle a funcion Dll : ok\n"); 
	}

	pData = NULL;
	pFloatData = NULL;

	// Pruebas: getData -> todo: llamarla solo en peticion del cliente:
	printf("\n RAW DATA :\n");
	pData = new BYTE[726];
	pFloatData = new float[726];
	//getData(pData);
	//getCalData(pFloatData);
	printf("\n FINGER TIPS RAW DATA  \n");
	pData = new BYTE[80];
	pFloatData = new float[80];
	getFingerTipsData(pData);
	getFingerTipsCalData(pFloatData);

	

	// Arrancar servidor
	// Socket // 
    // Set the TcpListener on port 13000.
    Int32 port = 13000;
    IPAddress^ localAddr = IPAddress::Parse( "172.18.34.88" );

    // TcpListener* server = new TcpListener(port);
    TcpListener^ server = gcnew TcpListener( localAddr,port );

    // Start listening for client requests.
    server->Start();

    // Buffer for reading data
    array<Byte>^bytes = gcnew array<Byte>(256);
    String^ data = nullptr;
	String^ res = nullptr;

    // Enter the listening loop.
    while ( true )
    {
        Console::Write( "Waiting for a connection... " );

        // Perform a blocking call to accept requests.
        // You could also user server.AcceptSocket() here.
        TcpClient^ client = server->AcceptTcpClient();
        Console::WriteLine( "Connected!" );
        data = nullptr;
		res = nullptr;
		pData = NULL;
		pFloatData = NULL;

        // Get a stream Object* for reading and writing
        NetworkStream^ stream = client->GetStream();
        Int32 i;

        // Loop to receive all the data sent by the client.
        while ( i = stream->Read( bytes, 0, bytes->Length ) )
		{
			// Translate data bytes to a ASCII String*.
			data = Text::Encoding::ASCII->GetString( bytes, 0, i );
			Console::WriteLine( "Received: {0}", data );

			// Process the data sent by the client.
			data = data->ToUpper();
			res = "";
			
			if(data=="RAWDATA")
			{
				pData = new BYTE[726];
				getData(pData);
				for(int k=1;k<=725;k++)
				{
					res += pData[k];
					res += "  ";
				}
			}
			if(data=="TIPSRAWDATA")
			{
				pData = new BYTE[80];
				getFingerTipsData(pData);
				for(int k=1;k<=80;k++)
				{
					res += pData[k];
					res += "  ";
				}
			}
			if(data=="CALDATA")
			{
				pFloatData = new float[726];
				getCalData(pFloatData);
				for(int k=1;k<725;k++)
				{
					res += pFloatData[k];
					res += "  ";
				}
			}
			if(data=="TIPSCALDATA")
			{
				pFloatData = new float[80];
				getFingerTipsCalData(pFloatData);
				for(int k=1;k<=80;k++)
				{
					res += pFloatData[k];
					res += "  ";
				}
			}

			// Send back a response. STRING
			array<Byte>^msg = Text::Encoding::ASCII->GetBytes( res );
			stream->Write( msg, 0, msg->Length );			
			Console::WriteLine( "Sent: {0}", res );
        }

        // Shutdown and end connection
        client->Close();
    }

    // Free the DLL module.
    fFreeResult = FreeLibrary(hinstLib); 
    
   }
   catch ( SocketException^ e ) 
   {
      Console::WriteLine( "SocketException: {0}", e );
   }

   Console::WriteLine( "\nHit enter to continue..." );
   Console::Read();
	return 0;
}

