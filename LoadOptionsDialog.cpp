/*
 *  HDRMerge - HDR exposure merging software.
 *  Copyright 2012 Javier Celaya
 *  jcelaya@gmail.com
 *
 *  This file is part of HDRMerge.
 *
 *  HDRMerge is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  HDRMerge is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with HDRMerge. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <QFormLayout>
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QPushButton>
#include <QFileDialog>
#include <QSettings>
#include "LoadOptionsDialog.hpp"

namespace hdrmerge {

LoadOptionsDialog::LoadOptionsDialog(QWidget * parent, Qt::WindowFlags f)
        : QDialog(parent, f), LoadOptions() {
    QVBoxLayout * layout = new QVBoxLayout(this);

    QWidget * fileSelector = new QWidget(this);
    QHBoxLayout * fileSelectorLayout = new QHBoxLayout(fileSelector);
    fileSelectorLayout->setMargin(0);
    fileList = new QListWidget(fileSelector);
    fileList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    fileSelectorLayout->addWidget(fileList, 1);

    QWidget * addRemoveButtons = new QWidget(this);
    QVBoxLayout * addRemoveButtonsLayout = new QVBoxLayout(addRemoveButtons);
    addRemoveButtonsLayout->setMargin(0);
    QPushButton * addButton = new QPushButton(tr("Add"), addRemoveButtons);
    QPushButton * removeButton = new QPushButton(tr("Remove"), addRemoveButtons);
    addRemoveButtonsLayout->addWidget(addButton, 0, Qt::AlignTop);
    addRemoveButtonsLayout->addWidget(removeButton, 0, Qt::AlignTop);
    addRemoveButtonsLayout->addStretch(1);
    fileSelectorLayout->addWidget(addRemoveButtons, 0);
    layout->addWidget(fileSelector, 1);
    connect(addButton, SIGNAL(clicked(bool)), this, SLOT(addFiles()));
    connect(removeButton, SIGNAL(clicked(bool)), this, SLOT(removeFiles()));

    alignBox = new QCheckBox(tr("Align source images."), this);
    alignBox->setChecked(true);
    layout->addWidget(alignBox, 0);

    cropBox = new QCheckBox(tr("Crop result image to optimal size."), this);
    cropBox->setChecked(true);
    layout->addWidget(cropBox, 0);

    QWidget * buttons = new QWidget(this);
    QHBoxLayout * buttonsLayout = new QHBoxLayout(buttons);
    QPushButton * acceptButton = new QPushButton(tr("Accept"), this);
    acceptButton->setDefault(true);
    connect(acceptButton, SIGNAL(clicked(bool)), this, SLOT(accept()));
    QPushButton * cancelButton = new QPushButton(tr("Cancel"), this);
    connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(reject()));
    buttonsLayout->addWidget(acceptButton);
    buttonsLayout->addWidget(cancelButton);
    layout->addWidget(buttons, 0, Qt::AlignHCenter);

    setLayout(layout);
    setWindowTitle(tr("Open raw images"));
    addFiles();
}


void LoadOptionsDialog::showEvent(QShowEvent * event) {
    if (fileList->count() == 0) {
        QMetaObject::invokeMethod(this, "reject", Qt::QueuedConnection);
    }
}


class FileItem : public QListWidgetItem {
public:
    FileItem(const QString & filename, QListWidget * parent) : QListWidgetItem(parent, 1000) {
        QDir file(filename);
        setText(file.path().section('/', -1));
        setData(Qt::UserRole, QVariant(filename));
        setSizeHint(QSize(0, 24));
    }
};


void LoadOptionsDialog::addFiles() {
    QSettings settings;
    QVariant lastDirSetting = settings.value("lastOpenDirectory");
    QString filter(tr("Raw images ("
    "*.3fr "
    "*.ari *.arw "
    "*.bay "
    "*.crw *.cr2 *.cap "
    "*.dcs *.dcr *.dng *.drf "
    "*.eip *.erf "
    "*.fff "
    "*.iiq "
    "*.k25 *.kdc "
    "*.mdc *.mef *.mos *.mrw "
    "*.nef *.nrw "
    "*.obm *.orf "
    "*.pef *.ptx *.pxn "
    "*.r3d *.raf *.raw *.rwl *.rw2 *.rwz "
    "*.sr2 *.srf *.srw "
    "*.x3f"
    ")"));
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Open raw images"),
        lastDirSetting.isNull() ? QDir::currentPath() : QDir(lastDirSetting.toString()).absolutePath(),
        filter, NULL, QFileDialog::DontUseNativeDialog);
    if (!files.empty()) {
        QString lastDir = QDir(files.front()).absolutePath();
        lastDir.truncate(lastDir.lastIndexOf('/'));
        settings.setValue("lastOpenDirectory", lastDir);
        for (auto & i : files) {
            new FileItem(i, fileList);
        }
    }
}


void LoadOptionsDialog::removeFiles() {
    QList<QListWidgetItem *> items = fileList->selectedItems();
    for (auto i : items) {
        delete i;
    }
}


void LoadOptionsDialog::accept() {
    align = alignBox->isChecked();
    crop = cropBox->isChecked();
    for (int i = 0; i < fileList->count(); ++i) {
        fileNames.push_back(fileList->item(i)->data(Qt::UserRole).toString().toUtf8().constData());
    }
    QDialog::accept();
}

} // namespace hdrmerge
