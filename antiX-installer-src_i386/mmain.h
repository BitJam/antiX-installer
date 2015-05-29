//
//  Copyright (C) 2003-2009 by Warren Woodford
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
//

// include files for QT
#include <fcntl.h>
#include <qapplication.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qcursor.h>
#include <qfiledialog.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistwidget.h>
#include <qlistview.h>
#include <qmainwindow.h>
#include <qprocess.h>
#include <qprogressbar.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qregexp.h>
#include <qspinbox.h>
#include <qtabwidget.h>
#include <qtextedit.h>
#include <qtimer.h>
#include <qstackedwidget.h>
#include <stdlib.h>
#include <string.h>


#include "ui_memain.h"

class MMain : public QDialog, public Ui::MeMain {
  Q_OBJECT
  
  public:
    MMain();
    ~MMain();

    void setHelpText(const QString &text);
    void closeEvent(QCloseEvent * e);

  public slots:
    virtual void closeClicked();
    virtual void showEvent(QShowEvent *e);
};


