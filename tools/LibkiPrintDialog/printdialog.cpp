/*
 * Copyright 2026 Ian Walls <ian@bywatersolutions.com>
 *
 * This file is part of Libki.
 *
 * Libki is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Libki is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Libki. If not, see <http://www.gnu.org/licenses/>.
 */


#include "printdialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFileInfo>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

PrintDialog::PrintDialog(SubmitPrintRequest &request, PrintInfoReply &info, QWidget *parent): QDialog(parent), request(request), info(info)
{
    setWindowTitle(tr("Print Document"));

    QLabel *documentLabel = new QLabel(tr("Document:"));
    documentValue = new QLabel(QFileInfo(request.filename).fileName());

    QLabel *pagesLabel = new QLabel(tr("Pages:"));
    pagesValue = new QLabel(QString::number(request.pageCount));

    QLabel *printerLabel = new QLabel(tr("Printer:"));
    printerCombo = new QComboBox();
    printerCombo->addItem(request.printer);

    QLabel *copiesLabel = new QLabel(tr("Copies:"));
    copiesSpin = new QSpinBox();
    copiesSpin->setMinimum(1);
    copiesSpin->setMaximum(999);
    copiesSpin->setValue(request.copies);

    connect(copiesSpin,
        SIGNAL(valueChanged(int)),
        this,
        SLOT(updateTotals()));

    costPerPageValue = new QLabel();
    estimatedCostValue = new QLabel();
    availableFundsValue = new QLabel();
    remainingBalanceValue = new QLabel();
    statusLabel = new QLabel();

    costPerPageValue->setText(money(info.costPerPage));
    estimatedCostValue->setText(money(info.estimatedCost));
    availableFundsValue->setText(money(info.availableFunds));
    remainingBalanceValue->setText(money(info.remainingBalance));

    buttonBox =
        new QDialogButtonBox(
            QDialogButtonBox::Ok |
            QDialogButtonBox::Cancel);

    buttonBox->button(QDialogButtonBox::Ok)
        ->setText(tr("Print"));

    QGridLayout *grid =
        new QGridLayout();

    grid->addWidget(documentLabel,0,0);
    grid->addWidget(documentValue,0,1);

    grid->addWidget(pagesLabel,1,0);
    grid->addWidget(pagesValue,1,1);

    grid->addWidget(printerLabel,2,0);
    grid->addWidget(printerCombo,2,1);

    grid->addWidget(copiesLabel,3,0);
    grid->addWidget(copiesSpin,3,1);

    grid->addWidget(new QLabel(tr("Cost per page:")), 4, 0);
    grid->addWidget(costPerPageValue,               4, 1);

    grid->addWidget(new QLabel(tr("Estimated cost:")), 5, 0);
    grid->addWidget(estimatedCostValue,               5, 1);

    grid->addWidget(new QLabel(tr("Current balance:")), 6, 0);
    grid->addWidget(availableFundsValue,               6, 1);

    grid->addWidget(new QLabel(tr("Balance after job:")), 7, 0);
    grid->addWidget(remainingBalanceValue,              7, 1);

    grid->addWidget(statusLabel, 8, 0);//, 1, 2);

    QVBoxLayout *layout =
        new QVBoxLayout();

    layout->addLayout(grid);
    layout->addWidget(buttonBox);

    setLayout(layout);

    connect(buttonBox,
            SIGNAL(accepted()),
            this,
            SLOT(accept()));

    connect(buttonBox,
            SIGNAL(rejected()),
            this,
            SLOT(reject()));

    resize(450,180);
    updateTotals();
}

void PrintDialog::accept() {
    request.printer = printerCombo->currentText();

    request.copies = copiesSpin->value();

    QDialog::accept();
}

QString PrintDialog::money(double value) const {
    return QLocale().toCurrencyString(
        value,
        info.currency);
}

void PrintDialog::updateTotals() {
    request.copies = copiesSpin->value();

    info.estimatedCost = info.costPerPage * request.pageCount * request.copies;

    info.remainingBalance = info.availableFunds - info.estimatedCost;

    estimatedCostValue->setText(money(info.estimatedCost));

    remainingBalanceValue->setText( money(info.remainingBalance));

    bool canPrint = info.remainingBalance >= 0.0;

    QPushButton *ok =
        buttonBox->button(QDialogButtonBox::Ok);

    if (canPrint) {
        ok->setText(tr("Print"));
        statusLabel->clear();
    } else {
        ok->setText(tr("Queue Job"));
        statusLabel->setText(
            tr("This job will be queued until sufficient funds are available."));
    }
}
