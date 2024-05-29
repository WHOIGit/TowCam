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

         QString myDesktop = QStandardPaths::locate(QStandardPaths::DesktopLocation,QString(), QStandardPaths::LocateDirectory);
         QString myIniFile = QDir::toNativeSeparators(myDesktop + "towcam.ini");
         enteredIniFileName = strdup(myIniFile.toLatin1().data());
      }
   qDebug() << "running";
   networkStructureT networkStructure;
   int	startup_return_code = iniFile.open_ini(enteredIniFileName);
   if(startup_return_code != GOOD_INI_FILE_READ)
      {
         QMessageBox::critical(nullptr, "Unable to open ini file", QStringLiteral("Unable to open config file from:\n\n%1").arg(enteredIniFileName));
         qDebug() << "couldnt open inifile";
         exit(0);
      }
   qDebug() << "opened inifile";

   int trial1 = iniFile.read_int("GENERAL","DEPTH_INCOMING_SOCKET",DEFAULT_IN_SOCKET);


   if(DEFAULT_IN_SOCKET == trial1 )
      {
         QMessageBox::critical(nullptr, "Invalid config file", QStringLiteral("Unable to open config file from:\n\n%1\n\nNo \"DEPTH_INCOMING_SOCKET\" value found.").arg(enteredIniFileName));
         exit(0);
      }
   else
      {
         networkStructure.depthInSocketNumber = (unsigned short)  trial1;
      }

   trial1 = iniFile.read_int("GENERAL","ALTIMETER_INCOMING_SOCKET",DEFAULT_IN_SOCKET);


   if(DEFAULT_IN_SOCKET == trial1 )
      {
         QMessageBox::critical(nullptr, "Invalid config file", QStringLiteral("Unable to open config file from:\n\n%1\n\nNo \"ALTIMETER_INCOMING_SOCKET\" value found.").arg(enteredIniFileName));
         exit(0);
      }

   else
      {
         networkStructure.altitudeInSocketNumber = (unsigned short)  trial1;
      }

   trial1 = iniFile.read_int("GENERAL","FL_ALTIMETER_INCOMING_SOCKET",DEFAULT_IN_SOCKET);


   if(DEFAULT_IN_SOCKET == trial1 )
      {
         QMessageBox::critical(nullptr, "Invalid config file", QStringLiteral("Unable to open config file from:\n\n%1\n\nNo \"FL_ALTIMETER_INCOMING_SOCKET\" value found.").arg(enteredIniFileName));
         exit(0);
      }

   else
      {
         networkStructure.flInSocketNumber = (unsigned short)  trial1;
      }
qDebug() << "got to switches";
   trial1 = iniFile.read_int("GENERAL","SWITCH_INCOMING_SOCKET",DEFAULT_IN_SOCKET);
   char *scratchString = iniFile.read_string("GENERAL","SWITCH_OUTGOING_IP","NO_ADDRESS");
   if(!strcmp("NO_ADDRESS",scratchString))
      {
         QMessageBox::critical(nullptr, "Invalid config file", QStringLiteral("Unable to open config file from:\n\n%1\n\nNo \"SWITCH_OUTGOING_IP\" value found.").arg(enteredIniFileName));
         exit(0);
      }
   networkStructure.switchHostAddress.setAddress(scratchString);

   if(DEFAULT_IN_SOCKET == trial1 )
      {
          QMessageBox::critical(nullptr, "Invalid config file", QStringLiteral("Unable to open config file from:\n\n%1\n\nNo \"SWITCH_INCOMING_SOCKET\" value found.").arg(enteredIniFileName));
          exit(0);
      }

   else
      {
         networkStructure.switchInSocketNumber = (unsigned short)  trial1;
      }

   trial1 = iniFile.read_int("GENERAL","SWITCH_OUTGOING_SOCKET",DEFAULT_IN_SOCKET);
   if(DEFAULT_IN_SOCKET == trial1 )
      {
          QMessageBox::critical(nullptr, "Invalid config file", QStringLiteral("Unable to open config file from:\n\n%1\n\nNo \"SWITCH_OUTGOING_SOCKET\" value found.").arg(enteredIniFileName));
          exit(0);
      }

   else
      {
         networkStructure.switchOutSocketNumber = (unsigned short)  trial1;
      }



   double latitude = iniFile.read_double("GENERAL", "LATITUDE", DEFAULT_LATITUDE);
   qDebug() << "launching thread";
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
   QObject::connect(socketThread,SIGNAL(newRange(QString)), theMainWindow,SLOT(gotANewRange(QString)));
   QObject::connect(socketThread,SIGNAL(newDepth(QString)), theMainWindow,SLOT(gotANewDepth(QString)));
   QObject::connect(theMainWindow,SIGNAL(switchCommand(QString)),socketThread, SLOT(sendSwitchCommand(QString)));
   QObject::connect(socketThread,SIGNAL(updateAltPlot(double)),theMainWindow, SLOT(updateAltPlot(double)));
   QObject::connect(socketThread,SIGNAL(updateRangePlot(double)),theMainWindow, SLOT(updateFLPlot(double)));
   QObject::connect(socketThread,SIGNAL(newSwitches(int,int,int,int)),theMainWindow, SLOT(newSwitches(int,int,int,int)));
   QObject::connect(socketThread,SIGNAL(oneHzData(QString,QString,QString,QString)), theMainWindow, SLOT(oneHzTimeout(QString, QString, QString,QString)));
   theMainWindow->show();

   // Increase font to 12pt
   auto font = QApplication::font();
   font.setPointSize(12);
   QApplication::setFont(font);
   
   return a.exec();
}
