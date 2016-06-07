#include "TowcamSocketThread.h"

TowcamSocketThread::TowcamSocketThread(networkStructureT theNetworkStructure) :
   QObject()
{


   myNetworkStructure = theNetworkStructure;
   depthMeasurement = -999.99;
   altimeterRange = -999.99;

}

void TowcamSocketThread::startup()
{
    loggingEnabled = false;
    secondsSinceSwitchUpdate = 0;
    oneHzTimer = new QTimer;

    myNetworkStructure.altitudeSocket = new   QUdpSocket();
    myNetworkStructure.altitudeSocket->bind(myNetworkStructure.altitudeInSocketNumber);

    myNetworkStructure.switchSocket = new   QUdpSocket();
    myNetworkStructure.switchSocket->bind(myNetworkStructure.switchInSocketNumber);
    //connect (switchSocket, SIGNAL(readyRead()),this,SLOT(sledSocketDataPending()));

    myNetworkStructure.depthSocket = new   QUdpSocket();
    myNetworkStructure.depthSocket->bind(myNetworkStructure.depthInSocketNumber);

    connect (myNetworkStructure.altitudeSocket, SIGNAL(readyRead()),this,SLOT(sledSocketDataPending()));
    connect (myNetworkStructure.switchSocket, SIGNAL(readyRead()),this,SLOT(sledSocketDataPending()));
    connect (myNetworkStructure.depthSocket, SIGNAL(readyRead()),this,SLOT(sledSocketDataPending()));

    theLog = new HabcamLog();
    theLog->setLoggingEnabled(true);
    connect(oneHzTimer,SIGNAL(timeout()), this, SLOT(oneHzTimeout()));

    oneHzTimer->start(1000);

}

void TowcamSocketThread::oneHzTimeout()
{
   QDateTime	currentDT;
   QString nowString;

   nowString = currentDT.currentDateTime().toUTC().toString("yyyy/MM/dd hh:mm:ss");

   // time to log a record
   QString logString = QString::number(altimeterRange,'f',2) + "\t" + QString::number(depthMeasurement,'g',2);
   theLog->timeStampAndLogThisString("TOW",logString);

   emit updateAltPlot(altimeterRange);
   emit oneHzData("Depth: " + QString::number(depthMeasurement,'f',2) + " m", "Alt: "+  QString::number(altimeterRange,'f',2) + " m", nowString);

   myNetworkStructure.switchSocket->writeDatagram("\r",myNetworkStructure.switchHostAddress,myNetworkStructure.switchOutSocketNumber);
   secondsSinceSwitchUpdate++;

   if(secondsSinceSwitchUpdate > 5)
      {
         emit noSwitchContact();
      }


}



