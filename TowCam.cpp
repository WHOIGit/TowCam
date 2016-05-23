#include "TowCam.h"

extern TowCam   *theMainWindow;



SwitchWidget::SwitchWidget(int mySwitchNumber, QString mySwitchLabel)
{
   switchNumber = mySwitchNumber;
   switchGroupBox = new QGroupBox(mySwitchLabel + " on/off");
   QPalette p = switchGroupBox->palette();
   offPalette = p;
   onPalette = p;
   errorPalette = p;
   onPalette.setColor(switchGroupBox->backgroundRole(), Qt::green);
   errorPalette.setColor(switchGroupBox->backgroundRole(), Qt::red);



   QGridLayout *switchLayout = new QGridLayout(switchGroupBox);
   switchOnRadioButton = new QRadioButton(mySwitchLabel + " on");
   switchOffRadioButton = new QRadioButton(mySwitchLabel + " off");
   switchStatusLabel = new QLabel(" status:  unknown");

   QFont myFont = switchOnRadioButton->font();
   offFont = myFont;
   myFont.setBold(true);
   myFont.setPointSize(16);
   onFont = myFont;


   connect(switchOnRadioButton, SIGNAL(clicked()), this, SLOT(switchStatusChange()));
   connect(switchOffRadioButton, SIGNAL(clicked()), this, SLOT(switchStatusChange()));

   switchLayout->addWidget(switchOnRadioButton,0,0);
   switchLayout->addWidget(switchOffRadioButton,0,1);
   switchLayout->addWidget(switchStatusLabel,1,0);

   QVBoxLayout *myLayout = new QVBoxLayout(this);

   myLayout->addWidget(switchGroupBox);

}
#if 1
void SwitchWidget::setErrorPalette()
{
   switchGroupBox->setPalette(errorPalette);
   switchGroupBox->setAutoFillBackground(true);
}
#endif
void SwitchWidget::changeSwitchLabels(char status)
{

   if(status == '1')
      {

         switchStatusLabel->setText("Switch is ON");
         switchStatusLabel->setFont(onFont);
         switchGroupBox->setPalette(onPalette);
         switchGroupBox->setAutoFillBackground(true);
      }
   else if(status == '0')
      {

         switchStatusLabel->setText("Switch is OFF");
         switchStatusLabel->setFont(offFont);
         switchGroupBox->setPalette(offPalette);
         switchGroupBox->setAutoFillBackground(true);

      }

}

void SwitchWidget::switchStatusChange()
{
   QRadioButton   *myButton = (QRadioButton *)sender();

   if(myButton == switchOnRadioButton)
      {
         theMainWindow->switchAction(switchNumber,true);
      }
   else
      {
          theMainWindow->switchAction(switchNumber,false);
      }
}




