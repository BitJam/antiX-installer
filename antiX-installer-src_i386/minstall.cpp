//
//  Copyright (C) 2003-2010 by Warren Woodford
//  Heavily edited, with permision, by anticapitalista for antiX 2011-2014.
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

#include <QHeaderView>
#include "minstall.h"
#include "mmain.h"

MInstall::MInstall(QWidget *parent) : QWidget(parent) {
  setupUi(this);
  char line[260];
  char *tok;
  FILE *fp;
  int i;

  // timezone
  timezoneCombo->clear();
  fp = popen("awk -F '\\t' '!/^#/ { print $3 }' /usr/share/zoneinfo/zone.tab | sort", "r");
  if (fp != NULL) {
    while (fgets(line, sizeof line, fp) != NULL) {
      i = strlen(line);
      line[--i] = '\0';
     if (line != NULL && strlen(line) > 1) {
        timezoneCombo->addItem(line);
      }
    }
    pclose(fp);
  }
  timezoneCombo->setCurrentIndex(timezoneCombo->findText(getCmdOut("cat /etc/timezone")));
    

  // keyboard
  system("ls -1 /usr/share/keymaps/i386/azerty > /tmp/mlocale");
  system("ls -1 /usr/share/keymaps/i386/qwerty >> /tmp/mlocale");
  system("ls -1 /usr/share/keymaps/i386/qwertz >> /tmp/mlocale");
  system("ls -1 /usr/share/keymaps/i386/dvorak >> /tmp/mlocale");
  system("ls -1 /usr/share/keymaps/i386/fgGIod >> /tmp/mlocale");
  system("ls -1 /usr/share/keymaps/mac >> /tmp/mlocale");
  keyboardCombo->clear();
  fp = popen("sort /tmp/mlocale", "r");
  if (fp != NULL) {
    while (fgets(line, sizeof line, fp) != NULL) {
      i = strlen(line) - 9;
      line[i] = '\0';
     if (line != NULL && strlen(line) > 1) {
        keyboardCombo->addItem(line);
      }
    }
    pclose(fp);
  }
  QString kb;
  kb = getCmdOut("grep XKBLAYOUT /etc/default/keyboard");
  kb = kb.section('=', 1);
  kb = kb.section(',', 0, 0);
  kb.remove(QChar('"'));
  keyboardCombo->setCurrentIndex(keyboardCombo->findText(kb));

  // locale
  localeCombo->clear();
  fp = popen("cat /usr/share/antiX/locales.template", "r");
  if (fp != NULL) {
    while (fgets(line, sizeof line, fp) != NULL) {
      i = strlen(line);
      line[--i] = '\0';
      tok = strtok(line, " ");
     if (tok != NULL && strlen(tok) > 1 && strncmp(tok, "#", 1) != 0) {
        localeCombo->addItem(tok);
      }
    }
    pclose(fp);
  }
  QString locale;
  locale = getCmdOut("grep ^LANG /etc/default/locale").section('=',1);
  localeCombo->setCurrentIndex(localeCombo->findText(locale));	

  proc = new QProcess(this);
  timer = new QTimer(this);

  // if it looks like an apple...
  if (system("grub-probe -d /dev/sda2 | grep hfsplus") == 0) {
    grubRootButton->setChecked(true);
    grubMbrButton->setEnabled(false);
    gmtCheckBox->setChecked(true);
  }

  //setup csView
  csView->header()->setMinimumSectionSize(150);
  csView->header()->resizeSection(0,150);
  QTreeWidgetItem *networkItem = new QTreeWidgetItem(csView);
  networkItem->setText(0, tr("Networking"));

  QString val = getCmdValue("dpkg -s wicd | grep '^Status'", "ok", " ", " ");
  if (val.compare("installed") == 0) {
  wicdItem = new QTreeWidgetItem(networkItem);
  wicdItem->setText(0, "wicd");
  wicdItem->setText(1, tr("Network connection"));
  wicdItem->setCheckState(0, Qt::Checked);
  } else {
    wicdItem = NULL;
  }

  val = getCmdValue("dpkg -s openssh-client | grep '^Status'", "ok", " ", " ");
  if (val.compare("installed") == 0) {
  sshItem = new QTreeWidgetItem(networkItem);
  sshItem->setText(0, "ssh");
  sshItem->setText(1, tr("ssh"));
  sshItem->setCheckState(0, Qt::Checked);
  } else {
    sshItem = NULL;
  }

  val = getCmdValue("dpkg -s transmission | grep '^Status'", "ok", " ", " ");
  if (val.compare("installed") == 0) {
  transmissionItem = new QTreeWidgetItem(networkItem);
  transmissionItem->setText(0, "transmission");
  transmissionItem->setText(1, tr("transmission"));
  transmissionItem->setCheckState(0, Qt::Checked);
  } else {
    transmissionItem = NULL;
  }

  val = getCmdValue("dpkg -s bluetooth | grep '^Status'", "ok", " ", " ");
  if (val.compare("installed") == 0) {
  bluetoothItem = new QTreeWidgetItem(networkItem);
  bluetoothItem->setText(0, "bluetooth");
  bluetoothItem->setText(1, tr("bluetooth"));
  bluetoothItem->setCheckState(0, Qt::Checked);
  } else {
    bluetoothItem = NULL;
  }

  networkItem->setExpanded(true);
  
  QTreeWidgetItem *hardwareItem = new QTreeWidgetItem(csView);
  hardwareItem->setText(0, tr("Hardware"));
  val = getCmdValue("dpkg -s cpufrequtils | grep '^Status'", "ok", " ", " ");
  if (val.compare("installed") == 0) {
    cpufreqItem = new QTreeWidgetItem(hardwareItem);
    cpufreqItem->setText(0, "cpufrequtils");
    cpufreqItem->setText(1, tr("CPU frequency, irqbalance"));
    cpufreqItem->setCheckState(0, Qt::Checked);
  } else {
    cpufreqItem = NULL;
  }

  val = getCmdValue("dpkg -s smartmontools | grep '^Status'", "ok", " ", " ");
  if (val.compare("installed") == 0) {
    smartmontoolsItem = new QTreeWidgetItem(hardwareItem);
    smartmontoolsItem->setText(0, "smartmontools");
    smartmontoolsItem->setText(1, tr("smartmontools"));
    smartmontoolsItem->setCheckState(0, Qt::Checked);
  } else {
    smartmontoolsItem = NULL;
  }

  val = getCmdValue("dpkg -s acpid | grep '^Status'", "ok", " ", " ");
  if (val.compare("installed") == 0) {
    acpidItem = new QTreeWidgetItem(hardwareItem);
    acpidItem->setText(0, "acpid");
    acpidItem->setText(1, tr("acpid"));
    acpidItem->setCheckState(0, Qt::Checked);
  } else {
    acpidItem = NULL;
  }

  val = getCmdValue("dpkg -s acpi-support | grep '^Status'", "ok", " ", " ");
  if (val.compare("installed") == 0) {
  acpisupportItem = new QTreeWidgetItem(hardwareItem);
  acpisupportItem->setText(0, "acpi-support");
  acpisupportItem->setText(1, tr("acpi-support"));
  acpisupportItem->setCheckState(0, Qt::Checked);
  } else {
    acpisupportItem = NULL;
  }

  val = getCmdValue("dpkg -s acpi-fakekey | grep '^Status'", "ok", " ", " ");
  if (val.compare("installed") == 0) {
    acpifakekeyItem = new QTreeWidgetItem(hardwareItem);
    acpifakekeyItem->setText(0, "acpi-fakekey");
    acpifakekeyItem->setText(1, tr("acpi-fakekey"));
    acpifakekeyItem->setCheckState(0, Qt::Checked);
  } else {
    acpifakekeyItem = NULL;
  }

  val = getCmdValue("dpkg -s dbus | grep '^Status'", "ok", " ", " ");
  if (val.compare("installed") == 0) {
    dbusItem = new QTreeWidgetItem(hardwareItem);
    dbusItem->setText(0, "dbus");
    dbusItem->setText(1, tr("dbus"));
    dbusItem->setCheckState(0, Qt::Checked);
  } else {
    dbusItem = NULL;
  }

  val = getCmdValue("dpkg -s cron | grep '^Status'", "ok", " ", " ");
  if (val.compare("installed") == 0) {
    cronItem = new QTreeWidgetItem(hardwareItem);
    cronItem->setText(0, "cron");
    cronItem->setText(1, tr("cron"));
    cronItem->setCheckState(0, Qt::Checked);
  } else {
    cronItem = NULL;
  }

  val = getCmdValue("dpkg -s rsync | grep '^Status'", "ok", " ", " ");
  if (val.compare("installed") == 0) {
    rsyncItem = new QTreeWidgetItem(hardwareItem);
    rsyncItem->setText(0, "rsync");
    rsyncItem->setText(1, tr("rsync"));
    rsyncItem->setCheckState(0, Qt::Checked);
  } else {
    rsyncItem = NULL;
  }

  val = getCmdValue("dpkg -s gpm | grep '^Status'", "ok", " ", " ");
  if (val.compare("installed") == 0) {
    gpmItem = new QTreeWidgetItem(hardwareItem);
    gpmItem->setText(0, "gpm");
    gpmItem->setText(1, tr("gpm"));
    gpmItem->setCheckState(0, Qt::Checked);
  } else {
    gpmItem = NULL;
  }

  val = getCmdValue("dpkg -s sane-utils | grep '^Status'", "ok", " ", " ");
  if (val.compare("installed") == 0) {
    sanedItem = new QTreeWidgetItem(hardwareItem);
    sanedItem->setText(0, "saned");
    sanedItem->setText(1, tr("saned"));
    sanedItem->setCheckState(0, Qt::Checked);
  } else {
    sanedItem = NULL;
  }

  hardwareItem->setExpanded(true);

  QTreeWidgetItem *printItem = new QTreeWidgetItem(csView);
  printItem->setText(0, tr("Printing"));

  val = getCmdValue("dpkg -s cups | grep '^Status'", "ok", " ", " ");
  if (val.compare("installed") == 0) {
    cupsItem = new QTreeWidgetItem(printItem);
    cupsItem->setText(0, "cups");
    cupsItem->setText(1, tr("Linux and OSX printer service"));
    cupsItem->setCheckState(0, Qt::Checked);
    cupsItem->setExpanded(true);
    printItem->setExpanded(true);
  } else {
    cupsItem = NULL;
  }

  val = getCmdValue("dpkg -s samba | grep '^Status'", "ok", " ", " ");
  if (val.compare("installed") == 0) {
    sambaCheckBox->setChecked(true);
  } else {
    sambaCheckBox->setChecked(false);
    sambaCheckBox->setEnabled(false);
    computerGroupLabel->setEnabled(false);
    computerGroupEdit->setEnabled(false);
    computerGroupEdit->setText("");
  }

}

MInstall::~MInstall() {
}

/////////////////////////////////////////////////////////////////////////
// util functions

QString MInstall::getCmdOut(QString cmd) {
  char line[260];
  const char* ret = "";
  FILE* fp = popen(cmd.toAscii(), "r");
  if (fp == NULL) {
    return QString (ret);
  }
  int i;
  if (fgets(line, sizeof line, fp) != NULL) {
    i = strlen(line);
    line[--i] = '\0';
    ret = line;
  }
  pclose(fp);
  return QString (ret);
}

QStringList MInstall::getCmdOuts(QString cmd) {
  char line[260];
  FILE* fp = popen(cmd.toAscii(), "r");
  QStringList results;
  if (fp == NULL) {
    return results;
  }
  int i;
  while (fgets(line, sizeof line, fp) != NULL) {
    i = strlen(line);
    line[--i] = '\0';
    results.append(line);
  }
  pclose(fp);
  return results;
}

QString MInstall::getCmdValue(QString cmd, QString key, QString keydel, QString valdel) {
  const char *ret = "";
  char line[260];

  QStringList strings = getCmdOuts(cmd);
  for (QStringList::Iterator it = strings.begin(); it != strings.end(); ++it) {
    strcpy(line, ((QString)*it).toAscii());
    char* keyptr = strstr(line, key.toAscii());
    if (keyptr != NULL) {
      // key found
      strtok(keyptr, keydel.toAscii());
      const char* val = strtok(NULL, valdel.toAscii());
      if (val != NULL) {
        ret = val;
      }
      break;
    }
  }
  return QString (ret);
}

