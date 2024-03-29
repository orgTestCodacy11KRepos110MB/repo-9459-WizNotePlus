#ifndef API_APIWIZHTMLEDITORAPP_H
#define API_APIWIZHTMLEDITORAPP_H

#include <QObject>

class WizDocumentWebView;

class ApiWizHtmlEditorApp : public QObject
{
    Q_OBJECT

private:
    WizDocumentWebView* m_documentWebView;

public:
    ApiWizHtmlEditorApp(WizDocumentWebView* webView, QObject *parent);

    // WizHtmlEditor Properties:

    Q_INVOKABLE QString getUserGuid();
    Q_PROPERTY(QString userGuid READ getUserGuid)

    Q_INVOKABLE QString getUserAlias();
    Q_PROPERTY(QString userAlias READ getUserAlias)

    Q_INVOKABLE QString getUserAvatarFilePath();
    Q_PROPERTY(QString userAvatarFilePath READ getUserAvatarFilePath)

    Q_INVOKABLE bool isPersonalDocument();
    Q_PROPERTY(bool isPersonalDocument READ isPersonalDocument NOTIFY isPersonalDocumentChanged)

    Q_INVOKABLE bool canEditNote();
    Q_PROPERTY(bool canEditNote READ canEditNote NOTIFY canEditNoteChanged)

    Q_INVOKABLE QString getCurrentNoteHtml();
    Q_PROPERTY(QString currentNoteHtml READ getCurrentNoteHtml NOTIFY currentHtmlChanged STORED false)

    Q_INVOKABLE bool hasEditPermissionOnCurrentNote();
    Q_PROPERTY(bool hasEditPermissionOnCurrentNote READ hasEditPermissionOnCurrentNote NOTIFY hasEditPermissionOnCurrentNoteChanged)

    // WizHtmlEditor Methods:
    Q_INVOKABLE void changeCurrentDocumentType(const QString& strType);
    Q_INVOKABLE bool checkListClickable();
    Q_INVOKABLE bool shouldAddCustomCSS();
    Q_INVOKABLE bool canRenderMarkdown();
    Q_INVOKABLE QString getLocalLanguage();
    Q_INVOKABLE void onSelectionChange(const QString& currentStyle);
    Q_INVOKABLE void onClickedSvg(const QString& data);
    Q_INVOKABLE void saveCurrentNote();
    Q_INVOKABLE void setModified(bool b);
    Q_INVOKABLE void onNoteLoadFinished();
    Q_INVOKABLE void onReturn();
    Q_INVOKABLE void doPaste();
    Q_INVOKABLE void doCopy();
    Q_INVOKABLE void afterCopied();
    Q_INVOKABLE void onMarkerUndoStatusChanged(QString data);
    Q_INVOKABLE void onMarkerInitiated(QString data);

signals:
    void isPersonalDocumentChanged();
    void canEditNoteChanged();
    void currentHtmlChanged();
    void hasEditPermissionOnCurrentNoteChanged();

    void clickingTodoCallBack(bool cancel, bool needCallAgain);
};

#endif // API_APIWIZHTMLEDITORAPP_H