TowCam::TowCam(IniFile  iniFile)
   : QWidget()
{
   pumpIsOn = false;
   //connect(oneHzTimer,SIGNAL(timeout()), this, SLOT(oneHzTimeout()));



   timeLabel = new QLabel("time");
   depthLabel = new QLabel("Depth: unknown");
   //depthValueLabel = new QLabel("unknown");
   QFont myFont = depthLabel->font();
   offFont = myFont;
   myFont.setBold(true);
   myFont.setPointSize(16);
   //onFont = myFont;
   depthLabel->setFont(myFont);
   timeLabel->setFont(myFont);
   //depthValueLabel->setFont(myFont);
   altitudeLabel = new QLabel("Altitude:  unknown");
   //altitudeValueLabel = new QLabel("unknown");
   altitudeLabel->setFont(myFont);
   //altitudeValueLabel->setFont(myFont);
   altimeterDataWindow = new QTextEdit();
   depthDataWindow = new QTextEdit();


   QGridLayout *depthLayout = new QGridLayout();
   depthLayout->addWidget(depthLabel,0,0);
   //depthLayout->addWidget(depthValueLabel,0,1);
   depthLayout->addWidget(depthDataWindow,1,0);


   QGridLayout *altitudeLayout = new QGridLayout();
   altitudeLayout->addWidget(altitudeLabel,0,0);
   //altitudeLayout->addWidget(altitudeValueLabel,0,1,1,1,Qt::AlignLeft);
   altitudeLayout->addWidget(altimeterDataWindow,1,0);

   QTextDocument *depthDocument = depthDataWindow->document();
   depthDocument->setMaximumBlockCount(256);

   QTextDocument *altDocument = altimeterDataWindow->document();
   altDocument->setMaximumBlockCount(256);


   for(int switchNumber = 0; switchNumber < 4; switchNumber++)
      {
         char scratchSwitchName[256];
         snprintf(scratchSwitchName,256,"SWITCH_%d_NAME",switchNumber+1);
         char *newSwitchName = iniFile.read_string("GENERAL",scratchSwitchName,"UNKNOWN");
         switches[switchNumber] = new SwitchWidget(switchNumber,(QString)newSwitchName);
         snprintf(scratchSwitchName,256,"SWITCH_%d_TYPE",switchNumber + 1);
         int thisSwitchType = iniFile.read_int("GENERAL",scratchSwitchName,(int)UNKNOWN_SWITCH_TYPE);
         if((0 <= thisSwitchType) && (ADAM_AIO <= thisSwitchType))
            {
               switchType[switchNumber] = (eSwitchTypeT)thisSwitchType;
            }
         else
            {
               switchType[switchNumber] =  UNKNOWN_SWITCH_TYPE;
            }
         if((ADAM_AIO == thisSwitchType) || (ADAM_DIO == thisSwitchType))
            {
               snprintf(scratchSwitchName,256,"SWITCH_%d_CHANNEL",switchNumber + 1);
               int thisSwitchChannel = iniFile.read_int("GENERAL",scratchSwitchName,ERRONEOUS_SWITCH_CHANNEL);
               if((0 <= thisSwitchChannel)  && (1 >= thisSwitchChannel) )
                  {
                     switchChannel[switchNumber] = thisSwitchChannel;
                  }
               else
                  {
                     switchChannel[switchNumber] = ERRONEOUS_SWITCH_CHANNEL;
                  }
            }
      }



   QHBoxLayout *switchLayout = new QHBoxLayout();

   for(int switchNumber = 0; switchNumber < 4; switchNumber++)
      {
         switchLayout->addWidget(switches[switchNumber]);
      }




   QHBoxLayout    *indicatorLayout = new QHBoxLayout();
   indicatorLayout->addLayout(depthLayout);
   indicatorLayout->addLayout(altitudeLayout);

   yAxisMinEdit = new QLineEdit;
   yAxisMinEdit->setValidator(new QDoubleValidator(-200.0, 200.0, 1, this));
   yAxisMinEdit->setMaxLength(4);
   yAxisMinEdit->setMaximumWidth(40);
   connect(yAxisMinEdit,SIGNAL(editingFinished()),this,SLOT(processDepthBounds()));
   yAxisMaxEdit = new QLineEdit;
   yAxisMaxEdit->setValidator(new QDoubleValidator(-200.0,200.0, 1, this));
   yAxisMaxEdit->setMaxLength(4);
   yAxisMaxEdit->setMaximumWidth(40);
   connect(yAxisMaxEdit,SIGNAL(editingFinished()),this,SLOT(processDepthBounds()));
   minLabel = new QLabel("min altitude");
   maxLabel = new QLabel("max altitude");




   flyingPlot = new QCustomPlot();
   flyingPlot->addGraph();
   flyingPlot->xAxis->setLabel("seconds in past");
   flyingPlot->yAxis->setLabel("altitude (m)");
   flyingPlot->xAxis->setRange(300,0);

   nOfFlyingPlotPoints = 0;
   QDateTime t;


   QVBoxLayout	*parameterLayout = new QVBoxLayout;
   parameterLayout->addWidget(yAxisMaxEdit);
   parameterLayout->addWidget(maxLabel);

   parameterLayout->addStretch(5);
   parameterLayout->addWidget(minLabel);
   parameterLayout->addWidget(yAxisMinEdit);
   QHBoxLayout *mainHLayout = new QHBoxLayout();

   mainHLayout->addLayout(parameterLayout);
   mainHLayout->addWidget(flyingPlot,5);

   for ( int i = 0; i < flyingHistory; i++ )
      time[i] = -i;


   QVBoxLayout *appLayout = new QVBoxLayout();

   appLayout->addWidget(timeLabel,1);
   appLayout->addLayout(mainHLayout,5);
   appLayout->addLayout(switchLayout,2);
   appLayout->addLayout(indicatorLayout,1);

   setLayout(appLayout);









}

void TowCam::newSwitches(int sw1,int sw2,int sw3,int sw4)
{
   switches[0]->changeSwitchLabels(sw1);
   switches[1]->changeSwitchLabels(sw2);
   switches[2]->changeSwitchLabels(sw3);
   switches[3]->changeSwitchLabels(sw4);
}