QStringList MInstall::getCmdValues(QString cmd, QString key, QString keydel, QString valdel) {
  char line[130];
  FILE* fp = popen(cmd.toAscii(), "r");
  QStringList results;
  if (fp == NULL) {
    return results;
  }
  int i;
  while (fgets(line, sizeof line, fp) != NULL) {
    i = strlen(line);
    line[--i] = '\0';
    char* keyptr = strstr(line, key.toAscii());
    if (keyptr != NULL) {
      // key found
      strtok(keyptr, keydel.toAscii());
      const char* val = strtok(NULL, valdel.toAscii());
      if (val != NULL) {
        results.append(val);
      }
    }
  }
  pclose(fp);
  return results;
}

bool MInstall::replaceStringInFile(QString oldtext, QString newtext, QString filepath) {

QString cmd = QString("sed -i 's/%1/%2/g' %3").arg(oldtext).arg(newtext).arg(filepath);
  if (system(cmd.toAscii()) != 0) {
    return false;
  }
  return true;
}

void MInstall::updateStatus(QString msg, int val) {
  installLabel->setText(msg.toAscii());
  progressBar->setValue(val);
  qApp->processEvents();
}

bool MInstall::mountPartition(QString dev, const char *point) {

  mkdir(point, 0755);
  QString cmd = QString("/bin/mount %1 %2").arg(dev).arg(point);
  if (system(cmd.toAscii()) != 0) {
    return false;
  }
  return true;
}

// checks SMART status of the selected disk, returs false if it detects errors and user chooses to abort
bool MInstall::checkDisk() {
    QString msg;
    int ans;
    QString output;
    
    QString drv = QString("/dev/%1").arg(diskCombo->currentText());
    output = getCmdOut("smartctl -H " + drv + "|grep -w FAILED");
    if (output.contains("FAILED")) {
      msg = output + "\n\nThe disk you selected for installation is failing.\nFor more information run \"smartctl -A " + drv + "\" in console, as root.\nYou are strongly advised to abort.\n\nDo you want to abort the installation?";
      ans = QMessageBox::critical(0, QString::null, msg,
        tr("Yes"), tr("No"));
      if (ans == 0) {
        return false;
      }
    }
    else {
      output = getCmdOut("smartctl -H " + drv + "|egrep \"Reallocated|Pending|Uncorrect\"");
      if (output != "") {
        msg = output + "\n\nThe disk you selected has a number of SMART warnings.\nFor more information run \"smartctl -A " + drv + "\" in console, as root.\n\nDo you want to abort the installation?";
        ans = QMessageBox::warning(0, QString::null, msg,
          tr("Yes"), tr("No"));
        if (ans == 0) {
          return false;
        }
      }
    }
    return true;
}

/////////////////////////////////////////////////////////////////////////
// install functions

bool isRootFormatted;
bool isHomeFormatted;
bool isFormatExt3;
bool isFormatReiserfs;
//added by anticapitalista
bool isFormatExt2;
bool isFormatExt4;
bool isFormatJfs;
bool isFormatXfs;
bool isFormatBtrfs;
bool isFormatReiser4;

int MInstall::getPartitionNumber()
{
    return getCmdOut("cat /proc/partitions | grep '[h,s,v].[a-z][1-9]$' | wc -l").toInt();
}

// unmount antiX in case we are retrying
void MInstall::prepareToInstall() {

  updateStatus(tr("Ready to install antiX filesystem"), 0);
  // unmount /home if it exists
  system("/bin/umount -l /mnt/antiX/home >/dev/null 2>&1");
  system("/bin/umount -l /mnt/antiX >/dev/null 2>&1");

  isRootFormatted = false;
  isHomeFormatted = false;
  isFormatExt4 = true;
}

bool MInstall::makeSwapPartition(QString dev) {
  QString cmd = QString("/sbin/mkswap %1").arg(dev);
  if (system(cmd.toAscii()) != 0) {
    // error
    return false;
  }
  return true;
}

bool MInstall::makeLinuxPartition(QString dev, const char *type, bool bad) {
  QString cmd;
  if (strncmp(type, "reiserfs", 4) == 0) {
    cmd = QString("/sbin/mkfs.reiserfs -q %1").arg(dev);
    
 } else {
    if (strncmp(type, "reiser4", 4) == 0) {
      // reiser4
        cmd = QString("/sbin/mkfs.reiser4 -f -y %1").arg(dev);
    
  } else {
    if (strncmp(type, "ext3", 4) == 0) {
      // ext3
      if (bad) {
        // do with badblocks
        cmd = QString("/sbin/mkfs.ext3 -c %1").arg(dev);
       } else {
        // do no badblocks
        cmd = QString("/sbin/mkfs.ext3 %1").arg(dev);
      } 
    } else {
     if (strncmp(type, "ext2", 4) == 0) {
      // ext2
      if (bad) {
        // do with badblocks
        cmd = QString("/sbin/mkfs.ext2 -c %1").arg(dev);
       } else {
        // do no badblocks
        cmd = QString("/sbin/mkfs.ext2 %1").arg(dev);
      } 
    } else {
     if (strncmp(type, "btrfs", 4) == 0) {
      // btrfs and set up fsck
      system("/bin/cp -fp /bin/true /sbin/fsck.auto");
      if (bad) {
        // do with badblocks
        cmd = QString("/sbin/mkfs.btrfs -c %1").arg(dev);
       } else {
        // do no badblocks
        cmd = QString("/sbin/mkfs.btrfs %1").arg(dev);
      }
    } else {
     //xfs
     if (strncmp(type, "xfs", 4) == 0) {
      if (bad) {
        // do with badblocks
        cmd = QString("/sbin/mkfs.xfs -f -c %1").arg(dev);
       } else {
        // do no badblocks
        cmd = QString("/sbin/mkfs.xfs -f %1").arg(dev);
      } 
    } else {
     //jfs
     if (strncmp(type, "jfs", 4) == 0) {
      if (bad) {
        // do with badblocks
        cmd = QString("/sbin/mkfs.jfs -q -c %1").arg(dev);
       } else {
        // do no badblocks
        cmd = QString("/sbin/mkfs.jfs -q %1").arg(dev);
      } 
    } else {
     // must be ext4
      if (bad) {
        // do with badblocks
        cmd = QString("/sbin/mkfs.ext4 -c %1").arg(dev);
       } else {
        // do no badblocks
        cmd = QString("/sbin/mkfs.ext4 %1").arg(dev);
      }
    }  
  }
 }  
}
}
    if (system(cmd.toAscii()) != 0) {
      // error
      return false;
    }
    system("sleep 1");

    if (strncmp(type, "ext*", 4) == 0) {
      // ext4 tuning
      cmd = QString("/sbin/tune2fs -c0 -C0 -i1m %1").arg(dev);
    } 
    if (system(cmd.toAscii()) != 0) {
      // error
    }
  }
  return true;
}
}
///////////////////////////////////////////////////////////////////////////
// in this case use all of the drive

bool MInstall::makeDefaultPartitions() {
  char line[130];
  int ans;

  QString drv = QString("/dev/%1").arg(diskCombo->currentText());
  QString rootdev = QString(drv).append("1");
  QString swapdev = QString(drv).append("2");
  QString msg = QString(tr("Ok to format and use the entire disk (%1) for antiX?")).arg(drv);
  ans = QMessageBox::information(0, QString::null, msg,
         tr("Yes"), tr("No"));
 if (ans != 0) {
    // don't format--stop install
    return false;
  }
  isRootFormatted = true;
  isHomeFormatted = false;
  isFormatExt3 = true;

  // entire disk, create partitions
  updateStatus(tr("Creating required partitions"), 1);

  // ready to partition
  // try to be sure that entire drive is available
  system("/sbin/swapoff -a 2>&1");

  // unmount root part
  QString cmd = QString("/bin/umount -l %1 >/dev/null 2>&1").arg(rootdev);
  if (system(cmd.toAscii()) != 0) {
    // error
  }

  bool free = false;
  bool fiok = true;
  int fi = freeSpaceEdit->text().toInt(&fiok,10);
  if (!fiok) {
    fi = 0;
  }

  const char *tstr;                    // total size

  // calculate new partition sizes
  // get the total disk size
  sleep(1);
  cmd = QString("/sbin/sfdisk -s %1").arg(drv);
  FILE *fp = popen(cmd.toAscii(), "r");
  fgets(line, sizeof line, fp);
  tstr = strtok(line," ");
  pclose(fp);
  int sz = atoi(tstr);
  sz = sz / 1024;
  // pre-compensate for rounding errors in disk geometry
  sz = sz - 32;
  // 2048 swap should be ample
  int si = 2048;
  if (sz < 2048) {
    si = 128;
  } else if (sz < 3096) {
    si = 256;
  } else if (sz < 4096) {
    si = 512;
  } else if (sz < 12288) {
    si = 1024;
  }
  int ri = sz - si;

  if (fi > 0 && ri > 8192) {
      // allow fi
      // ri is capped until fi is satisfied
      if (fi > ri - 8192) {
        fi = ri - 8192;
      }
      ri = ri - fi;
      free = true;             // free space will be unallocated
  } else {
    // no fi
    fi = 0;
    free = false;		// no free space
  }

  // new partition table
  cmd = QString("/bin/dd if=/dev/zero of=%1 bs=512 count=100").arg(drv);
  system(cmd.toAscii());
  cmd = QString("/sbin/sfdisk --no-reread -D -uM %1").arg(drv);
  fp = popen(cmd.toAscii(), "w");
  if (fp != NULL) {
    if (free) {
      cmd = QString(",%1,L,*\n,%2,S\n,,\n;\ny\n").arg(ri).arg(si);
    } else {
      cmd = QString(",%1,L,*\n,,S\n,,\n;\ny\n").arg(ri);
    }
    fputs(cmd.toAscii(), fp);
    fputc(EOF, fp);
    pclose(fp);
  } else {
    // error
    return false;
  }

  updateStatus(tr("Formatting swap partition"), 2);
  system("sleep 1");
  if (!makeSwapPartition(swapdev)) {
    return false;
  }
  system("sleep 1");
  system("/usr/sbin/buildfstab -r");
  system("/sbin/swapon -a 2>&1");

  updateStatus(tr("Formatting root partition"), 3);
  if (!makeLinuxPartition(rootdev, "ext4", false)) {
    return false;
  }

  system("sleep 1");
  // mount partitions
  if (!mountPartition(rootdev, "/mnt/antiX")) {
    return false;
  }

  // on root, make sure it exists
  system("sleep 1");
  mkdir("/mnt/antiX/home",0755);

  on_diskCombo_activated();
  rootCombo->setCurrentIndex(0);
  on_rootCombo_activated();
  swapCombo->setCurrentIndex(1);
  on_swapCombo_activated();
  homeCombo->setCurrentIndex(0);

  return true;
}

///////////////////////////////////////////////////////////////////////////
// Make the chosen partitions and mount them

