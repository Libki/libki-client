#ifndef PRINTPROTOCOL_H
#define PRINTPROTOCOL_H

#include <QString>

static const quint32 LIBKI_PRINT_PROTOCOL_VERSION = 1;
static const char LIBKI_PRINT_SERVER_NAME[] = "LibkiPrintServer";

enum PrintMessageType
{
    PrintMessage_GetPrinters = 1,
    PrintMessage_SubmitPrint = 2
};

struct SubmitPrintRequest
{
    QString filename;
    QString printer;
    int copies;
    int pageCount;
};

#endif