void TowCam::noSwitchContact()
{
   switches[0]->setErrorPalette();
   switches[1]->setErrorPalette();
   switches[2]->setErrorPalette();
   switches[3]->setErrorPalette();

}

void  TowCam::switchAction(int swID, bool switchState)
{
   QString myCommand;
   int  theRealSwitchID = swID+1;
   if(switchState)
      {
         if(BAILEY == switchType[swID])
            {
               myCommand = "SW" + QString::number(theRealSwitchID) + " 1\r\n";
            }
         else if (ADAM_DIO == switchType[swID])
            {
               if(switchChannel[swID] == ERRONEOUS_SWITCH_CHANNEL)
                  {
                     return;
                  }
               else
                  {
                     myCommand = "#011" + QString::number(switchChannel[swID]) + "01\r";
                  }
            }
         else if (ADAM_AIO == switchType[swID])
            {
               if(switchChannel[swID] == ERRONEOUS_SWITCH_CHANNEL)
                  {
                     return;
                  }
               else
                  {
                     myCommand = "#01" + QString::number(switchChannel[swID]) + "05.000\r";
                  }
            }
         else
            {
               return;
            }
      }
   else
      {
         if(BAILEY == switchType[swID])
            {
               myCommand = "SW" + QString::number(theRealSwitchID) + " 0\r\n";
            }
         else if (ADAM_DIO == switchType[swID])
            {
               if(switchChannel[swID] == ERRONEOUS_SWITCH_CHANNEL)
                  {
                     return;
                  }
               else
                  {
                     myCommand = "#011" + QString::number(switchChannel[swID]) + "00\r";
                  }
            }
         else if (ADAM_AIO == switchType[swID])
            {
               if(switchChannel[swID] == ERRONEOUS_SWITCH_CHANNEL)
                  {
                     return;
                  }
               else
                  {
                     myCommand = "#01" + QString::number(switchChannel[swID]) + "00.000\r";
                  }
            }
         else
            {
               return;
            }

      }
      emit switchCommand(myCommand);

}

#if 1
void TowCam::oneHzTimeout(QString DepthString, QString AltString, QString timeString)
{
   timeLabel->setText(timeString);
   // time to log a record


  depthLabel->setText(DepthString);
  altitudeLabel->setText(AltString);

}
#endif

void TowCam::gotANewDepth(QString theNewDepth)
{
   depthDataWindow->append(theNewDepth);
}

void TowCam::gotANewAltitude(QString theNewAlt)
{
    altimeterDataWindow->append(theNewAlt);
}

void TowCam::updateAltPlot(double inAlt)
{

  QDateTime	nowTime = QDateTime::currentDateTime();
  double currentTime = (double)nowTime.toTime_t() + (double)(nowTime.time().msec())/1000.0;

   for ( int i = nOfFlyingPlotPoints; i > 0; i-- )
      {

         if ( i < MAX_FLYING_HISTORY )
            {
               time[i] = time[i-1];
               altitude[i] = altitude[i-1];

               xData[i] =   currentTime - time[i];

            }
      }
   time[0] = currentTime;
   xData[0] = 0.0;
   altitude[0] = inAlt;

   if (  nOfFlyingPlotPoints < MAX_FLYING_HISTORY )
      {
         nOfFlyingPlotPoints++;
      }

   QVector<double> x(MAX_FLYING_HISTORY), y(MAX_FLYING_HISTORY);
   x.clear();
   y.clear();
   for(int vectorN = 0; vectorN < nOfFlyingPlotPoints; vectorN++)
    {
        x.append(xData[vectorN]);
        y.append(altitude[vectorN]);
    }
   flyingPlot->graph(0)->setData(x,y);

   flyingPlot->replot();

}

void TowCam::processDepthBounds()
{
   QLineEdit *theSender = (QLineEdit *)sender();

   if(theSender == yAxisMinEdit)
      {
         QString theText = yAxisMinEdit->text();
         yMinBound = theText.toDouble();
      }

   else if(theSender == yAxisMaxEdit)
      {
         QString theText = yAxisMaxEdit->text();
         yMaxBound = theText.toDouble();
      }
   flyingPlot->yAxis->setRange(yMinBound, yMaxBound);
   flyingPlot->replot();

}


TowCam::~TowCam()
{

}