bool MInstall::makeChosenPartitions() {
  char line[130];
  QString msg;
  int ans;
  char type[20];
  QString cmd;

  QString drv = QString("/dev/%1").arg(diskCombo->currentText());

  // get config
  strncpy(type, rootTypeCombo->currentText().toAscii(), 4);

  strcpy(line, rootCombo->currentText().toAscii());
  char *tok = strtok(line, " -");
  QString rootdev = QString("/dev/%1").arg(tok);

  strcpy(line, swapCombo->currentText().toAscii());
  tok = strtok(line, " -");
  QString swapdev = QString("/dev/%1").arg(tok);

  strcpy(line, homeCombo->currentText().toAscii());
  tok = strtok(line, " -");
  QString homedev = QString("/dev/%1").arg(tok);

  if (rootdev.compare("/dev/none") == 0) {
    QMessageBox::critical(0, QString::null,
      tr("You must choose a root partition.\nThe root partition must be at least 2.5 GB."));
    return false;
  }
  if (!rootCombo->currentText().contains("linux", Qt::CaseInsensitive)) {
    msg = QString(tr("The partition you selected for root, appears to be a MS-Windows partition.  Are you sure you want to reformat this partition?")).arg(rootdev);
    ans = QMessageBox::warning(0, QString::null, msg,
        tr("Yes"), tr("No"));
    if (ans != 0) {
      // don't format--stop install
      return false;
    }
  }
  if (!(saveHomeCheck->isChecked() && homedev.compare("/dev/root") == 0)) {
    msg = QString(tr("Ok to format and destroy all data on \n%1 for the / (root) partition?")).arg(rootdev);
  } else {
    msg = QString(tr("Ok to use (no format) \n%1 as the / (root) partition?")).arg(rootdev);
  }
  ans = QMessageBox::warning(0, QString::null, msg,
        tr("Yes"), tr("No"));
  if (ans != 0) {
    // don't format--stop install
    return false;
  }

  // format swap
  if (swapdev.compare("/dev/none") != 0) {
    msg = QString(tr("Ok to format and destroy all data on \n%1 for the swap partition?")).arg(swapdev);
    ans = QMessageBox::warning(0, QString::null, msg,
        tr("Yes"), tr("No"));
  if (ans != 0) {
      // don't format--stop install
      return false;
    }
  }
  
  // format /home?
  if (homedev.compare("/dev/root") != 0) {
    if (!rootCombo->currentText().contains("linux", Qt::CaseInsensitive)) {
      msg = QString(tr("The partition you selected for /home, appears to be a MS-Windows partition.  Are you sure you want to reformat this partition?")).arg(rootdev);
      ans = QMessageBox::warning(0, QString::null, msg,
        tr("Yes"), tr("No"));
      if (ans != 0) {
        // don't format--stop install
        return false;
      }
    }
    if (saveHomeCheck->isChecked()) {
      msg = QString(tr("Ok to reuse (no reformat) %1 as the /home partition?")).arg(homedev);
    } else {
      msg = QString(tr("Ok to format and destroy all data on %1 for the /home partition?")).arg(homedev);
    }

    ans = QMessageBox::warning(0, QString::null, msg,
        tr("Yes"), tr("No"));
  if (ans != 0) {
      // don't format--stop install
      return false;
    }
  }

  updateStatus(tr("Preparing required partitions"), 1);

  // unmount /home part if it exists
  if (homedev.compare("/dev/root") != 0) {
    // has homedev
    cmd = QString("pumount %1").arg(homedev);
    if (system(cmd.toAscii()) != 0) {
      // error
      if (swapoff(homedev.toAscii()) != 0) {
      }
    }
  }

  // unmount root part
  cmd = QString("pumount %1").arg(rootdev);
  if (system(cmd.toAscii()) != 0) {
    // error
    if (swapoff(rootdev.toAscii()) != 0) {
    }
  }

  if (swapdev.compare("/dev/none") != 0) {
    if (swapoff(swapdev.toAscii()) != 0) {
      cmd = QString("pumount %1").arg(swapdev);
      if (system(cmd.toAscii()) != 0) {
      }
    }
    updateStatus(tr("Formatting swap partition"), 2);
    // always set type
    QString cmd = QString("/sbin/sfdisk %1 -c %2 82").arg(swapdev.mid(0,8)).arg(swapdev.mid(8));
    system(cmd.toAscii());
    system("sleep 1");
    if (!makeSwapPartition(swapdev)) {
      return false;
    }
    // enable the new swap partition asap
    system("sleep 1");
    swapon(swapdev.toAscii(),0);
  }

  // maybe format root
  if (!(saveHomeCheck->isChecked() && homedev.compare("/dev/root") == 0)) {
    updateStatus(tr("Formatting the / (root) partition"), 3);
    // always set type
    QString cmd = QString("/sbin/sfdisk %1 -c %2 83").arg(rootdev.mid(0,8)).arg(rootdev.mid(8));
    system(cmd.toAscii());
    system("sleep 1");
    if (!makeLinuxPartition(rootdev, type, badblocksCheck->isChecked())) {
      return false;
    }
    system("sleep 1");
    if (!mountPartition(rootdev, "/mnt/antiX")) {
      return false;
    }
    isRootFormatted = true;
    if (strncmp(type, "ext4", 4) == 0) {
      isFormatExt3 = false;
      isFormatReiserfs = false;
    } else if (strncmp(type, "reis", 4) == 0) {
      isFormatExt3 = false;
      isFormatReiserfs = true;
    } else {
      isFormatExt3 = true;
      isFormatReiserfs = false;
    }
  }
  // maybe format home
  if (saveHomeCheck->isChecked()) {
    // save home
    if (homedev.compare("/dev/root") != 0) {
      // not on root
      system("rm -r -d /mnt/antiX/home >/dev/null 2>&1");
      updateStatus(tr("Mounting the /home partition"), 8);
      if (!mountPartition(homedev, "/mnt/antiX/home")) {
        return false;
      }
    } else {
      // on root, make sure it exists
      system("sleep 1");
      mkdir("/mnt/antiX/home",0755);
    }
  } else {
    // don't save home
    system("/bin/rm -r -d /mnt/antiX/home >/dev/null 2>&1");
    mkdir("/mnt/antiX/home",0755);
    if (homedev.compare("/dev/root") != 0) {
      // not on root
      updateStatus(tr("Formatting the /home partition"), 8);
      // always set type
      QString cmd = QString("/sbin/sfdisk %1 -c %2 83").arg(homedev.mid(0,8)).arg(homedev.mid(8));
      system(cmd.toAscii());
      system("sleep 1");
      if (!makeLinuxPartition(homedev, type, badblocksCheck->isChecked())) {
        return false;
      }
      system("sleep 1");
      if (!mountPartition(homedev, "/mnt/antiX/home")) {
        return false;
      }
      isHomeFormatted = true;
    }
  }
  // mount all swaps
  system("sleep 1");
  system("/usr/sbin/buildfstab -r");
  system("/sbin/swapon -a 2>&1");

  return true;
}

void MInstall::installLinux() {
  char line[130];

  QString drv = QString("/dev/%1").arg(diskCombo->currentText());

  strcpy(line, rootCombo->currentText().toAscii());
  char *tok = strtok(line, " -");
  QString rootdev = QString("/dev/%1").arg(tok);

  strcpy(line, homeCombo->currentText().toAscii());
  tok = strtok(line, " -");
  QString homedev = QString("/dev/%1").arg(tok);

  // maybe root was formatted
  if (isRootFormatted) {
    // yes it was
    copyLinux();
  } else {
    // no--it's being reused
    updateStatus(tr("Mounting the / (root) partition"), 3);
    mountPartition(rootdev, "/mnt/antiX");
    // set all connections in advance
    disconnect(timer, SIGNAL(timeout()), 0, 0);
    connect(timer, SIGNAL(timeout()), this, SLOT(delTime()));
    disconnect(proc, SIGNAL(started()), 0, 0);
    connect(proc, SIGNAL(started()), this, SLOT(delStart()));
    disconnect(proc, SIGNAL(finished(int, QProcess::ExitStatus)), 0, 0);
    connect(proc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(delDone(int, QProcess::ExitStatus)));
    // setup and start the process
    QString cmd = QString("/bin/rm -r -d /mnt/antiX/bin /mnt/antiX/boot");
    cmd.append(" /mnt/antiX/dev /mnt/antiX/etc /mnt/antiX/lib /mnt/antiX/mnt");
    cmd.append(" /mnt/antiX/opt /mnt/antiX/proc /mnt/antiX/root /mnt/antiX/sbin");
    cmd.append(" /mnt/antiX/selinux /mnt/antiX/sys /mnt/antiX/tmp /mnt/antiX/usr");
    cmd.append(" /mnt/antiX/var /mnt/antiX/run");
    proc->start(cmd);
  }
}

void MInstall::copyLinux() {
  char line[130];

  QString drv = QString("/dev/%1").arg(diskCombo->currentText());

  strcpy(line, rootCombo->currentText().toAscii());
  char *tok = strtok(line, " -");
  QString rootdev = QString("/dev/%1").arg(tok);

  strcpy(line, homeCombo->currentText().toAscii());
  tok = strtok(line, " -");
  QString homedev = QString("/dev/%1").arg(tok);

  // make empty dirs for opt, dev, proc, sys, run,
  // home already done
  updateStatus(tr("Creating system directories"), 9);
  mkdir("/mnt/antiX/opt", 0755);
  mkdir("/mnt/antiX/dev", 0755);
  mkdir("/mnt/antiX/proc", 0755);
  mkdir("/mnt/antiX/sys", 0755);
  mkdir("/mnt/antiX/run", 0755);

  // copy most except usr, mnt and home
  // must copy boot even if saving, the new files are required
  // media is already ok, usr will be done next, home will be done later
  // set all connections in advance
  disconnect(timer, SIGNAL(timeout()), 0, 0);
  connect(timer, SIGNAL(timeout()), this, SLOT(copyTime()));
  disconnect(proc, SIGNAL(started()), 0, 0);
  connect(proc, SIGNAL(started()), this, SLOT(copyStart()));
  disconnect(proc, SIGNAL(finished(int, QProcess::ExitStatus)), 0, 0);
  connect(proc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(copyDone(int, QProcess::ExitStatus)));
    // setup and start the process
  QString cmd = QString("/bin/cp -a /live/aufs/bin /live/aufs/boot /live/aufs/dev");
  cmd.append(" /live/aufs/etc /live/aufs/lib /live/aufs/media /live/aufs/mnt");
  cmd.append(" /live/aufs/opt /live/aufs/root /live/aufs/sbin /live/aufs/selinux /live/aufs/usr");
  cmd.append(" /live/aufs/var /live/aufs/home /mnt/antiX");
  proc->start(cmd);
}

///////////////////////////////////////////////////////////////////////////
// install loader

// build a grub configuration and install grub
bool MInstall::installLoader() {
  QString cmd;
  QString val = getCmdOut("ls /mnt/antiX/boot | grep 'initrd.img-3.6'");

  // the old initrd is not valid for this hardware
  if (!val.isEmpty()) {
    cmd = QString("rm -f /mnt/antiX/boot/%1").arg(val);
    system(cmd.toAscii());
  }

  // maybe replace the initrd.img file
  //if (initrdCheck->isChecked()) {
      setCursor(QCursor(Qt::WaitCursor));
      system("chroot /mnt/antiX mount /proc");
      cmd = QString("chroot /mnt/antiX mkinitramfs -o /boot/%1").arg(val);
      system(cmd.toAscii());
      system("chroot /mnt/antiX umount /proc");
      setCursor(QCursor(Qt::ArrowCursor));
  //}

  if (!grubCheckBox->isChecked()) {
    // skip it
    return true;
  }

  QString bootdrv = QString(grubBootCombo->currentText()).section(" ", 0, 0);
  QString rootpart = QString(rootCombo->currentText()).section(" ", 0, 0);
  QString boot;
  //int rootdev = diskCombo->currentIndex();


  if (grubMbrButton->isChecked()) {
    boot = bootdrv;
  } else {
    boot = rootpart;
  }

  // install Grub?
  QString msg = QString( tr("Ok to install GRUB bootloader at %1 ?")).arg(boot);
  int ans = QMessageBox::warning(this, QString::null, msg,
        tr("Yes"), tr("No"));
  if (ans != 0) {
    return false;
  }
  setCursor(QCursor(Qt::WaitCursor));

  // install new Grub now
  cmd = QString("grub-install --recheck --no-floppy --force --boot-directory=/mnt/antiX/boot /dev/%1").arg(boot);
  if (system(cmd.toAscii()) != 0) {
    // error, try again
    // this works for reiser-grub bug
    if (system(cmd.toAscii()) != 0) {
      // error
      setCursor(QCursor(Qt::ArrowCursor));
      QMessageBox::critical(this, QString::null,
        tr("Sorry, installing GRUB failed. This may be due to a change in the disk formatting. You can uncheck GRUB and finish installing antiX then reboot to the DVD and repair the installation with the reinstall GRUB function."));
      return false;
    }
  }
 
  // update grub config
  system("mount -o bind /dev /mnt/antiX/dev");
  system("mount -o bind /sys /mnt/antiX/sys");
  system("mount -o bind /proc /mnt/antiX/proc");

  system("chroot /mnt/antiX update-grub");

  system("umount /mnt/antiX/proc");
  system("umount /mnt/antiX/sys");
  system("umount /mnt/antiX/dev");

  setCursor(QCursor(Qt::ArrowCursor));
  return true;
}