void TowcamSocketThread::sledSocketDataPending()
{

   QUdpSocket *theSocket = (QUdpSocket *)sender();
   QByteArray	buffer(theSocket->pendingDatagramSize(),0);
   theSocket->readDatagram(buffer.data(),buffer.size());

   if(theSocket == myNetworkStructure.altitudeSocket)
      {
         if(!strncmp(buffer.data(),"$PRVAT",6))
            {
                // this is a valeport msg
                 int items = parseValeport(buffer.data());
                 if(2 == items)
                    {
                        QString altitudeString = QString::number(altimeterRange,'f',2);
                        emit newAltitude(altitudeString);
                        QString depthString = QString::number(depthMeasurement,'f',2);
                        emit newDepth(depthString);

                    }
                 return;
            }
         double	altimeterTemp,range;
         bool validAltitude;
         validAltitude = false;
         int items = sscanf( buffer.data(), "T%lf R%lf",&altimeterTemp,&range);
         if(2 == items)
            {
               altimeterTemperature = altimeterTemp;
               altimeterRange = range;
               lastAltimeter = QDateTime::currentDateTime ();
               validAltitude = true;
               buffer[buffer.size()-2] = '\0';
               if(loggingEnabled)
                  {
                     QString logString = QString::number(altimeterRange,'f',2) + "\t" + QString::number(altimeterTemperature,'f',2) +
                           " RAW," + (QString)buffer.data();
                     theLog->timeStampAndLogThisString("ALT ",logString);
                  }
               emit newAltitude((QString)buffer.data());


            }
         else
            {
               items = sscanf( buffer.data(), "R%lf",&range);
               if(items)
                  {
                     altimeterRange = range;
                     lastAltimeter = QDateTime::currentDateTime ();
                     validAltitude = true;

                     buffer[buffer.size()-2] = '\0';
                     altimeterTemp = 99.9;
                     if(loggingEnabled)
                        {
                           QString logString =QString::number(altimeterRange,'f',2)   +
                                 "\t99.99\tRAW," + (QString)buffer.data();
                           theLog->timeStampAndLogThisString("ALT ",logString);
                        }
                     emit newAltitude((QString)buffer.data());
                  }
               else
                  {
                     if(loggingEnabled)
                        {
                           QString logString = QString::number(-99.99,'f',2)  +
                                 "\t99.99 RAW," + (QString)buffer.data();
                           theLog->timeStampAndLogThisString("ALT ",logString);
                        }
                     emit newAltitude("bad altimeter data:");
                  }
            }


      }
   else if(theSocket == myNetworkStructure.depthSocket)
      {
         if(!strncmp(buffer.data(),"$PRVAT",6))
            {
                // this is a valeport msg
                 int items = parseValeport(buffer.data());
                 QString altitudeString = QString::number(altimeterRange,'f',2);
                 emit newAltitude(altitudeString);
                 QString depthString = QString::number(depthMeasurement,'f',2);
                 emit newDepth(depthString);
                 return;
            }
         double depthTemp;
         int items = sscanf( buffer.data(), "%lf",&depthTemp);
         buffer[buffer.size()-2] = '\0';
         if(items)
            {
               lastDepth = QDateTime::currentDateTime ();
               depthMeasurement = depthTemp;
               //depthValueLabel->setText( QString::number(depthMeasurement,'g',2) + " m");
               if(loggingEnabled)
                  {
                     QString logString = QString::number(depthMeasurement,'f',2) +  " RAW," + (QString)buffer.data();
                     theLog->timeStampAndLogThisString("DEP ",logString);
                  }


            }
         else{

               if(loggingEnabled)
                  {
                     QString logString = QString::number(-99.99,'f',2) +  " RAW," + (QString)buffer.data();
                     theLog->timeStampAndLogThisString("DEP ",logString);
                  }

            }
         emit newDepth((QString)buffer.data());

      }

   else if(theSocket == myNetworkStructure.switchSocket)
      {
         char sw1,sw2,sw3,sw4;
         if(buffer.length() <= 2)
            {
               return;
            }

         //int items = 4;
         secondsSinceSwitchUpdate = 0;
         //int items = sscanf(buffer.data(),"%c%c%c%c",&sw1,&sw2,&sw3,&sw4);
         sw1 = buffer.at(0);
         sw2 = buffer.at(1);
         sw3 = buffer.at(2);
         sw4 = buffer.at(3);


         if((sw1 != '0') && (sw1 != '1'))
            {
             return;
            }
         if((sw2 != '0') && (sw2 != '1'))
            {
             return;
            }
         if((sw3 != '0') && (sw3 != '1'))
            {
             return;
            }
         if((sw4 != '0') && (sw4 != '1'))
            {
             return;
            }
         emit newSwitches(sw1,sw2,sw3,sw4);


      }
}

int TowcamSocketThread::parseValeport(char *dataString)
{
            /* here is the Valeport data field structure
             *
            $PRVAT, xx.xxx,M, xxxx.xxx, dBar*hh<CR><LF>
            Field Number:
            1 Range from transducer
            2 Units
            3 Pressure
            4 Units
            5 Checksum
            Example:
            $PRVAT,00.115,M,0010.073,dBar*39
            */
    double trialRange;
    double trialPressure;
    int items = sscanf(dataString,"$PRVAT,%lf,M,%lf",&trialRange,&trialPressure) ;
    if(2 == items)
        {
            altimeterRange = trialRange;
            pressureMeasurement = trialPressure;

            // time to convert the pressure to depth
            // remove the tare
            trialPressure -= 10.132501;
            double  sine_lat2 = sin(latitude) * sin(latitude);
            double gr = (9.780318 *(1.0+(5.2788E-3 + 2.36E-5*sine_lat2)*sine_lat2)) + 1.092E-6*trialPressure;
            double depth = ((((-1.82E-15 * trialPressure) + 2.279E-10) * trialPressure - 2.2512E-5) * trialPressure + 9.72659) * trialPressure;
            depth = depth /gr;
            depthMeasurement = depth;


            return items;
        }
    else
    {
        return 0;
    }
}

void  TowcamSocketThread::sendSwitchCommand(QString myCommand)
{
   myNetworkStructure.switchSocket->writeDatagram(myCommand.toLatin1().data(),myNetworkStructure.switchHostAddress,myNetworkStructure.switchOutSocketNumber);
}
void  TowcamSocketThread::setLatitude(double inLatitude)
{
    latitude = inLatitude * 0.0174532925;

}
