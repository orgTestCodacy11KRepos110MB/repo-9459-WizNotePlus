﻿#include "WizFileImporter.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>
#include <QDebug>

#include "utils/WizPathResolve.h"
#include "utils/WizMisc.h"
#include "html/WizHtmlCollector.h"
#include "share/WizMisc.h"
#include "share/WizRtfReader.h"
#include "database/WizDatabase.h"
#include "core/WizNoteManager.h"
#include "mac/WizMacHelper.h"

WizFileImporter::WizFileImporter(WizDatabaseManager& dbMgr, QObject *parent)
    : QObject(parent)
    , m_dbMgr(dbMgr)
{
}

void WizFileImporter::importFiles(const QStringList& strFiles, const QString& strTargetFolderLocation)
{
    WIZTAGDATA tag;
    importFiles(strFiles, "", strTargetFolderLocation, tag);
}

void WizFileImporter::importFiles(const QStringList& strFiles, const QString& strKbGUID, const WIZTAGDATA& tag)
{
    QString location = m_dbMgr.db(strKbGUID).getDefaultNoteLocation();
    importFiles(strFiles, strKbGUID, location, tag);
}

/**
 * @brief Import files in bulk
 * @param strFiles List of file paths to import
 * @param strKbGUID
 * @param strTargetFolderLocation Path of target category
 * @param tag Tag for document
 */
void WizFileImporter::importFiles(const QStringList& strFiles, const QString& strKbGUID,
                                   const QString& strTargetFolderLocation, const WIZTAGDATA& tag)
{
    int nTotal = strFiles.count();
    int nFailed = 0;
    QString text(tr("%1 file(s) import failed:"));
    text += "\n";
    for (int i = 0; i < nTotal; ++i)
    {
        QString strFile = strFiles.at(i);
        if (!importFile(strFile, strKbGUID, strTargetFolderLocation, tag))
        {
            ++nFailed;
            text.append(strFile).append("\n");
        }

        emit importProgress(nTotal, i + 1);
    }
    text = text.arg(nFailed);
    emit importFinished(nFailed == 0, text, m_strKbGuid);
}

/**
 * @brief Load pure html file with local text encodings
 * 
 * @param strFileName 
 * @return QString 
 */
QString WizFileImporter::loadHtmlFileToHtml(const QString& strFileName)
{
    QFile file(strFileName);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
        return "";
    QTextStream in(&file);
    QString ret = in.readAll();
    file.close();

    return ret;
}

/**
 * @brief Load UTF-8 encoded html file.
 * 
 * @param strFileName 
 * @param isUTF8 
 * @return QString 
 */
QString WizFileImporter::loadHtmlFileToHtml(const QString& strFileName, const char *encoding)
{
    QFile file(strFileName);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
        return "";
    QTextStream in(&file);
    in.setCodec(encoding);
    QString ret = in.readAll();
    file.close();

    return ret;
}

/**
 * @brief 将纯文本导入成HTML
 * @param strFileName 文件地址
 * @return
 */
QString WizFileImporter::loadTextFileToHtml(const QString& strFileName)
{
    QFile file(strFileName);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
        return "";
    QTextStream in(&file);
    QString ret = in.readAll();
    file.close();
    ret = ret.toHtmlEscaped();
    ret.replace("\n","<br>");
    ret.replace(" ","&nbsp;");

    return ret;
}

QString WizFileImporter::loadTextFileToHtml(const QString& strFileName, const char *encoding)
{
    QFile file(strFileName);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
        return "";
    QTextStream in(&file);
    in.setCodec(encoding);
    QString ret = in.readAll();
    file.close();
    ret = ret.toHtmlEscaped();
    ret.replace("\n","<br>");
    ret.replace(" ","&nbsp;");

    return ret;
}

QString WizFileImporter::loadImageFileToHtml(const QString& strFileName)
{
    return QString("<img border=\"0\" src=\"%1\" />")
            .arg(QUrl::fromLocalFile(strFileName).toString());
}

/**
 * @brief Import file into WizNote database.
 * @param strFile File path
 * @param strKbGUID
 * @param strLocation
 * @param tag
 * @return
 */
bool WizFileImporter::importFile(const QString& strFile, const QString& strKbGUID,
                                  const QString& strLocation, const WIZTAGDATA& tag)
{
    m_strKbGuid = strKbGUID;
    QFileInfo fi(strFile);
    QString strHtml;
    QStringList textExtList, imageExtList, rtfExtList, docExtList, htmlExtList;
    textExtList << "txt" << "md" << "markdown" << "mht" << "cpp" << "h";
    imageExtList << "jpg" << "png" << "gif" << "tiff" << "jpeg" << "bmp" << "svg";
    rtfExtList << "rtf";
    docExtList << "doc" << "docx" << "pages";
    htmlExtList << "html" << "htm";

#ifdef Q_OS_MAC
    QStringList webExtList;
    webExtList << "webarchive";
#endif
    bool addAttach = false;
    bool containsImage = false;
    QString docType = fi.suffix();
    //
    qDebug() << "import file type: " << docType;
    //
    if (textExtList.contains(docType,Qt::CaseInsensitive))
    {
        strHtml = loadTextFileToHtml(strFile, "UTF-8");
        addAttach = true;
    }
    else if (imageExtList.contains(docType,Qt::CaseInsensitive))
    {
        strHtml = loadImageFileToHtml(strFile);
        containsImage = true;
    }
    else if (htmlExtList.contains(docType, Qt::CaseInsensitive))
    {
        strHtml = loadHtmlFileToHtml(strFile, "UTF-8");
        containsImage = true;
        addAttach = true;
    }
#ifdef Q_OS_MAC
    else if (rtfExtList.contains(docType, Qt::CaseInsensitive))
    {
        if (!wizDocumentToHtml(strFile, RTFTextDocumentType, strHtml))
            return false;
        WizGetBodyContentFromHtml(strHtml, true);
        addAttach = true;
    }
    else if (docExtList.contains(docType))
    {
        if (!wizDocumentToHtml(strFile, DocFormatTextDocumentType, strHtml))
            return false;
        WizGetBodyContentFromHtml(strHtml, true);
        addAttach = true;
    }
    else if (webExtList.contains(docType))
    {
        if (!wizDocumentToHtml(strFile, WebArchiveTextDocumentType, strHtml))
            return false;
        //qDebug() << strHtml;
        containsImage = true;
        addAttach = false;
    }
#endif
    else
    {
        addAttach = true;
    }

    QString strTitle = Utils::WizMisc::extractFileName(strFile);

    WizNoteManager manager(m_dbMgr);
    WIZDOCUMENTDATA doc;
    bool ret = manager.createNote(doc, strKbGUID, strTitle, strHtml, strFile, strLocation, tag);
    if (!ret)
    {
        qCritical() << "create note faile : " << strTitle;
        return false;
    }

    WizDatabase& db = m_dbMgr.db(strKbGUID);
    if (addAttach)
    {
        WIZDOCUMENTATTACHMENTDATA attach;
        if (!db.addAttachment(doc, strFile, attach))
        {
            qWarning() << "add attachment failed , " << strFile;
        }
    }
    else if (containsImage)
    {
        //为了提取和file路径相关联的图片，在创建之后更新笔记内容
        db.updateDocumentData(doc, strHtml, strFile, 0);
    }

    return true;
}