/////////////////////////////////////////////////////////////////////////
// create the user, can not be rerun

bool MInstall::setUserName() {
  int ans;
  DIR *dir;
  QString msg, cmd;

  // see if user directory already exists
  QString dpath = QString("/mnt/antiX/home/%1").arg(userNameEdit->text());
  if ((dir = opendir(dpath.toAscii())) != NULL) {
    // already exists
    closedir(dir);
    msg = QString( tr("The home directory for %1 already exists.Would you like to reuse the old home directory?")).arg(userNameEdit->text());
    ans = QMessageBox::information(0, QString::null, msg,
        tr("Yes"), tr("No"));
    if (ans != 0) {
      // don't reuse -- maybe save the old home
      msg = QString( tr("Would you like to save the old home directory\nand create a new home directory?"));
      ans = QMessageBox::information(0, QString::null, msg,
        tr("Yes"), tr("No"));
      if (ans == 0) {
        // save the old directory
        cmd = QString("mv -f %1 %2.001").arg(dpath).arg(dpath);
        if (system(cmd.toAscii()) != 0) {
          cmd = QString("mv -f %1 %2.002").arg(dpath).arg(dpath);
          if (system(cmd.toAscii()) != 0) {
            cmd = QString("mv -f %1 %2.003").arg(dpath).arg(dpath);
            if (system(cmd.toAscii()) != 0) {
              cmd = QString("mv -f %1 %2.004").arg(dpath).arg(dpath);
              if (system(cmd.toAscii()) != 0) {
                cmd = QString("mv -f %1 %2.005").arg(dpath).arg(dpath);
                if (system(cmd.toAscii()) != 0) {
                  QMessageBox::critical(0, QString::null,
                    tr("Sorry, failed to save old home directory. Before proceeding,\nyou'll have to select a different username or\ndelete a previously saved copy of your home directory."));
                  return false;
                }
              }
            }
          }
	      }	
      } else {
        // don't save and don't reuse -- delete?
        msg = QString( tr("Would you like to delete the old home directory for %1?")).arg(userNameEdit->text());
        ans = QMessageBox::information(0, QString::null, msg,
            tr("Yes"), tr("No"));
        if (ans == 0) {
          // delete the directory
          cmd = QString("rm -f %1").arg(dpath);
          if (system(cmd.toAscii()) != 0) {
            QMessageBox::critical(0, QString::null,
              tr("Sorry, failed to delete old home directory. Before proceeding, \nyou'll have to select a different username."));
            return false;
          }
        } else {  
	        // don't save, reuse or delete -- can't proceed
	        QMessageBox::critical(0, QString::null,
            tr("You've chosen to not use, save or delete the old home directory.\nBefore proceeding, you'll have to select a different username."));
          return false;
      	}
      }
    }
  }

  if ((dir = opendir(dpath.toAscii())) == NULL) {
    // dir does not exist, must create it
    // copy skel to demo
    if (system("cp -a /mnt/antiX/etc/skel /mnt/antiX/home") != 0) {
      QMessageBox::critical(0, QString::null,
        tr("Sorry, failed to create user directory."));
      return false;
    }
    cmd = QString("mv -f /mnt/antiX/home/skel %1").arg(dpath);
    if (system(cmd.toAscii()) != 0) {
      QMessageBox::critical(0, QString::null,
        tr("Sorry, failed to name user directory."));
      return false;
    }
  } else {
    // dir does exist, clean it up
    cmd = QString("cp /mnt/antiX/etc/skel/.bash_profile %1").arg(dpath);
    system(cmd.toAscii());
    cmd = QString("cp /mnt/antiX/etc/skel/.bashrc %1").arg(dpath);
    system(cmd.toAscii());
    cmd = QString("cp /mnt/antiX/etc/skel/.gtkrc %1").arg(dpath);
    system(cmd.toAscii());
    cmd = QString("cp /mnt/antiX/etc/skel/.gtkrc-2.0 %1").arg(dpath);
    system(cmd.toAscii());
  }
  // fix the ownership, demo=newuser
  cmd = QString("chown -R demo.users %1").arg(dpath);
  if (system(cmd.toAscii()) != 0) {
    QMessageBox::critical(0, QString::null,
      tr("Sorry, failed to set ownership of user directory."));
    return false;
  }

  // change in files
  replaceStringInFile("demo", userNameEdit->text(), "/mnt/antiX/etc/group");
  replaceStringInFile("demo", userNameEdit->text(), "/mnt/antiX/etc/gshadow");
  replaceStringInFile("demo", userNameEdit->text(), "/mnt/antiX/etc/passwd");
  replaceStringInFile("demo", userNameEdit->text(), "/mnt/antiX/etc/shadow"); 
  replaceStringInFile("demo", userNameEdit->text(), "/mnt/antiX/etc/slim.conf");
  if (autologinCheckBox->isChecked()) {
    replaceStringInFile("#auto_login", "auto_login", "/mnt/antiX/etc/slim.conf");
    replaceStringInFile("#default_user ", "default_user ", "/mnt/antiX/etc/slim.conf");
  }


  cmd = QString("touch /mnt/antiX/var/mail/%1").arg(userNameEdit->text());
  system(cmd.toAscii());

  return true;
}

bool MInstall::setPasswords() {
  FILE *fp = popen("chroot /mnt/antiX passwd root", "w");
  bool fpok = true;
  QString cmd = QString("%1\n").arg(rootPasswordEdit->text());
  if (fp != NULL) {
    sleep(6);
    if (fputs(cmd.toAscii(), fp) >= 0) {
      fflush(fp);
      sleep(2);
      if (fputs(cmd.toAscii(), fp) < 0) {
        fpok = false;
      }
      fflush(fp);
    } else {
      fpok = false;
    }
    pclose(fp);
  } else {
    fpok = false;
  }

  if (!fpok) {
    QMessageBox::critical(0, QString::null,
      tr("Sorry, unable to set root password."));
    return false;
  }

  fp = popen("chroot /mnt/antiX passwd demo", "w");
  fpok = true;
  cmd = QString("%1\n").arg(userPasswordEdit->text());
  if (fp != NULL) {
    sleep(1);
    if (fputs(cmd.toAscii(), fp) >= 0) {
      fflush(fp);
      sleep(1);
      if (fputs(cmd.toAscii(), fp) < 0) {
        fpok = false;
      }
      fflush(fp);
    } else {
      fpok = false;
    }
    pclose(fp);
  } else {
    fpok = false;
  }

  if (!fpok) {
    QMessageBox::critical(0, QString::null,
      tr("Sorry, unable to set user password."));
    return false;
  }
  return true;
}

bool MInstall::setUserInfo() {

  //validate data before proceeding
  // see if username is reasonable length
  if (strlen(userNameEdit->text().toAscii()) < 2) {
    QMessageBox::critical(0, QString::null,
      tr("The user name needs to be at least\n"
      "2 characters long. Please select\n"
      "a longer name before proceeding."));
    return false;
  }
  if (strlen(userPasswordEdit->text().toAscii()) < 2) {
    QMessageBox::critical(0, QString::null,
      tr("The user password needs to be at least\n"
      "2 characters long. Please select\n"
      "a longer password before proceeding."));
    return false;
  }
  if (strlen(rootPasswordEdit->text().toAscii()) < 2) {
    QMessageBox::critical(0, QString::null,
      tr("The root password needs to be at least\n"
      "2 characters long. Please select\n"
      "a longer password before proceeding."));
    return false;
  }
  // check that user name is not already used
  QString cmd = QString("grep '^%1' /etc/passwd >/dev/null").arg(userNameEdit->text());
  if (system(cmd.toAscii()) == 0) {
        QMessageBox::critical(0, QString::null,
            tr("Sorry that name is in use.\n"
              "Please select a different name.\n"));
      return false;
  }
        
  if (strcmp(userPasswordEdit->text().toAscii(), userPasswordEdit2->text().toAscii()) != 0) {
    QMessageBox::critical(0, QString::null,
      tr("The user password entries do\n"
        "not match.  Please try again."));
    return false;
  }
  if (strcmp(rootPasswordEdit->text().toAscii(), rootPasswordEdit2->text().toAscii()) != 0) {
    QMessageBox::critical(0, QString::null,
      tr("The root password entries do\n"
        " not match.  Please try again."));
    return false;
  }
  if (strlen(userPasswordEdit->text().toAscii()) < 2) {
    QMessageBox::critical(0, QString::null,
      tr("The user password needs to be at least\n"
      "2 characters long. Please select\n"
      "a longer password before proceeding."));
    return false;
  }
  if (strlen(rootPasswordEdit->text().toAscii()) < 2) {
    QMessageBox::critical(0, QString::null,
      tr("The root password needs to be at least\n"
      "2 characters long. Please select\n"
      "a longer password before proceeding."));
    return false;
  }

  if (!setPasswords()) {
    return false;
  }
  return setUserName();
}

/////////////////////////////////////////////////////////////////////////
// set the computer name, can not be rerun

bool MInstall::setComputerName() {

  // see if name is reasonable
  if (computerNameEdit->text().length() < 2) {
    QMessageBox::critical(0, QString::null,
      tr("Sorry your computer name needs to be\nat least 2 characters long. You'll have to\nselect a different name before proceeding."));
    return false;
  }
  // see if name is reasonable
  if (computerDomainEdit->text().length() < 2) {
    QMessageBox::critical(0, QString::null,
      tr("Sorry your computer domain needs to be at least\n2 characters long. You'll have to select a different\nname before proceeding."));
    return false;
  }

  QString val = getCmdValue("dpkg -s samba | grep '^Status'", "ok", " ", " ");
  if (val.compare("installed") == 0) {
    // see if name is reasonable
    if (computerGroupEdit->text().length() < 2) {
      QMessageBox::critical(0, QString::null,
        tr("Sorry your workgroup needs to be at least\n2 characters long. You'll have to select a different\nname before proceeding."));
      return false;
    }
    replaceStringInFile("antix1", computerNameEdit->text(), "/mnt/antiX/etc/samba/smb.conf");
    replaceStringInFile("WORKGROUP", computerGroupEdit->text(), "/mnt/antiX/etc/samba/smb.conf");
  }
  if (sambaCheckBox->isChecked()) {
    system("mv -f /mnt/antiX/etc/rc5.d/K01smbd /mnt/antiX/etc/rc5.d/S03smbd >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc4.d/K01smbd /mnt/antiX/etc/rc4.d/S03smbd >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc3.d/K01smbd /mnt/antiX/etc/rc3.d/S03smbd >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc2.d/K01smbd /mnt/antiX/etc/rc2.d/S03smbd >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc5.d/K01samba-ad-dc /mnt/antiX/etc/rc5.d/S01samba-ad-dc >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc4.d/K01samba-ad-dc /mnt/antiX/etc/rc4.d/S01samba-ad-dc >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc3.d/K01samba-ad-dc /mnt/antiX/etc/rc3.d/S01samba-ad-dc >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc2.d/K01samba-ad-dc /mnt/antiX/etc/rc2.d/S01samba-ad-dc >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc5.d/K01nmbd /mnt/antiX/etc/rc5.d/S01nmbd >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc4.d/K01nmbd /mnt/antiX/etc/rc4.d/S01nmbd >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc3.d/K01nmbd /mnt/antiX/etc/rc3.d/S01nmbd >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc2.d/K01nmbd /mnt/antiX/etc/rc2.d/S01nmbd >/dev/null 2>&1");
  } else {
    system("mv -f /mnt/antiX/etc/rc5.d/S03smbd /mnt/antiX/etc/rc5.d/K01smbd >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc4.d/S03smbd /mnt/antiX/etc/rc4.d/K01smbd >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc3.d/S03smbd /mnt/antiX/etc/rc3.d/K01smbd >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc2.d/S03smbd /mnt/antiX/etc/rc2.d/K01smbd >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc5.d/S01samba-ad-dc /mnt/antiX/etc/rc5.d/K01samba-ad-dc >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc4.d/S01samba-ad-dc /mnt/antiX/etc/rc4.d/K01samba-ad-dc >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc3.d/S01samba-ad-dc /mnt/antiX/etc/rc3.d/K01samba-ad-dc >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc2.d/S01samba-ad-dc /mnt/antiX/etc/rc2.d/K01samba-ad-dc >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc5.d/S01nmbd /mnt/antiX/etc/rc5.d/K01nmbd >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc4.d/S01nmbd /mnt/antiX/etc/rc4.d/K01nmbd >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc3.d/S01nmbd /mnt/antiX/etc/rc3.d/K01nmbd >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc2.d/S01nmbd /mnt/antiX/etc/rc2.d/K01nmbd >/dev/null 2>&1");
  }

  replaceStringInFile("antix1", computerNameEdit->text(), "/mnt/antiX/etc/hosts");

  QString cmd = QString("echo \"%1\" | cat > /mnt/antiX/etc/hostname").arg(computerNameEdit->text());
  system(cmd.toAscii());
  cmd = QString("echo \"%1\" | cat > /mnt/antiX/etc/mailname").arg(computerNameEdit->text());
  system(cmd.toAscii());
  cmd = QString("sed -i 's/.*send host-name.*/send host-name \"%1\";/g' /mnt/antiX/etc/dhcp/dhclient.conf").arg(computerNameEdit->text());
  system(cmd.toAscii());
  cmd = QString("echo \"%1\" | cat > /mnt/antiX/etc/defaultdomain").arg(computerDomainEdit->text());
  system(cmd.toAscii());
 
  return true;
}

