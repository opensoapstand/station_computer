//***************************************
//
// messageMediator.cpp
// messaging class owns the IP interfaces
//
// Holds reference and cordinates string 
// commands from GUI, GPIO's and 
// Database threads
//
// created: 12-06-2020
// by: Denis Londry
//
// copyright 2020 by Drinkfill Beverages Ltd
// all rights reserved
//***************************************

#include "messagemediator.h"
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <sys/socket.h>
#include <string>
#include <unistd.h>

#define DELAY_USEC 1

extern messageMediator df_messaging;

bool messageMediator::m_fExitThreads = false;
bool messageMediator::m_stringReady = false;
string messageMediator::m_processString;

messageMediator::messageMediator()
{
   debugOutput::sendMessage("------messageMediator------", INFO);
   // new_sock = new ServerSocket();
   m_fExitThreads = false;
   // m_pKBThread = -1;
}

messageMediator::~messageMediator()
{
   debugOutput::sendMessage("~messageMediator", INFO);

   //terminate the threads
   m_fExitThreads = true;
}

//needs params, but this will be !!called by states!! in order to send data to receivers
DF_ERROR messageMediator::sendMessage()
{
   DF_ERROR dfError = OK;

   return dfError;
}

//needs params, but this will be !!called by states!! in order to send data to receivers
DF_ERROR messageMediator::sendProgress(int percentComplete)
{
   DF_ERROR dfError = OK;

   return dfError;
}

DF_ERROR messageMediator::sendQtACK(string AckOrNak)
{
   DF_ERROR dfError = OK;
  try
    {
      ClientSocket client_socket ( "localhost", 1235 );
      std::string reply;
      try
	{
	  client_socket << AckOrNak;
	  client_socket >> reply;
	}
      catch ( SocketException& ) {}

      std::cout << "We received this response from the server:\n\"" << reply << "\"\n";;

    }
  catch ( SocketException& e )
    {
      std::cout << "Exception was caught:" << e.description() << "\n";
    }   

   dfError = ERROR_PTHREADS_IPTHREAD_NAK;
   return dfError;
}

DF_ERROR messageMediator::createThreads(pthread_t &kbThread, pthread_t &ipThread)
{
   debugOutput::sendMessage("messageMediator::createThreads", INFO);
   DF_ERROR df_ret = OK;
   int rc = 0;
   pthread_attr_t attr;
   pthread_attr_init(&attr);

   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
   rc = pthread_create(&kbThread, &attr, doKBThread, NULL);

   if (rc)
   {
      debugOutput::sendMessage("failed to create KB Thread", INFO);
      df_ret = ERROR_PTHREADS_KBTHREAD;
   }

   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
   rc = pthread_create(&ipThread, &attr, doIPThread, NULL);

   if (rc)
   {
      debugOutput::sendMessage("failed to create KB Thread", INFO);
      df_ret = ERROR_PTHREADS_IPTHREAD;
   }

   return df_ret;
}

//up to you I would have a delimiter like ";" and concat to a string. Then when the delim goes analyse or sends to the state
DF_ERROR messageMediator::updateCmdString(char key)
{
   DF_ERROR df_ret = ERROR_BAD_PARAMS;

   cout << key << endl;

   if (';' != key)
   {
      m_processString.push_back(key);
   } else if (';' == key)
   {
      m_processString.clear();
      m_stringReady = false;
   }
   else
   {
      debugOutput::sendMessage(m_processString, INFO);
      m_stringReady = true;
   }

   return df_ret;
}

DF_ERROR messageMediator::updateCmdString()
{
   DF_ERROR df_ret = ERROR_BAD_PARAMS;

   cout << m_processString << endl;

   for(int i= 0; i < m_processString.size(); i++) {
      df_ret = updateCmdString(m_processString[i]);
   }

   return df_ret;
}

void *messageMediator::doKBThread(void *pThreadArgs)
{
   debugOutput::sendMessage("doKBThread", INFO);
   DF_ERROR df_ret = OK;

   //m_fExitThreads = false;

   while (!m_fExitThreads)
   {
      if(m_processString.empty()) {
         char key;
         while (0 < scanf(" %c", &key))
         {
            updateCmdString(key);
         }         
      } else {
         updateCmdString();
      }
      usleep(DELAY_USEC);
   }

   // df_ret;
   return 0;
}

void *messageMediator::doIPThread(void *pThreadArgs)
{
   debugOutput::sendMessage("doIPThread", INFO);
   DF_ERROR df_ret = OK;

   try
   {
      // Create the socket
      ServerSocket fsm_comm_server(1234);

      while (!m_fExitThreads)
      {
         ServerSocket new_sock;
         fsm_comm_server.accept(new_sock);

         try
         {
            while (true)
            {
               std::string data;
               // *fsm_comm_socket >> data;
               // *fsm_comm_socket << "";
               new_sock >> data;

               // AckOrNakResult = "FSM ACK";               
               sendQtACK("ACK");
               cout << data << endl;
               m_processString = data;
               updateCmdString();
               // new_sock << data;
            }

            new_sock << "Hi Back";
         }
         catch (SocketException &sock)
         {
            std::cout << "Socket Transfer Exception was caught:" << sock.description() << "\nExiting.\n";
            // AckOrNakResult = "FSM NAK";           
            // sendQtACK(AckOrNakResult);
         }
      }
   }
   catch (SocketException &e)
   {
      std::cout << "Socket Creation Exception was caught:" << e.description() << "\nExiting.\n";
   }

   return 0;
}

string messageMediator::getProcessString()
{
   return m_processString;
}

void messageMediator::clearProcessString()
{
   m_processString.clear();
   m_stringReady = false;
}

bool messageMediator::getStringReady()
{
   return m_stringReady;
}