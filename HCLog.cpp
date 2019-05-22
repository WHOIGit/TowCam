#include <QtGui>
#include "HCLog.h"

/* 7 October 2015--there was a bug, the code didn't put new files in the right place */
/* so I remade the new directory name every time */


HabcamLog::HabcamLog()
{
	QDateTime	timeRightNow;
	QDateTime	currentTime = timeRightNow.currentDateTime().toUTC();
	QString rightNowString = currentTime.toString("yyyyMMdd_hhmm");
	//fileTimer = new QTimer();
	QTime nextTime = currentTime.time();

   loggingEnabled = false;
   writeToNetwork = false;
   networkParametersSet = false;
	
	int thisMinute = nextTime.minute();
	int thisSecond = nextTime.second();
	int thismSec = nextTime.msec();

	double msecsLeftInHour = 3600000.0 - (thisMinute*60.0 + thisSecond) * 1000.0 + thismSec;
    QTimer::singleShot(msecsLeftInHour, this, SLOT(timerExpiration()));

   QString myDesktop = QStandardPaths::locate(QStandardPaths::DesktopLocation,QString(), QStandardPaths::LocateDirectory);
   QString myData = myDesktop + "/towcamData/";
   if(!(QDir(myData).exists()))
      {
         QDir().mkdir(myData);
      }

   logFileName =  myData + rightNowString + ".txt";
           qDebug() << "opened "<< logFileName;
	logFile = new QFile(logFileName);

	logFile->open(QIODevice::ReadWrite|QIODevice::Append);
	loggingOut.setDevice(logFile);
	

}

void HabcamLog::setNetworkDistribution(bool yesNo)
{
   writeToNetwork = yesNo;
}
void HabcamLog::setNetworkParameters(QHostAddress address, unsigned short thePort, QUdpSocket *theSocket)
{
   destinationAddress = address;
   destinationSocket = thePort;
   socketToWriteTo = theSocket;
   networkParametersSet = true;
}

void HabcamLog::setLoggingEnabled(bool yesNo)
{
   loggingEnabled = yesNo;
}

void	HabcamLog::timerExpiration()
{
	logFile->close();
	QDateTime	timeRightNow;
	QDateTime	currentTime = timeRightNow.currentDateTime().toUTC();
	QString rightNowString = currentTime.toString("yyyyMMdd_hhmm");
	//fileTimer = new QTimer();
	QTime nextTime = currentTime.time();

	
	int thisMinute = nextTime.minute();
	int thisSecond = nextTime.second();
	int thismSec = nextTime.msec();

	double msecsLeftInHour = 3600000.0 - (thisMinute*60.0 + thisSecond) * 1000.0 + thismSec;
	QTimer::singleShot(msecsLeftInHour, this, SLOT(timerExpiration()));

    // problem here--what directory are we in?
    QString myDesktop = QStandardPaths::locate(QStandardPaths::DesktopLocation,QString(), QStandardPaths::LocateDirectory);
    QString myData = myDesktop + "/towcamData/";
    if(!(QDir(myData).exists()))
       {
          QDir().mkdir(myData);
       }
    logFileName =  myData + rightNowString + ".txt";


	logFile = new QFile(logFileName);				
	logFile->open(QIODevice::ReadWrite|QIODevice::Append);
	loggingOut.setDevice(logFile);

}
void	HabcamLog::closeLogFile()
{

	logFile->close();

}

void HabcamLog::timeStampAndLogThisString(QString label, QString inData)
{
   if(loggingEnabled)
      {
         QDateTime	currentDT;
         QString nowString;

         nowString = currentDT.currentDateTime().toUTC().toString("yyyy/MM/dd hh:mm:ss");

         QString LogOut=  label + "\t" + nowString + "\t" + inData + "\r\n";
         loggingOut.setDevice(logFile);
         loggingOut << LogOut;
         logFile->flush();

         if(networkParametersSet && writeToNetwork)
            {

               socketToWriteTo->writeDatagram(LogOut.toLatin1().data(),LogOut.length(),destinationAddress,destinationSocket);
            }
      }

}

void HabcamLog::logThisString(QString inData)
{
	loggingOut.setDevice(logFile);
	loggingOut << inData;
	logFile->flush();
}