void MInstall::setLocale() {
  QString cmd2;
  QString kb = keyboardCombo->currentText();
  //keyboard
  QString cmd = QString("chroot /mnt/antiX /usr/sbin/install-keymap \"%1\"").arg(kb);
  system(cmd.toAscii());
  if (kb == "uk") {
    kb = "gb";
  }
  cmd = QString("sed -i 's/.*us/XKBLAYOUT=\"%1,us\/g' /mnt/antiX/etc/default/keyboard").arg(kb);
  system(cmd.toAscii());

  //locale
  cmd = QString("chroot /mnt/antiX /usr/sbin/update-locale \"LANG=%1\"").arg(localeCombo->currentText());
  system(cmd.toAscii());
  cmd = QString("Language=%1").arg(localeCombo->currentText());

  //

  // timezone
  system("cp -f /etc/default/rcS /mnt/antiX/etc/default");
  system("cp -f /etc/adjtime /mnt/antiX/etc/");
  // /etc/localtime is a copy of one of the timezone files in /usr/share/zoneinfo. Use the one selected by the user.
  cmd = QString("cp -f /usr/share/zoneinfo/%1 /mnt/antiX/etc/localtime").arg(timezoneCombo->currentText());
  system(cmd.toAscii());
  // /etc/timezone is text file with the timezone written in it. Write the user-selected timezone in it now.
  cmd = QString("echo %1 > /mnt/antiX/etc/timezone").arg(timezoneCombo->currentText());
  system(cmd.toAscii());

  if (gmtCheckBox->isChecked()) {
    replaceStringInFile("^UTC", "LOCAL", "/mnt/antiX/etc/adjtime");
}

}

void MInstall::setServices() {
  setCursor(QCursor(Qt::WaitCursor));
 
  if (cpufreqItem != NULL && cpufreqItem->checkState(0) == Qt::Checked) {
    system("mv -f /mnt/antiX/etc/rc5.d/K97cpufrequtils /mnt/antiX/etc/rc5.d/S03cpufrequtils >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc4.d/K97cpufrequtils /mnt/antiX/etc/rc4.d/S03cpufrequtils >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc3.d/K97cpufrequtils /mnt/antiX/etc/rc3.d/S03cpufrequtils >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc2.d/K97cpufrequtils /mnt/antiX/etc/rc2.d/S03cpufrequtils >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc5.d/K98loadcpufreq /mnt/antiX/etc/rc5.d/S02loadcpufreq >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc4.d/K98loadcpufreq /mnt/antiX/etc/rc4.d/S02loadcpufreq >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc3.d/K98loadcpufreq /mnt/antiX/etc/rc3.d/S02loadcpufreq >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc2.d/K98loadcpufreq /mnt/antiX/etc/rc2.d/S02loadcpufreq >/dev/null 2>&1");   
    system("mv -f /mnt/antiX/etc/rc5.d/K01irqbalance /mnt/antiX/etc/rc5.d/S02irqbalance >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc4.d/K01irqbalance /mnt/antiX/etc/rc4.d/S02irqbalance >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc3.d/K01irqbalance /mnt/antiX/etc/rc3.d/S02irqbalance >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc2.d/K01irqbalance /mnt/antiX/etc/rc2.d/S02irqbalance >/dev/null 2>&1");
  } else {
    system("mv -f /mnt/antiX/etc/rc5.d/S03cpufrequtils /mnt/antiX/etc/rc5.d/K97cpufrequtils >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc4.d/S03cpufrequtils /mnt/antiX/etc/rc4.d/K97cpufrequtils >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc3.d/S03cpufrequtils /mnt/antiX/etc/rc3.d/K97cpufrequtils >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc2.d/S03cpufrequtils /mnt/antiX/etc/rc2.d/K97cpufrequtils >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc5.d/S02loadcpufreq /mnt/antiX/etc/rc5.d/K98loadcpufreq >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc4.d/S02loadcpufreq /mnt/antiX/etc/rc4.d/K98loadcpufreq >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc3.d/S02loadcpufreq /mnt/antiX/etc/rc3.d/K98loadcpufreq >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc2.d/S02loadcpufreq /mnt/antiX/etc/rc2.d/K98loadcpufreq >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc5.d/S02irqbalance /mnt/antiX/etc/rc5.d/K01irqbalance >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc4.d/S02irqbalance /mnt/antiX/etc/rc4.d/K01irqbalance >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc3.d/S02irqbalance /mnt/antiX/etc/rc3.d/K01irqbalance >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc2.d/S02irqbalance /mnt/antiX/etc/rc2.d/K01irqbalance >/dev/null 2>&1");
  }
    
  if (wicdItem != NULL && wicdItem->checkState(0) == Qt::Checked) {
    system("mv -f /mnt/antiX/etc/rc5.d/K01wicd /mnt/antiX/etc/rc5.d/S03wicd >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc4.d/K01wicd /mnt/antiX/etc/rc4.d/S03wicd >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc3.d/K01wicd /mnt/antiX/etc/rc3.d/S03wicd >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc2.d/K01wicd /mnt/antiX/etc/rc2.d/S03wicd >/dev/null 2>&1");
  } else {
    system("mv -f /mnt/antiX/etc/rc5.d/S03wicd /mnt/antiX/etc/rc5.d/K01wicd >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc4.d/S03wicd /mnt/antiX/etc/rc4.d/K01wicd >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc3.d/S03wicd /mnt/antiX/etc/rc3.d/K01wicd >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc2.d/S03wicd /mnt/antiX/etc/rc2.d/K01wicd >/dev/null 2>&1");
  }

  if (bluetoothItem != NULL && bluetoothItem->checkState(0) == Qt::Checked) {
    system("mv -f /mnt/antiX/etc/rc5.d/K01bluetooth /mnt/antiX/etc/rc5.d/S03bluetooth >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc4.d/K01bluetooth /mnt/antiX/etc/rc4.d/S03bluetooth >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc3.d/K01bluetooth /mnt/antiX/etc/rc3.d/S03bluetooth >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc2.d/K01bluetooth /mnt/antiX/etc/rc2.d/S03bluetooth >/dev/null 2>&1");
  } else {
    system("mv -f /mnt/antiX/etc/rc5.d/S03bluetooth /mnt/antiX/etc/rc5.d/K01bluetooth >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc4.d/S03bluetooth /mnt/antiX/etc/rc4.d/K01bluetooth >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc3.d/S03bluetooth /mnt/antiX/etc/rc3.d/K01bluetooth>/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc2.d/S03bluetooth /mnt/antiX/etc/rc2.d/K01bluetooth>/dev/null 2>&1");
  }

  if (sanedItem != NULL && sanedItem->checkState(0) == Qt::Checked) {
    system("mv -f /mnt/antiX/etc/rc5.d/K01saned /mnt/antiX/etc/rc5.d/S03saned >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc4.d/K01saned /mnt/antiX/etc/rc4.d/S03saned >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc3.d/K01saned /mnt/antiX/etc/rc3.d/S03saned >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc2.d/K01saned /mnt/antiX/etc/rc2.d/S03saned >/dev/null 2>&1");
  } else {
    system("mv -f /mnt/antiX/etc/rc5.d/S03saned /mnt/antiX/etc/rc5.d/K01saned >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc4.d/S03saned /mnt/antiX/etc/rc4.d/K01saned >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc3.d/S03saned /mnt/antiX/etc/rc3.d/K01saned >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc2.d/S03saned /mnt/antiX/etc/rc2.d/K01saned >/dev/null 2>&1");
  }

  if (sshItem != NULL && sshItem->checkState(0) == Qt::Checked) {
    system("mv -f /mnt/antiX/etc/rc5.d/K98ssh /mnt/antiX/etc/rc5.d/S02ssh >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc4.d/K98ssh /mnt/antiX/etc/rc4.d/S02ssh >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc3.d/K98ssh /mnt/antiX/etc/rc3.d/S02ssh >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc2.d/K98ssh /mnt/antiX/etc/rc2.d/S02ssh >/dev/null 2>&1");
  } else {
    system("mv -f /mnt/antiX/etc/rc5.d/S02ssh /mnt/antiX/etc/rc5.d/K98ssh >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc4.d/S02ssh /mnt/antiX/etc/rc4.d/K98ssh >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc3.d/S02ssh /mnt/antiX/etc/rc3.d/K98ssh >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc2.d/S02ssh /mnt/antiX/etc/rc2.d/K98ssh >/dev/null 2>&1");
  }

  if (transmissionItem != NULL && transmissionItem->checkState(0) == Qt::Checked) {
    system("mv -f /mnt/antiX/etc/rc5.d/K01transmission-daemon /mnt/antiX/etc/rc5.d/S01transmission-daemon >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc4.d/K01transmission-daemon /mnt/antiX/etc/rc4.d/S01transmission-daemon >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc3.d/K01transmission-daemon /mnt/antiX/etc/rc3.d/S01transmission-daemon >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc2.d/K01transmission-daemon /mnt/antiX/etc/rc2.d/S01transmission-daemon >/dev/null 2>&1");
  } else {
    system("mv -f /mnt/antiX/etc/rc5.d/S01transmission-daemon /mnt/antiX/etc/rc5.d/K01transmission-daemon >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc4.d/S01transmission-daemon /mnt/antiX/etc/rc4.d/K01transmission-daemon >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc3.d/S01transmission-daemon /mnt/antiX/etc/rc3.d/K01transmission-daemon >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc2.d/S01transmission-daemon /mnt/antiX/etc/rc2.d/K01transmission-daemon >/dev/null 2>&1");
  }
  

  if (acpidItem != NULL && acpidItem->checkState(0) == Qt::Checked) {
    system("mv -f /mnt/antiX/etc/rc5.d/K98acpid /mnt/antiX/etc/rc5.d/S02acpid >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc4.d/K98acpid /mnt/antiX/etc/rc4.d/S02acpid >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc3.d/K98acpid /mnt/antiX/etc/rc3.d/S02acpid >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc2.d/K98acpid /mnt/antiX/etc/rc2.d/S02acpid >/dev/null 2>&1");
  } else {
    system("mv -f /mnt/antiX/etc/rc5.d/S02acpid /mnt/antiX/etc/rc5.d/K98acpid >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc4.d/S02acpid /mnt/antiX/etc/rc4.d/K98acpid >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc3.d/S02acpid /mnt/antiX/etc/rc3.d/K98acpid >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc2.d/S02acpid /mnt/antiX/etc/rc2.d/K98acpid >/dev/null 2>&1");
  }

  if (acpisupportItem != NULL && acpisupportItem->checkState(0) == Qt::Checked) {
    system("mv -f /mnt/antiX/etc/rc5.d/K01acpi-support /mnt/antiX/etc/rc5.d/S02acpi-support >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc4.d/K01acpi-support /mnt/antiX/etc/rc4.d/S02acpi-support >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc3.d/K01acpi-support /mnt/antiX/etc/rc3.d/S02acpi-support >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc2.d/K01acpi-support /mnt/antiX/etc/rc2.d/S02acpi-support >/dev/null 2>&1");
  } else {
    system("mv -f /mnt/antiX/etc/rc5.d/S02acpi-support /mnt/antiX/etc/rc5.d/K01acpi-support >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc4.d/S02acpi-support /mnt/antiX/etc/rc4.d/K01acpi-support >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc3.d/S02acpi-support /mnt/antiX/etc/rc3.d/K01acpi-support >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc2.d/S02acpi-support /mnt/antiX/etc/rc2.d/K01acpi-support >/dev/null 2>&1");
  }

  if (acpifakekeyItem != NULL && acpifakekeyItem->checkState(0) == Qt::Checked) {
    system("mv -f /mnt/antiX/etc/rc5.d/K99acpi-fakekey /mnt/antiX/etc/rc5.d/S01acpi-fakekey >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc4.d/K99acpi-fakekey /mnt/antiX/etc/rc4.d/S01acpi-fakekey >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc3.d/K99acpi-fakekey /mnt/antiX/etc/rc3.d/S01acpi-fakekey >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc2.d/K99acpi-fakekey /mnt/antiX/etc/rc2.d/S01acpi-fakekey >/dev/null 2>&1");
  } else {
    system("mv -f /mnt/antiX/etc/rc5.d/S01acpi-fakekey /mnt/antiX/etc/rc5.d/K99acpi-fakekey >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc4.d/S01acpi-fakekey /mnt/antiX/etc/rc4.d/K99acpi-fakekey >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc3.d/S01acpi-fakekey /mnt/antiX/etc/rc3.d/K99acpi-fakekey >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc2.d/S01acpi-fakekey /mnt/antiX/etc/rc2.d/K99acpi-fakekey >/dev/null 2>&1");
  }

  if (smartmontoolsItem != NULL && smartmontoolsItem->checkState(0) == Qt::Checked) {
    system("mv -f /mnt/antiX/etc/rc5.d/K01smartmontools /mnt/antiX/etc/rc5.d/S02smartmontools >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc4.d/K01smartmontools /mnt/antiX/etc/rc4.d/S02smartmontools >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc3.d/K01smartmontools /mnt/antiX/etc/rc3.d/S02smartmontools >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc2.d/K01smartmontools /mnt/antiX/etc/rc2.d/S02smartmontools >/dev/null 2>&1");
  } else {
    system("mv -f /mnt/antiX/etc/rc5.d/S02smartmontools /mnt/antiX/etc/rc5.d/K01smartmontools >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc4.d/S02smartmontools /mnt/antiX/etc/rc4.d/K01smartmontools >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc3.d/S02smartmontools /mnt/antiX/etc/rc3.d/K01smartmontools >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc2.d/S02smartmontools /mnt/antiX/etc/rc2.d/K01smartmontools >/dev/null 2>&1");
  }


  if (rsyncItem != NULL && rsyncItem->checkState(0) == Qt::Checked) {
    system("mv -f /mnt/antiX/etc/rc5.d/K98rsync /mnt/antiX/etc/rc5.d/S02rsync >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc4.d/K98rsync /mnt/antiX/etc/rc4.d/S02rsync >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc3.d/K98rsync /mnt/antiX/etc/rc3.d/S02rsync >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc2.d/K98rsync /mnt/antiX/etc/rc2.d/S02rsync >/dev/null 2>&1");
  } else {
    system("mv -f /mnt/antiX/etc/rc5.d/S02rsync /mnt/antiX/etc/rc5.d/K98rsync >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc4.d/S02rsync /mnt/antiX/etc/rc4.d/K98rsync >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc3.d/S02rsync /mnt/antiX/etc/rc3.d/K98rsync >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc2.d/S02rsync /mnt/antiX/etc/rc2.d/K98rsync >/dev/null 2>&1");
  }

  if (cupsItem != NULL && cupsItem->checkState(0) == Qt::Checked) {
    system("mv -f /mnt/antiX/etc/rc5.d/K01cups /mnt/antiX/etc/rc5.d/S02cups >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc4.d/K01cups /mnt/antiX/etc/rc4.d/S02cups >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc3.d/K01cups /mnt/antiX/etc/rc3.d/S02cups >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc2.d/K01cups /mnt/antiX/etc/rc2.d/S02cups >/dev/null 2>&1");
  } else {
    system("mv -f /mnt/antiX/etc/rc5.d/S02cups /mnt/antiX/etc/rc5.d/K01cups >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc4.d/S02cups /mnt/antiX/etc/rc4.d/K01cups >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc3.d/S02cups /mnt/antiX/etc/rc3.d/K01cups >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc2.d/S02cups /mnt/antiX/etc/rc2.d/K01cups >/dev/null 2>&1");
  } 

  if (dbusItem != NULL && dbusItem->checkState(0) == Qt::Checked) {
    system("mv -f /mnt/antiX/etc/rc5.d/K98dbus /mnt/antiX/etc/rc5.d/S02dbus  >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc4.d/K98dbus /mnt/antiX/etc/rc4.d/S02dbus  >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc3.d/K98dbus /mnt/antiX/etc/rc3.d/S02dbus  >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc2.d/K98dbus /mnt/antiX/etc/rc2.d/S02dbus  >/dev/null 2>&1");
  } else {
    system("mv -f /mnt/antiX/etc/rc5.d/S02dbus  /mnt/antiX/etc/rc5.d/K98dbus >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc4.d/S02dbus  /mnt/antiX/etc/rc4.d/K98dbus >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc3.d/S02dbus  /mnt/antiX/etc/rc3.d/K98dbus >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc2.d/S02dbus  /mnt/antiX/etc/rc2.d/K98dbus >/dev/null 2>&1");
  } 

  if (cronItem != NULL && cronItem->checkState(0) == Qt::Checked) {
    system("mv -f /mnt/antiX/etc/rc5.d/K98cron /mnt/antiX/etc/rc5.d/S02cron  >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc4.d/K98cron /mnt/antiX/etc/rc4.d/S02cron  >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc3.d/K98cron /mnt/antiX/etc/rc3.d/S02cron  >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc2.d/K98cron /mnt/antiX/etc/rc2.d/S02cron  >/dev/null 2>&1");
  } else {
    system("mv -f /mnt/antiX/etc/rc5.d/S02cron  /mnt/antiX/etc/rc5.d/K98cron >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc4.d/S02cron  /mnt/antiX/etc/rc4.d/K98cron >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc3.d/S02cron  /mnt/antiX/etc/rc3.d/K98cron >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc2.d/S02cron  /mnt/antiX/etc/rc2.d/K98cron >/dev/null 2>&1");
  }

  if (gpmItem != NULL && gpmItem->checkState(0) == Qt::Checked) {
    system("mv -f /mnt/antiX/etc/rc5.d/K01gpm /mnt/antiX/etc/rc5.d/S02gpm  >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc4.d/K01gpm /mnt/antiX/etc/rc4.d/S02gpm  >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc3.d/K01gpm /mnt/antiX/etc/rc3.d/S02gpm  >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc2.d/K01gpm /mnt/antiX/etc/rc2.d/S02gpm  >/dev/null 2>&1");
  } else {
    system("mv -f /mnt/antiX/etc/rc5.d/S02gpm  /mnt/antiX/etc/rc5.d/K01gpm >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc4.d/S02gpm  /mnt/antiX/etc/rc4.d/K01gpm >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc3.d/S02gpm  /mnt/antiX/etc/rc3.d/K01gpm >/dev/null 2>&1");
    system("mv -f /mnt/antiX/etc/rc2.d/S02gpm  /mnt/antiX/etc/rc2.d/K01gpm >/dev/null 2>&1");
  } 

  setCursor(QCursor(Qt::ArrowCursor));

}

