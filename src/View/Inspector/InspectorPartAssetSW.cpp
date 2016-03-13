#include "InspectorPartAssetSW.h"

InspectorPartAssetSW::InspectorPartAssetSW(const std::string &labelString, const std::string &value, InspectorPartWidget *parent) :
    InspectorPartSW(parent)
{
    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->setSpacing(0);
    this->setLayout(vLayout);

    QLabel *textLabel = new QLabel(QString::fromStdString(labelString));
    textLabel->setContentsMargins(0,0,0,0);
    vLayout->addWidget(textLabel);

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setSpacing(0); hLayout->setContentsMargins(0,0,0,0);
    vLayout->addLayout(hLayout);

    filepathLineEdit = new QLineEdit();
    hLayout->addWidget(filepathLineEdit);

    QPushButton *browseButton = new QPushButton(QString::fromStdString("Browse"));
    connect(browseButton, SIGNAL(clicked()), this, SLOT(Browse()));
    hLayout->addWidget(browseButton);

    this->setContentsMargins(0,0,0,0);
    this->show();
}

void InspectorPartAssetSW::Browse()
{
    QString directory = QFileDialog::getExistingDirectory(this,
                            tr("Find Files"), QDir::currentPath());

    Logger_Log("ASDSADSA");
    if (!directory.isEmpty())
    {
        //if (directoryComboBox->findText(directory) == -1)
        //    directoryComboBox->addItem(directory);
        //directoryComboBox->setCurrentIndex(directoryComboBox->findText(directory));
    }
}

void InspectorPartAssetSW::SetValue(const std::string &assetPath)
{
    if(this->assetPath != assetPath)
    {
        this->assetPath = assetPath;
        filepathLineEdit->setText(QString::fromStdString(assetPath));
    }
}

std::string  InspectorPartAssetSW::GetValue()
{
    return assetPath;
}
