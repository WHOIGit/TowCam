#include <QtWidgets>
#include "TowCam.h"
#include "TowcamSocketThread.h"


TowCam     *theMainWindow;


int main(int argc, char *argv[])
{
   QApplication a(argc, argv);
   char	*enteredIniFileName;
   IniFile                     iniFile;

   if(argc > 1)
   {
         enteredIniFileName = strdup((char *)argv[1]);
      }
   else
   {
#ifdef WINDOWS

         enteredIniFileName = strdup("C:\\towcam.ini");

#else

         QString myDesktop = QStandardPaths::locate(QStandardPaths::DesktopLocation,QString(), QStandardPaths::LocateDirectory);
         QString myIniFile = myDesktop + "//towcam.ini";
         enteredIniFileName = strdup(myIniFile.toLatin1().data());

#endif
      }

   networkStructureT networkStructure;
   int	startup_return_code = iniFile.open_ini(enteredIniFileName);
   if(startup_return_code != GOOD_INI_FILE_READ)
      {
#if 0
         QMessageBox::warning(this, 	tr("towcam  GUI"),
                              tr("no ini file found"),
                              QMessageBox::Close | QMessageBox::Default);
#endif
         qDebug() << "couldnt open inifile";
         exit(0);
      }

   int trial1 = iniFile.read_int("GENERAL","DEPTH_INCOMING_SOCKET",DEFAULT_IN_SOCKET);


   if(DEFAULT_IN_SOCKET == trial1 )
      {
#if 0
         QMessageBox::warning(this, 	tr("towcam  GUI"),
                              tr("erroneous depth socket entry"),
                              QMessageBox::Close | QMessageBox::Default);
#endif
         exit(0);

      }
   else
      {
         networkStructure.depthInSocketNumber = (unsigned short)  trial1;
      }

   trial1 = iniFile.read_int("GENERAL","ALTIMETER_INCOMING_SOCKET",DEFAULT_IN_SOCKET);


   if(DEFAULT_IN_SOCKET == trial1 )
      {
#if 0
         QMessageBox::warning(this, 	tr("towcam  GUI"),
                              tr("erroneous altitude socket entry"),
                              QMessageBox::Close | QMessageBox::Default);
#endif
         exit(0);

      }

   else
      {
         networkStructure.altitudeInSocketNumber = (unsigned short)  trial1;
      }

   trial1 = iniFile.read_int("GENERAL","SWITCH_INCOMING_SOCKET",DEFAULT_IN_SOCKET);
   char *scratchString = iniFile.read_string("GENERAL","SWITCH_OUTGOING_IP","NO_ADDRESS");
   if(!strcmp("NO_ADDRESS",scratchString))
      {
#if 0
         QMessageBox::warning(this, 	tr("towcam  GUI"),
                              tr("erroneous switch outgoing IP entry"),
                              QMessageBox::Close | QMessageBox::Default);
#endif
         exit(0);

      }
   networkStructure.switchHostAddress.setAddress(scratchString);

   if(DEFAULT_IN_SOCKET == trial1 )
      {
 #if 0
         QMessageBox::warning(this, 	tr("towcam  GUI"),
                              tr("erroneous incoming switch socket entry"),
                              QMessageBox::Close | QMessageBox::Default);
#endif
         exit(0);

      }

   else
      {
         networkStructure.switchInSocketNumber = (unsigned short)  trial1;
      }

   trial1 = iniFile.read_int("GENERAL","SWITCH_OUTGOING_SOCKET",DEFAULT_IN_SOCKET);
   if(DEFAULT_IN_SOCKET == trial1 )
      {
#if 0
         QMessageBox::warning(this, 	tr("towcam  GUI"),
                              tr("erroneous outgoing switch socket entry"),
                              QMessageBox::Close | QMessageBox::Default);
#endif
         exit(0);

      }

   else
      {
         networkStructure.switchOutSocketNumber = (unsigned short)  trial1;
      }



   double latitude = iniFile.read_double("GENERAL", "LATITUDE", DEFAULT_LATITUDE);

   QThread* thread = new QThread;
   TowcamSocketThread* socketThread = new TowcamSocketThread(networkStructure);
   socketThread->setLatitude(latitude);
   socketThread->moveToThread(thread);
   //connect(socketThread, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
   QObject::connect(thread, SIGNAL(started()), socketThread, SLOT(startup()));
   QObject::connect(socketThread, SIGNAL(finished()), thread, SLOT(quit()));
   QObject::connect(socketThread, SIGNAL(finished()), socketThread, SLOT(deleteLater()));
   QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
   thread->start();


   theMainWindow = new TowCam(iniFile);
   iniFile.close_ini();
   QString	windowTitle;
   windowTitle = "macTowcam V " + (QString) PROGRAM_VERSION + " compiled "  + (QString) __DATE__;

   theMainWindow->setWindowTitle(windowTitle);

   QObject::connect(socketThread,SIGNAL(noSwitchContact()), theMainWindow,SLOT(noSwitchContact()));
   QObject::connect(socketThread,SIGNAL(newAltitude(QString)), theMainWindow,SLOT(gotANewAltitude(QString)));
   QObject::connect(socketThread,SIGNAL(newDepth(QString)), theMainWindow,SLOT(gotANewDepth(QString)));
   QObject::connect(theMainWindow,SIGNAL(switchCommand(QString)),socketThread, SLOT(sendSwitchCommand(QString)));
   QObject::connect(socketThread,SIGNAL(updateAltPlot(double)),theMainWindow, SLOT(updateAltPlot(double)));
   QObject::connect(socketThread,SIGNAL(newSwitches(int,int,int,int)),theMainWindow, SLOT(newSwitches(int,int,int,int)));
   QObject::connect(socketThread,SIGNAL(oneHzData(QString,QString,QString)), theMainWindow, SLOT(oneHzTimeout(QString, QString, QString)));
   theMainWindow->show();

   return a.exec();
}