void MInstall::stopInstall() {
  int curr = widgetStack->currentIndex();
  int c = widgetStack->count();

  if (curr == 3) {
    procAbort();
    QApplication::beep();
    return;
  } else if (curr >= c-3) {
    int ans = QMessageBox::information(0, QString::null,
      tr("antiX installation and configuration is complete.\n"
        "To use the new installation, reboot without the CD.\n"
        "Do you want to reboot now?"),
        tr("Yes"), tr("No"));
    if (ans == 0) {
      system("/bin/umount -l /mnt/antiX/home >/dev/null 2>&1");
      system("/bin/umount -l /mnt/antiX >/dev/null 2>&1");
      system("/usr/local/bin/persist-config --shutdown --command reboot");
      return;
    }

  } else if (curr > 3) {
    int ans = QMessageBox::critical(0, QString::null,
      tr("The installation and configuration is incomplete.\nDo you really want to stop now?"),
        tr("Yes"), tr("No"));
    if (ans != 0) {
      return;
    }
  }
  system("/bin/umount -l /mnt/antiX/home >/dev/null 2>&1");
  system("/bin/umount -l /mnt/antiX >/dev/null 2>&1");
}

void MInstall::unmountGoBack(QString msg) {
  system("/bin/umount -l /mnt/antiX/home >/dev/null 2>&1");
  system("/bin/umount -l /mnt/antiX >/dev/null 2>&1");
  goBack(msg);
}

void MInstall::goBack(QString msg) {
  QMessageBox::critical(0, QString::null, msg);
  gotoPage(1);
}

int MInstall::showPage(int curr, int next) {
  if (next == 1 && curr == 0) {
  } else if (next == 2 && curr == 1) {
    //agreeCheckBox->setChecked(false);
    if (entireDiskButton->isChecked()) {
      return 3;
    }
  } else if (next == 3 && curr == 4) {
    return 1;
  } else if (next == 5 && curr == 4) {
    if (!installLoader()) {
      return curr;
    }
  } else if (next == 9 && curr == 8) {
    if (!setUserInfo()) {
      return curr;
    }
  } else if (next == 7 && curr == 6) {
    if (!setComputerName()) {
      return curr;
    }
  } else if (next == 8 && curr == 7) {
    setLocale();
  } else if (next == 6 && curr == 5) {
    setServices();
  }
  return next;
}

