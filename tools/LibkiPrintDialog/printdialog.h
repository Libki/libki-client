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

#ifndef PRINTDIALOG_H
#define PRINTDIALOG_H

#include <QDialog>

#include "../../printprotocol.h"

class QLabel;
class QComboBox;
class QSpinBox;
class QPushButton;
class QDialogButtonBox;

class PrintDialog : public QDialog {
  Q_OBJECT

 public:
  explicit PrintDialog(SubmitPrintRequest &request, PrintInfoReply &info,
                       QWidget *parent = 0);

 private slots:
  void accept();
  void updateTotals();

 private:
  SubmitPrintRequest &request;
  PrintInfoReply &info;

  QString money(double value) const;

  QLabel *documentValue;
  QLabel *pagesValue;
  QLabel *costPerPageValue;
  QLabel *estimatedCostValue;
  QLabel *availableFundsValue;
  QLabel *availableGratisValue;
  QLabel *remainingFundsValue;
  QLabel *remainingGratisValue;
  QLabel *statusLabel;
  QLabel *gratisLabel;

  QComboBox *printerCombo;
  QSpinBox *copiesSpin;

  QDialogButtonBox *buttonBox;
};

#endif  // PRINTDIALOG_H