void MInstall::pageDisplayed(int next) {
  QString val;

  switch (next) {
    case 1:
      setCursor(QCursor(Qt::ArrowCursor));
      ((MMain *)mmn)->setHelpText(tr("<p><b>General Instructions</b><br/>BEFORE PROCEEDING, CLOSE ALL OTHER APPLICATIONS.</p>"
        "<p>On each page, please read the instructions, make your selections, and then click on Next when you are ready to proceed. "
        "You will be prompted for confirmation before any destructive actions are performed.</p>"
        "<p>Core versions of antiX require about 1 GB of space, Base versions about 1.5GB and Full versions of antiX require about 3 GB of space. 5 GB or more is preferred."
        "You can use the entire disk or you can put antiX on existing partitions. </p>"
        "<p>If you are using PC type hardware, run GParted from here if you need to modify some partitions before doing a custom install. If you are using Apple hardware, you must never use parted or GParted on your boot drive. Instead you must setup your partitions and boot manager in OSX before installing antiX.</p>"
"<p>The ext2, ext3, ext4, jfs, xfs, btrfs and reiserfs Linux filesystems are supported and ext4 is recommended.</p>"));
      break;
    
    case 2:
      ((MMain *)mmn)->setHelpText(tr("<p><b>Limitations</b><br/>Remember, this software is provided AS-IS with no warranty what-so-ever. "
         "It's solely your responsibility to backup your data before proceeding.</p>"
         "<p><b>Choose Partitions</b><br/>antiX requires a root partition. The swap partition is optional but highly recommended. If you want to use the Suspend-to-Disk feature of antiX, you will need a swap partition that is larger than your physical memory size.</p>"
         "<p>If you choose a separate /home partition it will be easier for you to upgrade in the future, but this will not be possible if you are upgrading from an installation that does not have a separate home partition.</p>"
         "<p><b>Upgrading</b><br/>To upgrade from an existing Linux installation, select the same home partition as before and check the preference to preserve data in /home.</p>"
         "<p>If you are preserving an existing /home directory tree located on your root partition, the installer will not reformat the root partition. "
         "As a result, the installation will take much longer than usual.</p>"
         "<p><b>Preferred Filesystem Type</b><br/>For antiX Linux, you may choose to format the partitions as ext2, ext3, ext4, jfs, xfs, btrfs or reiser. </p>"
         "<p><b>Bad Blocks</b><br/>If you choose ext2, ext3 or ext4 as the format type, you have the option of checking and correcting for badblocks on the drive. "
         "The badblock check is very time consuming, so you may want to skip this step unless you suspect that your drive has badblocks.</p>"));
      break;

    case 3:
      if (!checkDisk()) {
        goBack(tr("Returning to Step 1 to select another disk."));
        break;
      }
      setCursor(QCursor(Qt::WaitCursor));
      tipsEdit->setText(tr("<p><b>Special Thanks</b><br/>Thanks to everyone who has chosen to support antiX with their time, money, suggestions, work, praise, ideas, promotion, and/or encouragement.</p>"
      "<p>Without you there would be no antiX Linux.</p>"
      "<p>anticapitalista</p>"));
      ((MMain *)mmn)->setHelpText(tr("<p><b>Installation in Progress</b><br/>"
        "antiX is installing.  For a fresh install, this will probably take 3-20 minutes, depending on the speed of your system and the size of any partitions you are reformatting.</p>"
        "<p>If you click the Abort button, the installation will be stopped as soon as possible.</p>"));
      nextButton->setEnabled(false);
      prepareToInstall();
      if (entireDiskButton->isChecked()) {
        if (!makeDefaultPartitions()) {
          // failed
          system("sleep 1");
          system("/usr/sbin/buildfstab -r");
          system("/sbin/swapon -a 2>&1");
          nextButton->setEnabled(true);
          goBack(tr("Failed to create required partitions.\nReturning to Step 1."));
          break;
        }
      } else {
        if (!makeChosenPartitions()) {
          system("sleep 1");
          system("/usr/sbin/buildfstab -r");
          system("/sbin/swapon -a 2>&1");
          nextButton->setEnabled(true);
          goBack(tr("Failed to prepare chosen partitions.\nReturning to Step 1."));
          break;
        }
      }
      system("sleep 1");
      system("/usr/sbin/buildfstab -r");
      system("/sbin/swapon -a 2>&1");
      installLinux();
      break;

    case 4:
      setCursor(QCursor(Qt::ArrowCursor));
      ((MMain *)mmn)->setHelpText(tr("<p><b>Select Boot Method</b><br/>antiX uses the GRUB bootloader to boot antiX and MS-Windows. "
        "<p>By default GRUB2 is installed in the Master Boot Record of your boot drive and replaces the boot loader you were using before. This is normal.</p>"
        "<p>If you choose to install GRUB2 at root instead of MBR, then GRUB2 will be installed at the beginning of the root partition.  This option is for experts only.</p>"
        "<p>If you do not select the Install GRUB checkbox, GRUB will not be installed at this time.  This option is for experts only.</p>"));
      backButton->setEnabled(false);
      break;

    case 5:
      ((MMain *)mmn)->setHelpText(tr("<p><b>Common Services to Enable</b><br/>Select any of the these common services that you might need with your system configuration and the services will be started automatically when you start antiX.</p>"));
      nextButton->setEnabled(true);
      backButton->setEnabled(true);
      break;

    case 6:
      ((MMain *)mmn)->setHelpText(tr("<p><b>Computer Identity</b><br/>The computer name is a common unique name which will identify your computer if it is on a network. "
        "The computer domain is unlikely to be used unless your ISP or local network requires it.</p>"
        "<p>The SaMBa Server needs to be activated if you want to use it to share some of your directories or printer "
        "with a local computer that is running MS-Windows or Mac OSX.</p>"));
      nextButton->setEnabled(true);
      backButton->setEnabled(false);
      break;

    case 7:
      ((MMain *)mmn)->setHelpText(tr("<p><b>Localization Defaults</b><br/>Set the default keyboard and locale.  These will apply unless, they are overridden later by the user.</p>"
        "<p><b>Configure Clock</b><br/>If you have an Apple or a pure Unix computer, by default the system clock is set to GMT or Universal Time.  To change, check the box for 'System clock uses LOCAL.'</p>"
        "The CD boots with the timezone preset to GMT/UTC. To change the timezone, after you reboot into the new installation, right click on the clock in the Panel and select Adjust Date & Time...</p>"));
      nextButton->setEnabled(true);
      backButton->setEnabled(false);
      break;

    case 8:
      ((MMain *)mmn)->setHelpText(tr("<p><b>Default User Login</b><br/>The root user is similar to the Administrator user in some other operating systems. "
        "You should not use the root user as your daily user account. "
        "Please enter the name for a new (default) user account that you will use on a daily basis. "
        "If needed, you can add other user accounts later. </p>"
        "<p><b>Passwords</b><br/>Enter a new password for your default user account and for the root account. "
        "Each password must be entered twice.</p>"));
      nextButton->setEnabled(true);
      break;

    case 9:
      ((MMain *)mmn)->setHelpText(tr("<p><b>Congratulations!</b><br/>You have completed the installation of antiX Linux.</p>"
        "<p><b>Finding Applications</b><br/>There are hundreds of excellent applications installed with antiX. "
        "The best way to learn about them is to browse through the Menu and try them. "
        "Many of the apps were developed specifically for the IceWM/fluxbox environment. "
        "These are shown in the main menus. "
        "<p>In addition antiX includes many standard linux applications that are run only from the commandline and therefore do not show up in Menu.</p>"));
      nextButton->setEnabled(true);
      backButton->setEnabled(false);
      break;

    default:
      // case 0 or any other
      ((MMain *)mmn)->setHelpText("<p><b>Enjoy using antiX!</b></p>");
      break;
  }
}

void MInstall::gotoPage(int next) {
  int curr = widgetStack->currentIndex();
  next = showPage(curr, next);

  // modify ui for standard cases
  if (next == 0) {
    // entering first page
    nextButton->setDefault(true);
    nextButton->setText(tr("Next >"));
    backButton->setEnabled(false);
  } else {
    // default
    backButton->setEnabled(true);
  }

  int c = widgetStack->count();
  if (next >= c-1) {
    // entering the last page
    nextButton->setText(tr("Finish"));
  } else {
    nextButton->setText(tr("Next >"));
  }
  if (next > c-1) {
    // finished
    stopInstall();
    gotoPage(0);
    return;
  }
  // display the next page
  widgetStack->setCurrentIndex(next);

  // anything to do after displaying the page
  pageDisplayed(next);
}

void MInstall::firstRefresh(QDialog *main) {
  mmn = main;
  refresh();
}

void MInstall::updatePartitionWidgets() 
{
   int numberPartitions = this->getPartitionNumber();

   if (numberPartitions > 0) {
      existing_partitionsButton->show();
   }
   else {
      existing_partitionsButton->hide();
      entireDiskButton->toggle(); 
   }
}

// widget being shown
void MInstall::refresh() {
  char line[130];

  this->updatePartitionWidgets();

//  system("umount -a 2>/dev/null");
  FILE *fp = popen("cat /proc/partitions | grep '[h,s,v].[a-z]$' | sort --key=4 2>/dev/null", "r");
  if (fp == NULL) {
    return;
  }
  diskCombo->clear();
  grubBootCombo->clear();
  char *ndev, *nsz;
  int i, nsize;
  while (fgets(line, sizeof line, fp) != NULL) {
    i = strlen(line);
    line[--i] = '\0';
    strtok(line, " \t");
    strtok(NULL, " \t");
    nsz = strtok(NULL, " \t");
    ndev = strtok(NULL, " \t");
    if (ndev != NULL && strlen(ndev) == 3) {
      nsize = atoi(nsz) / 1024;
      if (nsize > 1000) {
        sprintf(line, "%s", ndev);
        diskCombo->addItem(line);
        grubBootCombo->addItem(line);
      }
    }
  }
  pclose(fp);

  on_diskCombo_activated();

  gotoPage(0);
}

/////////////////////////////////////////////////////////////////////////
// slots

void MInstall::on_passwordCheckBox_stateChanged(int state) {
  if (state == Qt::Unchecked) {
    // don't show
    userPasswordEdit->setEchoMode(QLineEdit::Password);
    userPasswordEdit2->setEchoMode(QLineEdit::Password);
    rootPasswordEdit->setEchoMode(QLineEdit::Password);
    rootPasswordEdit2->setEchoMode(QLineEdit::Password);
  } else {
    // show
    userPasswordEdit->setEchoMode(QLineEdit::Normal);
    userPasswordEdit2->setEchoMode(QLineEdit::Normal);
    rootPasswordEdit->setEchoMode(QLineEdit::Normal);
    rootPasswordEdit2->setEchoMode(QLineEdit::Normal);    
  }
}

void MInstall::on_nextButton_clicked() {
  int next = widgetStack->currentIndex() + 1;
  // make sure button is released
  nextButton->setDown(false);

  gotoPage(next);
}

void MInstall::on_backButton_clicked() {
  int curr = widgetStack->currentIndex();
  int next = curr - 1;

  gotoPage(next);
}

void MInstall::on_abortInstallButton_clicked() {
  procAbort();
  QApplication::beep();
}

void MInstall::on_qtpartedButton_clicked() {
  system("/sbin/swapoff -a 2>&1");
  system("/usr/sbin/gparted");
  system("/usr/sbin/buildfstab -r");
  system("/sbin/swapon -a 2>&1");
  this->updatePartitionWidgets();
  on_diskCombo_activated();
}

// disk selection changed, rebuild root
void MInstall::on_diskCombo_activated() {
  char line[130];
  QString drv = QString("/dev/%1").arg(diskCombo->currentText());

  rootCombo->clear();
  QString cmd = QString("/sbin/fdisk -l %1 | /bin/grep \"^/dev\"").arg(drv);
  FILE *fp = popen(cmd.toAscii(), "r");
  int rcount = 0;
  if (fp != NULL) {
    char *ndev, *nsz, *nsys, *nsys2;
    int nsize;
    int i;
    while (fgets(line, sizeof line, fp) != NULL) {
      i = strlen(line);
      line[--i] = '\0';
      strtok(line, " /*+\t");
      ndev = strtok(NULL, " /*+\t");
      strtok(NULL, " *+\t");
      strtok(NULL, " *+\t");
      nsz = strtok(NULL, " *+\t");
      strtok(NULL, " *+\t");
      nsys = strtok(NULL, " *+\t");
      nsys2 = strtok(NULL, " *+\t");
      nsize = atoi(nsz);
      nsize = nsize / 1024;

      if ((nsize >= 1200) && (strncmp(nsys, "Linux", 5) == 0 || (strncmp(nsys, "FAT", 3) == 0) || (strncmp(nsys, "W95", 3) == 0) || (strncmp(nsys, "HPFS", 4) == 0))) {;
        sprintf(line, "%s - %dMB - %s", ndev, nsize, nsys);
        rootCombo->addItem(line);
        rcount++;
      }
    }
    pclose(fp);
  }
  if (rcount == 0) {
    rootCombo->addItem("none");
  }
  on_rootCombo_activated();
}

// root partition changed, rebuild swap
void MInstall::on_rootCombo_activated() {
  char line[130];
  QString drv = QString("/dev/%1").arg(diskCombo->currentText());

  swapCombo->clear();
  swapCombo->addItem("none - or existing");
  int rcount = 1;
  QString cmd = QString("/sbin/fdisk -l %1 | /bin/grep \"^/dev\"").arg(drv);
  FILE *fp = popen(cmd.toAscii(), "r");
  if (fp != NULL) {
    char *ndev, *nsz, *nsys, *nsys2;
    int nsize, i;
    while (fgets(line, sizeof line, fp) != NULL) {
      i = strlen(line);
      line[--i] = '\0';
      strtok(line, " /*+\t");
      ndev = strtok(NULL, " /*+\t");
      strtok(NULL, " *+\t");
      strtok(NULL, " *+\t");
      nsz = strtok(NULL, " *+\t");
      strtok(NULL, " *+\t");
      nsys = strtok(NULL, " *+\t");
      nsys2 = strtok(NULL, " *+\t");
      nsize = atoi(nsz);
      nsize = nsize / 1024;
      if (nsys2 != NULL && strncmp(nsys2, "swap", 4) == 0) {
        sprintf(line, "%s - %dMB - %s", ndev, nsize, nsys2);
        swapCombo->addItem(line);
        rcount++;
      }  
    }
    pclose(fp);
  }
  on_swapCombo_activated();
}

// swap partition changed, rebuild home
void MInstall::on_swapCombo_activated() {;
  char line[130];
 
  homeCombo->clear();
  homeCombo->addItem("root"); 
  for (int i = 0; i < diskCombo->count(); ++i) {
    QString drv = QString("/dev/%1").arg(diskCombo->itemText(i));
    QString cmd = QString("/sbin/fdisk -l %1 | /bin/grep \"^/dev\"").arg(drv);
    FILE *fp = popen(cmd.toAscii(), "r");
    if (fp != NULL) {
      char *ndev, *nsz, *nsys, *nsys2;
      int nsize, i;
      while (fgets(line, sizeof line, fp) != NULL) {
        i = strlen(line);
        line[--i] = '\0';
        strtok(line, " /*+\t");
        ndev = strtok(NULL, " /*+\t");
        strtok(NULL, " *+\t");
        strtok(NULL, " *+\t");
        nsz = strtok(NULL, " *+\t");
        strtok(NULL, " *+\t");
        nsys = strtok(NULL, " *+\t");
        nsys2 = strtok(NULL, " *+\t");
        nsize = atoi(nsz);
        nsize = nsize / 1024;
        if (strcmp(ndev, rootCombo->currentText().section(' ', 0, 0).toAscii()) != 0 &&
          (nsize >= 100) && (strncmp(nsys, "Linux", 5) == 0) && (nsys2 == NULL)) {;
        sprintf(line, "%s - %dMB - %s", ndev, nsize, nsys);
        homeCombo->addItem(line);
          }
      }
      pclose(fp);
    }
  }
}

void MInstall::on_rootTypeCombo_activated() {;
  if (rootTypeCombo->currentText().startsWith("ext")) {
    badblocksCheck->setEnabled(true);
  } else {
    badblocksCheck->setEnabled(false);
  }
    badblocksCheck->setChecked(false);
}

void MInstall::procAbort() {
  proc->terminate();
  QTimer::singleShot(5000, proc, SLOT(kill()));
}

bool MInstall::close() {
  if (proc->state() != QProcess::NotRunning) {
    int ans = QMessageBox::warning(0, QString::null,
        tr("antiX is installing, are you \nsure you want to Close now?"),
        tr("Yes"), tr("No"));
    if (ans != 0) {
      return false;
    } else {
      procAbort();
    }
  }
//  system("umount -a 2>/dev/null");
  return QWidget::close();
}

/*
void MInstall::moreClicked(QListViewItem *item) {
  if (dansItem->isOn()) {
    squidItem->setOn(true);
  }

}
*/
/////////////////////////////////////////////////////////////////////////
// delete process events

void MInstall::delStart() {
  timer->start(20000);
  updateStatus(tr("Deleting old system"), 4);
}

void MInstall::delDone(int exitCode, QProcess::ExitStatus exitStatus) {
  if (exitStatus == QProcess::NormalExit) {
    copyLinux();
  } else {
    nextButton->setEnabled(true);
    unmountGoBack(tr("Failed to delete old antiX on destination.\nReturning to Step 1."));
  }
}

void MInstall::delTime() {
  progressBar->setValue(progressBar->value() + 1);
}

/////////////////////////////////////////////////////////////////////////
// copy process events

void MInstall::copyStart() {
  timer->start(2000);
  updateStatus(tr("Copying new system"), 15);
}

void MInstall::copyDone(int exitCode, QProcess::ExitStatus exitStatus) {
  char line[130];
  char rootdev[20];
  char swapdev[20];
  char homedev[20];

  // get config
  strcpy(line, rootCombo->currentText().toAscii());
  char *tok = strtok(line, " -");
  sprintf(rootdev, "/dev/%s", tok);
  strcpy(line, swapCombo->currentText().toAscii());
  tok = strtok(line, " -");
  sprintf(swapdev, "/dev/%s", tok);
  strcpy(line, homeCombo->currentText().toAscii());
  tok = strtok(line, " -");
  sprintf(homedev, "/dev/%s", tok);

  timer->stop();

  if (exitStatus == QProcess::NormalExit) {
    updateStatus(tr("Fixing configuration"), 99);
    chmod("/mnt/antiX/var/tmp",01777);
    system("cd /mnt/antiX && ln -s var/tmp tmp");
    system("ls /lib64 && cd /mnt/antiX && ln -s lib lib64");

    FILE *fp = fopen("/mnt/antiX/etc/fstab", "w");
    if (fp != NULL) {
      fputs("# Pluggable devices are handled by uDev, they are not in fstab\n", fp);
      if (isRootFormatted) {
        if (isFormatExt4) {
          sprintf(line, "%s / auto defaults,noatime 1 1\n", rootdev);
        } else if (isFormatExt3) {
          sprintf(line, "%s / auto defaults,noatime 1 1\n", rootdev);
        } else if (isFormatXfs) {
          sprintf(line, "%s / auto defaults,noatime 1 1\n", rootdev);
        } else if (isFormatJfs) {
          sprintf(line, "%s / auto defaults,noatime 1 1\n", rootdev);
        } else if (isFormatBtrfs) {
          sprintf(line, "%s / auto defaults,noatime 1 1\n", rootdev);
        } else if (isFormatReiserfs) {
          sprintf(line, "%s / reiserfs defaults,noatime,notail 0 0\n", rootdev);
        } else if (isFormatReiser4) {
          sprintf(line, "%s / reiser4 defaults,noatime,notail 0 0\n", rootdev);
        } else {
          sprintf(line, "%s / auto defaults,noatime 1 1\n", rootdev);
        }
      } else {
        sprintf(line, "%s / auto defaults,noatime 1 1\n", rootdev);
      }
      fputs(line, fp);
      if (strcmp(swapdev, "/dev/none") != 0) {
        sprintf(line, "%s swap swap sw,pri=1 0 0\n", swapdev);
        fputs(line, fp);
      }
      fputs("proc /proc proc defaults 0 0\n", fp);
      fputs("devpts /dev/pts devpts mode=0622 0 0\n", fp);
      if (strcmp(homedev, "/dev/root") != 0) {
        if (isHomeFormatted) {
          if (isFormatExt4) {
            sprintf(line, "%s /home auto defaults,noatime 1 2\n", homedev);
          } else if (isFormatExt3) {
            sprintf(line, "%s /home auto defaults,noatime 1 2\n", homedev);
          } else if (isFormatXfs) {
            sprintf(line, "%s /home auto defaults,noatime 1 2\n", homedev);
          } else if (isFormatJfs) {
            sprintf(line, "%s /home auto defaults,noatime 1 2\n", homedev);
          } else if (isFormatBtrfs) {
            sprintf(line, "%s /home auto defaults,noatime 1 2\n", homedev);
          } else if (isFormatReiserfs) {
            sprintf(line, "%s /home reiserfs defaults,noatime,notail 0 0\n", homedev);
          } else if (isFormatReiser4) {
            sprintf(line, "%s /home reiser4 defaults,noatime,notail 0 0\n", homedev);
          } else {
            sprintf(line, "%s /home auto defaults,noatime 1 2\n", homedev);
          }
        } else {
          sprintf(line, "%s /home auto defaults,noatime 1 2\n", homedev);
        }
        fputs(line, fp);
      }
      fclose(fp);
    }
    // Copy live set up to install and clean up.
    system("/bin/mv -f /etc/rc.local /mnt/antiX/etc/rc.local2");
    system("/bin/mv -f /etc/udev/rules.d/90-fstab-automount.rules /mnt/antiX/etc/udev/rules.d/90-fstab-automount.rules.live");
    system("/bin/rm /mnt/antiX/etc/udev/rules.d/90-fstab-automount.rules");
    system("/bin/cp -fp /usr/share/antiX/rc.local.live /mnt/antiX/etc/rc.local");
    system("/bin/cp -fp /etc/init.d/debian/cron /mnt/antiX/etc/init.d/cron");
    system("/bin/cp -fp /etc/init.d/debian/gpm /mnt/antiX/etc/init.d/gpm");
    system("/bin/cp -fp /etc/init.d/debian/umountfs /mnt/antiX/etc/init.d/umountfs");
    system("/bin/cp -fp /etc/init.d/debian/sendsigs /mnt/antiX/etc/init.d/sendsigs");
    system("/bin/cp -fp /etc/init.d/debian/console-setup /mnt/antiX/etc/init.d/console-setup");
    system("/bin/cp -fp /usr/share/antix-install/issue /mnt/antiX/etc/issue");
    system("/bin/cp -fp /usr/share/slim/themes/antiX/slim-install.conf /mnt/antiX/etc/slim.conf");
      
    system("chroot /mnt/antiX dpkg --purge live-init-antix");

    system("/bin/rm -rf /mnt/antiX/home/demo");
    system("/bin/rm -rf /mnt/antiX/media/sd*");
    system("/bin/rm -rf /mnt/antiX/media/hd*");
    system("/bin/mv -f /mnt/antiX/etc/X11/xorg.conf /mnt/antiX/etc/X11/xorg.conf.live >/dev/null 2>&1");

    progressBar->setValue(100);
    nextButton->setEnabled(true);
    QApplication::beep();
    setCursor(QCursor(Qt::ArrowCursor));
    on_nextButton_clicked();
  } else {
    nextButton->setEnabled(true);
    unmountGoBack(tr("Failed to write antiX to destination.\nReturning to Step 1."));
  }
}

void MInstall::copyTime() {
  char line[130];
  char rootdev[20];
  strcpy(line, rootCombo->currentText().toAscii());
  char *tok = strtok(line, " -");
  sprintf(rootdev, "/dev/%s", tok);

  QString val = getCmdValue("df /mnt/antiX", rootdev, " ", "/");
  QRegExp sep("\\s+");
  QString s = val.section(sep, 2, 2);
  int i = s.toInt();
  val = getCmdValue("df /dev/loop0", "/dev/loop0", " ", "/");
  s = val.section(sep, 2, 2);
  int j = s.toInt()/27;
  i = i/j;
  if (i > 79) {
     i = 80;
  }
  progressBar->setValue(i + 15);

  switch (i) {
    case 1:
      tipsEdit->setText(tr("<p><b>Getting Help</b><br/>"
        "Basic information about antiX Linux is at http://antix.mepis.com "
        "There are volunteers to help you at the antiX Forum, http://antix.freeforums.org</p>"
        "<p>If you ask for help, please remember to describe your problem and your computer "
        "in some detail. Usually statements like 'it didn't work' are not helpful.</p>"));
      break;
    
    case 15:
      tipsEdit->setText(tr("<p><b>Repairing Your Installation</b><br/>"
      "If antiX stops working from the hard drive, sometimes it's possible to fix the problem by booting from CD and running one of the utilities in System Configuration or by using one of the regular Linux tools to repair the system.</p>"
      "<p>You can also use your antiX CD to recover data from MS-Windows systems!</p>"));
      break;

    case 30:
      tipsEdit->setText(tr("<p><b>Support antiX</b><br/>"
      "antiX is supported by people like you. Some help others at the "
      "support forum - http://antix.freeforums.org, or translate help files into different "
      "languages, or make suggestions, write documentation, or help test new software.</p>"));
      break;

    case 45:
      tipsEdit->setText(tr("<p><b>Adjusting Your Sound Mixer</b><br/>"
      "antiX attempts to configure the sound mixer for you but sometimes it will be "
      "necessary for you to turn up volumes and unmute channels in the mixer "
      "in order to hear sound.</p> "
      "<p>The mixer shortcut is located in the tray. Click on it to open the mixer. </p>"));
      break;

    case 60:
      tipsEdit->setText(tr("<p><b>Keep Your Copy of antiX up-to-date</b><br/>"
        "For antiX information and updates please visit http://antix.freeforums.org. </p>"));
      break;

    default:
      break;
  } 
}
